/**
 * @file    WSITileGraphicsItemCache.h
 * @brief   瓦片图形项缓存类，管理瓦片图形项的内存和性能优化
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了DSV项目的瓦片图形项缓存功能，包括：
 *          - 瓦片图形项的内存管理和缓存
 *          - LRU（最近最少使用）缓存策略
 *          - 缓存大小控制和内存优化
 *          - 缓存命中率统计和性能监控
 *          - 瓦片项的创建、存储和释放
 *          - 内存不足时的自动清理机制
 *
 * @note    该类是DSV项目性能优化的关键组件，通过缓存机制
 *          减少瓦片图形项的重复创建，提升渲染性能
 * @see     WSITileGraphicsItem, TileCache, TileManager
 */

#pragma once

#include <QObject>
#include <TileCache.hpp>

class WSITileGraphicsItem;

/**
 * @brief   瓦片图形项缓存类
 * @details 继承自QObject和TileCache模板类，专门负责管理瓦片图形项的内存缓存。
 *          采用LRU（最近最少使用）策略进行缓存管理，当内存不足时自动清理
 *          最久未使用的瓦片图形项。该类是DSV项目性能优化的核心组件，
 *          通过缓存机制显著提升图像浏览的流畅性。
 *
 * @note    该类使用模板特化，专门缓存WSITileGraphicsItem*类型的对象。
 *          缓存键为位置字符串（x-y-level格式），值为瓦片项和大小信息。
 *
 * @example
 * @code
 * // 创建瓦片图形项缓存
 * WSITileGraphicsItemCache* cache = new WSITileGraphicsItemCache();
 *
 * // 存储瓦片图形项
 * WSITileGraphicsItem* tileItem = createTileItem(x, y, level);
 * cache->set(key, tileItem, tileSize);
 *
 * // 获取瓦片图形项
 * WSITileGraphicsItem* cachedItem;
 * unsigned int size;
 * cache->get(key, cachedItem, size);
 * @endcode
 */
class WSITileGraphicsItemCache : public QObject, public TileCache<WSITileGraphicsItem* > {
	Q_OBJECT

public:
	/**
	 * @brief   析构函数
	 * @details 清理缓存中的所有瓦片图形项，释放内存资源
	 */
	~WSITileGraphicsItemCache();

	/**
	 * @brief   清空缓存
	 * @details 移除缓存中的所有瓦片图形项，释放所有内存。
	 *          在图像切换或内存不足时调用此函数。
	 */
	void clear();

	/**
	 * @brief   获取瓦片图形项
	 * @param   k       缓存键（位置字符串）
	 * @param   tile    输出参数：瓦片图形项指针
	 * @param   size    输出参数：瓦片大小
	 * @details 从缓存中获取指定键的瓦片图形项。如果缓存命中，
	 *          返回缓存的瓦片项；如果缓存未命中，tile参数为nullptr。
	 *          同时更新LRU列表，将访问的项移到最前面。
	 *
	 * @note    该函数是线程安全的
	 */
	void get(const keyType& k, WSITileGraphicsItem*& tile, unsigned int& size);

	/**
	 * @brief   存储瓦片图形项
	 * @param   k           缓存键（位置字符串）
	 * @param   v           瓦片图形项指针
	 * @param   size        瓦片大小
	 * @param   topLevel    是否为顶层项（优先级更高）
	 * @return  操作结果：0表示成功，-1表示失败
	 * @details 将瓦片图形项存储到缓存中。如果缓存已满，
	 *          会自动清理最久未使用的项。支持顶层项标记，
	 *          顶层项具有更高的缓存优先级。
	 *
	 * @note    该函数是线程安全的，会自动处理内存不足的情况
	 */
	int set(const keyType& k, WSITileGraphicsItem* v, unsigned int size, bool topLevel = false);

	/**
	 * @brief   获取所有缓存项
	 * @return  所有瓦片图形项的指针向量
	 * @details 返回缓存中所有瓦片图形项的指针，用于调试、
	 *          统计或批量操作。
	 *
	 * @note    返回的向量包含缓存中所有项的副本
	 */
	std::vector<WSITileGraphicsItem*> getAllItems();

protected:
	/**
	 * @brief   清理缓存项
	 * @details 当缓存大小超过限制时，自动清理最久未使用的项。
	 *          清理过程会发送itemEvicted信号通知被清理的项。
	 *          该函数由基类TileCache调用。
	 *
	 * @note    该函数是虚函数重写，实现具体的清理逻辑
	 */
	void evict();

private:
	/**
	 * @brief   缓存数据结构
	 * @details 缓存的核心数据结构，使用std::map存储：
	 *          - 键：位置字符串（x-y-level格式）
	 *          - 值：包含瓦片项指针、大小和LRU迭代器的pair
	 *
	 * 结构说明：
	 * - 外层pair：瓦片项信息 + LRU迭代器
	 * - 内层pair：瓦片项指针 + 大小
	 * - keyTypeList::iterator：指向LRU列表中的位置
	 *
	 * @note    该结构支持O(log n)的查找和O(1)的LRU更新
	 */
	std::map<keyType, std::pair<std::pair<WSITileGraphicsItem*, unsigned int>, keyTypeList::iterator> > _cache;

signals:
	/**
	 * @brief   项被清理信号
	 * @param   item    被清理的瓦片图形项指针
	 * @details 当瓦片图形项因缓存策略被清理时，发送此信号。
	 *          可用于监控缓存行为、统计清理频率等。
	 *
	 * @note    该信号在evict()函数中发送
	 */
	void itemEvicted(WSITileGraphicsItem* item);
};