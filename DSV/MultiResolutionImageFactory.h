/**
 * @file    MultiResolutionImageFactory.h
 * @brief   多分辨率图像工厂类，实现图像格式的自动识别和加载
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的图像工厂模式，包括：
 *          - 多分辨率图像工厂基类
 *          - 图像格式自动识别和注册机制
 *          - 支持多种图像格式的扩展加载
 *          - 工厂优先级管理
 *          - OpenSlide图像格式的专门实现
 *          该设计允许系统动态加载不同格式的图像文件，
 *          通过文件扩展名自动选择合适的工厂进行处理。
 *
 * @note    该文件实现了工厂模式和策略模式的结合
 * @see     MultiResolutionImage, OpenSlideImage
 */

#pragma once

#include <string>
#include <map>
#include <vector> 
#include <set>

 // 前向声明
class MultiResolutionImage;

/**
 * @class  MultiResolutionImageFactory
 * @brief  多分辨率图像工厂基类，实现图像格式的自动识别和加载
 * @details 该类实现了工厂模式，用于根据文件扩展名自动创建
 *          相应类型的多分辨率图像对象。支持动态注册新的图像格式，
 *          并具有优先级管理机制，确保选择合适的工厂处理特定文件。
 *
 *          主要功能包括：
 *          - 图像格式自动识别和注册
 *          - 工厂优先级管理
 *          - 支持的文件扩展名管理
 *          - 静态工厂注册表维护
 *          - 图像对象的创建和初始化
 *
 * @note   该类是抽象基类，不能直接实例化
 * @example
 *          // 使用示例
 *          MultiResolutionImage* img = MultiResolutionImageFactory::openImage("image.svs");
 *          if (img) {
 *              // 使用图像对象
 *              img->initialize("image.svs");
 *          }
 * @see     MultiResolutionImage, OpenSlideImageFactory
 */
class MultiResolutionImageFactory
{
public:
    /**
     * @brief   构造函数
     * @details 创建图像工厂实例，设置工厂名称、支持的文件扩展名和优先级
     *
     * @param   factoryName 工厂名称，用于标识不同的图像格式处理器
     * @param   supported_extensions 支持的文件扩展名集合
     * @param   priority 工厂优先级，数值越小优先级越高
     * @note    构造函数会自动将工厂注册到全局注册表中
     */
    MultiResolutionImageFactory(const std::string& factoryName, const std::set<std::string>& supported_extensions, const unsigned int priority);

    /**
     * @brief   打开图像文件（静态方法）
     * @details 根据文件名自动选择合适的工厂来打开图像文件。
     *          系统会根据文件扩展名查找支持该格式的工厂，
     *          并按照优先级选择最合适的工厂进行处理。
     *
     * @param   fileName 图像文件路径
     * @param   factoryName 指定的工厂名称，默认为"default"
     * @return  成功时返回图像对象指针，失败时返回nullptr
     * @note    调用者负责释放返回的图像对象内存
     * @see     openImageWithFactory
     * @example
     *          // 自动识别格式
     *          MultiResolutionImage* img1 = MultiResolutionImageFactory::openImage("image.svs");
     *          // 指定工厂
     *          MultiResolutionImage* img2 = MultiResolutionImageFactory::openImage("image.tiff", "openslide");
     */
    static MultiResolutionImage* openImage(const std::string& fileName, const std::string factoryName = std::string("default"));

    /**
     * @brief   注册外部文件格式（静态方法）
     * @details 注册系统中可用的外部图像格式支持。
     *          该方法会扫描系统中可用的图像格式库，
     *          并自动注册支持的文件格式到工厂系统中。
     *
     * @note    该方法通常在系统初始化时调用一次
     * @see     _externalFormatsRegistered
     */
    static void registerExternalFileFormats();

    /**
     * @brief   获取已加载的工厂和支持的扩展名（静态方法）
     * @details 返回所有已注册的工厂及其支持的文件扩展名列表。
     *          用于系统诊断和用户界面显示支持的文件格式。
     *
     * @return  包含工厂名称和扩展名集合的向量对
     * @note    返回的数据可以用于构建文件过滤器或格式列表
     * @example
     *          auto factories = MultiResolutionImageFactory::getLoadedFactoriesAndSupportedExtensions();
     *          for (const auto& factory : factories) {
     *              std::cout << "Factory: " << factory.first << std::endl;
     *              for (const auto& ext : factory.second) {
     *                  std::cout << "  Supports: " << ext << std::endl;
     *              }
     *          }
     */
    static std::vector<std::pair<std::string, std::set<std::string> > > getLoadedFactoriesAndSupportedExtensions();

    /**
     * @brief   获取所有支持的扩展名（静态方法）
     * @details 返回系统中所有工厂支持的文件扩展名集合。
     *          用于构建文件选择对话框的过滤器。
     *
     * @return  所有支持的文件扩展名集合
     * @note    返回的扩展名不包含重复项
     * @example
     *          auto extensions = MultiResolutionImageFactory::getAllSupportedExtensions();
     *          std::string filter = "Supported Files (";
     *          for (const auto& ext : extensions) {
     *              filter += "*." + ext + " ";
     *          }
     *          filter += ")";
     */
    static std::set<std::string> getAllSupportedExtensions();

    /**
     * @brief   工厂比较操作符
     * @details 根据工厂优先级进行比较，用于工厂排序。
     *          优先级数值越小，工厂优先级越高。
     *
     * @param   other 要比较的另一个工厂对象
     * @return  true表示当前工厂优先级更高，false表示other优先级更高
     * @note    该操作符用于工厂注册表中的排序
     */
    bool operator< (const MultiResolutionImageFactory& other) const;

protected:
    /**
     * @brief 工厂注册表类型定义
     * @details 使用map存储工厂名称到工厂对象和扩展名的映射关系
     */
    typedef std::map<std::string, std::pair<std::set<std::string>, MultiResolutionImageFactory*> > FactoryMap;

    /**
     * @brief   获取工厂注册表引用（静态方法）
     * @details 返回全局工厂注册表的引用，用于管理所有已注册的工厂
     *
     * @return  工厂注册表的引用
     * @note    该方法是protected的，只允许派生类访问
     */
    static FactoryMap& registry();

    /**
     * @brief   添加支持的扩展名（静态方法）
     * @details 将新的文件扩展名添加到全局支持的扩展名集合中
     *
     * @param   extensions 要添加的文件扩展名集合
     * @note    该方法会自动去重，避免重复添加相同的扩展名
     */
    static void addSupportedExtensions(const std::set<std::string>& extensions);

    /**
     * @brief   使用指定工厂打开图像（静态方法）
     * @details 使用指定的工厂对象来打开图像文件
     *
     * @param   fileName 图像文件路径
     * @param   factory 要使用的工厂对象指针
     * @return  成功时返回图像对象指针，失败时返回nullptr
     * @note    该方法会调用工厂的readImage方法
     * @see     readImage
     */
    static MultiResolutionImage* openImageWithFactory(const std::string& fileName, const MultiResolutionImageFactory* factory);

    /**
     * @brief   读取图像文件（纯虚函数）
     * @details 由派生类实现的图像读取逻辑
     *
     * @param   fileName 图像文件路径
     * @return  成功时返回图像对象指针，失败时返回nullptr
     * @note    该函数是纯虚函数，必须由派生类实现
     * @see     OpenSlideImageFactory::readImage
     */
    virtual MultiResolutionImage* readImage(const std::string& fileName) const = 0;

    /**
     * @brief   检查是否可以读取图像（纯虚函数）
     * @details 检查指定的文件是否可以被当前工厂读取
     *
     * @param   fileName 图像文件路径
     * @return  true表示可以读取，false表示不能读取
     * @note    该函数是纯虚函数，必须由派生类实现
     * @see     OpenSlideImageFactory::canReadImage
     */
    virtual bool canReadImage(const std::string& fileName) const = 0;

private:
    /** @brief 外部格式是否已注册的标志 */
    static bool _externalFormatsRegistered;

    /** @brief 所有支持的文件扩展名集合 */
    static std::set<std::string> _allSupportedExtensions;

    /** @brief 工厂名称 */
    const std::string _factoryName;

    /** @brief 工厂优先级，数值越小优先级越高 */
    const unsigned int _priority;
};

/**
 * @class  MultiResolutionImageReader
 * @brief  多分辨率图像读取器，提供简化的图像加载接口
 * @details 该类是对MultiResolutionImageFactory的封装，提供更简单的
 *          图像加载接口。主要用于应用程序层面的图像文件加载。
 *
 * @note    该类是工具类，提供便捷的图像加载功能
 * @see     MultiResolutionImageFactory
 * @example
 *          // 使用示例
 *          MultiResolutionImageReader reader;
 *          MultiResolutionImage* img = reader.open("image.svs");
 *          if (img) {
 *              // 使用图像对象
 *          }
 */
class MultiResolutionImageReader
{
public:
    /**
     * @brief   默认构造函数
     * @details 创建图像读取器实例
     */
    MultiResolutionImageReader();

    /**
     * @brief   析构函数
     * @details 清理图像读取器资源
     */
    ~MultiResolutionImageReader();

    /**
     * @brief   打开图像文件
     * @details 使用工厂模式打开指定的图像文件
     *
     * @param   fileName 图像文件路径
     * @param   factoryName 指定的工厂名称，默认为"default"
     * @return  成功时返回图像对象指针，失败时返回nullptr
     * @note    调用者负责释放返回的图像对象内存
     * @see     MultiResolutionImageFactory::openImage
     */
    MultiResolutionImage* open(const std::string& fileName, const std::string factoryName = std::string("default"));
};

/**
 * @class  OpenSlideImageFactory
 * @brief  OpenSlide图像格式工厂，专门处理OpenSlide格式的图像文件
 * @details 该类是MultiResolutionImageFactory的具体实现，专门用于
 *          处理OpenSlide格式的图像文件（如.svs, .tiff等）。
 *          支持Aperio、Hamamatsu、Leica等扫描仪生成的数字病理图像。
 *
 * @note    该类会自动注册到工厂系统中
 * @see     MultiResolutionImageFactory, OpenSlideImage
 * @example
 *          // 使用示例
 *          OpenSlideImageFactory factory;
 *          MultiResolutionImage* img = factory.readImage("pathology.svs");
 */
class OpenSlideImageFactory : public MultiResolutionImageFactory {
public:
    /**
     * @brief   构造函数
     * @details 创建OpenSlide图像工厂，设置支持的文件扩展名和优先级
     * @note    构造函数会自动注册到工厂系统中
     */
    OpenSlideImageFactory();

private:
    /**
     * @brief   读取OpenSlide图像文件
     * @details 实现OpenSlide格式图像文件的读取逻辑
     *
     * @param   fileName 图像文件路径
     * @return  成功时返回OpenSlideImage对象指针，失败时返回nullptr
     * @note    该函数会检查文件格式并创建相应的图像对象
     * @see     OpenSlideImage
     */
    MultiResolutionImage* readImage(const std::string& fileName) const;

    /**
     * @brief   检查是否可以读取OpenSlide图像
     * @details 检查指定文件是否为OpenSlide支持的格式
     *
     * @param   fileName 图像文件路径
     * @return  true表示可以读取，false表示不能读取
     * @note    该函数会检查文件扩展名和文件头信息
     */
    bool canReadImage(const std::string& fileName) const;
};

/**
 * @brief   文件类型加载函数（C接口）
 * @details 用于动态加载外部文件格式支持的C接口函数。
 *          该函数通常在系统初始化时被调用，用于注册
 *          系统中可用的图像格式库。
 *
 * @note    该函数使用extern "C"声明，确保C语言兼容性
 * @see     registerExternalFileFormats
 */
extern "C" {
    void filetypeLoad();
}