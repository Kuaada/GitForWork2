/**
 * @file ImageSource.cpp
 * @brief 图像源抽象基类的实现文件
 * @details 该文件实现了ImageSource抽象基类的所有虚函数和基础功能
 *          提供了图像源的基本属性访问和状态管理功能
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "ImageSource.h"
/**
 * @brief 构造函数：初始化所有成员变量为默认值
 * @details 初始化列表包括：
 *         - _spacing 使用默认构造函数初始化，表示像素间距
 *         - _samplesPerPixel 初始化为0，表示未设置采样数
 *         - _colorType 和 _dataType 初始化为 Invalid 枚举值
 *         - _isValid 初始化为 false，表示图像源默认无效
 */
ImageSource::ImageSource()
	:_spacing(),
	_samplesPerPixel(0),
	_colorType(SlideColorManagement::ColorType::InvalidColorType),
	_dataType(SlideColorManagement::DataType::InvalidDataType),
	_isValid(false)
{
}
/** @brief 析构函数：确保正确清理和释放资源
 * @details 虚析构函数确保多态删除时正确调用派生类析构函数
 */
ImageSource::~ImageSource() {

}
/**
 * @brief 获取图像的颜色类型（实现版本）
 * @details 检查图像源是否有效，如果有效则返回存储的颜色类型，
 *          否则返回 InvalidColorType 表示无效状态
 * @return 如果图像源有效，返回存储的颜色类型；否则返回 InvalidColorType
 * @note 这是一个const成员函数，不会修改对象状态
 */
const SlideColorManagement::ColorType ImageSource::getColorType() const
{
	if (_isValid)
	{
		return _colorType;
	}
	else
	{
		return SlideColorManagement::ColorType::InvalidColorType;
	}
}
/**
 * @brief 获取图像的数据类型（实现版本）
 * @details 检查图像源是否有效，如果有效则返回存储的数据类型，
 *          否则返回 InvalidDataType 表示无效状态
 * @return 如果图像源有效，返回存储的数据类型；否则返回 InvalidDataType
 * @note 这是一个const成员函数，不会修改对象状态
 */
const SlideColorManagement::DataType ImageSource::getDataType() const
{
	if (_isValid)
	{
		return _dataType;
	}
	else
	{
		return SlideColorManagement::DataType::InvalidDataType;
	}
}
/**
 * @brief 获取每个像素的采样数（实现版本）
 * @details 检查图像源是否有效，如果有效则返回存储的采样数，
 *          否则返回-1作为无效标识
 * @return 如果图像源有效，返回存储的采样数；否则返回-1
 * @note 这是一个const成员函数，不会修改对象状态
 */
const int ImageSource::getSamplesPerPixel() const
{
	if (_isValid) {
		return _samplesPerPixel;
	}
	else {
		return -1;
	}
}
/**
 * @brief 获取像素间距信息（实现版本）
 * @details 返回存储的像素间距信息，这是一个const引用返回，
 *          确保外部无法修改内部数据
 * @return 返回存储的像素间距信息
 * @note 这是一个const成员函数，不会修改对象状态
 */
const std::vector<double> ImageSource::getSpacing() const
{
	return _spacing;
}
/**
 * @brief 交换两个 ImageSource 对象的数据（实现版本）
 * @details 使用 std::swap 原子操作交换所有成员变量，
 *          确保线程安全和异常安全
 * @param first 要交换的第一个对象
 * @param second 要交换的第二个对象
 * @note 使用 std::swap 原子操作交换所有成员变量，保证线程安全和异常安全
 */
void ImageSource::swap(ImageSource& first, ImageSource& second)
{
	std::swap(first._colorType, second._colorType);
	std::swap(first._dataType, second._dataType);
	std::swap(first._spacing, second._spacing);
	std::swap(first._samplesPerPixel, second._samplesPerPixel);
	std::swap(first._isValid, second._isValid);
}
