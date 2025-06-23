/**
 * @file    RectRenderElement.h
 * @brief   矩形渲染元素类，实现矩形的绘制和交互功能
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了DSV项目的矩形渲染功能，包括：
 *          - 矩形的绘制和显示
 *          - 矩形的拖拽和缩放操作
 *          - 矩形面积和周长计算
 *          - 鼠标事件处理和交互操作
 *          - 控制点管理和视觉反馈
 *          - JSON序列化和反序列化
 *          - 悬停效果和测量信息显示
 * 
 * @note    该类是DSV项目测量工具的重要组件，用于面积测量、
 *          区域标注、ROI（感兴趣区域）选择等功能
 * @see     RenderElement, QGraphicsRectItem, LineRenderElement
 */

#pragma once
//#define M_PI 3.14
#include<QString>
#include<QPainter>
#include<QImage>
#include<QWidget>
#include<QJsonObject>
#include<QGraphicsItem>
#include<QGraphicsRectItem>
#include<QGraphicsSceneMouseEvent>
#include"RenderElement.h"
#include<QGraphicsSimpleTextItem>
#include<QGraphicsScene>

/**
 * @brief   矩形渲染元素类
 * @details 继承自QGraphicsRectItem和RenderElement，专门负责矩形的渲染和交互。
 *          提供矩形的绘制、拖拽、缩放、面积计算等功能。支持多个控制点，
 *          允许用户通过拖拽控制点来调整矩形的大小和位置。
 * 
 * @note    该类是DSV项目测量工具的核心组件，用于面积测量、
 *          区域标注等功能。支持JSON序列化，便于数据保存和恢复。
 * 
 * @example
 * @code
 * // 创建矩形渲染元素
 * RectRenderElement* rect = new RectRenderElement("测量区域", QRectF(0, 0, 100, 100));
 * 
 * // 添加到场景
 * scene->addItem(rect);
 * 
 * // 获取矩形面积
 * float area = rect->getArea();
 * 
 * // 获取矩形周长
 * float perimeter = rect->getPerimeter();
 * @endcode
 */
class RectRenderElement :public QGraphicsRectItem, public RenderElement
{
public:
    /**
     * @brief   构造函数（仅名称）
     * @param   strName    矩形名称
     * @param   parent     父图形项指针
     * @details 创建一个空的矩形渲染元素，需要后续设置矩形区域
     */
    RectRenderElement(QString strName, QGraphicsItem *parent=nullptr);
    
    /**
     * @brief   构造函数（名称和矩形区域）
     * @param   strName    矩形名称
     * @param   rect       矩形区域
     * @param   parent     父图形项指针
     * @details 创建一个指定区域的矩形渲染元素，自动计算面积和周长
     */
    RectRenderElement(QString strName,const QRectF& rect,QGraphicsItem* parent = nullptr);
    
    /**
     * @brief   构造函数（JSON数据）
     * @param   json    JSON对象，包含矩形的所有属性
     * @details 通过JSON数据创建矩形渲染元素，用于数据恢复和加载
     * 
     * @note    JSON格式应包含名称、矩形区域、样式等信息
     */
    RectRenderElement(QJsonObject json);
    
    /**
     * @brief   获取矩形描述信息
     * @return  包含矩形信息的描述字符串
     * @details 返回矩形的详细信息，包括名称、位置、大小、面积等
     */
    QString getDescription();
    
    /**
     * @brief   更新矩形区域
     * @param   rect    新的矩形区域
     * @details 更新矩形的区域，重新计算面积、周长和控制点位置
     */
    void updateRect(QRectF rect);
    
    /**
     * @brief   析构函数
     * @details 清理矩形渲染元素的资源，释放控制点和文本项
     */
    virtual ~RectRenderElement();
    
    /**
     * @brief   计算矩形面积
     * @return  矩形的像素面积
     * @details 计算矩形的面积（宽度 × 高度）
     * 
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual float getArea();
    
    /**
     * @brief   计算矩形周长
     * @return  矩形的像素周长
     * @details 计算矩形的周长（2 × (宽度 + 高度)）
     * 
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual float getPerimeter();
    
    /**
     * @brief   转换为JSON数据
     * @return  包含矩形所有属性的JSON对象
     * @details 将矩形渲染元素的所有属性序列化为JSON格式，
     *          包括名称、矩形区域、样式等
     * 
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual QJsonObject toJson();

    /**
     * @brief   鼠标悬停进入事件
     * @param   event   悬停事件对象
     * @details 当鼠标悬停在矩形上时触发，显示控制点和测量信息
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    
    /**
     * @brief   鼠标悬停离开事件
     * @param   event   悬停事件对象
     * @details 当鼠标离开矩形时触发，隐藏控制点和测量信息
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

protected:
    /**
     * @brief   鼠标按下事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标按下事件，检测是否点击了控制点，
     *          如果是则开始拖拽或缩放操作
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    
    /**
     * @brief   鼠标移动事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标移动事件，如果正在拖拽或缩放，
     *          则更新矩形区域和控制点位置
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    
    /**
     * @brief   鼠标释放事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标释放事件，结束拖拽或缩放操作，清理状态
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    /** @brief 控制点的大小（像素），可根据需求调整 */
    qreal m_fControlSize = 6;
    
    /** @brief 文本标签项，显示矩形面积、周长或名称 */
    QGraphicsSimpleTextItem* m_pTextItem = nullptr;
    
    /** @brief 是否正在调整大小的标志 */
    bool m_isResizing;
    
    /** @brief 上次鼠标位置，用于计算拖拽距离 */
    QPointF m_lastMousePos;
    
    /** @brief 控制点列表，包含矩形的所有控制点 */
    QList<QGraphicsRectItem*> m_controlPoints;
    
    /**
     * @brief   创建控制点
     * @details 为矩形的四个角创建控制点，用于拖拽调整矩形大小
     */
    void createControlPoints();
    
    /**
     * @brief   更新控制点位置
     * @details 根据当前矩形区域，更新所有控制点的位置
     */
    void updateControlPointsPosition();
};
