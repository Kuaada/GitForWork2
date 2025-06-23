/**
 * @file WSITileGraphicsItem.cpp
 * @brief WSI瓦片图形项实现文件
 * @details 该文件实现了数字病理切片瓦片的图形项功能，包括：
 *          - 瓦片的渲染和显示
 *          - 多层级细节管理
 *          - 前景图像叠加
 *          - 透明度控制
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "WSITileGraphicsItem.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include "TileManager.h"
#include <iostream>
#include "ImageSource.h"

/**
 * @brief 构造函数：初始化WSI瓦片图形项
 * @param item 瓦片像素图
 * @param tileX 瓦片X坐标
 * @param tileY 瓦片Y坐标
 * @param tileSize 瓦片大小
 * @param tileByteSize 瓦片字节大小
 * @param itemLevel 瓦片层级
 * @param lastRenderLevel 最后渲染层级
 * @param imgDownsamples 图像降采样比例向量
 * @param manager 瓦片管理器
 * @param foregroundPixmap 前景像素图
 * @param foregroundTile 前景瓦片
 * @param foregroundOpacity 前景透明度
 * @param renderForeground 是否渲染前景
 * @details 初始化瓦片图形项的所有属性，计算物理尺寸和LOD范围
 */
WSITileGraphicsItem::WSITileGraphicsItem(QPixmap* item, unsigned int tileX, unsigned int tileY, unsigned int tileSize, unsigned int tileByteSize, unsigned int itemLevel, unsigned int lastRenderLevel, const std::vector<float>& imgDownsamples, TileManager* manager, QPixmap* foregroundPixmap, ImageSource* foregroundTile, float foregroundOpacity, bool renderForeground) :
    QGraphicsItem(),
    _item(NULL),
    _manager(NULL),
    _tileX(tileX),
    _tileY(tileY),
    _tileSize(tileSize),
    _tileByteSize(tileByteSize),
    _itemLevel(itemLevel),
    _lastRenderLevel(lastRenderLevel),
    _foregroundPixmap(foregroundPixmap),
    _foregroundTile(foregroundTile),
    _foregroundOpacity(foregroundOpacity),
    _renderForeground(renderForeground)
{
    if (item) {
        _item = item;
    }
    if (manager) {
        _manager = manager;
    }
    _physicalSize = _tileSize / (imgDownsamples[_lastRenderLevel] / imgDownsamples[_itemLevel]);
    float lastRenderLevelDownsample = imgDownsamples[_lastRenderLevel];
    float itemLevelLOD = lastRenderLevelDownsample / imgDownsamples[_itemLevel];
    if (_itemLevel == _lastRenderLevel) {
        _lowerLOD = 0.;
    }
    else {
        float avgDownsample = (imgDownsamples[_itemLevel + 1] + imgDownsamples[_itemLevel]) / 2.;
        _lowerLOD = lastRenderLevelDownsample / avgDownsample;
    }
    if (_itemLevel == 0) {
        _upperLOD = std::numeric_limits<float>::max();
    }
    else {
        float avgDownsample = (imgDownsamples[_itemLevel - 1] + imgDownsamples[_itemLevel]) / 2.;
        _upperLOD = lastRenderLevelDownsample / avgDownsample;
    }
    this->setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
    _boundingRect = QRectF(-_physicalSize / 2., -_physicalSize / 2., _physicalSize, _physicalSize);
}

/**
 * @brief 析构函数：清理瓦片图形项资源
 * @details 删除瓦片像素图、前景像素图和前景瓦片对象
 */
WSITileGraphicsItem::~WSITileGraphicsItem() {
    if (_item) {
        delete _item;
        _item = NULL;
    }
    if (_foregroundPixmap) {
        delete _foregroundPixmap;
        _foregroundPixmap = NULL;
    }
    if (_foregroundTile) {
        delete _foregroundTile;
        _foregroundTile = NULL;
    }
    if (_manager) {
        _manager = NULL;
    }
}

/**
 * @brief 获取边界矩形
 * @return 瓦片的边界矩形
 * @details 返回瓦片在场景中的边界矩形，用于碰撞检测和渲染优化
 */
QRectF WSITileGraphicsItem::boundingRect() const {
    // 返回尺寸
    return _boundingRect;
}

/**
 * @brief 绘制瓦片
 * @param painter 绘制器
 * @param option 样式选项
 * @param widget 小部件（未使用）
 * @details 根据LOD级别决定是否绘制瓦片，支持前景图像叠加和透明度控制
 */
void WSITileGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
    QWidget* widget) {
    float lod = option->levelOfDetailFromTransform(painter->worldTransform());
    if (lod > _lowerLOD) {
        if (_item) {
            bool draw = false;
            if (lod <= _upperLOD) {
                draw = true;
            }
            else if (!_manager->isCovered(_itemLevel, _tileX, _tileY)) {
                draw = true;
            }
            if (draw) {
                QRectF pixmapArea = QRectF((option->exposedRect.left() + (_physicalSize / 2)) * (_tileSize / _physicalSize), (option->exposedRect.top() + (_physicalSize / 2)) * (_tileSize / _physicalSize), option->exposedRect.width() * (_tileSize / _physicalSize), option->exposedRect.height() * (_tileSize / _physicalSize));
                painter->drawPixmap(option->exposedRect, *_item, pixmapArea);
                if (_foregroundPixmap && _renderForeground && _foregroundOpacity > 0.0001) {
                    painter->setOpacity(_foregroundOpacity);
                    painter->drawPixmap(option->exposedRect, *_foregroundPixmap, pixmapArea);
                }
            }
        }
    }
}

/**
 * @brief 调试打印
 * @details 打印瓦片的位置、可见性、层级等调试信息
 */
void WSITileGraphicsItem::debugPrint() {
    std::cout << "Position (x,y): (" << this->pos().x() << ", " << this->pos().y() << ")" << std::endl;
    std::cout << "Has pixmap: " << (_item ? "Yes" : "No") << std::endl;
    std::cout << "Visible: " << this->isVisible() << std::endl;
    std::cout << "Level: " << _itemLevel << std::endl;
    std::cout << "Bounding rectangle (x,y,w,h): (" << _boundingRect.x() << ", " << _boundingRect.y() << ", " << _boundingRect.width() << ", " << _boundingRect.height() << ")" << std::endl;
}

/**
 * @brief 设置前景像素图
 * @param foregroundPixmap 新的前景像素图
 * @details 替换当前的前景像素图并删除旧的像素图
 */
void WSITileGraphicsItem::setForegroundPixmap(QPixmap* foregroundPixmap) {
    QPixmap* oldPixmap = _foregroundPixmap;
    _foregroundPixmap = foregroundPixmap;
    delete oldPixmap;
    this->update();
}

/**
 * @brief 获取前景瓦片
 * @return 前景瓦片对象指针
 * @details 返回前景瓦片对象，用于后续处理
 */
ImageSource* WSITileGraphicsItem::getForegroundTile() {
    return _foregroundTile;
}

/**
 * @brief 设置前景透明度
 * @param opacity 透明度值（0.0-1.0）
 * @details 设置前景图像的透明度并触发重绘
 */
void WSITileGraphicsItem::setForegroundOpacity(float opacity) {
    _foregroundOpacity = opacity;
    this->update();
}

/**
 * @brief 获取前景透明度
 * @return 当前前景透明度值
 */
float WSITileGraphicsItem::getForegroundOpacity() {
    return _foregroundOpacity;
}

/**
 * @brief 设置是否渲染前景
 * @param renderForeground 是否渲染前景的标志
 * @details 控制是否显示前景图像并触发重绘
 */
void WSITileGraphicsItem::setRenderForeground(bool renderForeground)
{
    _renderForeground = renderForeground;
    this->update();
}

/**
 * @brief 获取是否渲染前景
 * @return 当前是否渲染前景的标志
 */
bool WSITileGraphicsItem::getRenderForeground()
{
    return _renderForeground;
}
