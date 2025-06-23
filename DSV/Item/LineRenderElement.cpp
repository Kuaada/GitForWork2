/**
 * @file LineRenderElement.cpp
 * @brief 线条渲染元素实现文件
 * @details 实现数字病理切片查看器中的线条标注功能，包括线条绘制、测量、交互等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "LineRenderElement.h"

/**
 * @brief 线条渲染元素构造函数（仅名称）
 * @param strName 元素名称
 * @param parent 父图形项指针
 * @details 创建基本的线条渲染元素，设置交互属性和工具提示
 */
LineRenderElement::LineRenderElement(QString strName, QGraphicsItem* parent) :RenderElement(strName), QGraphicsLineItem(parent)
{
    m_elementType = Line;  // 设置元素类型为线条
    setAcceptHoverEvents(true);    // 接受悬停事件
    setAcceptTouchEvents(true);    // 接受触摸事件
    setAcceptDrops(true);          // 接受拖放事件
    setZValue(10);                 // 设置Z轴值
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);  // 设置交互标志
    //m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    setToolTip(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));


}

/**
 * @brief 线条渲染元素构造函数（JSON对象）
 * @param json JSON对象，包含线条的序列化数据
 * @details 从JSON对象恢复线条渲染元素的状态
 */
LineRenderElement::LineRenderElement(QJsonObject json) :RenderElement(json)
{
    m_elementType = Line;
    // m_pTextItem = new QGraphicsSimpleTextItem(this);
     //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
     //scene()->addItem(m_pTextItem);
}

/**
 * @brief 线条渲染元素构造函数（完整参数）
 * @param strName 元素名称
 * @param pt1 线条起点
 * @param pt2 线条终点
 * @param parent 父图形项指针
 * @details 创建具有指定起点和终点的线条渲染元素，并设置文本显示
 */
LineRenderElement::LineRenderElement(QString strName, const QPointF& pt1, const QPointF& pt2, QGraphicsItem* parent)
    :RenderElement(strName), QGraphicsLineItem(QLineF(pt1, pt2), parent)
{
    m_elementType = Line;  // 设置元素类型为线条
    setAcceptHoverEvents(true);    // 接受悬停事件
    setAcceptTouchEvents(true);    // 接受触摸事件
    setAcceptDrops(true);          // 接受拖放事件
    setZValue(10);                 // 设置Z轴值
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);  // 设置交互标志
    setToolTip(QStringLiteral("长度:%2").arg(getPerimeter(), 0, 'g', 4));
    m_pTextItem = new QGraphicsSimpleTextItem(this);
    m_pTextItem->setFont(QFont("宋体", 16));
    m_pTextItem->setFlag(ItemIgnoresTransformations);  // 忽略变换
    m_pTextItem->setText(QStringLiteral("%2 mm").arg(getPerimeter(), 0, 'g', 4));
    m_pTextItem->setPen(m_pen);
    m_pTextItem->setPos((pt1 + pt2) / 2.0);  // 设置文本位置为线条中点
    m_pTextItem->setFlag(ItemIgnoresTransformations);
    updateContrlPoints();  // 更新控制点
    //m_pTextItem->setPos(mapToScene(pos()));
   // scene()->addItem(m_pTextItem);

}

/**
 * @brief 线条渲染元素析构函数
 * @details 清理资源，释放内存
 */
LineRenderElement::~LineRenderElement()
{

}

/**
 * @brief 更新线条位置
 * @param pt1 新的起点
 * @param pt2 新的终点
 * @details 更新线条的起点和终点，并重新计算相关属性
 */
void LineRenderElement::updateLine(const QPointF& pt1, const QPointF& pt2)
{
    QGraphicsLineItem::setLine(QLineF(pt1, pt2));  // 设置新的线条
    setToolTip(QStringLiteral("长度:%2").arg(getPerimeter(), 0, 'g', 4));
    if (!m_pTextItem)
    {
        m_pTextItem = new QGraphicsSimpleTextItem(this);
        m_pTextItem->setPen(m_pen);
        m_pTextItem->setFont(QFont("宋体", 16));
        m_pTextItem->setFlag(ItemIgnoresTransformations);
    }

    m_pTextItem->setText(QStringLiteral("%2 um").arg(getPerimeter(), 0, 'g', 4));
    m_pTextItem->setPos((pt1 + pt2) / 2.0 + QPointF(10.0, 10.0));

    updateContrlPoints();  // 更新控制点


}

/**
 * @brief 转换为JSON对象
 * @return 包含线条数据的JSON对象
 * @details 将线条渲染元素序列化为JSON格式
 */
QJsonObject LineRenderElement::toJson()
{
    QJsonObject base = RenderElement::toJson();
    return base;
}

/**
 * @brief 计算线条周长（长度）
 * @return 线条的实际长度（微米）
 * @details 根据像素距离和缩放因子计算线条的实际长度
 */
float LineRenderElement::getPerimeter()
{
    QPointF p1 = line().p1();  // 获取起点
    QPointF p2 = line().p2();  // 获取终点
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();
    return sqrt(dx * dx + dy * dy) * data(0).toDouble();
}

/**
 * @brief 鼠标悬停进入事件处理
 * @param event 悬停事件对象
 * @details 当鼠标悬停在线条上时，改变光标样式和线条颜色
 */
void LineRenderElement::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(QCursor(Qt::CrossCursor));  // 设置十字光标
    QPen pen;
    pen.setWidth(m_pen.width());
    pen.setColor(Qt::blue);
    setPen(pen);
    QGraphicsItem::hoverEnterEvent(event);
}

/**
 * @brief 鼠标悬停离开事件处理
 * @param event 悬停事件对象
 * @details 当鼠标离开线条时，恢复光标样式和线条颜色
 */
void LineRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(QCursor(Qt::PointingHandCursor));  // 设置手型光标
    setPen(m_pen);  // 恢复原始线条样式
    QGraphicsItem::hoverLeaveEvent(event);
}

/**
 * @brief 更新控制点位置
 * @details 根据线条的起点和终点更新控制点的位置
 */
void LineRenderElement::updateContrlPoints()
{
    QPointF pt1 = line().p1();  // 获取起点
    QPointF pt2 = line().p2();  // 获取终点
    if (!m_controlPoint1)
    {
        m_controlPoint1 = new QGraphicsRectItem(pt1.x() - m_fControlSize, pt1.y() - m_fControlSize, m_fControlSize * 2.0, m_fControlSize * 2.0, this);
        //m_controlPoint1->setFlags(ItemIsSelectable | ItemIsFocusable);

    }
    if (!m_controlPoint2)
    {
        m_controlPoint2 = new QGraphicsRectItem(pt2.x() - m_fControlSize, pt2.y() - m_fControlSize, m_fControlSize * 2.0, m_fControlSize * 2.0, this);
        //m_controlPoint2->setFlags(ItemIsSelectable | ItemIsFocusable);
    }
    m_controlPoint1->setRect(pt1.x() - m_fControlSize, pt1.y() - m_fControlSize, m_fControlSize * 2.0, m_fControlSize * 2.0);
    m_controlPoint2->setRect(pt2.x() - m_fControlSize, pt2.y() - m_fControlSize, m_fControlSize * 2.0, m_fControlSize * 2.0);
}