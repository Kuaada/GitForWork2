/**
 * @file    TileCache.hpp
 * @brief   瓦片缓存模板类，实现LRU（最近最少使用）缓存算法
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的瓦片缓存系统，包括：
 *          - LRU（最近最少使用）缓存算法
 *          - 模板化设计，支持任意数据类型
 *          - 自动内存管理和垃圾回收
 *          - 缓存大小限制和动态调整
 *          - 高效的缓存查找和更新机制
 *          该缓存系统专门为数字病理图像的大规模瓦片数据设计，
 *          提供高效的内存管理和数据访问性能。
 *
 * @note    该类是模板类，支持任意数据类型的瓦片缓存
 * @see     MultiResolutionImage, WSITileGraphicsItem
 */

#pragma once
#include <list>
#include <map>
#include <string>
#include <vector>

 /**
  * @class  TileCache
  * @brief  瓦片缓存模板类，实现LRU缓存算法
  * @details 该类实现了高效的瓦片缓存系统，使用LRU（最近最少使用）
  *          算法来管理缓存中的数据。当缓存空间不足时，会自动淘汰
  *          最久未使用的数据，为新数据腾出空间。
  *
  *          主要特性：
  *          - 模板化设计，支持任意数据类型的瓦片
  *          - LRU算法确保最常用的数据保留在缓存中
  *          - 自动内存管理，防止内存泄漏
  *          - 可配置的缓存大小限制
  *          - 高效的O(1)查找和更新操作
  *
  * @tparam T 瓦片数据类型模板参数
  * @note   该类是线程不安全的，多线程使用时需要外部同步
  * @example
  *          // 使用示例
  *          TileCache<unsigned char> cache(1024 * 1024); // 1MB缓存
  *          unsigned char* data = new unsigned char[1024];
  *          cache.set("tile_1", data, 1024);
  *
  *          unsigned char* retrieved_data;
  *          unsigned int size;
  *          cache.get("tile_1", retrieved_data, size);
  * @see     MultiResolutionImage, WSITileGraphicsItem
  */
template <typename T>
class TileCache {
public:
    /**
     * @brief   默认构造函数
     * @details 创建瓦片缓存对象，设置默认的缓存大小限制
     * @note    默认缓存大小为0，需要调用setMaxCacheSize设置有效大小
     */
    TileCache() :
        _cacheMaxByteSize(0),
        _cacheCurrentByteSize(0),
        _LRU(),
        _cache()
    {
    }

    /**
     * @brief   带缓存大小参数的构造函数
     * @details 创建瓦片缓存对象，并设置最大缓存字节大小
     *
     * @param   cacheMaxByteSize 最大缓存字节大小
     * @note    当缓存数据超过此大小时，会自动淘汰最久未使用的数据
     */
    TileCache(unsigned int cacheMaxByteSize) :
        _cacheMaxByteSize(cacheMaxByteSize),
        _cacheCurrentByteSize(0),
        _LRU(),
        _cache()
    {
    }

    /**
     * @brief   虚析构函数
     * @details 清理缓存中存储的所有动态分配的内存。
     *          遍历_cache映射，使用delete[]释放每个瓦片数据的内存。
     * @note    该析构函数确保不会发生内存泄漏
     */
    virtual ~TileCache()
    {
        for (key_iterator it = _cache.begin(); it != _cache.end(); ++it) {
            delete[] it->second.first.first;
        }
    }

    /** @brief 缓存键类型定义，使用字符串作为瓦片的唯一标识 */
    typedef std::string keyType;

    /** @brief LRU列表类型定义，用于存储键的列表以实现LRU顺序 */
    typedef std::list<keyType> keyTypeList;

    /**
     * @brief 缓存迭代器类型定义
     * @details 定义为std::map的迭代器类型，用于遍历_cache映射
     */
    typedef typename std::map<keyType, std::pair<std::pair<T*, unsigned int>, std::list<keyType>::iterator> >::iterator key_iterator;

    /**
     * @brief   从缓存中获取指定键对应的瓦片数据
     * @details 根据键查找缓存中的数据，如果找到则将数据移动到LRU列表末尾
     *          （表示最近使用），并返回数据指针和大小。
     *
     * @param   k 瓦片的唯一标识键
     * @param   tile 输出参数，指向瓦片数据的指针
     * @param   size 输出参数，瓦片数据的大小（字节）
     * @note    如果键不存在于缓存中，tile将被设置为NULL
     * @see     set, evict
     * @example
     *          // 使用示例
     *          unsigned char* data;
     *          unsigned int size;
     *          cache.get("tile_1", data, size);
     *          if (data != NULL) {
     *              // 使用数据
     *          }
     */
    virtual void get(const keyType& k, T*& tile, unsigned int& size) {
        // 在缓存中查找键k是否存在
        key_iterator it = _cache.find(k);

        if (it == _cache.end()) {
            // 如果键不存在，将tile设置为NULL并返回
            tile = NULL;
            return;
        }
        else {
            // 使用_LRU.splice将对应元素移动到_LRU列表末尾，
            // 表示该元素是最近使用的。然后返回数据指针和大小
            _LRU.splice(
                _LRU.end(),
                _LRU,
                it->second.second
            );
            tile = it->second.first.first;
            size = it->second.first.second;
            return;
        }
    }

    /**
     * @brief   向缓存中插入一个新的瓦片数据
     * @details 将新的瓦片数据插入到缓存中。如果缓存空间不足，
     *          会自动调用evict()淘汰最久未使用的数据，直到有足够空间。
     *
     * @param   k 瓦片的唯一标识键
     * @param   v 瓦片数据指针
     * @param   size 瓦片数据大小（字节）
     * @return  0表示插入成功，1表示插入失败
     * @note    插入失败的原因可能是：键已存在或数据大小超过最大缓存大小
     * @see     get, evict
     * @example
     *          // 使用示例
     *          unsigned char* data = new unsigned char[1024];
     *          int result = cache.set("tile_1", data, 1024);
     *          if (result == 0) {
     *              // 插入成功
     *          }
     */
    virtual int set(const keyType& k, T* v, unsigned int size) {

        if (_cache.find(k) != _cache.end()) {
            // 首先检查k是否已经存在于缓存中，如果存在则返回1表示插入失败
            return 1;
        }
        if (size > _cacheMaxByteSize) {
            // 然后检查数据的大小是否超过最大缓存字节大小，如果超过则返回1表示插入失败
            return 1;
        }
        while (_cacheCurrentByteSize + size > _cacheMaxByteSize && _cacheCurrentByteSize != 0) {
            // 如果缓存空间不足，调用evict()淘汰最久未使用的数据，直到有足够的空间
            evict();
        }
        // 将k插入到_LRU列表末尾，然后将数据指针、大小和_LRU列表中的迭代器存储到_cache映射中
        keyTypeList::iterator it = _LRU.insert(_LRU.end(), k);
        _cache[k] = std::make_pair(std::make_pair(v, size), it);
        // 更新当前使用的字节大小，返回0表示插入成功
        _cacheCurrentByteSize += size;
        return 0;
    }

    /**
     * @brief   获取当前缓存使用的字节大小
     * @details 返回缓存当前占用的内存字节数
     *
     * @return  当前缓存使用的字节大小
     * @see     maxCacheSize, setMaxCacheSize
     */
    unsigned long long currentCacheSize() const { return _cacheCurrentByteSize; }

    /**
     * @brief   获取缓存的最大字节大小
     * @details 返回缓存允许使用的最大内存字节数
     *
     * @return  缓存的最大字节大小
     * @see     currentCacheSize, setMaxCacheSize
     */
    unsigned long long maxCacheSize() const { return _cacheMaxByteSize; }

    /**
     * @brief   设置缓存的最大字节大小
     * @details 设置缓存允许使用的最大内存字节数。如果当前使用的字节大小
     *          超过新的最大字节大小，会调用evict()淘汰数据直到满足要求。
     *
     * @param   cacheMaxByteSize 新的最大缓存字节大小
     * @note    该操作可能会导致部分缓存数据被淘汰
     * @see     currentCacheSize, maxCacheSize, evict
     * @example
     *          // 使用示例
     *          cache.setMaxCacheSize(2 * 1024 * 1024); // 设置为2MB
     */
    void setMaxCacheSize(const unsigned long long& cacheMaxByteSize) {
        _cacheMaxByteSize = cacheMaxByteSize;
        while (_cacheCurrentByteSize > _cacheMaxByteSize) {
            evict();
        }
    }

    /**
     * @brief   清空缓存
     * @details 清空所有缓存数据。释放缓存中存储的所有动态分配的内存，
     *          然后清空_cache映射和_LRU列表，最后将当前使用的字节大小设为0。
     * @note    该操作会释放所有缓存的数据，请谨慎使用
     * @see     evict
     * @example
     *          // 使用示例
     *          cache.clear(); // 清空所有缓存数据
     */
    virtual void clear() {
        for (key_iterator it = _cache.begin(); it != _cache.end(); ++it) {
            delete[] it->second.first.first;
        }
        _cache.clear();
        _LRU.clear();
        _cacheCurrentByteSize = 0;
    }

protected:
    /** @brief 表示当前缓存使用的字节大小 */
    unsigned long long _cacheCurrentByteSize;

    /** @brief 表示缓存的最大字节大小 */
    unsigned long long _cacheMaxByteSize;

    /** @brief 用于存储键的列表，实现LRU顺序 */
    std::list<keyType> _LRU;

    /**
     * @brief 缓存映射表
     * @details _cache是一个std::map，用于存储瓦片数据，键为keyType，
     *          值为一个std::pair，包含数据指针、大小和_LRU列表中的迭代器
     */
    std::map<keyType, std::pair<std::pair<T*, unsigned int>, keyTypeList::iterator> > _cache;

    /**
     * @brief   淘汰最久未使用的瓦片数据
     * @details 实现LRU算法的核心函数，淘汰最久未使用的数据。
     *          找到_LRU列表的第一个元素（最久未使用的键），
     *          然后在_cache映射中查找该键对应的数据。
     *
     * @note    该函数是protected的，只能由类内部调用
     * @see     set, clear
     */
    virtual void evict() {
        // 首先找到_LRU列表的第一个元素，即最久未使用的键，然后在_cache映射中查找该键对应的数据
        key_iterator it = _cache.find(_LRU.front());

        // 释放该瓦片数据的内存，更新当前使用的字节大小，从_cache映射中删除该数据，然后从_LRU列表中删除该键
        _cacheCurrentByteSize -= it->second.first.second;
        delete[] it->second.first.first;
        _cache.erase(it);
        _LRU.pop_front();
    }
};