/**
 * @file    OpenSlideImage.h
 * @brief   OpenSlide图像格式实现类，支持数字病理图像格式
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的OpenSlide图像格式支持，包括：
 *          - OpenSlide库的集成和封装
 *          - 数字病理图像格式的读取和处理
 *          - 多分辨率图像数据的访问
 *          - 图像元数据和属性的获取
 *          - 背景色和错误状态管理
 *          该类是DSV项目中图像数据访问的核心实现，
 *          支持Aperio、Hamamatsu、Leica等扫描仪生成的数字病理图像格式。
 *
 * @note    该类依赖OpenSlide库，需要确保OpenSlide库正确安装
 * @see     MultiResolutionImage, openslide_t
 */

#pragma once
#include "MultiResolutionImage.h"
#include <QImage>

 // OpenSlide库的前向声明
struct _openslide;
typedef struct _openslide openslide_t;

/**
 * @class  OpenSlideImage
 * @brief  OpenSlide图像格式实现类，支持数字病理图像格式
 * @details 该类是MultiResolutionImage的具体实现，专门用于处理
 *          OpenSlide支持的数字病理图像格式。OpenSlide是一个开源库，
 *          支持多种数字病理图像格式，包括Aperio (.svs)、Hamamatsu (.ndpi)、
 *          Leica (.scn)、MIRAX (.mrxs)等。
 *
 *          主要功能包括：
 *          - 数字病理图像文件的读取和解析
 *          - 多分辨率层级数据的访问
 *          - 图像元数据和属性的获取
 *          - 背景色和错误状态管理
 *          - 与OpenSlide库的底层交互
 *
 * @note   该类是线程安全的，支持多线程并发访问
 * @example
 *          // 使用示例
 *          OpenSlideImage* image = new OpenSlideImage();
 *          if (image->initializeType("pathology.svs")) {
 *              auto dimensions = image->getDimensions();
 *              auto levels = image->getNumberOfLevels();
 *              // 获取图像数据
 *              unsigned char* data = new unsigned char[width * height * 3];
 *              image->getRawRegion<unsigned char>(0, 0, width, height, 0, data);
 *          }
 * @see     MultiResolutionImage, openslide_t, SlideColorManagement
 */
class OpenSlideImage : public MultiResolutionImage
{
public:
    /**
     * @brief   默认构造函数
     * @details 创建OpenSlide图像对象，初始化OpenSlide库句柄和成员变量
     * @note    构造函数不会打开图像文件，需要调用initializeType来加载图像
     */
    OpenSlideImage();

    /**
     * @brief   析构函数
     * @details 清理OpenSlide库句柄和相关资源，确保不会发生内存泄漏
     * @note    析构函数会自动调用cleanup()方法
     */
    ~OpenSlideImage();

    /**
     * @brief   初始化OpenSlide图像文件
     * @details 使用OpenSlide库打开指定的图像文件，解析图像格式，
     *          获取多分辨率层级信息和元数据。该方法会验证文件格式
     *          并设置相应的图像属性。
     *
     * @param   imagePath 图像文件路径
     * @return  true表示初始化成功，false表示失败
     * @note    该方法会调用OpenSlide库的openslide_open函数
     * @see     openslide_open, cleanup
     * @example
     *          // 使用示例
     *          OpenSlideImage image;
     *          if (image.initializeType("sample.svs")) {
     *              std::cout << "图像加载成功" << std::endl;
     *          } else {
     *              std::cout << "图像加载失败: " << image.getOpenSlideErrorState() << std::endl;
     *          }
     */
    bool initializeType(const std::string& imagePath);

    /**
     * @brief   获取通道最小值
     * @details 返回指定通道的最小值。对于OpenSlide图像，
     *          通常使用8位或16位数据，最小值通常为0。
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  通道最小值，通常为0
     * @note    该函数是虚函数的重写实现
     * @see     getMaxValue
     */
    double getMinValue(int channel = -1) { return 0.; }

    /**
     * @brief   获取通道最大值
     * @details 返回指定通道的最大值。对于OpenSlide图像，
     *          通常使用8位或16位数据，最大值通常为255（8位）或65535（16位）。
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  通道最大值，通常为255
     * @note    该函数是虚函数的重写实现
     * @see     getMinValue
     */
    double getMaxValue(int channel = -1) { return 255.; }

    /**
     * @brief   获取图像属性
     * @details 获取OpenSlide图像的元数据属性，如扫描仪信息、放大倍数、
     *          图像尺寸、颜色信息等。这些属性来自OpenSlide库的元数据。
     *
     * @param   propertyName 属性名称
     * @return  属性值字符串，如果属性不存在则返回空字符串
     * @note    该函数会调用OpenSlide库的openslide_get_property_value函数
     * @see     openslide_get_property_value, getProperties
     * @example
     *          // 使用示例
     *          std::string scanner = image.getProperty("openslide.vendor");
     *          std::string magnification = image.getProperty("openslide.objective-power");
     *          std::string mpp = image.getProperty("openslide.mpp-x");
     */
    std::string getProperty(const std::string& propertyName);

    /**
     * @brief   获取OpenSlide错误状态
     * @details 返回OpenSlide库的当前错误状态信息。当OpenSlide操作
     *          失败时，可以使用此函数获取详细的错误信息。
     *
     * @return  错误状态字符串，如果没有错误则返回空字符串
     * @note    该函数会调用OpenSlide库的openslide_get_error函数
     * @see     openslide_get_error
     * @example
     *          // 使用示例
     *          if (!image.initializeType("invalid.svs")) {
     *              std::string error = image.getOpenSlideErrorState();
     *              std::cout << "OpenSlide错误: " << error << std::endl;
     *          }
     */
    std::string getOpenSlideErrorState();

    /**
     * @brief   设置缓存大小
     * @details 设置OpenSlide图像的缓存大小，影响内存使用和性能。
     *          较大的缓存可以提高访问速度，但会增加内存使用。
     *
     * @param   cacheSize 新的缓存大小（字节）
     * @note    该函数会调用父类的setCacheSize方法
     * @see     MultiResolutionImage::setCacheSize
     */
    void setCacheSize(const unsigned long long cacheSize);

    /**
     * @brief   获取标签图像
     * @details 获取OpenSlide图像的标签或缩略图。标签图像通常是一个
     *          低分辨率的概览图像，用于快速预览整个病理切片。
     *
     * @return  QImage格式的标签图像
     * @note    该函数是虚函数的重写实现
     * @see     openslide_read_associated_image
     * @example
     *          // 使用示例
     *          QImage label = image.getLabel();
     *          if (!label.isNull()) {
     *              label.save("label.png");
     *          }
     */
    const QImage getLabel();

    /**
     * @brief   获取图像属性列表
     * @details 获取OpenSlide图像的所有元数据属性，包括扫描仪信息、
     *          图像参数、颜色信息等。返回的属性信息可以用于
     *          图像分析和处理。
     *
     * @return  包含所有属性信息的向量
     * @note    该函数是虚函数的重写实现
     * @see     getProperty, SlideColorManagement::PropertyInfo
     * @example
     *          // 使用示例
     *          auto properties = image.getProperties();
     *          for (const auto& prop : properties) {
     *              std::cout << prop.name << ": " << prop.value << std::endl;
     *          }
     */
    const std::vector<SlideColorManagement::PropertyInfo> getProperties();

protected:
    /**
     * @brief   清理资源
     * @details 清理OpenSlide库句柄和相关资源，释放内存。
     *          该方法在析构函数中自动调用，也可以手动调用。
     * @note    该函数是虚函数的重写实现
     * @see     openslide_close
     */
    void cleanup();

    /**
     * @brief   从图像读取数据
     * @details 从OpenSlide图像中读取指定区域的原始数据。该方法
     *          会调用OpenSlide库的openslide_read_region函数来
     *          获取指定层级和区域的图像数据。
     *
     * @param   startX 起始X坐标
     * @param   startY 起始Y坐标
     * @param   width 区域宽度
     * @param   height 区域高度
     * @param   level 层级索引
     * @return  指向原始数据的void指针，调用者负责释放内存
     * @note    该函数是纯虚函数的重写实现
     * @see     openslide_read_region, getRawRegion
     * @example
     *          // 使用示例
     *          void* data = image.readDataFromImage(0, 0, 512, 512, 0);
     *          if (data) {
     *              // 使用数据...
     *              delete[] data;
     *          }
     */
    void* readDataFromImage(const long long& startX, const long long& startY, const unsigned long long& width,
        const unsigned long long& height, const unsigned int& level);

    /** @brief OpenSlide库句柄，用于与OpenSlide库交互 */
    openslide_t* _slide;

private:
    /** @brief OpenSlide错误状态字符串，记录最近的错误信息 */
    std::string _errorState;

    /** @brief 背景色红色分量 */
    unsigned char _bg_r;

    /** @brief 背景色绿色分量 */
    unsigned char _bg_g;

    /** @brief 背景色蓝色分量 */
    unsigned char _bg_b;
};