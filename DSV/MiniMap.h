/**
 * @file    MiniMap.h
 * @brief   小地图组件类，显示全局缩略图和视场范围
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的小地图功能，包括：
 *          - 全局缩略图的显示
 *          - 当前视场范围的高亮
 *          - 覆盖度区域的显示与切换
 *          - 与主视图的交互（点击定位、拖拽视场）
 *          该类是DSV项目的全局导航组件，为用户提供
 *          快速定位和全局视野。
 *
 * @note    该类继承自QWidget，是DSV项目的小地图组件
 * @see     PathologyViewer, TileManager
 */

#pragma once
#include <QWidget>
#include <QPointer>

class QPixmap;
class TileManager;

/**
 * @class  MiniMap
 * @brief  小地图组件类，显示全局缩略图和视场范围
 * @details 该类是DSV项目的小地图组件，负责：
 *          - 显示全局缩略图
 *          - 高亮当前视场范围
 *          - 显示和切换覆盖度区域
 *          - 处理用户交互（点击、拖拽）
 *          - 与TileManager联动
 *
 *          主要功能包括：
 *          - 缩略图显示：显示全局图像的缩略图
 *          - 视场高亮：高亮显示当前主视图的视场范围
 *          - 覆盖度显示：显示已加载或已访问区域
 *          - 交互操作：支持点击和拖拽定位主视图
 *          - 界面自适应：根据窗口大小自适应缩放
 *
 * @note   该类继承自QWidget，提供全局导航和定位功能
 * @example
 *          // 使用示例
 *          MiniMap* miniMap = new MiniMap(overviewPixmap, parent);
 *
 *          // 设置瓦片管理器
 *          miniMap->setTileManager(tileManager);
 *
 *          // 更新视场范围
 *          miniMap->updateFieldOfView(fieldOfViewRect);
 *
 *          // 切换覆盖度显示
 *          miniMap->toggleCoverageMap(true);
 * @see     PathologyViewer, TileManager
 */
class MiniMap : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建小地图对象并初始化缩略图
     *
     * @param   overview 全局缩略图像素图
     * @param   parent 父窗口指针
     * @note    构造函数会初始化缩略图和相关参数
     * @see     setTileManager
     */
    MiniMap(const QPixmap& overview, QWidget* parent);

    /**
     * @brief   推荐尺寸
     * @details 返回小地图的推荐显示尺寸
     *
     * @return  推荐的QSize尺寸
     * @note    用于界面布局自适应
     */
    QSize sizeHint() const;

    /**
     * @brief   按宽度自适应高度
     * @details 根据给定宽度返回合适的高度
     *
     * @param   w 目标宽度
     * @return  计算得到的高度
     * @note    用于界面布局自适应
     */
    int heightForWidth(int w) const;

    /**
     * @brief   设置瓦片管理器
     * @details 设置用于覆盖度显示的TileManager指针
     *
     * @param   manager TileManager对象指针
     * @note    用于联动主视图和覆盖度显示
     */
    void setTileManager(TileManager* manager);

public slots:
    /**
     * @brief   更新视场范围
     * @details 更新当前高亮显示的视场矩形
     *
     * @param   fieldOfView 新的视场范围矩形
     * @note    该槽函数用于主视图联动
     */
    void updateFieldOfView(const QRectF& fieldOfView);

    /**
     * @brief   切换覆盖度显示
     * @details 启用或禁用覆盖度区域的显示
     *
     * @param   drawCoverageMap 是否显示覆盖度区域
     * @note    该槽函数用于用户切换覆盖度显示
     */
    void toggleCoverageMap(bool drawCoverageMap);

    /**
     * @brief   覆盖度更新槽
     * @details 当覆盖度数据更新时刷新显示
     * @note    该槽函数用于TileManager联动
     */
    void onCoverageUpdated();

protected:
    /**
     * @brief   鼠标按下事件
     * @details 处理鼠标点击，实现定位主视图
     *
     * @param   event 鼠标事件对象
     * @note    该函数会发出positionClicked信号
     */
    void mousePressEvent(QMouseEvent* event);

    /**
     * @brief   鼠标移动事件
     * @details 处理鼠标拖拽，实现视场范围拖动
     *
     * @param   event 鼠标事件对象
     * @note    支持拖拽定位主视图
     */
    void mouseMoveEvent(QMouseEvent* event);

    /**
     * @brief   绘制事件
     * @details 处理小地图的绘制，包括缩略图、视场和覆盖度
     *
     * @param   event 绘制事件对象
     * @note    该函数会绘制所有可视元素
     */
    void paintEvent(QPaintEvent* event);

private:
    /** @brief 全局缩略图像素图 */
    QPixmap _overview;

    /** @brief 当前高亮显示的视场范围 */
    QRectF _fieldOfView;

    /** @brief 瓦片管理器指针，用于覆盖度显示 */
    QPointer<TileManager> _manager;

    /** @brief 小地图宽高比 */
    float _aspectRatio; //Width / height

    /** @brief 覆盖度颜色表 */
    static const char* const coverageColors[];

    /** @brief 是否显示覆盖度区域 */
    bool _drawCoverageMap;

signals:
    /**
     * @brief   位置点击信号
     * @details 当用户点击小地图时发出该信号
     *
     * @param   position 点击位置的场景坐标
     * @note    用于主视图定位
     */
    void positionClicked(QPointF position);

};