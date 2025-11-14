/**
 * @file MultiResolutionImageFactory.cpp
 * @brief 多分辨率图像工厂类实现文件
 * @details 该文件实现了多分辨率图像工厂模式，用于创建和管理不同类型的多分辨率图像
 *          支持多种图像格式的自动识别和加载，包括OpenSlide格式
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "MultiResolutionImageFactory.h"

#define NOMINMAX
#include <windows.h>
#include "MultiResolutionImage.h"
#include <algorithm>

/**
 * @brief 外部格式注册标志
 * @details 用于标记是否已经注册了外部文件格式
 */
bool MultiResolutionImageFactory::_externalFormatsRegistered = false;

/**
 * @brief 所有支持的扩展名集合
 * @details 存储所有已注册工厂支持的图像文件扩展名
 */
std::set<std::string> MultiResolutionImageFactory::_allSupportedExtensions;

/**
 * @brief 获取工厂注册表
 * @return 工厂注册表的引用
 * @details 返回静态工厂注册表，用于存储所有已注册的图像工厂
 */
MultiResolutionImageFactory::FactoryMap& MultiResolutionImageFactory::registry() {
    static FactoryMap typeRegistry;
    return typeRegistry;
}

/**
 * @brief 添加支持的扩展名
 * @param extensions 要添加的扩展名集合
 * @details 将新的扩展名添加到全局支持的扩展名集合中
 */
void MultiResolutionImageFactory::addSupportedExtensions(const std::set<std::string>& extensions)
{
    _allSupportedExtensions.insert(extensions.begin(), extensions.end());
}

/**
 * @brief 构造函数：注册图像工厂
 * @param factoryName 工厂名称
 * @param supported_extensions 支持的扩展名集合
 * @param priority 工厂优先级
 * @details 创建工厂对象并注册到全局注册表中，同时添加支持的扩展名
 */
MultiResolutionImageFactory::MultiResolutionImageFactory(const std::string& factoryName, const std::set<std::string>& supported_extensions, const unsigned int priority) :
    _factoryName(factoryName),
    _priority(priority)
{
    registry()[factoryName] = std::make_pair(supported_extensions, this);
    addSupportedExtensions(supported_extensions);
}

/**
 * @brief 获取已加载的工厂和支持的扩展名
 * @return 工厂名称和扩展名集合的向量
 * @details 注册外部文件格式后，返回所有已注册工厂的信息
 */
std::vector<std::pair<std::string, std::set<std::string>>> MultiResolutionImageFactory::getLoadedFactoriesAndSupportedExtensions()
{
    MultiResolutionImageFactory::registerExternalFileFormats();
    std::vector<std::pair<std::string, std::set<std::string>>> factoriesAndExtensions;
    for (auto it = registry().begin(); it != registry().end(); ++it) {
        factoriesAndExtensions.push_back(std::make_pair(it->first, it->second.first));
    }
    return factoriesAndExtensions;
}

/**
 * @brief 获取所有支持的扩展名
 * @return 所有支持的文件扩展名集合
 * @details 注册外部文件格式后，返回所有支持的扩展名
 */
std::set<std::string> MultiResolutionImageFactory::getAllSupportedExtensions()
{
    MultiResolutionImageFactory::registerExternalFileFormats();
    return _allSupportedExtensions;
}

/**
 * @brief 工厂优先级比较操作符
 * @param other 要比较的另一个工厂
 * @return 如果当前工厂优先级更低则返回true
 * @details 用于工厂的排序，优先级高的工厂优先使用
 */
bool MultiResolutionImageFactory::operator<(const MultiResolutionImageFactory& other) const
{
    return this->_priority < other._priority;
}

/**
 * @brief 打开图像文件
 * @param fileName 图像文件路径
 * @param factoryName 工厂名称（可选）
 * @return 成功时返回图像对象指针，失败时返回NULL
 * @details 使用OpenSlide工厂尝试打开图像文件
 */
MultiResolutionImage* MultiResolutionImageFactory::openImage(const std::string& fileName, const std::string factoryName) {
    MultiResolutionImageFactory* factory = new OpenSlideImageFactory();
    MultiResolutionImage* img = MultiResolutionImageFactory::openImageWithFactory(fileName, factory);
    if (img)
    {
        return img;
    }

    return NULL;
}

/**
 * @brief 使用指定工厂打开图像文件
 * @param fileName 图像文件路径
 * @param factory 要使用的工厂对象
 * @return 成功时返回图像对象指针，失败时返回NULL
 * @details 使用指定的工厂对象尝试读取图像文件
 */
MultiResolutionImage* MultiResolutionImageFactory::openImageWithFactory(const std::string& fileName, const MultiResolutionImageFactory* factory)
{
    MultiResolutionImage* img = factory->readImage(fileName);
    if (img) {
        return img;
    }
    return NULL;
}

/**
 * @brief 文件格式加载器函数指针类型
 * @details 用于动态加载外部文件格式支持
 */
typedef void (*FileFormatLoader)();

/**
 * @brief 注册外部文件格式
 * @details 目前为空实现，预留接口用于未来扩展外部文件格式支持
 */
void MultiResolutionImageFactory::registerExternalFileFormats() {
    //目前这个实现也不需要了：
    return;
}

using std::string;

/**
 * @brief 多分辨率图像读取器构造函数
 * @details 创建图像读取器对象
 */
MultiResolutionImageReader::MultiResolutionImageReader()
{
}

/**
 * @brief 多分辨率图像读取器析构函数
 * @details 清理读取器资源
 */
MultiResolutionImageReader::~MultiResolutionImageReader() {
}

/**
 * @brief 打开图像文件
 * @param fileName 图像文件路径
 * @param factoryName 工厂名称
 * @return 成功时返回图像对象指针，失败时返回NULL
 * @details 使用工厂模式打开图像文件
 */
MultiResolutionImage* MultiResolutionImageReader::open(const std::string& fileName, const std::string factoryName) {
    return MultiResolutionImageFactory::openImage(fileName, factoryName);
}

#include "OpenSlideImage.h"

/**
 * @brief OpenSlide图像工厂构造函数
 * @details 创建OpenSlide工厂，支持多种数字病理切片格式
 *          支持的格式包括：svs, tif, tiff, mrxs, vms, vmu, ndpi, scn, svslide, bif
 *          优先级设置为1，表示高优先级
 */
OpenSlideImageFactory::OpenSlideImageFactory()
    : MultiResolutionImageFactory("OpenSlide Formats", { "svs", "tif", "tiff", "mrxs", "vms", "vmu", "ndpi", "scn", "svslide", "bif" }, 1) {
}

/**
 * @brief 读取图像文件
 * @param fileName 图像文件路径
 * @return 成功时返回OpenSlideImage对象指针，失败时返回NULL
 * @details 创建OpenSlideImage对象并尝试初始化，如果成功则返回对象指针
 */
MultiResolutionImage* OpenSlideImageFactory::readImage(const std::string& fileName) const {
    OpenSlideImage* img = new OpenSlideImage();
    img->initialize(fileName);
    if (img->valid()) {
        return img;
    }
    else {
        delete img;
        return NULL;
    }
}

/**
 * @brief 检查是否可以读取指定文件
 * @param fileName 图像文件路径
 * @return 如果可以读取则返回true，否则返回false
 * @details 尝试创建OpenSlideImage对象并初始化，用于检查文件格式是否支持
 */
bool OpenSlideImageFactory::canReadImage(const std::string& fileName) const {
    OpenSlideImage* img = new OpenSlideImage();
    bool canOpen = img->initialize(fileName);
    delete img;
    return canOpen;
}

/**
 * @brief 文件类型加载函数
 * @details 静态函数，用于确保OpenSlideImageFactory被正确注册
 *          创建静态工厂对象，确保在程序启动时注册工厂
 */
void filetypeLoad()
{
    static OpenSlideImageFactory filetypeFactory;
}