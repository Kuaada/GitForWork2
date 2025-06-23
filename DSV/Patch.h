/**
 * @file    Patch.h
 * @brief   图像块模板类，封装图像数据片段和元信息
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的图像块数据结构，包括：
 *          - 模板化图像数据容器
 *          - 多维数组支持和步长计算
 *          - 算术运算符重载
 *          - 内存管理和所有权控制
 *          - 图像元数据管理（最小值、最大值、间距等）
 *          - 支持多种颜色类型和数据类型
 *          该类是DSV项目中图像数据处理的核心数据结构，
 *          用于封装从多分辨率图像中提取的图像块数据。
 *
 * @note    该类是模板类，支持任意数据类型的图像块
 * @see     ImageSource, MultiResolutionImage, SlideColorManagement
 */

#pragma once
#include <vector>
#include "ImageSource.h"

 /**
  * @class  Patch
  * @brief  图像块模板类，封装图像数据片段和元信息
  * @details 该类实现了高效的图像块数据结构，继承自ImageSource，
  *          提供多维数组支持、内存管理、算术运算等功能。
  *          主要用于封装从多分辨率图像中提取的图像块数据，
  *          支持不同的颜色类型和数据类型。
  *
  *          主要特性：
  *          - 模板化设计，支持任意数据类型的图像数据
  *          - 多维数组支持，自动计算步长
  *          - 算术运算符重载，支持图像数据的数学运算
  *          - 内存所有权控制，支持共享和独占数据
  *          - 图像元数据管理（最小值、最大值、物理间距等）
  *          - 高效的随机访问和批量操作
  *
  * @tparam T 图像数据类型模板参数
  * @note   该类支持深拷贝和浅拷贝，通过ownData参数控制
  * @example
  *          // 使用示例
  *          std::vector<unsigned long long> dims = {512, 512, 3}; // RGB图像
  *          Patch<unsigned char> patch(dims, SlideColorManagement::ColorType::RGB);
  *          patch.fill(128); // 填充灰度值
  *
  *          // 算术运算
  *          Patch<unsigned char> result = patch * 2;
  * @see     ImageSource, MultiResolutionImage
  */
template<typename T>
class Patch : public ImageSource {

private:
    /** @brief 图像数据缓冲区指针 */
    T* _buffer;

    /** @brief 缓冲区大小（元素个数） */
    unsigned long long _bufferSize;

    /** @brief 数据所有权标志，true表示拥有数据，false表示共享数据 */
    bool _ownData;

    /** @brief 图像维度数组，如[宽度, 高度, 通道数] */
    std::vector<unsigned long long> _dimensions;

    /** @brief 步长数组，用于多维数组的索引计算 */
    std::vector<unsigned long long> _strides;

    /** @brief WSI图像各通道的最小值 */
    std::vector<double> _wsiMinValues;

    /** @brief WSI图像各通道的最大值 */
    std::vector<double> _wsiMaxValues;

    /**
     * @brief   计算步长数组
     * @details 根据维度数组计算多维数组的步长，用于高效的索引计算
     * @note    该函数在构造函数中自动调用
     */
    void calculateStrides();

    /**
     * @brief   交换两个Patch对象的内容
     * @details 用于实现拷贝赋值操作符的异常安全版本
     *
     * @param   first 第一个Patch对象
     * @param   second 第二个Patch对象
     * @note    该函数是私有辅助函数
     */
    void swap(Patch<T>& first, Patch<T>& second);

public:
    /**
     * @brief   默认构造函数
     * @details 创建空的图像块对象
     */
    Patch();

    /**
     * @brief   析构函数
     * @details 释放图像数据缓冲区，如果拥有数据所有权
     */
    ~Patch();

    /**
     * @brief   拷贝构造函数
     * @details 创建当前对象的深拷贝
     *
     * @param   rhs 要拷贝的Patch对象
     * @note    新对象拥有独立的数据副本
     */
    Patch(const Patch& rhs);

    /**
     * @brief   拷贝赋值操作符
     * @details 使用拷贝交换技术实现异常安全的赋值操作
     *
     * @param   rhs 要赋值的Patch对象
     * @return  当前对象的引用
     * @note    使用拷贝交换技术确保异常安全
     */
    Patch& operator=(const Patch rhs);

    /**
     * @brief   带参数的构造函数
     * @details 创建指定维度和颜色类型的图像块对象
     *
     * @param   dimensions 图像维度数组，如[宽度, 高度, 通道数]
     * @param   ctype 颜色类型，默认为单色
     * @param   data 外部数据指针，默认为NULL
     * @param   ownData 数据所有权标志，默认为true
     * @param   wsiMinValues WSI图像各通道的最小值
     * @param   wsiMaxValues WSI图像各通道的最大值
     * @note    如果data为NULL且ownData为true，会自动分配内存
     * @see     SlideColorManagement::ColorType
     */
    Patch(const std::vector<unsigned long long>& dimensions, const SlideColorManagement::ColorType& ctype = SlideColorManagement::ColorType::Monochrome, T* data = NULL, bool ownData = true, std::vector<double> wsiMinValues = std::vector<double>(), std::vector<double> wsiMaxValues = std::vector<double>());

    /**
     * @brief   克隆图像块对象
     * @details 创建当前对象的深拷贝
     *
     * @return  新创建的ImageSource对象指针
     * @note    调用者负责释放返回的对象
     */
    ImageSource* clone();

    // 算术运算符重载
    /**
     * @brief   乘法运算符（标量）
     * @details 将图像块的所有像素值与标量相乘
     *
     * @param   val 标量值
     * @return  新的Patch对象，包含乘法结果
     * @note    原对象不会被修改
     */
    Patch<T> operator*(const T& val);

    /**
     * @brief   乘法赋值运算符（标量）
     * @details 将图像块的所有像素值与标量相乘，并修改原对象
     *
     * @param   val 标量值
     * @return  当前对象的引用
     */
    Patch<T>& operator*=(const T& val);

    /**
     * @brief   除法运算符（标量）
     * @details 将图像块的所有像素值与标量相除
     *
     * @param   val 标量值
     * @return  新的Patch对象，包含除法结果
     * @note    原对象不会被修改
     */
    Patch<T> operator/(const T& val);

    /**
     * @brief   除法赋值运算符（标量）
     * @details 将图像块的所有像素值与标量相除，并修改原对象
     *
     * @param   val 标量值
     * @return  当前对象的引用
     */
    Patch<T>& operator/=(const T& val);

    /**
     * @brief   加法运算符（标量）
     * @details 将图像块的所有像素值与标量相加
     *
     * @param   val 标量值
     * @return  新的Patch对象，包含加法结果
     * @note    原对象不会被修改
     */
    Patch<T> operator+(const T& val);

    /**
     * @brief   加法赋值运算符（标量）
     * @details 将图像块的所有像素值与标量相加，并修改原对象
     *
     * @param   val 标量值
     * @return  当前对象的引用
     */
    Patch<T>& operator+=(const T& val);

    /**
     * @brief   减法运算符（标量）
     * @details 将图像块的所有像素值与标量相减
     *
     * @param   val 标量值
     * @return  新的Patch对象，包含减法结果
     * @note    原对象不会被修改
     */
    Patch<T> operator-(const T& val);

    /**
     * @brief   减法赋值运算符（标量）
     * @details 将图像块的所有像素值与标量相减，并修改原对象
     *
     * @param   val 标量值
     * @return  当前对象的引用
     */
    Patch<T>& operator-=(const T& val);

    /**
     * @brief   获取常量数据指针
     * @details 返回指向图像数据的常量指针
     *
     * @return  指向图像数据的常量指针
     * @note    返回的指针指向连续的内存区域
     */
    const T* getPointer() const;

    /**
     * @brief   获取数据指针
     * @details 返回指向图像数据的指针
     *
     * @return  指向图像数据的指针
     * @note    返回的指针指向连续的内存区域
     */
    T* getPointer();

    /**
     * @brief   获取步长数组
     * @details 返回多维数组的步长数组
     *
     * @return  步长数组
     * @note    步长用于计算多维索引对应的线性索引
     */
    std::vector<unsigned long long> getStrides();

    /**
     * @brief   获取WSI图像通道最小值
     * @details 获取WSI图像中指定通道的最小值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  通道最小值
     * @note    该值来自原始WSI图像的元数据
     */
    double getWSIMinValue(int channel = -1) const;

    /**
     * @brief   获取WSI图像通道最大值
     * @details 获取WSI图像中指定通道的最大值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  通道最大值
     * @note    该值来自原始WSI图像的元数据
     */
    double getWSIMaxValue(int channel = -1) const;

    /**
     * @brief   获取当前图像块通道最小值（非const版本）
     * @details 计算当前图像块中指定通道的最小值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  通道最小值
     * @note    该函数会遍历所有数据计算最小值
     */
    double getMinValue(int channel = -1);

    /**
     * @brief   获取当前图像块通道最大值（非const版本）
     * @details 计算当前图像块中指定通道的最大值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  通道最大值
     * @note    该函数会遍历所有数据计算最大值
     */
    double getMaxValue(int channel = -1);

    /**
     * @brief   获取当前图像块通道最小值（const版本）
     * @details 计算当前图像块中指定通道的最小值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  通道最小值
     * @note    该函数会遍历所有数据计算最小值
     */
    double getMinValue(int channel = -1) const;

    /**
     * @brief   获取当前图像块通道最大值（const版本）
     * @details 计算当前图像块中指定通道的最大值
     *
     * @param   channel 通道索引，-1表示所有通道
     * @return  通道最大值
     * @note    该函数会遍历所有数据计算最大值
     */
    double getMaxValue(int channel = -1) const;

    /**
     * @brief   获取指定索引位置的像素值
     * @details 根据多维索引获取对应位置的像素值
     *
     * @param   index 多维索引数组
     * @return  指定位置的像素值
     * @note    索引数组的大小必须与维度数组匹配
     * @example
     *          // 使用示例
     *          std::vector<unsigned long long> pos = {100, 200, 0}; // x=100, y=200, channel=0
     *          T value = patch.getValue(pos);
     */
    T getValue(const std::vector<unsigned long long>& index) const;

    /**
     * @brief   设置指定索引位置的像素值
     * @details 根据多维索引设置对应位置的像素值
     *
     * @param   index 多维索引数组
     * @param   value 要设置的像素值
     * @note    索引数组的大小必须与维度数组匹配
     * @example
     *          // 使用示例
     *          std::vector<unsigned long long> pos = {100, 200, 0}; // x=100, y=200, channel=0
     *          patch.setValue(pos, 255);
     */
    void setValue(const std::vector<unsigned long long>& index, const T& value);

    /**
     * @brief   填充图像块
     * @details 将图像块的所有像素值设置为指定值
     *
     * @param   value 填充值
     * @note    该操作会修改所有像素值
     * @example
     *          // 使用示例
     *          patch.fill(0); // 填充黑色
     */
    void fill(const T& value);

    /**
     * @brief   设置物理间距
     * @details 设置图像块的物理间距信息
     *
     * @param   spacing 物理间距数组
     * @note    物理间距用于计算图像的实际物理尺寸
     */
    void setSpacing(const std::vector<double>& spacing);

    /**
     * @brief   检查图像块是否为空
     * @details 检查图像块是否包含数据
     *
     * @return  true表示图像块为空，false表示包含数据
     */
    bool empty();

    /**
     * @brief   获取图像维度
     * @details 返回图像的维度数组
     *
     * @return  维度数组
     * @note    该函数继承自ImageSource
     */
    const std::vector<unsigned long long> getDimensions() const;

    /**
     * @brief   获取每像素采样数
     * @details 返回每像素的采样数（通道数）
     *
     * @return  每像素采样数
     * @note    该值等于维度数组的最后一个元素
     */
    const int getSamplesPerPixel() const;

    /**
     * @brief   获取缓冲区大小
     * @details 返回图像数据缓冲区的大小（元素个数）
     *
     * @return  缓冲区大小
     */
    const unsigned long long getBufferSize() const;

    /**
     * @brief   获取数据类型
     * @details 返回图像数据的类型
     *
     * @return  数据类型枚举值
     * @note    该函数通过模板特化实现
     * @see     SlideColorManagement::DataType
     */
    const SlideColorManagement::DataType getDataType() const;
};

// 包含实现文件
#include "Patch.hpp"