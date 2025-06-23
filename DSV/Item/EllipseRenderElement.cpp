/**
 * @file EllipseRenderElement.cpp
 * @brief 椭圆渲染元素实现文件
 * @details 实现数字病理切片查看器中的椭圆标注功能，包括椭圆绘制、测量、交互等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "EllipseRenderElement.h"

/**
 * @brief 椭圆渲染元素构造函数（仅名称）
 * @param strName 元素名称
 * @param parent 父图形项指针
 * @details 创建基本的椭圆渲染元素，设置交互属性和控制点
 */
EllipseRenderElement::EllipseRenderElement(QString strName, QGraphicsItem* parent):RenderElement(strName), QGraphicsEllipseItem(parent)
{
	m_elementType = Ellipse;  // 设置元素类型为椭圆
    setAcceptHoverEvents(true);    // 接受悬停事件
    setAcceptTouchEvents(true);    // 接受触摸事件
    setAcceptDrops(true);          // 接受拖放事件
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);  // 设置交互标志
    setZValue(10);                 // 设置Z轴值
    updateControlPoints();         // 更新控制点
}

/**
 * @brief 椭圆渲染元素构造函数（JSON对象）
 * @param json JSON对象，包含椭圆的序列化数据
 * @details 从JSON对象恢复椭圆渲染元素的状态
 */
EllipseRenderElement::EllipseRenderElement(QJsonObject json):RenderElement(json)
{
    m_elementType = Ellipse;  // 设置元素类型为椭圆
}

/**
 * @brief 椭圆渲染元素构造函数（完整参数）
 * @param strName 元素名称
 * @param rc 椭圆区域
 * @param parent 父图形项指针
 * @details 创建具有指定区域的椭圆渲染元素，并设置文本显示
 */
EllipseRenderElement::EllipseRenderElement(QString strName, const QRectF& rc, QGraphicsItem* parent)
	:RenderElement(strName), QGraphicsEllipseItem(rc,parent)
{
    m_elementType = Ellipse;  // 设置元素类型为椭圆
    setAcceptHoverEvents(true);    // 接受悬停事件
    setAcceptTouchEvents(true);    // 接受触摸事件
    setAcceptDrops(true);          // 接受拖放事件
    setFlags(ItemIsMovable|ItemIsSelectable|ItemIsFocusable);  // 设置交互标志
    setZValue(10);                 // 设置Z轴值
    setToolTip(getDescription());  // 设置工具提示
    
    // 创建文本项显示描述信息
    if (!m_pTextItem)
    {
        m_pTextItem = new QGraphicsSimpleTextItem(this);
        m_pTextItem->setPen(m_pen);
        m_pTextItem->setFont(QFont("宋体", 16));
        m_pTextItem->setFlag(ItemIgnoresTransformations);
    }
    m_pTextItem->setPos(rect().center());  // 设置文本位置为椭圆中心
    m_pTextItem->setText(getDescription());
}

/**
 * @brief 椭圆渲染元素析构函数
 * @details 清理资源，释放内存
 */
EllipseRenderElement::~EllipseRenderElement()
{
    // 析构函数实现
}

/**
 * @brief 更新椭圆区域
 * @param rc 新的椭圆区域
 * @details 更新椭圆的区域，并重新计算相关属性
 */
void EllipseRenderElement::updateRect(QRectF rc)
{
    QGraphicsEllipseItem::setRect(rc);  // 设置椭圆区域
    updateControlPoints();              // 更新控制点
    setToolTip(getDescription());       // 更新工具提示
    
    // 更新文本项
    if (!m_pTextItem)
    {
        m_pTextItem = new QGraphicsSimpleTextItem(this);
        m_pTextItem->setPen(m_pen);
        m_pTextItem->setFont(QFont("宋体", 16));
        m_pTextItem->setFlag(ItemIgnoresTransformations);
    }
    m_pTextItem->setPos(rect().center());  // 设置文本位置为椭圆中心
    m_pTextItem->setText(getDescription());
}

/**
 * @brief 获取描述信息
 * @return 包含面积和周长的描述字符串
 * @details 返回椭圆的面积和周长信息
 */
QString EllipseRenderElement::getDescription()
{
    return QStringLiteral("面积:%1 um^2\r\n周长:%2 um").arg(getArea(), 0, 'f', 1).arg(getPerimeter(), 0, 'f', 1);
}

/**
 * @brief 转换为JSON对象
 * @return 包含椭圆数据的JSON对象
 * @details 将椭圆渲染元素序列化为JSON格式
 */
QJsonObject EllipseRenderElement::toJson()
{
	QJsonObject base = RenderElement::toJson();
	return base;
}

/**
 * @brief 计算椭圆面积
 * @return 椭圆的实际面积（平方微米）
 * @details 根据像素面积和缩放因子计算椭圆的实际面积
 */
float EllipseRenderElement::getArea() {
    QRectF rc = rect();  // 获取椭圆区域
   
    return M_PI_4*rc.width()* rc.height()*data(0).toDouble()*data(0).toDouble();
}

/**
 * @brief 计算椭圆周长
 * @return 椭圆的实际周长（微米）
 * @details 使用拉马努金近似公式计算椭圆周长
 */
float EllipseRenderElement::getPerimeter()
{
    double a = rect().width();   // 获取宽度（长轴）
    double b = rect().height();  // 获取高度（短轴）
    double h = pow((a - b) / (a + b), 2);  // 计算参数h
    return M_PI * (a + b) * (1 + 3 * h / (10 + sqrt(4 - 3 * h)))*data(0).toDouble();
}

/**
 * @brief 鼠标悬停进入事件处理
 * @param event 悬停事件对象
 * @details 当鼠标悬停在椭圆上时，改变椭圆颜色
 */
void EllipseRenderElement::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    // 改变椭圆颜色为蓝色
    QPen pen;
    pen.setWidth(m_pen.width());
    pen.setColor(Qt::blue);
    setPen(pen);
}

/**
 * @brief 鼠标悬停离开事件处理
 * @param event 悬停事件对象
 * @details 当鼠标离开椭圆时，恢复椭圆颜色
 */
void EllipseRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setPen(m_pen);  // 恢复原始椭圆样式
}

/**
 * @brief 鼠标按下事件处理
 * @param event 鼠标事件对象
 * @details 检测是否点击在控制点上，如果是则开始调整大小
 */
void EllipseRenderElement::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF clickPos = event->pos();
    qreal ellipseWidth = rect().width();
    qreal ellipseHeight = rect().height();
    
    // 遍历控制点，判断是否点击在控制点上
    for (int  i = 0; i < m_controlPoints.size(); i++)
    {
        if (m_controlPoints[i]->contains(clickPos))
        {
            m_isResizing = true;
            m_lastMousePos = clickPos;
            break;
        }
    }

    QGraphicsEllipseItem::mousePressEvent(event);
}

/**
 * @brief 鼠标移动事件处理
 * @param event 鼠标事件对象
 * @details 在调整大小模式下，根据鼠标移动调整椭圆大小
 */
void EllipseRenderElement::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_isResizing)
    {
        QPointF currentPos = event->pos();
        qreal dx = currentPos.x() - m_lastMousePos.x();
        qreal dy = currentPos.y() - m_lastMousePos.y();
        qreal newWidth = rect().width() + dx;
        qreal newHeight = rect().height() + dy;
        
        // 确保新的宽度和高度为正数
        if (newWidth > 0 && newHeight > 0)
        {
            setRect(rect().x(), rect().y(), newWidth, newHeight);

            // 更新控制点位置
            updateControlPoints();
            m_lastMousePos = currentPos;
            setToolTip(getDescription());
            
            // 更新文本项
            if (!m_pTextItem)
            {
                m_pTextItem = new QGraphicsSimpleTextItem(this);
                m_pTextItem->setPen(m_pen);
                m_pTextItem->setFont(QFont("宋体", 16));
            }
           
            m_pTextItem->setPos(rect().center());  // 设置文本位置为椭圆中心
            m_pTextItem->setText(getDescription());
        }
        return;
    }
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

/**
 * @brief 鼠标释放事件处理
 * @param event 鼠标事件对象
 * @details 结束调整大小模式
 */
void EllipseRenderElement::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    m_isResizing = false;  // 结束调整大小模式
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

/**
 * @brief 更新控制点位置
 * @details 在椭圆的上下左右四个方向创建或更新控制点
 */
void EllipseRenderElement::updateControlPoints()
{
    // 如果控制点数量不足，创建新的控制点
    if (m_controlPoints.size()<4)
    {
        // 上控制点
        QGraphicsRectItem* topPoint = new QGraphicsRectItem(rect().center().x() - m_fcontrolSize, rect().top() - m_fcontrolSize, 
                                                           2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize, this);
        topPoint->setBrush(QBrush(Qt::red));
        topPoint->setPen(QPen(Qt::red));
        m_controlPoints.append(topPoint);
        
        // 下控制点
        QGraphicsRectItem* bottomPoint = new QGraphicsRectItem(rect().center().x() - m_fcontrolSize, rect().bottom() - m_fcontrolSize, 
                                                              2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize, this);
        bottomPoint->setBrush(QBrush(Qt::red));
        bottomPoint->setPen(QPen(Qt::red));
        m_controlPoints.append(bottomPoint);
        
        // 左控制点
        QGraphicsRectItem* LeftPoint = new QGraphicsRectItem(rect().left() - m_fcontrolSize, rect().center().y() - m_fcontrolSize, 
                                                            2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize, this);
        LeftPoint->setBrush(QBrush(Qt::red));
        LeftPoint->setPen(QPen(Qt::red));
        m_controlPoints.append(LeftPoint);
        
        // 右控制点
        QGraphicsRectItem* RightPoint = new QGraphicsRectItem(rect().right() - m_fcontrolSize, rect().center().y() - m_fcontrolSize, 
                                                             2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize, this);
        RightPoint->setBrush(QBrush(Qt::red));
        RightPoint->setPen(QPen(Qt::red));
        m_controlPoints.append(RightPoint);
    }
    else
    {
        // 更新现有控制点的位置
        m_controlPoints[0]->setRect(rect().center().x() - m_fcontrolSize, rect().top() - m_fcontrolSize, 
                                   2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize);
        m_controlPoints[1]->setRect(rect().center().x() - m_fcontrolSize, rect().bottom() - m_fcontrolSize, 
                                   2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize);
        m_controlPoints[2]->setRect(rect().left() - m_fcontrolSize, rect().center().y() - m_fcontrolSize, 
                                   2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize);
        m_controlPoints[3]->setRect(rect().right() - m_fcontrolSize, rect().center().y() - m_fcontrolSize, 
                                   2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize);
    }
}

