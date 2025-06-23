/**
 * @file TileManager.cpp
 * @brief 瓦片管理器实现文件
 * @details 该文件实现了多分辨率图像的瓦片管理功能，包括：
 *          - 瓦片的加载、缓存和渲染
 *          - 视野范围管理
 *          - 瓦片覆盖状态跟踪
 *          - 多线程瓦片加载协调
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "TileManager.h"
#include "MultiResolutionImage.h"
#include "IOThread.h"
#include "ImageSource.h"
#include "WSITileGraphicsItem.h"
#include "WSITileGraphicsItemCache.h"

/**
 * @brief 构造函数：初始化瓦片管理器
 * @param img 多分辨率图像对象
 * @param tileSize 瓦片大小
 * @param lastRenderLevel 最后渲染层级
 * @param ioThread IO线程对象
 * @param cache 瓦片缓存对象
 * @param scene Qt图形场景对象
 * @details 初始化瓦片管理器的所有成员变量，包括：
 *          - IO线程和缓存引用
 *          - 瓦片大小和渲染层级
 *          - 层级降采样比例和尺寸信息
 *          - 覆盖状态管理
 */
TileManager::TileManager(std::shared_ptr<MultiResolutionImage> img, unsigned int tileSize, unsigned int lastRenderLevel, IOThread* ioThread, WSITileGraphicsItemCache* cache, QGraphicsScene* scene) :
    _ioThread(ioThread),
    _tileSize(tileSize),
    _lastRenderLevel(lastRenderLevel),
    _lastFOV(),
    _lastLevel(),
    _coverage(),
    _cache(cache),
    _scene(scene),
    _coverageMaps(),
    _coverageMapCacheMode(false),
    _renderForeground(true)
{
    for (unsigned int i = 0; i < img->getNumberOfLevels(); ++i) {
        _levelDownsamples.push_back(img->getLevelDownsample(i));
        _levelDimensions.push_back(img->getLevelDimensions(i));
    }
}

/**
 * @brief 析构函数：清理瓦片管理器资源
 * @details 清理所有指针引用，但不删除对象（由外部管理）
 */
TileManager::~TileManager() {
    _ioThread = NULL;
    _cache = NULL;
    _scene = NULL;
}

/**
 * @brief 重置指定层级的覆盖状态
 * @param level 要重置的层级
 * @details 清空指定层级的覆盖状态映射和覆盖路径
 */
void TileManager::resetCoverage(unsigned int level) {
    _coverage[level] = std::map<int, std::map<int, unsigned char> >();
    if (_coverageMaps.size() > level) {
        _coverageMaps[level] = QPainterPath();
    }
}

/**
 * @brief 将像素坐标转换为瓦片坐标
 * @param coordinate 像素坐标
 * @param level 图像层级
 * @return 瓦片坐标
 * @details 根据层级降采样比例和瓦片大小计算瓦片坐标
 */
QPoint TileManager::pixelCoordinatesToTileCoordinates(QPointF coordinate, unsigned int level) {
    if (level < _levelDownsamples.size()) {
        return QPoint(std::floor((coordinate.x() / _levelDownsamples[level]) / this->_tileSize), std::floor((coordinate.y() / _levelDownsamples[level]) / this->_tileSize));
    }
    else {
        return QPoint();
    }
}

/**
 * @brief 将瓦片坐标转换为像素坐标
 * @param coordinate 瓦片坐标
 * @param level 图像层级
 * @return 像素坐标
 * @details 根据层级降采样比例和瓦片大小计算像素坐标
 */
QPointF TileManager::tileCoordinatesToPixelCoordinates(QPoint coordinate, unsigned int level) {
    if (level < _levelDownsamples.size()) {
        return QPointF(coordinate.x() * _levelDownsamples[level] * this->_tileSize, coordinate.y() * _levelDownsamples[level] * this->_tileSize);
    }
    else {
        return QPoint();
    }
}

/**
 * @brief 获取指定层级的瓦片数量
 * @param level 图像层级
 * @return 瓦片数量（宽度、高度）
 * @details 根据层级尺寸和瓦片大小计算瓦片网格的尺寸
 */
QPoint TileManager::getLevelTiles(unsigned int level) {
    if (level < _levelDimensions.size()) {
        std::vector<unsigned long long> dims = _levelDimensions[level];
        return QPoint(std::ceil(dims[0] / static_cast<float>(_tileSize)), std::ceil(dims[1] / static_cast<float>(_tileSize)));
    }
    else {
        return QPoint();
    }
}

/**
 * @brief 加载指定层级的所有瓦片
 * @param level 要加载的层级
 * @details 加载指定层级的所有瓦片，用于预加载或全图显示
 */
void TileManager::loadAllTilesForLevel(unsigned int level) {
    if (_ioThread) {
        if (level < _levelDownsamples.size()) {
            std::vector<unsigned long long> baseLevelDims = _levelDimensions[0];
            this->loadTilesForFieldOfView(QRectF(0, 0, baseLevelDims[0], baseLevelDims[1]), level);
        }
    }
}

/**
 * @brief 为指定视野范围加载瓦片
 * @param FOV 视野范围（像素坐标）
 * @param level 图像层级
 * @details 计算视野范围内的瓦片坐标，并为未加载的瓦片创建加载任务
 *          避免重复加载已存在的瓦片
 */
void TileManager::loadTilesForFieldOfView(const QRectF& FOV, const unsigned int level) {
    if (level > _lastRenderLevel) {
        return;
    }
    if (_ioThread) {
        QPoint topLeftTile = this->pixelCoordinatesToTileCoordinates(FOV.topLeft(), level);
        QPoint bottomRightTile = this->pixelCoordinatesToTileCoordinates(FOV.bottomRight(), level);
        QRect FOVTile = QRect(topLeftTile, bottomRightTile);
        QPoint nrTiles = getLevelTiles(level);
        float levelDownsample = _levelDownsamples[level];
        if (FOVTile != _lastFOV || level != _lastLevel) {
            _lastLevel = level;
            _lastFOV = FOVTile;
            for (int x = topLeftTile.x(); x <= bottomRightTile.x(); ++x) {
                if (x >= 0 && x <= nrTiles.x()) {
                    for (int y = topLeftTile.y(); y <= bottomRightTile.y(); ++y) {
                        if (y >= 0 && y <= nrTiles.y()) {
                            if (providesCoverage(level, x, y) < 1) {
                                setCoverage(level, x, y, 1);
                                _ioThread->addJob(_tileSize, x, y, level);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * @brief 更新瓦片前景
 * @details 清空当前任务队列，等待所有工作线程完成，然后为已加载的瓦片创建前景渲染任务
 */
void TileManager::updateTileForegounds() {
    _ioThread->clearJobs();
    while (_ioThread->getWaitingThreads() != _ioThread->getWorkers().size()) {
    }
    QCoreApplication::processEvents();
    if (_cache) {
        std::vector<WSITileGraphicsItem*> cachedTiles = _cache->getAllItems();
        for (auto item : cachedTiles) {
            unsigned int tileSize = item->getTileSize();
            unsigned int tileLevel = item->getTileLevel();
            unsigned int tileX = item->getTileX();
            unsigned int tileY = item->getTileY();
            if (providesCoverage(tileLevel, tileX, tileY) == 2) {
                ImageSource* foregroundTile = item->getForegroundTile()->clone();
                _ioThread->addJob(tileSize, tileX, tileY, tileLevel, foregroundTile);
            }
        }
    }
}

/**
 * @brief 前景瓦片渲染完成回调
 * @param tile 渲染的前景瓦片
 * @param tileX 瓦片X坐标
 * @param tileY 瓦片Y坐标
 * @param tileLevel 瓦片层级
 * @details 将渲染完成的前景瓦片设置到对应的瓦片图形项中
 */
void TileManager::onForegroundTileRendered(QPixmap* tile, unsigned int tileX, unsigned int tileY, unsigned int tileLevel) {
    if (_cache) {
        std::stringstream ss;
        ss << tileX << "_" << tileY << "_" << tileLevel;
        std::string key;
        ss >> key;

        WSITileGraphicsItem* item = NULL;
        unsigned int size = 0;
        _cache->get(key, item, size);
        if (item) {
            if (tile) {
                item->setForegroundPixmap(tile);
            }
            setCoverage(tileLevel, tileX, tileY, 2);
        }
        else {
            setCoverage(tileLevel, tileX, tileY, 0);
        }
    }
}

/**
 * @brief 瓦片加载完成回调
 * @param tile 加载的瓦片
 * @param tileX 瓦片X坐标
 * @param tileY 瓦片Y坐标
 * @param tileSize 瓦片大小
 * @param tileByteSize 瓦片字节大小
 * @param tileLevel 瓦片层级
 * @param foregroundTile 前景瓦片
 * @param foregroundPixmap 前景像素图
 * @details 创建瓦片图形项并添加到场景中，同时更新缓存和覆盖状态
 */
void TileManager::onTileLoaded(QPixmap* tile, unsigned int tileX, unsigned int tileY, unsigned int tileSize, unsigned int tileByteSize, unsigned int tileLevel, ImageSource* foregroundTile, QPixmap* foregroundPixmap) {
    if (tile) {
        WSITileGraphicsItem* item = new WSITileGraphicsItem(tile, tileX, tileY, tileSize, tileByteSize, tileLevel, _lastRenderLevel, _levelDownsamples, this, foregroundPixmap, foregroundTile, _foregroundOpacity, _renderForeground);
        std::stringstream ss;
        ss << tileX << "_" << tileY << "_" << tileLevel;
        std::string key;
        ss >> key;
        if (_scene) {
            setCoverage(tileLevel, tileX, tileY, 2);
            float tileDownsample = _levelDownsamples[tileLevel];
            float maxDownsample = _levelDownsamples[_lastRenderLevel];
            float posX = (tileX * tileDownsample * tileSize) / maxDownsample + ((tileSize * tileDownsample) / (2 * maxDownsample));
            float posY = (tileY * tileDownsample * tileSize) / maxDownsample + ((tileSize * tileDownsample) / (2 * maxDownsample));
            _scene->addItem(item);
            item->setPos(posX, posY);
            item->setZValue(1. / ((float)tileLevel + 1.));
        }
        if (_cache) {
            _cache->set(key, item, tileByteSize, tileLevel == _lastRenderLevel);
        }
    }
    else {
        setCoverage(tileLevel, tileX, tileY, 0);
    }
}

/**
 * @brief 瓦片移除回调
 * @param tile 要移除的瓦片
 * @details 从场景中移除瓦片图形项，更新覆盖状态并删除对象
 */
void TileManager::onTileRemoved(WSITileGraphicsItem* tile) {
    _scene->removeItem(tile);
    setCoverage(tile->getTileLevel(), tile->getTileX(), tile->getTileY(), 0);
    delete tile;
}

/**
 * @brief 前景透明度改变回调
 * @param opacity 新的透明度值
 * @details 更新所有缓存瓦片的前景透明度
 */
void TileManager::onForegroundOpacityChanged(float opacity) {
    _foregroundOpacity = opacity;
    std::vector<WSITileGraphicsItem*> cachedTiles = _cache->getAllItems();
    for (auto item : cachedTiles) {
        item->setForegroundOpacity(opacity);
    }
}

/**
 * @brief 前景渲染开关改变回调
 * @param renderForeground 是否渲染前景
 * @details 更新所有缓存瓦片的前景渲染开关
 */
void TileManager::onRenderForegroundChanged(bool renderForeground) {
    _renderForeground = renderForeground;
    std::vector<WSITileGraphicsItem*> cachedTiles = _cache->getAllItems();
    for (auto item : cachedTiles) {
        item->setRenderForeground(renderForeground);
    }
}

/**
 * @brief 设置覆盖图模式为缓存模式
 * @details 在缓存模式下，移除瓦片时会从覆盖图中减去对应区域
 */
void TileManager::setCoverageMapModeToCache() {
    _coverageMapCacheMode = true;
}

/**
 * @brief 设置覆盖图模式为访问模式
 * @details 在访问模式下，移除瓦片时不会从覆盖图中减去对应区域
 */
void TileManager::setCoverageMapModeToVisited() {
    _coverageMapCacheMode = false;
}

/**
 * @brief 获取瓦片覆盖状态
 * @param level 图像层级
 * @param tile_x 瓦片X坐标
 * @param tile_y 瓦片Y坐标
 * @return 覆盖状态（0=未覆盖，1=加载中，2=已覆盖）
 * @details 返回指定瓦片的覆盖状态，如果坐标为负数则检查整个层级
 */
unsigned char TileManager::providesCoverage(unsigned int level, int tile_x, int tile_y) {
    std::map<int, std::map<int, unsigned char> >& cover_level = _coverage[level];
    if (cover_level.empty()) {
        return 0;
    }

    if (tile_x < 0 || tile_y < 0) {
        for (std::map<int, std::map<int, unsigned char> >::iterator it_x = cover_level.begin(); it_x != cover_level.end(); ++it_x) {
            for (std::map<int, unsigned char>::iterator it_y = it_x->second.begin(); it_y != it_x->second.end(); ++it_y) {
                if (it_y->second != 2) {
                    return 0;
                }
            }
        }
        return 2;
    }

    return cover_level[tile_x][tile_y];
}

/**
 * @brief 检查瓦片是否被覆盖
 * @param level 图像层级
 * @param tile_x 瓦片X坐标
 * @param tile_y 瓦片Y坐标
 * @return 是否被覆盖
 * @details 检查指定瓦片是否被覆盖，对于非0层级会检查下级瓦片的覆盖状态
 */
bool TileManager::isCovered(unsigned int level, int tile_x, int tile_y) {
    if (level > 0) {
        if (tile_x < 0 || tile_y < 0) {
            return providesCoverage(level) == 2;
        }
        else {
            bool covered = true;
            unsigned int downsample = _levelDownsamples[level] / _levelDownsamples[level - 1];
            for (unsigned int x = 0; x < downsample; ++x) {
                for (unsigned int y = 0; y < downsample; ++y) {
                    covered &= providesCoverage(level - 1, downsample * tile_x + x, downsample * tile_y + y) == 2;
                }
            }
            return covered;
        }
    }
    else {
        return false;
    }
}

/**
 * @brief 设置瓦片覆盖状态
 * @param level 图像层级
 * @param tile_x 瓦片X坐标
 * @param tile_y 瓦片Y坐标
 * @param covers 覆盖状态
 * @details 设置指定瓦片的覆盖状态，并更新覆盖图路径
 */
void TileManager::setCoverage(unsigned int level, int tile_x, int tile_y, unsigned char covers) {
    _coverage[level][tile_x][tile_y] = covers;
    if (_coverageMaps.empty()) {
        _coverageMaps.resize(_lastRenderLevel + 1);
    }
    if (level != _lastRenderLevel) {
        if (covers == 2 || covers == 0) {
            float rectSize = _tileSize / (_levelDownsamples[_lastRenderLevel] / _levelDownsamples[level]);
            QPainterPath rect;
            rect.addRect(QRectF(tile_x * rectSize - 1, tile_y * rectSize - 1, rectSize + 1, rectSize + 1));
            if (covers == 2) {
                _coverageMaps[level] = _coverageMaps[level].united(rect);
            }
            else if (covers == 0) {
                if (_coverageMapCacheMode) {
                    _coverageMaps[level] = _coverageMaps[level].subtracted(rect);
                }
            }
        }
    }
    emit coverageUpdated();
}

/**
 * @brief 获取覆盖图路径
 * @return 覆盖图路径向量
 * @details 返回所有层级的覆盖图路径，用于可视化显示
 */
std::vector<QPainterPath> TileManager::getCoverageMaps() {
    return _coverageMaps;
}

/**
 * @brief 清空所有瓦片
 * @details 清空任务队列、缓存、场景中的所有瓦片，并重置覆盖状态
 */
void TileManager::clear() {
    _ioThread->clearJobs();
    while (_ioThread->getWaitingThreads() != _ioThread->getWorkers().size()) {
    }
    QCoreApplication::processEvents();
    if (_cache) {
        _cache->clear();
    }
    QList<QGraphicsItem*> itms = _scene->items();
    for (QList<QGraphicsItem*>::iterator it = itms.begin(); it != itms.end(); ++it) {
        WSITileGraphicsItem* itm = dynamic_cast<WSITileGraphicsItem*>((*it));
        if (itm) {
            _scene->removeItem(itm);
            delete itm;
        }
    }
    _coverage.clear();
    _coverageMaps.clear();
    emit coverageUpdated();
}

/**
 * @brief 刷新瓦片管理器
 * @details 清空所有瓦片后重新加载最后的视野范围
 */
void TileManager::refresh() {
    clear();
    this->reloadLastFOV();
}

/**
 * @brief 重新加载最后的视野范围
 * @details 保存当前的视野范围，清空后重新加载所有层级和指定视野范围的瓦片
 */
void TileManager::reloadLastFOV() {
    QRect FOV = _lastFOV;
    QPoint topLeft = FOV.topLeft();
    QPoint bottomRight = FOV.bottomRight();
    _lastFOV = QRect();
    unsigned int level = _lastLevel;
    loadAllTilesForLevel(_lastRenderLevel);
    loadTilesForFieldOfView(QRectF(tileCoordinatesToPixelCoordinates(topLeft, level), tileCoordinatesToPixelCoordinates(bottomRight, level)), level);
}