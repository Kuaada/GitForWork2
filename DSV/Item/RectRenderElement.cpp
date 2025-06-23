/**
 * @file RectRenderElement.cpp
 * @brief 矩形渲染元素实现文件
 * @details 实现数字病理切片查看器中的矩形标注功能，包括矩形绘制、测量、交互等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "RectRenderElement.h"

/**
 * @brief 矩形渲染元素构造函数（仅名称）
 * @param strName 元素名称
 * @param parent 父图形项指针
 * @details 创建基本的矩形渲染元素，设置交互属性和工具提示
 */
RectRenderElement::RectRenderElement(QString strName, QGraphicsItem* parent) :RenderElement(strName), QGraphicsRectItem(parent)
{
	m_elementType = Rectangle;  // 设置元素类型为矩形
    setAcceptHoverEvents(true);    // 接受悬停事件
    setAcceptTouchEvents(true);    // 接受触摸事件
    setAcceptDrops(true);          // 接受拖放事件
    setZValue(10);                 // 设置Z轴值
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);  // 设置交互标志
   // m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    setToolTip(QStringLiteral("面积:%1 um^2 周长:%2 um").arg(getArea(),0,'f',1).arg(getPerimeter(),0,'f',1));

    if (!m_pTextItem)
    {
        m_pTextItem = new QGraphicsSimpleTextItem(this);
        m_pTextItem->setPen(m_pen);
        m_pTextItem->setPos(10,10);
        m_pTextItem->setFont(QFont("宋体", 16));
        m_pTextItem->setFlag(ItemIgnoresTransformations);
    }
    m_pTextItem->setText(QStringLiteral("%2 um").arg(getPerimeter(), 0, 'g', 4));
    //pRectItem->setParentItem(this);
    createControlPoints();

}

/**
 * @brief 矩形渲染元素构造函数（JSON对象）
 * @param json JSON对象，包含矩形的序列化数据
 * @details 从JSON对象恢复矩形渲染元素的状态
 */
RectRenderElement::RectRenderElement(QJsonObject json):RenderElement(json)
{
    m_elementType = Rectangle;
   // m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    //scene()->addItem(m_pTextItem);
    createControlPoints();
}

/**
 * @brief 矩形渲染元素构造函数（完整参数）
 * @param strName 元素名称
 * @param rect 矩形区域
 * @param parent 父图形项指针
 * @details 创建具有指定区域的矩形渲染元素，并设置文本显示
 */
RectRenderElement::RectRenderElement(QString strName, const QRectF& rect, QGraphicsItem* parent)
	:RenderElement(strName), QGraphicsRectItem(rect,parent)
{
    m_elementType = Ellipse;
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    setAcceptDrops(true);
    setFlags(ItemIsMovable|ItemIsSelectable|ItemIsFocusable);
    setToolTip(getDescription());
    createControlPoints();
    setZValue(10);
    if (!m_pTextItem)
    {
        m_pTextItem = new QGraphicsSimpleTextItem(this);
        m_pTextItem->setPen(m_pen);
        m_pTextItem->setPos(10, 10);
        m_pTextItem->setFont(QFont("宋体", 16));
        m_pTextItem->setFlag(ItemIgnoresTransformations);
    }
    m_pTextItem->setText(getDescription());
}

/**
 * @brief 矩形渲染元素析构函数
 * @details 清理资源，释放内存
 */
RectRenderElement::~RectRenderElement()
{

}

/**
 * @brief 更新矩形区域
 * @param rect 新的矩形区域
 * @details 更新矩形的区域，并重新计算相关属性
 */
void RectRenderElement::updateRect(QRectF rect)
{
    QGraphicsRectItem::setRect(0,0,rect.width(),rect.height());
    setPos(rect.topLeft());
    setToolTip(QStringLiteral("面积:%1 um^2 周长:%2 um").arg(getArea(), 0, 'f', 1).arg(getPerimeter(), 0, 'f', 1));
    updateControlPointsPosition();
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'f', 4).arg(getPerimeter(), 0, 'g', 4));
        //scene()->addItem(m_pTextItem);
    if (!m_pTextItem)
    {
        m_pTextItem = new QGraphicsSimpleTextItem(this);
        m_pTextItem->setPen(m_pen);
        m_pTextItem->setPos(10, 10);
        m_pTextItem->setFont(QFont("宋体", 16));
        m_pTextItem->setFlag(ItemIgnoresTransformations);
    }
    m_pTextItem->setText(getDescription());
}

/**
 * @brief 转换为JSON对象
 * @return 包含矩形数据的JSON对象
 * @details 将矩形渲染元素序列化为JSON格式
 */
QJsonObject RectRenderElement::toJson()
{
	QJsonObject base = RenderElement::toJson();
	return base;
}

/**
 * @brief 计算矩形面积
 * @return 矩形的实际面积（平方微米）
 * @details 根据像素面积和缩放因子计算矩形的实际面积
 */
float RectRenderElement::getArea() {
    QRectF rc =rect();
   
    return rc.width()* rc.height() * data(0).toDouble() * data(0).toDouble();
}

/**
 * @brief 计算矩形周长
 * @return 矩形的实际周长（微米）
 * @details 根据像素周长和缩放因子计算矩形的实际周长
 */
float RectRenderElement::getPerimeter()
{
    double a = rect().width();
    double b = rect().height();
    
    return (a + b) * 2.0 * data(0).toDouble();
    
}

/**
 * @brief 鼠标悬停进入事件处理
 * @param event 悬停事件对象
 * @details 当鼠标悬停在矩形上时，改变光标样式和矩形颜色
 */
void RectRenderElement::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    QPen pen;
    pen.setWidth(m_pen.width());
    pen.setColor(Qt::blue);
    setPen(pen);
}

/**
 * @brief 鼠标悬停离开事件处理
 * @param event 悬停事件对象
 * @details 当鼠标离开矩形时，恢复光标样式和矩形颜色
 */
void RectRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setPen(m_pen);
    setCursor(QCursor(Qt::PointingHandCursor));
}

/**
 * @brief 创建控制点
 * @details 在矩形的各个关键位置创建控制点，用于调整矩形大小和形状
 */
void RectRenderElement::createControlPoints()
{
    qreal rectWidth = rect().width();
    qreal rectHeight = rect().height();
    // 左上角控制点
    QGraphicsRectItem* topLeftPoint = new QGraphicsRectItem(-m_fControlSize, -m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize,this);
    topLeftPoint->setBrush(QBrush(Qt::green));
    topLeftPoint->setPen(QPen(Qt::green));
    m_controlPoints.append(topLeftPoint);

    // 右上角控制点
    QGraphicsRectItem* topRightPoint = new QGraphicsRectItem(rectWidth - m_fControlSize, -m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize,this);
    topRightPoint->setBrush(QBrush(Qt::green));
    topRightPoint->setPen(QPen(Qt::green));
    m_controlPoints.append(topRightPoint);

    // 左下角控制点
    QGraphicsRectItem* bottomLeftPoint = new QGraphicsRectItem(-m_fControlSize, rectHeight - m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize,this);
    bottomLeftPoint->setBrush(QBrush(Qt::green));
    bottomLeftPoint->setPen(QPen(Qt::green));
    m_controlPoints.append(bottomLeftPoint);

    // 右下角控制点
    QGraphicsRectItem* bottomRightPoint = new QGraphicsRectItem(rectWidth - m_fControlSize, rectHeight - m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize,this);
    bottomRightPoint->setBrush(QBrush(Qt::green));
    bottomRightPoint->setPen(QPen(Qt::green));
    m_controlPoints.append(bottomRightPoint);

    // 左边中间控制点
    QGraphicsRectItem* leftMidPoint = new QGraphicsRectItem(-m_fControlSize, rectHeight / 2 - m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0,this);
    leftMidPoint->setBrush(QBrush(Qt::green));
    leftMidPoint->setPen(QPen(Qt::green));
    m_controlPoints.append(leftMidPoint);

    // 右边中间控制点
    QGraphicsRectItem* rightMidPoint = new QGraphicsRectItem(rectWidth - m_fControlSize, rectHeight / 2 - m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0,this);
    rightMidPoint->setBrush(QBrush(Qt::green));
    rightMidPoint->setPen(QPen(Qt::green));
    m_controlPoints.append(rightMidPoint);

    // 上边中间控制点
    QGraphicsRectItem* topMidPoint = new QGraphicsRectItem(rectWidth / 2 - m_fControlSize, -m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0,this);
    topMidPoint->setBrush(QBrush(Qt::green));
    topMidPoint->setPen(QPen(Qt::green));
    m_controlPoints.append(topMidPoint);

    // 下边中间控制点
    QGraphicsRectItem* bottomMidPoint = new QGraphicsRectItem(rectWidth / 2 - m_fControlSize, rectHeight - m_fControlSize, m_fControlSize*2.0, m_fControlSize*2.0,this);
    bottomMidPoint->setBrush(QBrush(Qt::green));
    bottomMidPoint->setPen(QPen(Qt::green));
    m_controlPoints.append(bottomMidPoint);

   
}

/**
 * @brief 鼠标按下事件处理
 * @param event 鼠标事件对象
 * @details 检测是否点击在控制点上，如果是则开始调整大小
 */
void RectRenderElement::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF clickPos = event->pos();
    qreal rectWidth = rect().width();
    qreal rectHeight = rect().height();

    // 遍历控制点，判断是否点击在控制点上
    for (int i = 0; i < m_controlPoints.size(); ++i)
    {
        QRectF rectItem=m_controlPoints[i]->rect();
        rectItem=mapRectFromItem(m_controlPoints[i],rectItem);
        if (rectItem.contains(clickPos))
        {
            m_isResizing = true;
            m_lastMousePos = clickPos;
            setFocus();
            return;
        }
    }

    QGraphicsRectItem::mousePressEvent(event);
}

/**
 * @brief 鼠标移动事件处理
 * @param event 鼠标事件对象
 * @details 在调整大小模式下，根据鼠标移动调整矩形大小
 */
void RectRenderElement::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_isResizing)
    {
        
        QPointF currentPos = event->pos();
        qreal dx = currentPos.x() - m_lastMousePos.x();
        qreal dy = currentPos.y() - m_lastMousePos.y();
        qDebug() << "Adjust Size x:"<<dx<<"y:"<<dy;
        qreal newWidth = rect().width();
        qreal newHeight = rect().height();

        // 根据点击的不同控制点，计算宽度和高度的变化
        if (m_controlPoints[0]->contains(m_lastMousePos) || m_controlPoints[1]->contains(m_lastMousePos) ||
            m_controlPoints[2]->contains(m_lastMousePos) || m_controlPoints[3]->contains(m_lastMousePos))
        {
            newWidth += dx;
            newHeight += dy;
        }
        else if (m_controlPoints[4]->contains(m_lastMousePos) || m_controlPoints[5]->contains(m_lastMousePos))
        {
            newWidth += dx;
        }
        else if (m_controlPoints[6]->contains(m_lastMousePos) || m_controlPoints[7]->contains(m_lastMousePos))
        {
            newHeight += dy;
        }

        if (newWidth > 0 && newHeight > 0)
        {
            setRect(rect().x(), rect().y(), newWidth, newHeight);
            
            m_lastMousePos = currentPos;
            updateControlPointsPosition();
        }
        setToolTip(getDescription());
        if (!m_pTextItem)
        {
            m_pTextItem = new QGraphicsSimpleTextItem(this);
            m_pTextItem->setPen(m_pen);
            m_pTextItem->setPos(10, 10);
            m_pTextItem->setFont(QFont("宋体", 16));
            m_pTextItem->setFlag(ItemIgnoresTransformations);
        }
        m_pTextItem->setText(getDescription());
        return;
    }
    

    QGraphicsRectItem::mouseMoveEvent(event);
}
QString RectRenderElement::getDescription()
{
    return QStringLiteral("面积:%1 um^2\r\n周长:%2 um").arg(getArea(), 0, 'f', 1).arg(getPerimeter(), 0, 'f', 1);
}
void RectRenderElement::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    m_isResizing = false;
    QGraphicsRectItem::mouseReleaseEvent(event);
}/*
void RectRenderElement::updateControlPointsPosition()
{
    qreal rectWidth = rect().width();
    qreal rectHeight = rect().height();
    qreal m_fControlSize = 10;

    //QGraphicsItem *parent= m_controlPoints[0]->parentItem();
    QPointF posTopLeft = pos();
    float x = posTopLeft.x();
    float y = posTopLeft.y();
    m_controlPoints[0]->setPos(0, 0);
                               
    m_controlPoints[1]->setPos(rectWidth - controlSize, 0);
    m_controlPoints[2]->setPos(0, rectHeight - controlSize);
    m_controlPoints[3]->setPos(rectWidth - controlSize, rectHeight - controlSize);
                               
    m_controlPoints[4]->setPos(0, rectHeight / 2 - controlSize / 2);
    m_controlPoints[5]->setPos(rectWidth - controlSize, rectHeight / 2 - controlSize / 2);
                               
    m_controlPoints[6]->setPos(rectWidth / 2 - controlSize / 2, 0);
    m_controlPoints[7]->setPos(rectWidth / 2 - controlSize / 2, rectHeight - controlSize);
}*/

void RectRenderElement::updateControlPointsPosition()
{
    qreal rectWidth = rect().width();
    qreal rectHeight = rect().height();
    //qreal controlSize = 6;

    //QGraphicsItem *parent= m_controlPoints[0]->parentItem();
    QPointF posTopLeft = pos();
    float x = posTopLeft.x();
    float y = posTopLeft.y();
    m_controlPoints[0]->setRect(-m_fControlSize, -m_fControlSize, 2.0 * m_fControlSize, 2.0 * m_fControlSize);
    m_controlPoints[1]->setRect(rectWidth - m_fControlSize, -m_fControlSize, 2.0 * m_fControlSize, 2.0 * m_fControlSize);
    m_controlPoints[2]->setRect(-m_fControlSize, rectHeight - m_fControlSize, 2.0 * m_fControlSize, 2.0 * m_fControlSize);
    m_controlPoints[3]->setRect(rectWidth - m_fControlSize, rectHeight - m_fControlSize, 2.0 * m_fControlSize, 2.0 * m_fControlSize);

    m_controlPoints[4]->setRect(-m_fControlSize, rectHeight / 2 - m_fControlSize, m_fControlSize * 2.0, m_fControlSize * 2.0);
    m_controlPoints[5]->setRect(rectWidth - m_fControlSize, rectHeight / 2 - m_fControlSize, m_fControlSize * 2.0, m_fControlSize * 2.0);

    m_controlPoints[6]->setRect(rectWidth / 2 - m_fControlSize, -m_fControlSize, m_fControlSize * 2.0, m_fControlSize * 2.0);
    m_controlPoints[7]->setRect(rectWidth / 2 - m_fControlSize, rectHeight - m_fControlSize, m_fControlSize * 2.0, m_fControlSize * 2.0);
}