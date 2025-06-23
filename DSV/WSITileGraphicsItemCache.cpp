/**
 * @file WSITileGraphicsItemCache.cpp
 * @brief WSI瓦片图形项缓存实现文件
 * @details 该文件实现了瓦片图形项的LRU缓存管理功能，包括：
 *          - 基于LRU算法的缓存淘汰
 *          - 内存使用量控制
 *          - 顶级瓦片的永久缓存
 *          - 缓存项的查找和更新
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "WSITileGraphicsItemCache.h"
#include "WSITileGraphicsItem.h"

/**
 * @brief 析构函数：清理缓存资源
 * @details 调用clear函数清空所有缓存项
 */
WSITileGraphicsItemCache::~WSITileGraphicsItemCache() {
    clear();
}

/**
 * @brief 淘汰最近最少使用的缓存项
 * @details 从LRU列表前端移除最久未使用的项，并从缓存映射中删除对应记录
 */
void WSITileGraphicsItemCache::evict() {
    // 识别最近最少使用的键
    std::map<keyType, std::pair<std::pair<WSITileGraphicsItem*, unsigned int>, keyTypeList::iterator> >::iterator it = _cache.find(_LRU.front());

    // 删除两个元素以完全清除记录
    WSITileGraphicsItem* itemToEvict = it->second.first.first;
    _cacheCurrentByteSize -= it->second.first.second;
    _cache.erase(it);
    _LRU.pop_front();
    emit itemEvicted(itemToEvict);
}

/**
 * @brief 清空缓存
 * @details 清空缓存映射、LRU列表，并重置当前字节大小
 */
void WSITileGraphicsItemCache::clear() {
    _cache.clear();
    _LRU.clear();
    _cacheCurrentByteSize = 0;
}

/**
 * @brief 获取缓存项
 * @param k 缓存键
 * @param tile 输出瓦片图形项指针
 * @param size 输出瓦片大小
 * @details 根据键查找缓存项，如果找到则更新LRU位置并返回瓦片信息
 */
void WSITileGraphicsItemCache::get(const keyType& k, WSITileGraphicsItem*& tile, unsigned int& size) {

    std::map<keyType, std::pair<std::pair<WSITileGraphicsItem*, unsigned int>, keyTypeList::iterator> >::iterator it = _cache.find(k);

    if (it == _cache.end()) {
        tile = NULL;
        return;
    }
    else {
        if (it->second.second != _LRU.end()) {
            _LRU.splice(
                _LRU.end(),
                _LRU,
                it->second.second
            );
        }
        tile = it->second.first.first;
        size = it->second.first.second;
        return;
    }
}

/**
 * @brief 获取所有缓存项
 * @return 所有瓦片图形项的向量
 * @details 遍历缓存映射，返回所有缓存的瓦片图形项
 */
std::vector<WSITileGraphicsItem*> WSITileGraphicsItemCache::getAllItems() {
    std::vector<WSITileGraphicsItem*> allItems;
    for (auto it = _cache.begin(); it != _cache.end(); ++it) {
        allItems.push_back(it->second.first.first);
    }
    return allItems;
}

/**
 * @brief 设置缓存项
 * @param k 缓存键
 * @param v 瓦片图形项指针
 * @param size 瓦片大小
 * @param topLevel 是否为顶级瓦片
 * @return 0表示成功，1表示失败
 * @details 添加新的缓存项，如果超出内存限制则淘汰旧项
 *          顶级瓦片不会被加入LRU列表，因此不会被淘汰
 */
int WSITileGraphicsItemCache::set(const keyType& k, WSITileGraphicsItem* v, unsigned int size, bool topLevel) {
    if (_cache.find(k) != _cache.end()) {
        return 1;
    }
    if (size > _cacheMaxByteSize) {
        return 1;
    }
    while (_cacheCurrentByteSize + size > _cacheMaxByteSize && _cacheCurrentByteSize != 0) {
        evict();
    }

    // 如果是顶级项则不添加到LRU中，这样它永远不会被移除
    if (!topLevel) {
        keyTypeList::iterator it = _LRU.insert(_LRU.end(), k);
        _cache[k] = std::make_pair(std::make_pair(v, size), it);
    }
    else {
        _cache[k] = std::make_pair(std::make_pair(v, size), _LRU.end());
    }
    _cacheCurrentByteSize += size;
    return 0;
}