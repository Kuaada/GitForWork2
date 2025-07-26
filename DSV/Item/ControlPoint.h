/**
 * @file    ControlPoint.h
 * @brief   控制点类，用于图形元素的交互控制
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了自定义控制点，大小基于屏幕比例，保持合适的视觉大小
 * 
 * @note    该类是DSV项目图形交互的重要组件，用于矩形、椭圆、线条等
 *          图形元素的拖拽和缩放控制
 */

#pragma once

#include <QGraphicsRectItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QTransform>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <cmath>

/**
 * @brief 控制点类
 * @details 自定义控制点类，大小基于屏幕比例，保持合适的视觉大小
 *          优化了拾取范围管理，解决放大时拾取范围过大的问题
 */
class ControlPoint : public QGraphicsRectItem
{
public:
    /**
     * @brief 构造函数
     * @param x 控制点x坐标
     * @param y 控制点y坐标
     * @param width 控制点宽度
     * @param height 控制点高度
     * @param parent 父图形项指针
     */
    ControlPoint(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = nullptr)
        : QGraphicsRectItem(x, y, width, height, parent)
    {
        // 设置为空心方块，不填充
        setBrush(QBrush(Qt::transparent));
        setPen(QPen(Qt::green, 2));
        setZValue(20);
        
        // 启用鼠标事件，让控制点可以处理交互
        setAcceptHoverEvents(true);
        setAcceptTouchEvents(true);
        setFlag(ItemIsSelectable, false);
        setFlag(ItemIsFocusable, false);
        
        // 初始化拾取范围
        updatePickupArea();
    }

    /**
     * @brief 自定义绘制函数
     * @param painter 绘制器
     * @param option 绘制选项
     * @param widget 绘制目标窗口
     * @details 自定义绘制控制点，大小基于屏幕比例
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        // 保存当前变换
        painter->save();
        
        // 获取当前变换矩阵
        QTransform transform = painter->transform();
        
        // 计算缩放因子
        qreal scale = std::sqrt(transform.m11() * transform.m11() + transform.m12() * transform.m12());
        
        // 获取视图大小，计算基于屏幕的控制点大小
        qreal screenBasedSize = getScreenBasedSize();
        
        // 计算实际绘制大小（考虑缩放）
        qreal actualSize = screenBasedSize / scale;
        
        // 创建新的画笔，调整线宽以抵消缩放
        QPen pen = this->pen();
        pen.setWidthF(pen.widthF() / scale);
        
        // 根据悬停状态设置颜色
        if (m_isHovered) {
            pen.setColor(Qt::red);
            painter->setBrush(QBrush(Qt::transparent)); // 保持空心
        } else {
            pen.setColor(Qt::green);
            painter->setBrush(QBrush(Qt::transparent)); // 保持空心
        }
        
        painter->setPen(pen);
        
        // 绘制基于屏幕大小的控制点，以控制点的rect中心为基准
        QRectF originalRect = rect();
        QPointF center = originalRect.center();
        
        // 绘制基于屏幕大小的空心控制点
        QRectF drawRect(center.x() - actualSize/2, center.y() - actualSize/2, actualSize, actualSize);
        painter->drawRect(drawRect);
        
        // 恢复变换
        painter->restore();
    }

    /**
     * @brief 鼠标悬停进入事件
     * @param event 悬停事件对象
     * @details 当鼠标悬停在控制点上时触发
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override
    {
        m_isHovered = true;
        update(); // 重绘以显示悬停效果
        QGraphicsRectItem::hoverEnterEvent(event);
    }

    /**
     * @brief 鼠标悬停离开事件
     * @param event 悬停事件对象
     * @details 当鼠标离开控制点时触发
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override
    {
        m_isHovered = false;
        update(); // 重绘以隐藏悬停效果
        QGraphicsRectItem::hoverLeaveEvent(event);
    }

    /**
     * @brief 鼠标按下事件
     * @param event 鼠标事件对象
     * @details 当鼠标按下控制点时触发
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        // 禁用鼠标事件，让父Item处理
        QGraphicsRectItem::mousePressEvent(event);
    }



    /**
     * @brief 形状函数重写
     * @return 控制点的实际拾取形状
     * @details 返回基于屏幕比例的拾取形状，解决放大时拾取范围过大的问题
     */
    QPainterPath shape() const override
    {
        QPainterPath path;
        
        // 获取当前视图的缩放因子
        qreal scale = 1.0;
        if (scene()) {
            QList<QGraphicsView*> views = scene()->views();
            if (!views.isEmpty()) {
                QGraphicsView* view = views.first();
                QTransform viewTransform = view->transform();
                scale = std::sqrt(viewTransform.m11() * viewTransform.m11() + viewTransform.m12() * viewTransform.m12());
            }
        }
        
        // 计算基于屏幕的拾取范围
        qreal pickupSize = getScreenBasedPickupSize() / scale;
        
        // 以控制点中心为基准创建拾取区域
        QRectF originalRect = rect();
        QPointF center = originalRect.center();
        QRectF pickupRect(center.x() - pickupSize/2, center.y() - pickupSize/2, pickupSize, pickupSize);
        
        path.addRect(pickupRect);
        return path;
    }

    /**
     * @brief 更新拾取区域
     * @details 根据当前视图状态更新控制点的拾取区域
     */
    void updatePickupArea()
    {
        // 强制更新形状
        prepareGeometryChange();
    }

private:
    /** @brief 是否正在悬停的标志 */
    bool m_isHovered = false;

    /**
     * @brief 获取基于屏幕大小的控制点尺寸
     * @return 基于屏幕的控制点大小
     * @details 根据当前视图大小计算合适的控制点大小
     */
    qreal getScreenBasedSize()
    {
        // 默认大小（像素）
        qreal defaultSize = 8.0;
        
        // 尝试获取视图
        if (scene()) {
            QList<QGraphicsView*> views = scene()->views();
            if (!views.isEmpty()) {
                QGraphicsView* view = views.first();
                QSize viewSize = view->size();
                
                // 计算基于视图大小的控制点大小
                // 使用视图对角线长度的比例
                qreal diagonal = std::sqrt(viewSize.width() * viewSize.width() + viewSize.height() * viewSize.height());
                
                // 控制点大小约为视图对角线的0.5%
                qreal screenBasedSize = diagonal * 0.005;
                
                // 设置最小和最大限制
                screenBasedSize = qMax(screenBasedSize, 4.0);  // 最小4像素
                screenBasedSize = qMin(screenBasedSize, 20.0); // 最大20像素
                
                return screenBasedSize;
            }
        }
        
        // 如果无法获取视图，返回默认大小
        return defaultSize;
    }

    /**
     * @brief 获取基于屏幕大小的拾取范围尺寸
     * @return 基于屏幕的拾取范围大小
     * @details 根据当前视图大小计算合适的拾取范围，确保拾取范围与视觉大小匹配
     */
    qreal getScreenBasedPickupSize() const
    {
        // 默认拾取范围（像素）
        qreal defaultPickupSize = 12.0;
        
        // 尝试获取视图
        if (scene()) {
            QList<QGraphicsView*> views = scene()->views();
            if (!views.isEmpty()) {
                QGraphicsView* view = views.first();
                QSize viewSize = view->size();
                
                // 计算基于视图大小的拾取范围
                // 使用视图对角线长度的比例
                qreal diagonal = std::sqrt(viewSize.width() * viewSize.width() + viewSize.height() * viewSize.height());
                
                // 拾取范围约为视图对角线的0.8%（比视觉大小稍大，便于拾取）
                qreal screenBasedPickupSize = diagonal * 0.008;
                
                // 设置最小和最大限制
                screenBasedPickupSize = qMax(screenBasedPickupSize, 8.0);   // 最小8像素
                screenBasedPickupSize = qMin(screenBasedPickupSize, 32.0);  // 最大32像素
                
                return screenBasedPickupSize;
            }
        }
        
        // 如果无法获取视图，返回默认大小
        return defaultPickupSize;
    }
}; 