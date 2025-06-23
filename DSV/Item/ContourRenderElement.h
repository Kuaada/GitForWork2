/**
 * @file    ContourRenderElement.h
 * @brief   轮廓渲染元素类，实现轮廓的绘制和交互功能
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了DSV项目的轮廓渲染功能，包括：
 *          - 轮廓的绘制和显示
 *          - 轮廓的拖拽和编辑操作
 *          - 轮廓面积和周长计算
 *          - 轮廓点的添加和管理
 *          - 鼠标事件处理和交互操作
 *          - 悬停效果和视觉反馈
 *          - JSON序列化和反序列化
 *          - 复杂形状的测量和分析
 * 
 * @note    该类是DSV项目高级测量工具的重要组件，用于复杂轮廓测量、
 *          病灶轮廓分析、细胞形态分析等功能
 * @see     RenderElement, QGraphicsPolygonItem, TextRenderElement
 */

#pragma once
#include<QString>
#include<QPainter>
#include<QImage>
#include<QWidget>
#include<QJsonObject>
#include<QGraphicsItem>
#include<QGraphicsPolygonItem>
#include<QGraphicsSceneMouseEvent>
#include"RenderElement.h"
#include<QGraphicsSimpleTextItem>
#include<QGraphicsScene>

/**
 * @brief   轮廓渲染元素类
 * @details 继承自QGraphicsPolygonItem和RenderElement，专门负责轮廓的渲染和交互。
 *          提供轮廓的绘制、拖拽、编辑、面积计算等功能。支持动态添加轮廓点，
 *          允许用户通过鼠标交互来创建和编辑复杂的轮廓形状。
 * 
 * @note    该类是DSV项目高级测量工具的重要组件，用于复杂轮廓测量、
 *          病灶轮廓分析、细胞形态分析等功能。支持JSON序列化，便于数据保存和恢复。
 * 
 * @example
 * @code
 * // 创建轮廓渲染元素
 * QVector<QPointF> points = {QPointF(0, 0), QPointF(100, 0), QPointF(100, 100), QPointF(0, 100)};
 * ContourRenderElement* contour = new ContourRenderElement("病灶轮廓", points);
 * 
 * // 添加到场景
 * scene->addItem(contour);
 * 
 * // 添加新的轮廓点
 * contour->addPoint(QPointF(50, 50));
 * 
 * // 获取轮廓面积
 * float area = contour->getArea();
 * 
 * // 获取轮廓周长
 * float perimeter = contour->getPerimeter();
 * @endcode
 */
class ContourRenderElement :public QGraphicsPolygonItem, public RenderElement
{
public:
    /**
     * @brief   构造函数（仅名称）
     * @param   strName    轮廓名称
     * @param   parent     父图形项指针
     * @details 创建一个空的轮廓渲染元素，需要后续添加轮廓点
     */
    ContourRenderElement(QString strName, QGraphicsItem *parent=nullptr);
    
    /**
     * @brief   构造函数（名称和轮廓点）
     * @param   strName    轮廓名称
     * @param   pts        轮廓点集合
     * @param   parent     父图形项指针
     * @details 创建一个指定轮廓点的轮廓渲染元素，自动计算面积和周长
     */
    ContourRenderElement(QString strName,QVector<QPointF> pts,QGraphicsItem* parent = nullptr);
    
    /**
     * @brief   构造函数（JSON数据）
     * @param   json    JSON对象，包含轮廓的所有属性
     * @details 通过JSON数据创建轮廓渲染元素，用于数据恢复和加载
     * 
     * @note    JSON格式应包含名称、轮廓点坐标、样式等信息
     */
    ContourRenderElement(QJsonObject json);
    
    /**
     * @brief   更新轮廓点
     * @param   pts     新的轮廓点集合
     * @details 更新轮廓的所有点，重新计算面积、周长和显示
     */
    void updateContour(QVector<QPointF> pts);
    
    /**
     * @brief   添加轮廓点
     * @param   pt      新的轮廓点坐标
     * @details 向轮廓中添加一个新的点，动态扩展轮廓形状
     */
    void addPoint(const QPointF& pt);
    
    /**
     * @brief   获取轮廓点数量
     * @return  轮廓中点的数量
     * @details 返回轮廓多边形的顶点数量
     */
    int size() { return polygon().size(); }
    
    /**
     * @brief   析构函数
     * @details 清理轮廓渲染元素的资源，释放文本项
     */
    virtual ~ContourRenderElement();
    
    /**
     * @brief   转换为JSON数据
     * @return  包含轮廓所有属性的JSON对象
     * @details 将轮廓渲染元素的所有属性序列化为JSON格式，
     *          包括名称、轮廓点坐标、样式等
     * 
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual QJsonObject toJson();
    
    /**
     * @brief   鼠标悬停进入事件
     * @param   event   悬停事件对象
     * @details 当鼠标悬停在轮廓上时触发，显示测量信息
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    
    /**
     * @brief   鼠标悬停离开事件
     * @param   event   悬停事件对象
     * @details 当鼠标离开轮廓时触发，隐藏测量信息
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    
    /**
     * @brief   计算轮廓面积
     * @return  轮廓的像素面积
     * @details 计算轮廓多边形的面积，使用多边形面积公式
     * 
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual float getArea();
    
    /**
     * @brief   计算轮廓周长
     * @return  轮廓的像素周长
     * @details 计算轮廓多边形的周长，即所有边的长度之和
     * 
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual float getPerimeter();
    
    /**
     * @brief   自定义绘制函数（注释掉）
     * @details 该函数被注释掉，使用默认的多边形绘制方式。
     *          如需自定义绘制样式，可以取消注释并实现自定义逻辑。
     */
    //void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
    //    QWidget* widget) override
    //{
    //   
    //    //m_pen.setWidth(5);
    //    //m_pen.setColor(Qt::green);
    //    painter->setPen(m_pen);
    //    painter->drawPolyline();
    //    
    //    
    //}

protected:
    /**
     * @brief   鼠标按下事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标按下事件，调用基类的默认处理
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        //QPointF pos = mapToScene(event->pos());
        
        QGraphicsItem::mousePressEvent(event);
    }

    /**
     * @brief   鼠标移动事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标移动事件，调用多边形项的默认处理
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        QGraphicsPolygonItem::mouseMoveEvent(event);
    }

    /**
     * @brief   鼠标释放事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标释放事件，调用多边形项的默认处理
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        QGraphicsPolygonItem::mouseReleaseEvent(event);
    }

private:
    /**
     * @brief   获取轮廓描述信息
     * @return  包含轮廓信息的描述字符串
     * @details 返回轮廓的详细信息，包括名称、点数量、面积等
     */
    QString getDescription();
    
    /** @brief 文本标签项，显示轮廓面积、周长或名称 */
    QGraphicsSimpleTextItem* m_pTextItem=nullptr;
};
