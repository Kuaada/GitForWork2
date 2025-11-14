/**
 * @file    WSITileGraphicsItem.h
 * @brief   瓦片图形项类，负责瓦片的图形渲染和显示
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了DSV项目的瓦片图形渲染功能，包括：
 *          - 瓦片的图形项创建和管理
 *          - 瓦片图像的加载和渲染
 *          - 前景图像叠加和透明度控制
 *          - 瓦片缓存和内存管理
 *          - 瓦片加载状态跟踪
 *          - 渲染性能优化和LOD控制
 *
 * @note    该类是DSV项目渲染系统的核心组件，继承自Qt的QGraphicsItem，
 *          负责将图像瓦片转换为可显示的图形项
 * @see     TileManager, MultiResolutionImage, QGraphicsItem, ImageSource
 */

#pragma once

#include <QGraphicsItem>

class ImageSource;
class TileManager;

/**
 * @brief   瓦片图形项类
 * @details 继承自QGraphicsItem，专门负责将图像瓦片转换为Qt图形项进行显示。
 *          支持背景瓦片和前景瓦片的叠加渲染，提供透明度控制和LOD（细节层次）
 *          管理功能。该类是DSV项目渲染系统的核心组件，负责将多分辨率图像
 *          的瓦片数据转换为可视化的图形元素。
 *
 * @note    该类管理瓦片的位置、大小、级别等信息，并提供渲染接口。
 *          支持前景图像的叠加显示，用于显示标注、测量等叠加信息。
 *
 * @example
 * @code
 * // 创建瓦片图形项
 * WSITileGraphicsItem* tileItem = new WSITileGraphicsItem(
 *     backgroundPixmap, tileX, tileY, tileSize, tileByteSize, level,
 *     lastRenderLevel, downsamples, tileManager,
 *     foregroundPixmap, foregroundSource, opacity, true);
 *
 * // 添加到图形场景
 * scene->addItem(tileItem);
 * @endcode
 */
class WSITileGraphicsItem : public QGraphicsItem {
public:
    /**
     * @brief   构造函数
     * @param   item                背景瓦片图像指针
     * @param   tileX               瓦片X坐标
     * @param   tileY               瓦片Y坐标
     * @param   tileSize            瓦片大小（像素）
     * @param   tileByteSize        瓦片字节大小
     * @param   itemLevel           瓦片分辨率级别
     * @param   lastRenderLevel     上次渲染的级别
     * @param   imgDownsamples      图像下采样因子数组
     * @param   manager             瓦片管理器指针
     * @param   foregroundPixmap    前景瓦片图像指针（可选）
     * @param   foregroundTile      前景瓦片数据源（可选）
     * @param   foregroundOpacity   前景透明度（0.0-1.0）
     * @param   renderForeground    是否渲染前景
     * @details 初始化瓦片图形项，设置瓦片的位置、大小、级别等属性。
     *          支持背景和前景瓦片的配置，以及透明度控制。
     *
     * @note    构造函数中item参数必须设置为NULL，实际图像数据通过其他方式加载
     * @see     TileManager::createTileItem
     */
    WSITileGraphicsItem(QPixmap* item, unsigned int tileX, unsigned int tileY, unsigned int tileSize, unsigned int tileByteSize, unsigned int itemLevel,
        unsigned int lastRenderLevel, const std::vector<float>& imgDownsamples, TileManager* manager,
        QPixmap* foregroundPixmap = NULL, ImageSource* foregroundTile = NULL, float foregroundOpacity = 1.0, bool renderForeground = true);

    /**
     * @brief   析构函数
     * @details 清理瓦片图形项资源，释放图像数据和内存
     *
     * @note    需要实现析构函数、拷贝构造函数和赋值操作符
     */
    ~WSITileGraphicsItem();

    /**
     * @brief   获取边界矩形
     * @return  瓦片图形项的边界矩形
     * @details 返回瓦片图形项在场景坐标系中的边界矩形，用于
     *          碰撞检测、视图裁剪等操作
     *
     * @note    该函数是QGraphicsItem的虚函数重写
     */
    QRectF boundingRect() const;

    /**
     * @brief   绘制函数
     * @param   painter     绘制器指针
     * @param   option      绘制选项指针
     * @param   widget      绘制目标窗口指针
     * @details 执行瓦片图形项的实际绘制操作，包括：
     *          - 背景瓦片图像的绘制
     *          - 前景瓦片图像的叠加绘制
     *          - 透明度效果的应用
     *          - 渲染优化和性能控制
     *
     * @note    该函数是QGraphicsItem的虚函数重写
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
        QWidget* widget);

    /**
     * @brief   调试打印函数
     * @details 打印瓦片图形项的调试信息，包括位置、大小、级别等属性
     */
    void debugPrint();

    /**
     * @brief   获取瓦片X坐标
     * @return  瓦片在图像中的X坐标
     */
    unsigned int getTileX() { return _tileX; }

    /**
     * @brief   获取瓦片Y坐标
     * @return  瓦片在图像中的Y坐标
     */
    unsigned int getTileY() { return _tileY; }

    /**
     * @brief   获取瓦片级别
     * @return  瓦片的分辨率级别
     */
    unsigned int getTileLevel() { return _itemLevel; }

    /**
     * @brief   获取瓦片大小
     * @return  瓦片的像素大小
     */
    unsigned int getTileSize() { return _tileSize; }

    /**
     * @brief   设置前景瓦片图像
     * @param   foregroundPixmap    前景瓦片图像指针
     * @details 设置用于叠加显示的前景瓦片图像，通常用于显示
     *          标注、测量、分割结果等叠加信息
     */
    void setForegroundPixmap(QPixmap* foregroundPixmap);

    /**
     * @brief   获取前景瓦片数据源
     * @return  前景瓦片数据源指针
     */
    ImageSource* getForegroundTile();

    /**
     * @brief   设置前景透明度
     * @param   opacity     透明度值（0.0-1.0）
     * @details 设置前景瓦片的透明度，0.0表示完全透明，1.0表示完全不透明
     */
    void setForegroundOpacity(float opacity);

    /**
     * @brief   获取前景透明度
     * @return  当前前景透明度值
     */
    float getForegroundOpacity();

    /**
     * @brief   设置是否渲染前景
     * @param   renderForeground    是否渲染前景的标志
     * @details 控制是否显示前景瓦片，可用于动态开关前景显示
     */
    void setRenderForeground(bool renderForeground);

    /**
     * @brief   获取前景渲染状态
     * @return  当前是否渲染前景
     */
    bool getRenderForeground();

private:
    /** @brief 背景瓦片图像指针，存储实际的图像数据 */
    QPixmap* _item;

    /** @brief 前景瓦片图像指针，用于叠加显示 */
    QPixmap* _foregroundPixmap;

    /** @brief 前景瓦片数据源指针，提供前景图像数据 */
    ImageSource* _foregroundTile;

    /** @brief 前景透明度值（0.0-1.0） */
    float _foregroundOpacity;

    /** @brief 瓦片的物理大小（微米） */
    float _physicalSize;

    /** @brief 是否渲染前景的标志 */
    bool _renderForeground;

    /** @brief 上界LOD（细节层次）阈值 */
    float _upperLOD;

    /** @brief 下界LOD（细节层次）阈值 */
    float _lowerLOD;

    /** @brief 瓦片的分辨率级别 */
    unsigned int _itemLevel;

    /** @brief 瓦片在图像中的X坐标 */
    unsigned int _tileX;

    /** @brief 瓦片在图像中的Y坐标 */
    unsigned int _tileY;

    /** @brief 瓦片的像素大小 */
    unsigned int _tileSize;

    /** @brief 瓦片的字节大小 */
    unsigned int _tileByteSize;

    /** @brief 上次渲染的分辨率级别 */
    unsigned int _lastRenderLevel;

    /** @brief 瓦片图形项的边界矩形 */
    QRectF _boundingRect;

    /** @brief 瓦片管理器指针，用于瓦片管理和协调 */
    TileManager* _manager;
};