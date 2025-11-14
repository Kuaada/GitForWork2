/**
 * @file    ImageSource.h
 * @brief   图像数据源抽象基类，定义图像数据获取、有效性校验及克隆等接口
 * @author  [JianZhang] ([])
 * @date    2025-05-19
 * @version 1.0.0
 * @details 该类定义了图像数据源的基本接口，包括图像元数据获取、
 *          数据有效性检查、对象克隆等功能。所有具体的图像数据源
 *          实现类都应该继承自此类，确保接口的一致性和可扩展性。
 *
 * @note    该类是抽象基类，不能直接实例化
 * @see     MultiResolutionImage, OpenSlideImage
 */

#pragma once
#include <vector>
#include "SlideColorManagement.h"

 /**
  * @class  ImageSource
  * @brief  图像数据源抽象基类，提供图像元数据获取、有效性校验及克隆等接口
  * @details 该类定义了图像数据源的核心功能，包括：
  *          - 图像尺寸和元数据获取
  *          - 颜色类型和数据类型管理
  *          - 数据有效性检查
  *          - 对象克隆功能
  *          使用此类可以确保不同图像格式实现的一致性，
  *          并支持多态操作和内存管理。
  *
  * @note   该类是线程安全的，支持多线程并发访问
  * @example
  *          // 使用示例
  *          ImageSource* source = new OpenSlideImage();
  *          if (source->valid()) {
  *              auto dimensions = source->getDimensions();
  *              auto colorType = source->getColorType();
  *          }
  * @see     MultiResolutionImage, SlideColorManagement
  */
class ImageSource
{
public:
    /**
     * @brief   默认构造函数
     * @details 初始化图像源对象，设置默认状态
     */
    ImageSource();

    /**
     * @brief   虚析构函数
     * @details 确保派生类对象能够正确释放资源
     */
    virtual ~ImageSource();

    /**
     * @brief   检查图像数据源是否有效
     * @details 检查图像数据是否合法可用，包括文件完整性、
     *          数据格式正确性等
     *
     * @return  true表示数据源有效，false表示无效
     * @note    该函数是线程安全的
     */
    virtual bool valid() const { return _isValid; }

    /**
     * @brief   获取图像尺寸信息
     * @details 获取图像在各个维度上的尺寸信息，如宽度、高度等。
     *          返回的向量按特定顺序排列，具体顺序取决于图像类型。
     *
     * @return  包含图像尺寸信息的向量，无法获取时返回空向量
     * @note    该函数是纯虚函数，必须由派生类实现
     * @see     getLevelDimensions
     */
    virtual const std::vector<unsigned long long> getDimensions() const = 0;

    /**
     * @brief   获取图像颜色类型
     * @details 获取图像的颜色表示类型，如灰度、RGB、RGBA等
     *
     * @return  颜色类型枚举值，无效时返回InvalidColorType
     * @see     SlideColorManagement::ColorType
     */
    virtual const SlideColorManagement::ColorType getColorType() const;

    /**
     * @brief   获取图像数据类型
     * @details 获取图像像素数据的存储类型，如无符号字符、16位整数等
     *
     * @return  数据类型枚举值，无效时返回InvalidDataType
     * @see     SlideColorManagement::DataType
     */
    virtual const SlideColorManagement::DataType getDataType() const;

    /**
     * @brief   获取每像素的采样数
     * @details 获取每个像素包含的采样数量，如RGB图像为3，灰度图像为1
     *
     * @return  每像素的采样数量
     */
    virtual const int getSamplesPerPixel() const;

    /**
     * @brief   获取图像间距信息
     * @details 获取图像在X、Y、Z轴方向上的物理间距（单位：微米）
     *
     * @return  包含间距信息的向量
     */
    virtual const std::vector<double> getSpacing() const;

    /**
     * @brief   获取图像数据的最小值
     * @details 获取指定通道或所有通道的数据最小值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  数据最小值
     * @note    该函数是纯虚函数，必须由派生类实现
     */
    virtual double getMinValue(int channel = -1) = 0;

    /**
     * @brief   获取图像数据的最大值
     * @details 获取指定通道或所有通道的数据最大值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  数据最大值
     * @note    该函数是纯虚函数，必须由派生类实现
     */
    virtual double getMaxValue(int channel = -1) = 0;

    /**
     * @brief   克隆图像源对象
     * @details 创建当前图像源对象的深拷贝，包括所有数据和状态
     *
     * @return  新创建的图像源对象指针，调用者负责释放内存
     * @note    该函数是纯虚函数，必须由派生类实现
     */
    virtual ImageSource* clone() = 0;

    /**
     * @brief   交换两个图像源对象的内容
     * @details 使用std::swap实现两个对象内容的交换
     *
     * @param   first  第一个图像源对象
     * @param   second 第二个图像源对象
     * @note    该函数是静态函数，不修改当前对象
     */
    void swap(ImageSource& first, ImageSource& second);

protected:
    /** @brief 数据源有效性标识，true表示数据合法可用 */
    bool _isValid;

    /**
     * @brief 物理间距信息，包含X、Y、Z轴方向的间距（单位：微米）
     * @note  用于计算图像的物理尺寸和比例
     */
    std::vector<double> _spacing;

    /**
     * @brief 每像素的采样数量，如RGB图像为3，灰度图像为1
     * @note  决定了图像数据的存储格式和访问方式
     */
    unsigned int _samplesPerPixel;

    /**
     * @brief 图像颜色类型枚举值，定义在SlideColorManagement模块中
     * @see   SlideColorManagement::ColorType
     */
    SlideColorManagement::ColorType _colorType;

    /**
     * @brief 图像数据类型枚举值，定义在SlideColorManagement模块中
     * @see   SlideColorManagement::DataType
     */
    SlideColorManagement::DataType _dataType;
};