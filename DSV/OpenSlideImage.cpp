/**
 * @file OpenSlideImage.cpp
 * @brief OpenSlide图像实现文件
 * @details 该文件实现了基于OpenSlide库的数字病理切片图像读取功能
 *          支持多种数字病理切片格式，包括SVS、TIFF、MRXS、NDPI等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "OpenSlideImage.h"
#include <shared_mutex>
#include "openslide/openslide.h"
#include <sstream>
#include <algorithm>

/**
 * @brief 构造函数：初始化OpenSlide图像对象
 * @details 初始化OpenSlide相关的成员变量，包括：
 *          - 继承自MultiResolutionImage的成员
 *          - OpenSlide句柄
 *          - 背景颜色设置
 */
OpenSlideImage::OpenSlideImage() 
    : MultiResolutionImage(), 
    _slide(NULL),
    _bg_r(255), 
    _bg_g(255), 
    _bg_b(255) {
    }

/**
 * @brief 析构函数：清理OpenSlide资源
 * @details 获取独占锁后清理OpenSlide句柄和相关资源
 */
OpenSlideImage::~OpenSlideImage() {
	std::unique_lock<std::shared_mutex> l(*_openCloseMutex);
	cleanup();
	MultiResolutionImage::cleanup();
}

/**
 * @brief 设置OpenSlide缓存大小
 * @param cacheSize 缓存大小（字节数）
 * @details 仅在自定义OpenSlide版本时设置缓存大小
 */
void OpenSlideImage::setCacheSize(const unsigned long long cacheSize) {
#ifdef CUSTOM_OPENSLIDE
	if (_slide) {
		openslide_set_cache_size(_slide, cacheSize);
	}
#endif
}

/**
 * @brief 获取OpenSlide错误状态
 * @return 错误状态字符串
 * @details 返回OpenSlide库的错误信息，如果没有错误则返回"No file opened."
 */
std::string OpenSlideImage::getOpenSlideErrorState() {
	if (_errorState.empty()) {
		return "No file opened.";
	}
	return _errorState;
}

/**
 * @brief 初始化OpenSlide图像类型
 * @param imagePath 图像文件路径
 * @return 初始化是否成功
 * @details 使用OpenSlide库打开图像文件并读取相关属性信息，包括：
 *          - 层级数量和尺寸
 *          - 数据类型和颜色类型
 *          - 像素间距信息
 *          - 背景颜色
 *          - 厂商信息
 */
bool OpenSlideImage::initializeType(const std::string& imagePath) {
    std::unique_lock<std::shared_mutex> l(*_openCloseMutex);
    cleanup();

    if (openslide_detect_vendor(imagePath.c_str())) {
        _slide = openslide_open(imagePath.c_str());
        if (const char* error = openslide_get_error(_slide)) {
            _errorState = error;
        }
        else {
            _errorState = "";
        }
        
        // 对于某些已知的非致命错误，仍然尝试初始化
        bool canContinue = _errorState.empty();
        if (!canContinue && !_errorState.empty()) {
            std::string errorStr(_errorState);
            if (errorStr.find("Invalid tile byte count") != std::string::npos ||
                errorStr.find("TIFFRGBAImageGet failed") != std::string::npos) {
                canContinue = true;
            }
        }
        
        if (canContinue) {
            _numberOfLevels = openslide_get_level_count(_slide);
            _dataType = SlideColorManagement::DataType::UChar;
            _samplesPerPixel = 3;
            _colorType = SlideColorManagement::ColorType::RGB;
            for (int i = 0; i < _numberOfLevels; ++i) {
                int64_t x, y;
                openslide_get_level_dimensions(_slide, i, &x, &y);
                std::vector<unsigned long long> tmp;
                tmp.push_back(x);
                tmp.push_back(y);
                _levelDimensions.push_back(tmp);
            }
            std::stringstream ssm;
            if (openslide_get_property_value(_slide, OPENSLIDE_PROPERTY_NAME_MPP_X)) {
                ssm << openslide_get_property_value(_slide, OPENSLIDE_PROPERTY_NAME_MPP_X);
                float tmp;
                ssm >> tmp;
                _spacing.push_back(tmp);
                ssm.clear();
            }
            if (openslide_get_property_value(_slide, OPENSLIDE_PROPERTY_NAME_MPP_Y)) {
                ssm << openslide_get_property_value(_slide, OPENSLIDE_PROPERTY_NAME_MPP_Y);
                float tmp;
                ssm >> tmp;
                _spacing.push_back(tmp);
                ssm.clear();
            }
            _fileType = openslide_get_property_value(_slide, OPENSLIDE_PROPERTY_NAME_VENDOR);
            const char* bg_color_hex = openslide_get_property_value(_slide, "openslide.background-color");
            if (bg_color_hex) {
                unsigned int bg_color = std::stoi(bg_color_hex, 0, 16);
                _bg_r = ((bg_color >> 16) & 0xff);
                _bg_g = ((bg_color >> 8) & 0xff);
                _bg_b = (bg_color & 0xff);
            }
            _isValid = true;
        }
        else {
            _isValid = false;
        }
    }
    else {
        _isValid = false;
    }
    return _isValid;
    
}

/**
 * @brief 获取图像属性
 * @param propertyName 属性名称
 * @return 属性值字符串
 * @details 从OpenSlide图像中读取指定属性的值
 */
std::string OpenSlideImage::getProperty(const std::string& propertyName) {
    std::string propertyValue;
    if (_slide) {
        if (openslide_get_property_value(_slide, propertyName.c_str())) {
            propertyValue = openslide_get_property_value(_slide, propertyName.c_str());
        }
    }
    return propertyValue;
}

/**
 * @brief 从图像中读取数据
 * @param startX 起始X坐标
 * @param startY 起始Y坐标
 * @param width 区域宽度
 * @param height 区域高度
 * @param level 图像层级
 * @return 图像数据指针，失败时返回NULL
 * @details 使用OpenSlide库读取指定区域的图像数据，并将BGRA格式转换为RGB格式
 *          处理透明度信息，透明像素使用背景颜色填充
 */

#include <QImage>
#include <QDebug>
#include <string>
#include <cstring>
#include <algorithm>

 // 测试代码：保存图像以验证读取结果
#define SAVE_TEST_IMAGE 0  // 定义宏来控制是否保存，1启用，0禁用

void* OpenSlideImage::readDataFromImage(const long long& startX, const long long& startY, const unsigned long long& width,
    const unsigned long long& height, const unsigned int& level) {

    if (!_isValid) {
        return NULL;
    }

    std::shared_lock<std::shared_mutex> l(*_openCloseMutex);
    unsigned int* temp = new unsigned int[width * height];
    

    
    // 激进方案：智能句柄选择策略
    openslide_t* readSlide = _slide;
    bool usingTempSlide = false;
    
    // 检查主句柄是否可用
    const char* preError = openslide_get_error(_slide);
    if (preError) {
        // qDebug() << "Main slide has error, trying temporary handle...";
        readSlide = openslide_open(m_filePath.c_str());
        if (readSlide) {
            usingTempSlide = true;
            // qDebug() << "Using temporary slide handle";
        } else {
            // qDebug() << "Failed to create temporary handle, using main slide anyway";
            readSlide = _slide;
        }
    }
    
    // 读取数据
    openslide_read_region(readSlide, temp, startX, startY, level, width, height);
    
    // 获取错误信息
    const char* error = openslide_get_error(readSlide);
    

    
    // 检查数据有效性（修复后的宽松策略）
    bool hasValidData = false;
    int nonZeroCount = 0;
    int opaqueCount = 0;
    uint32_t firstNonZero = 0;
    
    for (unsigned long long i = 0; i < width * height; ++i) {
        if (temp[i] != 0) {
            if (!hasValidData) {
                firstNonZero = temp[i];
                hasValidData = true;
            }
            nonZeroCount++;
            
            unsigned char alpha = (temp[i] >> 24) & 0xff;
            if (alpha == 255) opaqueCount++;
        }
    }
    
    // 双重保障：如果没有有效数据且还没有使用临时句柄，尝试临时句柄
    if (!hasValidData && !usingTempSlide) {
        // qDebug() << "No valid data with main handle, trying temporary handle as fallback...";
        
        openslide_t* fallbackSlide = openslide_open(m_filePath.c_str());
        if (fallbackSlide) {
            // qDebug() << "Created fallback temporary handle, re-reading...";
            
            // 重新读取数据
            memset(temp, 0, width * height * sizeof(uint32_t));
            openslide_read_region(fallbackSlide, temp, startX, startY, level, width, height);
            
            // 重新检查数据
            nonZeroCount = 0;
            opaqueCount = 0;
            hasValidData = false;
            
            for (unsigned long long i = 0; i < width * height; ++i) {
                if (temp[i] != 0) {
                    if (!hasValidData) {
                        firstNonZero = temp[i];
                        hasValidData = true;
                    }
                    nonZeroCount++;
                    
                    unsigned char alpha = (temp[i] >> 24) & 0xff;
                    if (alpha == 255) opaqueCount++;
                }
            }
            
            openslide_close(fallbackSlide);
            // qDebug() << "Closed fallback temporary handle";
            
            // if (hasValidData) {
            //     qDebug() << "✅ Fallback temporary handle succeeded!";
            // }
        }
    }
    
    // 清理主要的临时句柄
    if (usingTempSlide && readSlide) {
        openslide_close(readSlide);
        // qDebug() << "Closed temporary slide handle";
    }
    
    // 记录结果（注释掉详细调试信息）
    // if (error) {
    //     qDebug() << "OpenSlide reported error:" << error;
    // }
    // qDebug() << "Data analysis: NonZero=" << nonZeroCount << "/" << (width * height) << ", Opaque=" << opaqueCount;
    
    if (hasValidData) {
        // unsigned char a = (firstNonZero >> 24) & 0xff;
        // unsigned char r = (firstNonZero >> 16) & 0xff;
        // unsigned char g = (firstNonZero >> 8) & 0xff;
        // unsigned char b = firstNonZero & 0xff;
        // qDebug() << "First pixel ARGB:" << (int)a << (int)r << (int)g << (int)b;
        // qDebug() << "✅ Using valid data" << (error ? " (ignoring error)" : "");
    } else {
        // qDebug() << "❌ No valid data found, filling with white";
        for (unsigned long long i = 0; i < width * height; ++i) {
            temp[i] = 0xFFFFFFFF;  // 白色ARGB
        }
    }

    unsigned char* rgb = new unsigned char[width * height * 3];
    unsigned char* bgra = (unsigned char*)temp;
    for (unsigned long long i = 0, j = 0; i < width * height * 4; i += 4, j += 3) {
        if (bgra[i + 3] == 255) {
            rgb[j] = bgra[i + 2];
            rgb[j + 1] = bgra[i + 1];
            rgb[j + 2] = bgra[i];
        }
        else if (bgra[i + 3] == 0) {
            rgb[j] = _bg_r;
            rgb[j + 1] = _bg_g;
            rgb[j + 2] = _bg_b;
        }
        else {
            rgb[j] = (255. * bgra[i + 2]) / bgra[i + 3];
            rgb[j + 1] = (255. * bgra[i + 1]) / bgra[i + 3];
            rgb[j + 2] = (255. * bgra[i]) / bgra[i + 3];
        }
    }
    delete[] temp;

    return rgb;
}

/**
 * @brief 清理OpenSlide资源
 * @details 关闭OpenSlide句柄并重置为NULL
 */
void OpenSlideImage::cleanup() {
    if (_slide) {
        openslide_close(_slide);
        _slide = NULL;
    }
}

/**
 * @brief 获取标签图像
 * @return 标签图像的QImage对象
 * @details 从OpenSlide图像中读取关联的标签图像，如果存在则转换为QImage格式
 *          标签图像通常包含病理切片的标注信息
 */
const QImage OpenSlideImage::getLabel()
{
    const char* const* names = openslide_get_associated_image_names(_slide);
    for (int i = 0; names[i] != NULL; i++)
    {
        if (strcmp(names[i], "label") == 0)
        {
            int64_t w, h;
            // 获取label图尺寸
            openslide_get_associated_image_dimensions(_slide, "label", &w, &h);
            // 分配内存
            uint32_t* buffer = (uint32_t*)malloc(4 * w * h);
            // 读取label图到内存
            openslide_read_associated_image(_slide, "label", buffer);

            // 创建 QImage 对象
            QImage labelImage(w, h, QImage::Format_ARGB32);

            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    uint32_t pixel = buffer[y * w + x];
                    // 提取 ARGB 通道
                    unsigned char a = (pixel >> 24) & 0xff;
                    unsigned char r = (pixel >> 16) & 0xff;
                    unsigned char g = (pixel >> 8) & 0xff;
                    unsigned char b = pixel & 0xff;
                    // 存储到 QImage 中
                    labelImage.setPixel(x, y, qRgba(r, g, b, a));
                }
            }
            free(buffer);
            
            return labelImage;
        }
    }
    return QImage();
}

/**
 * @brief 获取图像属性列表
 * @return 属性信息向量
 * @details 获取OpenSlide图像的所有属性，并判断属性值是否为数值类型
 *          数值类型的属性存储为数值，非数值类型的属性存储为字符串
 */
const std::vector<SlideColorManagement::PropertyInfo> OpenSlideImage::getProperties()
{
    const char* const* property_names = openslide_get_property_names(_slide);
    if (property_names){

        for (int i = 0; property_names[i] != NULL; i++) {
            const char* property_name = property_names[i];
            const char* property_value = openslide_get_property_value(_slide, property_name);
            if (property_value) {
                // 尝试将属性值转换为数值类型，通过判断是否为纯数字字符串
                bool is_numeric = true;
                for (int j = 0; property_value[j] != '\0'; j++) {
                    if ((property_value[j] < '0' || property_value[j] > '9') && property_value[j] != '.' && property_value[j] != '-') {
                        is_numeric = false;
                        break;
                    }
                }
                if (is_numeric) {
                    // 如果是数值，存储到数值数据结构中
                    double numValue = std::stod(property_value);
                    _properties.emplace_back(property_name, true, numValue);
                }
                else {
                    // 如果不是数值，存储为字符串
                    _properties.emplace_back(property_name, false, 0.0, property_value);
                }
            }
        }

    }
    return _properties;
}
