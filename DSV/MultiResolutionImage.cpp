/**
 * @file MultiResolutionImage.cpp
 * @brief 多分辨率图像基类实现文件
 * @details 该文件实现了MultiResolutionImage基类的所有功能，包括：
 *          - 多分辨率图像数据的读取和转换
 *          - 缓存管理
 *          - 层级管理
 *          - 数据类型转换
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "MultiResolutionImage.h"
#include <cmath>

/**
 * @brief 获取原始图像区域数据（float类型特化）
 * @details 从指定层级读取图像数据并转换为float类型
 * @param startX 起始X坐标
 * @param startY 起始Y坐标
 * @param width 区域宽度
 * @param height 区域高度
 * @param level 图像层级
 * @param data 输出数据指针（float类型）
 * @note 该函数会根据源数据类型进行相应的类型转换
 */
template <> void MultiResolutionImage::getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
	const unsigned long long& height, const unsigned int& level, float*& data) {
	if (level >= getNumberOfLevels()) {
		return;
	}
	unsigned int nrSamples = getSamplesPerPixel();
	if (this->getDataType() == SlideColorManagement::DataType::Float) {
		delete[] data;
		data = (float*)readDataFromImage(startX, startY, width, height, level);
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
		std::transform(temp, temp + width * height * nrSamples, data, [](unsigned int a) { return static_cast<float>(a); });
		delete[] temp;
	}
}

/**
 * @brief 获取原始图像区域数据（unsigned char类型特化）
 * @details 从指定层级读取图像数据并转换为unsigned char类型
 * @param startX 起始X坐标
 * @param startY 起始Y坐标
 * @param width 区域宽度
 * @param height 区域高度
 * @param level 图像层级
 * @param data 输出数据指针（unsigned char类型）
 * @note 该函数会根据源数据类型进行相应的类型转换
 */
template <> void MultiResolutionImage::getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
	const unsigned long long& height, const unsigned int& level, unsigned char*& data) {
	if (level >= getNumberOfLevels()) {
		return;
	}
	unsigned int nrSamples = getSamplesPerPixel();
	if (this->getDataType() == SlideColorManagement::DataType::Float) {
		float* temp = (float*)readDataFromImage(startX, startY, width, height, level);
		std::transform(temp, temp + width * height * nrSamples, data, [](float a) { return static_cast<unsigned char>(a); });
		delete[] temp;
	}
	else if (this->getDataType() == SlideColorManagement::DataType::UChar) {
		delete[] data;
		data = (unsigned char*)readDataFromImage(startX, startY, width, height, level);
	}
	else if (this->getDataType() == SlideColorManagement::DataType::UInt16) {
		unsigned short* temp = (unsigned short*)readDataFromImage(startX, startY, width, height, level);
		std::transform(temp, temp + width * height * nrSamples, data, [](unsigned short a) { return static_cast<unsigned char>(a); });
		delete[] temp;
	}
	else if (this->getDataType() == SlideColorManagement::DataType::UInt32) {
		unsigned int* temp = (unsigned int*)readDataFromImage(startX, startY, width, height, level);
		std::copy(temp, temp + width * height * nrSamples, data);
		delete[] temp;
	}
}

/**
 * @brief 获取原始图像区域数据（unsigned short类型特化）
 * @details 从指定层级读取图像数据并转换为unsigned short类型
 * @param startX 起始X坐标
 * @param startY 起始Y坐标
 * @param width 区域宽度
 * @param height 区域高度
 * @param level 图像层级
 * @param data 输出数据指针（unsigned short类型）
 * @note 该函数会根据源数据类型进行相应的类型转换
 */
template <> void MultiResolutionImage::getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
	const unsigned long long& height, const unsigned int& level, unsigned short*& data) {
	if (level >= getNumberOfLevels()) {
		return;
	}
	unsigned int nrSamples = getSamplesPerPixel();
	if (this->getDataType() == SlideColorManagement::DataType::Float) {
		float* temp = (float*)readDataFromImage(startX, startY, width, height, level);
		std::transform(temp, temp + width * height * nrSamples, data, [](float a) { return static_cast<unsigned short>(a); });
		delete[] temp;
	}
	else if (this->getDataType() == SlideColorManagement::DataType::UChar) {
		unsigned char* temp = (unsigned char*)readDataFromImage(startX, startY, width, height, level);
		std::copy(temp, temp + width * height * nrSamples, data);
		delete[] temp;
	}
	else if (this->getDataType() == SlideColorManagement::DataType::UInt16) {
		delete[] data;
		data = (unsigned short*)readDataFromImage(startX, startY, width, height, level);
	}
	else if (this->getDataType() == SlideColorManagement::DataType::UInt32) {
		unsigned int* temp = (unsigned int*)readDataFromImage(startX, startY, width, height, level);
		std::copy(temp, temp + width * height * nrSamples, data);
		delete[] temp;
	}
}

/**
 * @brief 获取原始图像区域数据（unsigned int类型特化）
 * @details 从指定层级读取图像数据并转换为unsigned int类型
 * @param startX 起始X坐标
 * @param startY 起始Y坐标
 * @param width 区域宽度
 * @param height 区域高度
 * @param level 图像层级
 * @param data 输出数据指针（unsigned int类型）
 * @note 该函数会根据源数据类型进行相应的类型转换
 */
template <> void MultiResolutionImage::getRawRegion(const long long& startX, const long long& startY, const unsigned long long& width,
	const unsigned long long& height, const unsigned int& level, unsigned int*& data) {
	if (level >= getNumberOfLevels()) {
		return;
	}
	unsigned int nrSamples = getSamplesPerPixel();
	if (this->getDataType() == SlideColorManagement::DataType::Float) {
		float* temp = (float*)readDataFromImage(startX, startY, width, height, level);
		std::transform(temp, temp + width * height * nrSamples, data, [](float a) { return static_cast<unsigned int>(a); });
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
		delete[] data;
		data = (unsigned int*)readDataFromImage(startX, startY, width, height, level);
	}
}

/**
 * @brief 构造函数：初始化多分辨率图像对象
 * @details 初始化所有成员变量，包括：
 *          - 继承自ImageSource的成员
 *          - 缓存相关成员
 *          - 层级维度信息
 *          - 文件路径和类型
 *          - Z平面信息
 *          - 互斥锁
 */
MultiResolutionImage::MultiResolutionImage()
	:ImageSource(),
	m_cacheSize(0),
	_levelDimensions(),
	_numberOfLevels(),
	m_filePath(),
	 _fileType(),
	m_numberOfZPlanes(1),
	m_currentZPlaneIndex(0),
	m_cache()
{
	m_cacheMutex.reset(new std::mutex());//reset会先释放当前指针的对象（如果有的话），然后让指针指向一个新的对象
	_openCloseMutex.reset(new std::shared_mutex());
}

/**
 * @brief 析构函数：清理资源
 * @details 获取独占锁后调用cleanup函数清理所有资源
 */
MultiResolutionImage::~MultiResolutionImage()
{
	std::unique_lock<std::shared_mutex> l(*_openCloseMutex);
	cleanup();
}

/**
 * @brief 克隆图像源对象
 * @return 返回nullptr，基类不支持克隆
 * @note 派生类需要重写此函数以提供克隆功能
 */
ImageSource* MultiResolutionImage::clone()
{
	return nullptr;
}

/**
 * @brief 初始化多分辨率图像
 * @param imagePath 图像文件路径
 * @return 初始化是否成功
 * @details 设置文件路径并调用initializeType进行具体初始化
 */
bool MultiResolutionImage::initialize(const std::string& imagePath)
{
	m_filePath = imagePath;
	return initializeType(imagePath);
}

/**
 * @brief 获取Z平面数量
 * @return Z平面数量
 * @details 返回图像包含的Z平面数量，默认为1
 */
int MultiResolutionImage::getNumberOfZPlanes() const
{
	return m_numberOfZPlanes;
}

/**
 * @brief 设置当前Z平面索引
 * @param zPlaneIndex 要设置的Z平面索引
 * @details 获取独占锁确保线程安全，使用三元运算符确保索引在有效范围内
 *          如果索引超出范围，则设置为最大有效索引
 */
void MultiResolutionImage::setCurrentZPlaneIndex(const unsigned int& zPlaneIndex)
{
	std::unique_lock<std::shared_mutex> l(*_openCloseMutex);
	//获取独占锁，l会尝试获取_openCloseMutex所指向的std::shared_mutex对象的独占锁
	//线程同步确保在同一时间只有一个线程可以获取这个独占锁，从而避免对共享资源的读写冲突
	//三元运算符  A?B:C      如果A为真执行B，否则执行C
	zPlaneIndex < m_numberOfZPlanes ? m_currentZPlaneIndex = zPlaneIndex : m_currentZPlaneIndex = m_numberOfZPlanes - 1;
}                                                                                                 

/**
 * @brief 获取当前Z平面索引
 * @return 当前Z平面索引
 */
unsigned int MultiResolutionImage::getCurrentZPlaneIndex() const
{
	return m_currentZPlaneIndex;
}

/**
 * @brief 获取缓存大小
 * @return 当前缓存大小（字节数）
 * @details 根据数据类型获取对应类型缓存的当前大小
 */
const unsigned long long MultiResolutionImage::getCacheSize()
{
	unsigned long long cacheSize = 0;
	m_cacheMutex->lock();
	if (m_cache && _isValid)
	{
		if (_dataType == SlideColorManagement::DataType::UInt32) {
			cacheSize = (std::static_pointer_cast<TileCache<unsigned int>>(m_cache))->maxCacheSize();
		}
		else if (_dataType == SlideColorManagement::DataType::UInt16) {
			cacheSize = (std::static_pointer_cast<TileCache<unsigned short>>(m_cache))->maxCacheSize();
		}
		else if (_dataType == SlideColorManagement::DataType::UChar) {
			cacheSize = (std::static_pointer_cast<TileCache<unsigned char>>(m_cache))->maxCacheSize();
		}
		else if (_dataType == SlideColorManagement::DataType::Float) {
			cacheSize = (std::static_pointer_cast<TileCache<float>>(m_cache))->maxCacheSize();
		}
		m_cacheMutex->unlock();
	}

	return cacheSize;
}

/**
 * @brief 设置缓存大小
 * @param cacheSize 要设置的缓存大小（字节数）
 * @details 根据数据类型设置对应类型缓存的最大大小
 */
void MultiResolutionImage::setCacheSize(const unsigned long long cacheSize)
{
	m_cacheMutex->lock();
	if (m_cache && _isValid) {
		if (_dataType == SlideColorManagement::DataType::UInt32) {
			(std::static_pointer_cast<TileCache<unsigned int>>(m_cache))->setMaxCacheSize(cacheSize);
		}
		else if (_dataType == SlideColorManagement::DataType::UInt16) {
			(std::static_pointer_cast<TileCache<unsigned short>>(m_cache))->setMaxCacheSize(cacheSize);
		}
		else if (_dataType == SlideColorManagement::DataType::UChar) {
			(std::static_pointer_cast<TileCache<unsigned char>>(m_cache))->setMaxCacheSize(cacheSize);
		}
		else if (_dataType == SlideColorManagement::DataType::Float) {
			(std::static_pointer_cast<TileCache<float>>(m_cache))->setMaxCacheSize(cacheSize);
		}
		m_cacheMutex->unlock();
	}
}

/**
 * @brief 获取图像层级数量
 * @return 图像层级数量，如果图像无效则返回-1
 * @details 返回多分辨率图像的层级数量，用于确定可用的分辨率级别
 */
const int MultiResolutionImage::getNumberOfLevels() const
{
	if (_isValid)
	{
		return _numberOfLevels;
	}
	else
	{
		return -1;
	}
}

/**
 * @brief 获取图像尺寸
 * @return 图像尺寸向量（宽度、高度），如果图像无效则返回空向量
 * @details 返回最高分辨率级别（第0级）的图像尺寸
 */
const std::vector<unsigned long long> MultiResolutionImage::getDimensions() const
{
	std::vector<unsigned long long> dims;
	if (_isValid) {
		return _levelDimensions[0];
	}
	return dims;
}

/**
 * @brief 获取指定层级的图像尺寸
 * @param level 层级索引
 * @return 指定层级的图像尺寸向量，如果层级无效则返回空向量
 * @details 返回指定分辨率级别的图像尺寸
 */
const std::vector<unsigned long long> MultiResolutionImage::getLevelDimensions(const unsigned int& level) const
{
	std::vector<unsigned long long> dims;
	if (_isValid && (level < getNumberOfLevels())) {
		return _levelDimensions[level];
	}
	return dims;
}

/**
 * @brief 获取指定层级的降采样比例
 * @param level 层级索引
 * @return 降采样比例，如果层级无效则返回-1.0
 * @details 计算指定层级相对于最高分辨率级别的降采样比例
 */
const double MultiResolutionImage::getLevelDownsample(const unsigned int& level) const
{
	if (_isValid && (level < getNumberOfLevels())) {
		return static_cast<float>(_levelDimensions[0][0]) / _levelDimensions[level][0];
	}
	else {
		return -1.0;
	}
}

/**
 * @brief 获取最适合指定降采样比例的层级
 * @param downsample 目标降采样比例
 * @return 最适合的层级索引，如果图像无效则返回-1
 * @details 遍历所有层级，找到与目标降采样比例最接近的层级
 */
const int MultiResolutionImage::getBestLevelForDownSample(const double& downsample) const
{
	if (_isValid) {
		float previousDownsample = 1.0;
		if (downsample < 1.0) {
			return 0;
		}
		for (int i = 1; i < _levelDimensions.size(); ++i) {
			double currentDownSample = (double)_levelDimensions[0][0] / (double)_levelDimensions[i][0];
			double previousDownSample = (double)_levelDimensions[0][0] / (double)_levelDimensions[i - 1][0];
			if (downsample < currentDownSample) {

				if (std::abs(currentDownSample - downsample) > std::abs(previousDownSample - downsample)) {
					return i - 1;
				}
				else {
					return i;
				}

				return i - 1;
			}
		}
		return getNumberOfLevels() - 1;
	}
	else {
		return -1;
	}
}

/**
 * @brief 获取文件类型
 * @return 文件类型字符串
 * @details 返回图像文件的类型标识
 */
const std::string MultiResolutionImage::getFileType() const
{
	return  _fileType;
}

/**
 * @brief 清理资源
 * @details 清理所有成员变量，将对象重置为初始状态
 *          包括清空向量、重置数值、设置无效状态等
 */
void MultiResolutionImage::cleanup()
{
	_levelDimensions.clear();
	_spacing.clear();
	_samplesPerPixel = 0;
	_numberOfLevels = 0;
	_colorType = SlideColorManagement::ColorType::InvalidColorType;
	_dataType = SlideColorManagement::DataType::InvalidDataType;
	_isValid = false;
	 _fileType = "";
	m_filePath = "";
}
