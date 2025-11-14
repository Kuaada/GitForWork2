/**
 * @file    TileManager.h
 * @brief   瓦片管理器类，负责多分辨率图像的瓦片加载、缓存和渲染管理
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的瓦片管理系统，包括：
 *          - 多分辨率图像的瓦片分割和管理
 *          - 瓦片加载和缓存策略
 *          - 视场（FOV）相关的瓦片调度
 *          - 瓦片覆盖度跟踪和管理
 *          - 前景瓦片渲染管理
 *          - 与IO线程和图形场景的协调
 *          该类是DSV项目中图像显示性能的核心组件，
 *          通过智能的瓦片管理实现高效的大图像浏览。
 *
 * @note    该类继承自QObject，支持Qt的信号槽机制
 * @see     MultiResolutionImage, IOThread, WSITileGraphicsItem, WSITileGraphicsItemCache
 */

#pragma once

#include <QObject>
#include <QRect>
#include <QGraphicsScene>
#include <QPainterPath>
#include <cmath>
#include <QCoreApplication>
#include <sstream>
#include <QPointer>

 // 前向声明
class IOThread;
class MultiResolutionImage;
class ImageSource;
class WSITileGraphicsItem;
class WSITileGraphicsItemCache;

/**
 * @class  TileManager
 * @brief  瓦片管理器类，负责多分辨率图像的瓦片加载、缓存和渲染管理
 * @details 该类是DSV项目中瓦片系统的核心组件，负责管理多分辨率图像的
 *          瓦片分割、加载、缓存和渲染。通过智能的瓦片调度策略，实现
 *          高效的大图像浏览，支持视场相关的瓦片加载和覆盖度跟踪。
 *
 *          主要功能包括：
 *          - 多分辨率瓦片分割和管理
 *          - 视场相关的瓦片调度
 *          - 瓦片覆盖度跟踪和可视化
 *          - 前景瓦片渲染管理
 *          - 与IO线程的异步协调
 *          - 瓦片缓存策略管理
 *
 * @note   该类继承自QObject，支持Qt的信号槽机制
 * @example
 *          // 使用示例
 *          auto image = std::make_shared<MultiResolutionImage>();
 *          TileManager* manager = new TileManager(image, 256, 0, ioThread, cache, scene);
 *
 *          // 加载指定视场的瓦片
 *          QRectF fov(0, 0, 1000, 1000);
 *          manager->loadTilesForFieldOfView(fov, 0);
 * @see     MultiResolutionImage, IOThread, WSITileGraphicsItem
 */
class TileManager : public QObject {
    Q_OBJECT

private:
    /** @brief 各层级的下采样因子数组 */
    std::vector<float> _levelDownsamples;

    /** @brief 各层级的图像尺寸数组，每个元素包含[宽度, 高度] */
    std::vector<std::vector<unsigned long long> > _levelDimensions;

    /** @brief 瓦片大小（像素） */
    unsigned int _tileSize;

    /** @brief 最后一个视场（Field of View）的矩形区域 */
    QRect _lastFOV;

    /** @brief 最后一个请求的层级 */
    unsigned int _lastLevel;

    /** @brief 最后一个渲染层级 */
    unsigned int _lastRenderLevel;

    /**
     * @brief 瓦片覆盖度映射表
     * @details 三级映射：层级 -> X坐标 -> Y坐标 -> 覆盖度值
     *          用于跟踪每个瓦片的加载和渲染状态
     */
    std::map<unsigned int, std::map<int, std::map<int, unsigned char> > > _coverage;

    /** @brief IO线程指针，用于异步瓦片加载 */
    QPointer<IOThread> _ioThread;

    /** @brief 瓦片图形项缓存指针 */
    QPointer<WSITileGraphicsItemCache> _cache;

    /** @brief Qt图形场景指针 */
    QPointer<QGraphicsScene> _scene;

    /** @brief 覆盖度地图的绘制路径数组 */
    std::vector<QPainterPath> _coverageMaps;

    /** @brief 覆盖度地图缓存模式标志 */
    bool _coverageMapCacheMode;

    /** @brief 前景瓦片透明度 */
    float _foregroundOpacity;

    /** @brief 是否渲染前景瓦片标志 */
    bool _renderForeground;

    /**
     * @brief   将像素坐标转换为瓦片坐标
     * @details 根据指定的层级将像素坐标转换为对应的瓦片坐标
     *
     * @param   coordinate 像素坐标
     * @param   level 层级索引
     * @return  瓦片坐标
     * @note    该函数用于计算像素位置对应的瓦片索引
     */
    QPoint pixelCoordinatesToTileCoordinates(QPointF coordinate, unsigned int level);

    /**
     * @brief   将瓦片坐标转换为像素坐标
     * @details 根据指定的层级将瓦片坐标转换为对应的像素坐标
     *
     * @param   coordinate 瓦片坐标
     * @param   level 层级索引
     * @return  像素坐标
     * @note    该函数用于计算瓦片索引对应的像素位置
     */
    QPointF tileCoordinatesToPixelCoordinates(QPoint coordinate, unsigned int level);

    /**
     * @brief   获取指定层级的瓦片数量
     * @details 计算指定层级在X和Y方向上的瓦片数量
     *
     * @param   level 层级索引
     * @return  瓦片数量，x()为X方向数量，y()为Y方向数量
     */
    QPoint getLevelTiles(unsigned int level);

    /**
     * @brief   私有拷贝构造函数
     * @details 禁用拷贝构造，确保TileManager对象的唯一性
     *
     * @param   that 要拷贝的TileManager对象
     * @note    该构造函数是私有的，防止意外拷贝
     */
    TileManager(const TileManager& that);

signals:
    /**
     * @brief   覆盖度更新信号
     * @details 当瓦片覆盖度发生变化时发出此信号
     * @note    该信号用于通知UI更新覆盖度显示
     */
    void coverageUpdated();

public:
    /**
     * @brief   构造函数
     * @details 创建瓦片管理器对象，初始化多分辨率图像、瓦片大小、
     *          IO线程、缓存和图形场景等组件
     *
     * @param   img 多分辨率图像对象的共享指针
     * @param   tileSize 瓦片大小（像素）
     * @param   lastRenderLevel 最后渲染层级
     * @param   renderThread IO渲染线程指针
     * @param   _cache 瓦片图形项缓存指针
     * @param   scene Qt图形场景指针
     * @note    构造函数会初始化所有成员变量，确保item指针为NULL
     */
    TileManager(std::shared_ptr<MultiResolutionImage> img, unsigned int tileSize, unsigned int lastRenderLevel, IOThread* renderThread, WSITileGraphicsItemCache* _cache, QGraphicsScene* scene);

    /**
     * @brief   析构函数
     * @details 清理瓦片管理器资源，包括瓦片缓存和IO线程
     * @note    析构函数会自动清理所有相关的资源
     */
    ~TileManager();

    /**
     * @brief   加载指定层级的所有瓦片
     * @details 预加载指定层级的所有瓦片，用于全图预览或离线处理
     *
     * @param   level 要加载的层级索引
     * @note    该操作会消耗大量内存，请谨慎使用
     * @see     loadTilesForFieldOfView
     */
    void loadAllTilesForLevel(unsigned int level);

    /**
     * @brief   加载指定视场的瓦片
     * @details 根据视场矩形和层级加载相应的瓦片，实现视场相关的瓦片调度
     *
     * @param   FOV 视场矩形（像素坐标）
     * @param   level 层级索引
     * @note    该函数会智能调度瓦片加载，优先加载视场内的瓦片
     * @see     loadAllTilesForLevel, reloadLastFOV
     */
    void loadTilesForFieldOfView(const QRectF& FOV, const unsigned int level);

    /**
     * @brief   更新瓦片前景
     * @details 更新所有已加载瓦片的前景渲染，应用当前的透明度设置
     * @note    该函数会遍历所有瓦片并更新其前景显示
     * @see     onForegroundOpacityChanged
     */
    void updateTileForegounds();

    /**
     * @brief   重置指定层级的覆盖度
     * @details 清除指定层级的所有瓦片覆盖度信息
     *
     * @param   level 要重置的层级索引
     * @note    该操作会清除该层级的所有覆盖度记录
     * @see     setCoverage, providesCoverage
     */
    void resetCoverage(unsigned int level);

    /**
     * @brief   获取瓦片覆盖度
     * @details 获取指定瓦片的覆盖度值，用于跟踪瓦片的加载状态
     *
     * @param   level 层级索引
     * @param   tile_x 瓦片X坐标，-1表示查询整个层级
     * @param   tile_y 瓦片Y坐标，-1表示查询整个层级
     * @return  覆盖度值，0表示未覆盖，255表示完全覆盖
     * @note    该函数用于查询瓦片的加载和渲染状态
     * @see     setCoverage, isCovered
     */
    unsigned char providesCoverage(unsigned int level, int tile_x = -1, int tile_y = -1);

    /**
     * @brief   检查瓦片是否已覆盖
     * @details 检查指定瓦片是否已经加载和渲染完成
     *
     * @param   level 层级索引
     * @param   tile_x 瓦片X坐标，-1表示检查整个层级
     * @param   tile_y 瓦片Y坐标，-1表示检查整个层级
     * @return  true表示已覆盖，false表示未覆盖
     * @note    该函数是providesCoverage的布尔版本
     * @see     providesCoverage, setCoverage
     */
    bool isCovered(unsigned int level, int tile_x = -1, int tile_y = -1);

    /**
     * @brief   设置瓦片覆盖度
     * @details 设置指定瓦片的覆盖度值，用于跟踪瓦片的加载状态
     *
     * @param   level 层级索引
     * @param   tile_x 瓦片X坐标
     * @param   tile_y 瓦片Y坐标
     * @param   covers 覆盖度值（0-255）
     * @note    该函数用于更新瓦片的加载和渲染状态
     * @see     providesCoverage, resetCoverage
     */
    void setCoverage(unsigned int level, int tile_x, int tile_y, unsigned char covers);

    /**
     * @brief   获取覆盖度地图
     * @details 获取所有层级的覆盖度地图绘制路径，用于可视化显示
     *
     * @return  覆盖度地图的绘制路径数组
     * @note    返回的路径可以用于在UI中绘制覆盖度可视化
     * @see     setCoverageMapModeToCache, setCoverageMapModeToVisited
     */
    std::vector<QPainterPath> getCoverageMaps();

    /**
     * @brief   设置覆盖度地图模式为缓存模式
     * @details 将覆盖度地图模式设置为显示缓存中的瓦片
     * @note    该模式会显示所有已缓存的瓦片
     * @see     setCoverageMapModeToVisited, getCoverageMaps
     */
    void setCoverageMapModeToCache();

    /**
     * @brief   设置覆盖度地图模式为访问模式
     * @details 将覆盖度地图模式设置为显示已访问的瓦片
     * @note    该模式会显示所有已访问过的瓦片
     * @see     setCoverageMapModeToCache, getCoverageMaps
     */
    void setCoverageMapModeToVisited();

    /**
     * @brief   清空所有瓦片
     * @details 清空所有已加载的瓦片和缓存，释放内存
     * @note    该操作会清除所有瓦片数据，请谨慎使用
     * @see     refresh
     */
    void clear();

    /**
     * @brief   刷新瓦片显示
     * @details 刷新所有瓦片的显示，重新渲染当前视场
     * @note    该函数会重新加载当前视场的瓦片
     * @see     clear, reloadLastFOV
     */
    void refresh();

    /**
     * @brief   重新加载最后一个视场
     * @details 重新加载最后一个请求的视场瓦片
     * @note    该函数用于重新加载之前请求的视场
     * @see     loadTilesForFieldOfView, refresh
     */
    void reloadLastFOV();

public slots:
    /**
     * @brief   前景瓦片渲染完成槽函数
     * @details 当前景瓦片渲染完成时调用此槽函数
     *
     * @param   tile 渲染完成的瓦片像素图
     * @param   tileX 瓦片X坐标
     * @param   tileY 瓦片Y坐标
     * @param   tileLevel 瓦片层级
     * @note    该槽函数由IO线程调用，用于通知渲染完成
     */
    void onForegroundTileRendered(QPixmap* tile, unsigned int tileX, unsigned int tileY, unsigned int tileLevel);

    /**
     * @brief   瓦片加载完成槽函数
     * @details 当瓦片加载完成时调用此槽函数
     *
     * @param   tile 加载完成的瓦片像素图
     * @param   tileX 瓦片X坐标
     * @param   tileY 瓦片Y坐标
     * @param   tileSize 瓦片大小
     * @param   tileByteSize 瓦片字节大小
     * @param   tileLevel 瓦片层级
     * @param   foregroundTile 前景瓦片图像源
     * @param   foregroundPixmap 前景瓦片像素图
     * @note    该槽函数由IO线程调用，用于通知加载完成
     */
    void onTileLoaded(QPixmap* tile, unsigned int tileX, unsigned int tileY, unsigned int tileSize, unsigned int tileByteSize, unsigned int tileLevel, ImageSource* foregroundTile, QPixmap* foregroundPixmap);

    /**
     * @brief   瓦片移除槽函数
     * @details 当瓦片从缓存中移除时调用此槽函数
     *
     * @param   tile 被移除的瓦片图形项
     * @note    该槽函数由缓存系统调用，用于通知瓦片移除
     */
    void onTileRemoved(WSITileGraphicsItem* tile);

    /**
     * @brief   前景透明度改变槽函数
     * @details 当前景瓦片透明度设置改变时调用此槽函数
     *
     * @param   opacity 新的透明度值（0.0-1.0）
     * @note    该槽函数会更新所有瓦片的前景透明度
     * @see     updateTileForegounds
     */
    void onForegroundOpacityChanged(float opacity);

    /**
     * @brief   前景渲染开关改变槽函数
     * @details 当前景瓦片渲染开关状态改变时调用此槽函数
     *
     * @param   renderForeground 是否渲染前景瓦片
     * @note    该槽函数会更新前景瓦片的显示状态
     */
    void onRenderForegroundChanged(bool renderForeground);
};