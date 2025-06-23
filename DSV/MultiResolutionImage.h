/**
 * @file    MultiResolutionImage.h
 * @brief   多分辨率图像抽象基类，提供多层级图像数据的访问接口
 * @author  [JianZhang] ([])
 * @date    2025-05-19
 * @version 1.0.0
 * @details 该类实现了多分辨率图像的核心功能，包括：
 *          - 多层级图像数据的存储和管理
 *          - 瓦片缓存机制和内存管理
 *          - 线程安全的数据访问
 *          - 图像元数据和属性管理
 *          - Z平面和通道支持
 *          该类是DSV项目中图像处理的核心抽象层，为不同的图像格式
 *          提供统一的接口，支持高效的图像数据访问和处理。
 *
 * @note    该类是线程安全的，支持多线程并发访问
 * @see     ImageSource, OpenSlideImage, TileCache, Patch
 */

#pragma once
#include "ImageSource.h"
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include "TileCache.hpp"
#include "Patch.h"

 /**
  * @class  MultiResolutionImage
  * @brief  多分辨率图像抽象基类，提供多层级图像数据的访问接口
  * @details 该类是DSV项目中图像处理的核心抽象层，继承自ImageSource，
  *          为不同的图像格式提供统一的接口。支持多分辨率层级、Z平面、
  *          多通道等高级功能，并提供线程安全的数据访问机制。
  *
  *          主要功能包括：
  *          - 多层级图像数据的存储和访问
  *          - 智能缓存机制，提高数据访问效率
  *          - 线程安全的数据读写操作
  *          - 图像元数据和属性管理
  *          - 支持不同数据类型的图像数据
  *
  * @note   该类是抽象基类，不能直接实例化，需要派生类实现具体功能
  * @example
  *          // 使用示例
  *          std::shared_ptr<MultiResolutionImage> img = std::make_shared<OpenSlideImage>();
  *          if (img->initialize("path/to/image.svs")) {
  *              auto dimensions = img->getDimensions();
  *              auto levels = img->getNumberOfLevels();
  *              // 获取图像数据
  *              unsigned char* data = new unsigned char[width * height * 3];
  *              img->getRawRegion<unsigned char>(0, 0, width, height, 0, data);
  *          }
  * @see     ImageSource, OpenSlideImage, TileCache
  */
class MultiResolutionImage : public ImageSource
{
public:
    /**
     * @brief   默认构造函数
     * @details 初始化多分辨率图像对象，设置默认状态和成员变量
     */
    MultiResolutionImage();

    /**
     * @brief   虚析构函数
     * @details 确保派生类对象能够正确释放资源，包括缓存和互斥锁
     */
    virtual ~MultiResolutionImage();

    /**
     * @brief   克隆图像对象
     * @details 创建当前图像对象的深拷贝，包括所有数据和状态
     *
     * @return  新创建的图像对象指针，调用者负责释放内存
     * @note    该函数是虚函数，派生类可以重写以实现特定的克隆逻辑
     */
    ImageSource* clone();

    /**
     * @brief   初始化图像文件
     * @details 根据文件路径初始化图像，调用具体的派生类实现
     *
     * @param   imagePath 图像文件路径
     * @return  true表示初始化成功，false表示失败
     * @note    该函数会调用initializeType虚函数进行具体的初始化
     * @see     initializeType
     */
    bool initialize(const std::string& imagePath);

    /**
     * @brief   初始化图像类型（纯虚函数）
     * @details 由派生类实现的图像类型特定初始化函数
     *
     * @param   imagePath 图像文件路径
     * @return  true表示初始化成功，false表示失败
     * @note    该函数是纯虚函数，必须由派生类实现
     * @see     OpenSlideImage::initializeType
     */
    virtual bool initializeType(const std::string& imagePath) = 0;

    /**
     * @brief   获取Z平面数量
     * @details 获取图像中Z平面（深度层）的数量
     *
     * @return  Z平面数量，通常为1（2D图像）
     */
    int getNumberOfZPlanes() const;

    /**
     * @brief   设置当前Z平面索引
     * @details 设置当前要访问的Z平面索引
     *
     * @param   zPlaneIndex Z平面索引
     * @note    索引必须在有效范围内（0到getNumberOfZPlanes()-1）
     */
    void setCurrentZPlaneIndex(const unsigned int& zPlaneIndex);

    /**
     * @brief   获取当前Z平面索引
     * @details 获取当前正在访问的Z平面索引
     *
     * @return  当前Z平面索引
     */
    unsigned int getCurrentZPlaneIndex() const;

    /**
     * @brief   获取图像属性
     * @details 获取图像的元数据属性，如放大倍数、扫描仪信息等
     *
     * @param   propertyName 属性名称
     * @return  属性值字符串，如果属性不存在则返回空字符串
     * @note    该函数是虚函数，派生类可以重写以提供特定的属性访问
     */
    virtual std::string getProperty(const std::string& propertyName) { return std::string(); };

    /**
     * @brief   获取缓存大小
     * @details 获取当前图像缓存的大小（字节数）
     *
     * @return  缓存大小（字节）
     */
    virtual const unsigned long long getCacheSize();

    /**
     * @brief   设置缓存大小
     * @details 设置图像缓存的最大大小，影响内存使用和性能
     *
     * @param   cacheSize 新的缓存大小（字节）
     * @note    较大的缓存可以提高访问速度，但会增加内存使用
     */
    virtual void setCacheSize(const unsigned long long cacheSize);

    /**
     * @brief   获取层级数量
     * @details 获取多分辨率图像中不同缩放级别的数量
     *
     * @return  层级数量
     * @note    层级0通常是最高分辨率，层级数越大分辨率越低
     */
    virtual const int getNumberOfLevels() const;

    /**
     * @brief   获取图像尺寸
     * @details 获取原始图像（最高分辨率）的尺寸信息
     *
     * @return  包含图像尺寸的向量，通常为[宽度, 高度]
     * @note    该函数继承自ImageSource
     */
    virtual const std::vector<unsigned long long> getDimensions() const;

    /**
     * @brief   获取指定层级的尺寸
     * @details 获取指定缩放级别的图像尺寸
     *
     * @param   level 层级索引
     * @return  指定层级的图像尺寸向量
     * @note    层级索引必须在有效范围内
     */
    virtual const std::vector<unsigned long long> getLevelDimensions(const unsigned int& level) const;

    /**
     * @brief   获取层级下采样因子
     * @details 获取指定层级相对于原始图像的下采样因子
     *
     * @param   level 层级索引
     * @return  下采样因子，表示该层级相对于原始图像的缩放比例
     * @note    下采样因子越大，图像分辨率越低
     */
    virtual const double getLevelDownsample(const unsigned int& level) const;

    /**
     * @brief   获取最佳层级
     * @details 根据目标下采样因子找到最合适的层级
     *
     * @param   downsample 目标下采样因子
     * @return  最佳层级的索引
     * @note    返回的层级的下采样因子最接近但不小于目标值
     */
    virtual const int getBestLevelForDownSample(const double& downsample) const;

    /**
     * @brief   获取通道最小值（纯虚函数）
     * @details 获取指定通道或所有通道的数据最小值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  数据最小值
     * @note    该函数是纯虚函数，必须由派生类实现
     */
    virtual double getMinValue(int channel = -1) = 0;

    /**
     * @brief   获取通道最大值（纯虚函数）
     * @details 获取指定通道或所有通道的数据最大值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  数据最大值
     * @note    该函数是纯虚函数，必须由派生类实现
     */
    virtual double getMaxValue(int channel = -1) = 0;

    /**
     * @brief   获取文件类型
     * @details 获取图像文件的格式类型
     *
     * @return  文件类型字符串
     */
    const std::string getFileType() const;

    /**
     * @brief   获取标签图像（纯虚函数）
     * @details 获取图像的标签或缩略图
     *
     * @return  QImage格式的标签图像
     * @note    该函数是纯虚函数，必须由派生类实现
     */
    virtual const QImage getLabel() = 0;

    /**
     * @brief   获取图像属性列表（纯虚函数）
     * @details 获取图像的所有元数据属性
     *
     * @return  属性信息向量
     * @note    该函数是纯虚函数，必须由派生类实现
     */
    virtual const std::vector<SlideColorManagement::PropertyInfo> getProperties() = 0;

    /**
     * @brief   获取图像块数据
     * @details 获取指定区域的图像数据，返回Patch对象
     *
     * @tparam  T 数据类型模板参数
     * @param   startX 起始X坐标
     * @param   startY 起始Y坐标
     * @param   width 区域宽度
     * @param   height 区域高度
     * @param   level 层级索引
     * @return  Patch<T>对象，包含图像数据和元信息
     * @note    该函数会自动处理内存分配和数据拷贝
     * @see     Patch, getRawRegion
     * @example
     *          // 使用示例
     *          auto patch = img->getPatch<unsigned char>(0, 0, 512, 512, 0);
     *          auto data = patch.getData();
     *          auto dimensions = patch.getDimensions();
     */
    template <typename T>
    Patch<T> getPatch(const long long& startX, const long long& startY, const unsigned long long& width,
        const unsigned long long& height, const unsigned int& level)
    {
        // 设置图像块维度
        std::vector<unsigned long long> dims(3, 0);
        dims[0] = width;
        dims[1] = height;
        dims[2] = _samplesPerPixel;

        // 分配内存并获取原始数据
        T* data = new T[width * height * _samplesPerPixel];
        getRawRegion<T>(startX, startY, width, height, level, data);

        // 计算图像块的物理间距
        std::vector<double> patchSpacing(_spacing.size(), 1.0);
        double levelDownsample = this->getLevelDownsample(level);
        for (unsigned int i = 0; i < _spacing.size(); ++i) {
            patchSpacing[i] = _spacing[i] * levelDownsample;
        }

        // 获取通道的最小值和最大值
        std::vector<double> minValues, maxValues;
        for (unsigned int i = 0; i < this->getSamplesPerPixel(); ++i) {
            minValues.push_back(this->getMinValue(i));
            maxValues.push_back(this->getMaxValue(i));
        }

        // 创建并返回Patch对象
        Patch<T> patch = Patch<T>(dims, this->getColorType(), data, true, minValues, maxValues);
        patch.setSpacing(patchSpacing);
        return patch;
    }

    /**
     * @brief   获取原始图像区域数据
     * @details 获取指定区域的原始图像数据，支持多种数据类型。
     *          该函数会根据图像的实际数据类型进行相应的转换，
     *          确保返回的数据格式正确。注意，对于int32 ARGB数据，
     *          OpenSlide使用ARGB颜色顺序，而Windows通常使用BGRA。
     *
     * @tparam  T 目标数据类型模板参数
     * @param   startX 起始X坐标
     * @param   startY 起始Y坐标
     * @param   width 区域宽度
     * @param   height 区域高度
     * @param   level 层级索引
     * @param   data 输出数据指针，调用者负责分配足够的内存
     * @note    调用者必须确保data指针指向足够大的内存空间
     * @see     readDataFromImage
     * @example
     *          // 使用示例
     *          unsigned char* data = new unsigned char[width * height * 3];
     *          img->getRawRegion<unsigned char>(0, 0, width, height, 0, data);
     *          // 使用数据...
     *          delete[] data;
     */
    template <typename T>
    void getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
        const unsigned long long& height, const unsigned int& level, T*& data) {
        // 检查层级索引是否有效
        if (level >= getNumberOfLevels()) {
            return;
        }

        unsigned int nrSamples = getSamplesPerPixel();

        // 根据图像数据类型进行相应的处理
        if (this->getDataType() == SlideColorManagement::DataType::Float) {
            float* temp = (float*)readDataFromImage(startX, startY, width, height, level);
            std::copy(temp, temp + width * height * nrSamples, data);
            delete[] temp;
        }
        else if (this->getDataType() == SlideColorManagement::DataType::UChar) {
            unsigned char* temp = (unsigned char*)readDataFromImage(startX, startY, width, height, level);
            std::copy(temp, temp + width * height * nrSamples, data);
            delete[] temp;
        }
        else if (this->getDataType() == SlideColorManagement::DataType::UInt16) {
            unsigned short* temp = (unsigned short*)readDataFromImage(startX, startY, width, height, level);
            std::copy(temp, temp + width * height * nrSamples, data);
            delete[] temp;
        }
        else if (this->getDataType() == SlideColorManagement::DataType::UInt32) {
            unsigned int* temp = (unsigned int*)readDataFromImage(startX, startY, width, height, level);
            std::copy(temp, temp + width * height * nrSamples, data);
            delete[] temp;
        }
    }

protected:
    // 线程安全相关成员
    /**
     * @brief 文件打开/关闭互斥锁
     * @details unique_ptr是独占所有权的，同一时间只能有一个unique_ptr
     *          指向std::shared_mutex，确保多线程同时只能以只读方式访问，
     *          写操作需要独占访问
     */
    std::unique_ptr<std::shared_mutex> _openCloseMutex;

    /**
     * @brief 缓存互斥锁
     * @details 指向mutex的unique_ptr，确保缓存操作的线程安全
     */
    std::unique_ptr<std::mutex> m_cacheMutex;

    /**
     * @brief 缓存对象
     * @details 指向任意类型的void*的shared_ptr，实现一个通用的缓存接口，
     *          支持多线程共享访问缓存对象
     */
    std::shared_ptr<void> m_cache;

    // 图像数据相关成员
    /** @brief 各层级的图像尺寸，每个元素包含[宽度, 高度] */
    std::vector<std::vector<unsigned long long> > _levelDimensions;

    /** @brief 层级数量 */
    unsigned int _numberOfLevels;

    /** @brief Z平面数量 */
    unsigned int m_numberOfZPlanes;

    /** @brief 当前Z平面索引 */
    unsigned int m_currentZPlaneIndex;

    /** @brief 图像属性信息列表 */
    std::vector<SlideColorManagement::PropertyInfo> _properties;

    /** @brief 缓存大小（字节） */
    unsigned long long m_cacheSize;

    /** @brief 文件类型字符串 */
    std::string _fileType;

    /** @brief 文件路径 */
    std::string m_filePath;

    /**
     * @brief   清理资源（虚函数）
     * @details 清理图像相关的资源，如文件句柄、缓存等
     * @note    该函数是虚函数，派生类可以重写以实现特定的清理逻辑
     */
    virtual void cleanup();

    /**
     * @brief   从图像读取数据（纯虚函数）
     * @details 从图像中读取指定区域的原始数据
     *
     * @param   startX 起始X坐标
     * @param   startY 起始Y坐标
     * @param   width 区域宽度
     * @param   height 区域高度
     * @param   level 层级索引
     * @return  指向原始数据的void指针，调用者负责释放内存
     * @note    该函数是纯虚函数，必须由派生类实现
     */
    virtual void* readDataFromImage(const long long& startX, const long long& startY, const unsigned long long& width,
        const unsigned long long& height, const unsigned int& level) = 0;

    /**
     * @brief   创建缓存（模板函数）
     * @details 根据指定的数据类型创建相应的缓存对象
     *
     * @tparam  T 缓存数据类型
     * @note    只有当图像有效时才会创建缓存
     * @see     TileCache
     */
    template <typename T> void createCache() {
        if (_isValid) {
            m_cache.reset(new TileCache<T>(m_cacheSize));
        }
    }
};

// 模板特化声明
/**
 * @brief   getRawRegion函数的unsigned char特化版本
 * @details 为unsigned char类型提供优化的实现
 */
template <> void MultiResolutionImage::getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
    const unsigned long long& height, const unsigned int& level, unsigned char*& data);

/**
 * @brief   getRawRegion函数的unsigned short特化版本
 * @details 为unsigned short类型提供优化的实现
 */
template <> void MultiResolutionImage::getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
    const unsigned long long& height, const unsigned int& level, unsigned short*& data);

/**
 * @brief   getRawRegion函数的unsigned int特化版本
 * @details 为unsigned int类型提供优化的实现
 */
template <> void MultiResolutionImage::getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
    const unsigned long long& height, const unsigned int& level, unsigned int*& data);

/**
 * @brief   getRawRegion函数的float特化版本
 * @details 为float类型提供优化的实现
 */
template <> void MultiResolutionImage::getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
    const unsigned long long& height, const unsigned int& level, float*& data);