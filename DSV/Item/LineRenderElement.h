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
#include<cmath>
#include<QGraphicsSceneHoverEvent>
#include"ControlPoint.h"



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
class LineRenderElement :public QObject, public RenderElement, public QGraphicsLineItem
{
    Q_OBJECT
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
    * @brief   获取直线描述信息
    * @return  包含直线信息的描述字符串
    * @details 返回直线的详细信息，包括名称、位置、大小、长度等
    */
    QString getDescription();

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
     * @brief   鼠标悬停移动事件
     * @param   event   悬停事件对象
     * @details 当鼠标在线条上移动时触发，检查是否悬停在控制点上并设置相应光标
     *
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    
    /**
     * @brief   鼠标悬停离开事件
     * @param   event   悬停事件对象
     * @details 当鼠标离开线条时触发，隐藏控制点和测量信息
     *
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief   自定义绘制函数
     * @param   painter     绘制器
     * @param   option      绘制选项
     * @param   widget      绘制目标窗口
     * @details 自定义绘制线条，保持线宽不受缩放影响
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    /**
     * @brief   元素变化事件处理
     * @param   change      变化类型
     * @param   value       变化值
     * @details 监听元素变换变化，更新字体大小
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

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
        QPointF clickPos = event->pos();
        bool controlPointClicked = false;
        
        // 检查是否点击在控制点上
        if (m_controlPoint1 && m_controlPoint2)
        {
            // 获取控制点在场景中的实际位置
            QPointF control1ScenePos = m_controlPoint1->mapToScene(m_controlPoint1->rect().center());
            QPointF control2ScenePos = m_controlPoint2->mapToScene(m_controlPoint2->rect().center());
            QPointF clickScenePos = mapToScene(clickPos);
            
            // 计算控制点大小（考虑缩放）
            qreal controlSize = m_fControlSize;
            if (scene()) {
                QList<QGraphicsView*> views = scene()->views();
                if (!views.isEmpty()) {
                    QGraphicsView* view = views.first();
                    QTransform viewTransform = view->transform();
                    qreal scale = std::sqrt(viewTransform.m11() * viewTransform.m11() + viewTransform.m12() * viewTransform.m12());
                    controlSize = m_fControlSize / scale;
                }
            }
            
            // 增加拾取容差，使控制点更容易点击
            qreal pickTolerance = qMax(controlSize, 12.0); // 增加最小拾取范围到12像素
            
            // 检查点击位置是否在控制点范围内
            qreal distance1 = std::sqrt(std::pow(clickScenePos.x() - control1ScenePos.x(), 2) + 
                                        std::pow(clickScenePos.y() - control1ScenePos.y(), 2));
            qreal distance2 = std::sqrt(std::pow(clickScenePos.x() - control2ScenePos.x(), 2) + 
                                        std::pow(clickScenePos.y() - control2ScenePos.y(), 2));
            
            if (distance1 <= pickTolerance)
            {
                m_pDragingItem = m_controlPoint1;
                m_isResizing = true;  // 开始调整大小
                setCursor(Qt::CrossCursor);
                controlPointClicked = true;
                event->accept();
                return; // 直接返回，不调用基类事件
            }
            else if (distance2 <= pickTolerance)
            {
                m_pDragingItem = m_controlPoint2;
                m_isResizing = true;  // 开始调整大小
                setCursor(Qt::CrossCursor);
                controlPointClicked = true;
                event->accept();
                return; // 直接返回，不调用基类事件
            }
        }
        
        // 如果没有点击控制点，调用基类事件处理
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
            QPointF pos = event->pos();
            QLineF currentLine = this->line();
            
            if (m_pDragingItem == m_controlPoint1)
            {
                // 更新线条起点
                setLine(QLineF(pos, currentLine.p2()));
                updateContrlPoints();
            }
            else if (m_pDragingItem == m_controlPoint2)
            {
                // 更新线条终点
                setLine(QLineF(currentLine.p1(), pos));
                updateContrlPoints();
            }

            // 更新文本项
            if (m_pTextItem) {
                m_pTextItem->setText(QStringLiteral("长度: %1").arg(RenderElement::formatMeasurement(getPerimeter(), false)));
                m_pTextItem->setPos((currentLine.p1() + currentLine.p2()) / 2.0 + QPointF(10.0, 10.0));
            }
            emit sendLength(getPerimeter());  // 发送最终长度
            event->accept();
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
        if (m_pDragingItem) {
            m_pDragingItem = nullptr;
            m_isResizing = false;  // 结束调整大小
            emit sendLength(getPerimeter());  // 发送最终长度
            setCursor(Qt::ArrowCursor);
            event->accept();
            return; // 直接返回，不调用基类事件
        }
        QGraphicsLineItem::mouseReleaseEvent(event);
    }

private:
    /**
     * @brief   更新控制点位置
     * @details 根据当前线条端点位置，更新控制点和文本标签的位置
     */
    void updateContrlPoints();
    // 动态字体大小
    int getDynamicFontSize();
    
    /**
     * @brief   更新文本字体大小
     * @details 根据当前视图大小更新文本项的字体大小
     */
    void updateFontSize();

    /** @brief 起点控制点，用于拖拽编辑起点位置 */
    ControlPoint* m_controlPoint1 = nullptr;

    /** @brief 终点控制点，用于拖拽编辑终点位置 */
    ControlPoint* m_controlPoint2 = nullptr;

    /** @brief 文本标签项，显示线条长度或名称 */
    QGraphicsSimpleTextItem* m_pTextItem = nullptr;

    /** @brief 当前正在拖拽的控制点指针 */
    ControlPoint* m_pDragingItem = nullptr;
    
    /** @brief 控制点大小，用于创建和定位控制点 */
    qreal m_fControlSize = 10.0;
    
    /** @brief 是否正在调整大小，用于绘制调整提示 */
    bool m_isResizing = false;

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
signals:
    void sendLength(float length);
};