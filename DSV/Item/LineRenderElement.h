/**
 * @file    LineRenderElement.h
 * @brief   线条渲染元素类，实现线条的绘制和交互功能
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了DSV项目的线条渲染功能，包括：
 *          - 线条的绘制和显示
 *          - 线条端点的交互控制
 *          - 线条长度计算和测量
 *          - 鼠标事件处理和拖拽操作
 *          - JSON序列化和反序列化
 *          - 悬停效果和视觉反馈
 *
 * @note    该类继承自QGraphicsLineItem和RenderElement，是DSV项目
 *          中最常用的渲染元素之一，用于测量、标注和绘图功能
 * @see     RenderElement, QGraphicsLineItem, QGraphicsScene
 */

#pragma once
 //#define M_PI 3.14
#include<QString>
#include<QPainter>
#include<QImage>
#include<QWidget>
#include<QJsonObject>
#include<QGraphicsItem>
#include<QGraphicsLineItem>
#include<QGraphicsSceneMouseEvent>
#include"RenderElement.h"
#include<QGraphicsSimpleTextItem>
#include<QGraphicsScene>
#include<QGraphicsSceneHoverEvent>

/**
 * @brief   线条渲染元素类
 * @details 继承自QGraphicsLineItem和RenderElement，专门负责线条的渲染和交互。
 *          提供线条的绘制、端点控制、长度测量、拖拽编辑等功能。
 *          支持鼠标交互，包括端点拖拽、悬停效果等。
 *
 * @note    该类是DSV项目测量工具的核心组件，用于距离测量、
 *          线条标注等功能。支持JSON序列化，便于数据保存和恢复。
 *
 * @example
 * @code
 * // 创建线条渲染元素
 * LineRenderElement* line = new LineRenderElement("测量线", QPointF(0, 0), QPointF(100, 100));
 *
 * // 添加到场景
 * scene->addItem(line);
 *
 * // 获取线条长度
 * float length = line->getPerimeter();
 * @endcode
 */
class LineRenderElement :public QGraphicsLineItem, public RenderElement
{
public:
    /**
     * @brief   构造函数（仅名称）
     * @param   strName    线条名称
     * @param   parent     父图形项指针
     * @details 创建一个空的线条渲染元素，需要后续设置端点
     */
    LineRenderElement(QString strName, QGraphicsItem* parent = nullptr);

    /**
     * @brief   构造函数（名称和端点）
     * @param   strName    线条名称
     * @param   pt1        起点坐标
     * @param   pt2        终点坐标
     * @param   parent     父图形项指针
     * @details 创建一个指定端点的线条渲染元素，自动计算长度和显示
     */
    LineRenderElement(QString strName, const QPointF& pt1, const QPointF& pt2, QGraphicsItem* parent = nullptr);

    /**
     * @brief   构造函数（JSON数据）
     * @param   json    JSON对象，包含线条的所有属性
     * @details 通过JSON数据创建线条渲染元素，用于数据恢复和加载
     *
     * @note    JSON格式应包含名称、端点坐标、样式等信息
     */
    LineRenderElement(QJsonObject json);

    /**
     * @brief   更新线条端点
     * @param   pt1    新的起点坐标
     * @param   pt2    新的终点坐标
     * @details 更新线条的端点位置，重新计算长度和显示控制点
     */
    void updateLine(const QPointF& pt1, const QPointF& pt2);

    /**
     * @brief   析构函数
     * @details 清理线条渲染元素的资源，释放控制点和文本项
     */
    virtual ~LineRenderElement();

    /**
     * @brief   计算线条长度
     * @return  线条的像素长度
     * @details 计算线条两个端点之间的欧几里得距离
     *
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual float getPerimeter();

    /**
     * @brief   转换为JSON数据
     * @return  包含线条所有属性的JSON对象
     * @details 将线条渲染元素的所有属性序列化为JSON格式，
     *          包括名称、端点坐标、样式等
     *
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual QJsonObject toJson();

    /**
     * @brief   鼠标悬停进入事件
     * @param   event   悬停事件对象
     * @details 当鼠标悬停在线条上时触发，显示控制点和测量信息
     *
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief   鼠标悬停离开事件
     * @param   event   悬停事件对象
     * @details 当鼠标离开线条时触发，隐藏控制点和测量信息
     *
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

protected:
    /**
     * @brief   鼠标按下事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标按下事件，检测是否点击了控制点，
     *          如果是则开始拖拽操作
     *
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        //QPointF pos = mapToScene(event->pos());
        QPointF pos = event->pos();
        QRectF rect1 = m_controlPoint1->rect();
        QRectF rect2 = m_controlPoint2->rect();
        if (rect1.contains(pos))
        {
            m_pDragingItem = m_controlPoint1;
        }
        if (rect2.contains(pos))
        {
            m_pDragingItem = m_controlPoint2;
        }
        QGraphicsLineItem::mousePressEvent(event);
    }

    /**
     * @brief   鼠标移动事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标移动事件，如果正在拖拽控制点，
     *          则更新线条端点和控制点位置
     *
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (m_pDragingItem)
        {
            //QPointF pos = mapToScene(event->pos());
            QPointF pos = event->pos();
            QLineF Line = this->line();
            if (m_pDragingItem == m_controlPoint1)
            {

                updateLine(pos, Line.p2());
            }
            else
            {

                updateLine(Line.p1(), pos);
            }
            updateContrlPoints();
            return;
        }
        QGraphicsLineItem::mouseMoveEvent(event);
    }

    /**
     * @brief   鼠标释放事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标释放事件，结束拖拽操作，清理拖拽状态
     *
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        m_pDragingItem = nullptr;
        QGraphicsLineItem::mouseReleaseEvent(event);
    }

private:
    /**
     * @brief   更新控制点位置
     * @details 根据当前线条端点位置，更新控制点和文本标签的位置
     */
    void updateContrlPoints();

    /** @brief 起点控制点，用于拖拽编辑起点位置 */
    QGraphicsRectItem* m_controlPoint1 = nullptr;

    /** @brief 终点控制点，用于拖拽编辑终点位置 */
    QGraphicsRectItem* m_controlPoint2 = nullptr;

    /** @brief 文本标签项，显示线条长度或名称 */
    QGraphicsSimpleTextItem* m_pTextItem = nullptr;

    /** @brief 当前正在拖拽的控制点指针 */
    QGraphicsRectItem* m_pDragingItem = nullptr;

    /** @brief 控制点的大小（像素） */
    int m_fControlSize = 10;

    /**
     * @brief   计算两点间距离
     * @param   p1      第一个点坐标
     * @param   p2      第二个点坐标
     * @return  两点间的欧几里得距离
     * @details 使用勾股定理计算两点间的直线距离
     */
    double distance(QPointF p1, QPointF p2)
    {
        double dx = p2.x() - p1.x();
        double dy = p2.y() - p1.y();
        return sqrt(dx * dx + dy * dy);
    }
};