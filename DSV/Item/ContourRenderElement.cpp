/**
 * @file ContourRenderElement.cpp
 * @brief 轮廓渲染元素实现文件
 * @details 实现数字病理切片查看器中的轮廓标注功能，包括轮廓绘制、测量、交互等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "ContourRenderElement.h"

/**
 * @brief 轮廓渲染元素构造函数（仅名称）
 * @param strName 元素名称
 * @param parent 父图形项指针
 * @details 创建基本的轮廓渲染元素，设置交互属性
 */
ContourRenderElement::ContourRenderElement(QString strName, QGraphicsItem* parent) :RenderElement(strName), QGraphicsPolygonItem(parent)
{
	m_elementType = Angle;  // 设置元素类型为角度（轮廓）
    setAcceptHoverEvents(true);    // 接受悬停事件
    setAcceptTouchEvents(true);    // 接受触摸事件
    setAcceptDrops(true);          // 接受拖放事件
    setZValue(10);                 // 设置Z轴值
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);  // 设置交互标志
   // m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    
  

}

/**
 * @brief 轮廓渲染元素构造函数（JSON对象）
 * @param json JSON对象，包含轮廓的序列化数据
 * @details 从JSON对象恢复轮廓渲染元素的状态
 */
ContourRenderElement::ContourRenderElement(QJsonObject json):RenderElement(json)
{
    m_elementType = Angle;  // 设置元素类型为角度（轮廓）
 
}

/**
 * @brief 轮廓渲染元素构造函数（完整参数）
 * @param strName 元素名称
 * @param pts 轮廓点集合
 * @param parent 父图形项指针
 * @details 创建具有指定点集合的轮廓渲染元素，并设置文本显示
 */
ContourRenderElement::ContourRenderElement(QString strName, QVector<QPointF> pts, QGraphicsItem* parent)
	:RenderElement(strName), QGraphicsPolygonItem(QPolygonF(pts))
{
    m_elementType = Angle;  // 设置元素类型为角度（轮廓）
    setAcceptHoverEvents(true);    // 接受悬停事件
    setAcceptTouchEvents(true);    // 接受触摸事件
    setAcceptDrops(true);          // 接受拖放事件
    setZValue(10);                 // 设置Z轴值
    setFlags(ItemIsMovable|ItemIsSelectable|ItemIsFocusable);  // 设置交互标志
    
    // 创建文本项显示描述信息
    m_pTextItem = new QGraphicsSimpleTextItem(this);
    m_pTextItem->setFont(QFont("宋体", 16));
    m_pTextItem->setText(getDescription());
    m_pTextItem->setPen(m_pen);
}

/**
 * @brief 轮廓渲染元素析构函数
 * @details 清理资源，释放内存
 */
ContourRenderElement::~ContourRenderElement()
{
    // 析构函数实现
}

/**
 * @brief 添加轮廓点
 * @param pt 要添加的点
 * @details 向轮廓中添加新的点
 */
void ContourRenderElement::addPoint(const QPointF& pt)
{
    QPolygonF poly = polygon();  // 获取当前多边形
    poly.append(pt);             // 添加新点
    updateContour(poly);         // 更新轮廓
}

/**
 * @brief 更新轮廓
 * @param pts 新的点集合
 * @details 使用新的点集合更新轮廓，并重新计算相关属性
 */
void ContourRenderElement::updateContour(QVector<QPointF> pts)
{
    setPolygon(QPolygonF(pts));  // 设置新的多边形
    setToolTip(getDescription()); // 更新工具提示
    
    // 创建或更新文本项
    if (!m_pTextItem)
    {
        m_pTextItem = new QGraphicsSimpleTextItem(this);
        m_pTextItem->setPen(m_pen);
        m_pTextItem->setFont(QFont("宋体",12));
        m_pTextItem->setFlag(ItemIgnoresTransformations);
    }
    m_pTextItem->setText(getDescription());
    m_pTextItem->setPos(boundingRect().center());  // 设置文本位置为轮廓中心
}

/**
 * @brief 转换为JSON对象
 * @return 包含轮廓数据的JSON对象
 * @details 将轮廓渲染元素序列化为JSON格式
 */
QJsonObject ContourRenderElement::toJson()
{
	QJsonObject base = RenderElement::toJson();
	return base;
}

/**
 * @brief 鼠标悬停进入事件处理
 * @param event 悬停事件对象
 * @details 当鼠标悬停在轮廓上时，改变光标样式和轮廓颜色
 */
void ContourRenderElement::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(QCursor(Qt::CrossCursor));  // 设置十字光标
    
    // 改变轮廓颜色为蓝色
    QPen pen;
    pen.setWidth(m_pen.width());
    pen.setColor(Qt::blue);
    setPen(pen);
}

/**
 * @brief 计算轮廓面积
 * @return 轮廓的实际面积（平方微米）
 * @details 使用多边形面积公式计算轮廓的实际面积
 */
float ContourRenderElement:: getArea()
{
    QPolygonF poly = polygon();  // 获取多边形
    double area = 0;
    int numPoints = poly.size();
    
    // 使用多边形面积公式计算面积
    for (int i = 0; i < numPoints - 1; ++i) {
        QPointF currentPoint = poly.at(i);
        QPointF nextPoint = poly.at(i + 1);
        area += (currentPoint.x() * nextPoint.y() - nextPoint.x() * currentPoint.y());
    }
    
    return qAbs(area / 4.0)*data(0).toDouble()*data(0).toDouble();
}

/**
 * @brief 计算轮廓周长
 * @return 轮廓的实际周长（微米）
 * @details 计算轮廓各边长的总和
 */
float ContourRenderElement::getPerimeter()
{
    QPolygonF poly = polygon();  // 获取多边形
    double perimeter = 0;
    int numPoints = poly.size();
    
    // 计算各边长度的总和
    for (int i = 0; i < numPoints; ++i) {
        int nextIndex = (i + 1) % numPoints;  // 下一个顶点索引，循环处理最后一个顶点到第一个顶点的情况
        QPointF currentPoint = poly.at(i);
        QPointF nextPoint = poly.at(nextIndex);
        double dx = nextPoint.x() - currentPoint.x();
        double dy = nextPoint.y() - currentPoint.y();
        perimeter += sqrt(dx * dx + dy * dy);
    }
    
    return perimeter * data(0).toDouble();
}

/**
 * @brief 鼠标悬停离开事件处理
 * @param event 悬停事件对象
 * @details 当鼠标离开轮廓时，恢复光标样式和轮廓颜色
 */
void ContourRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setPen(m_pen);  // 恢复原始轮廓样式
    setCursor(QCursor(Qt::PointingHandCursor));  // 设置手型光标
}

/**
 * @brief 获取描述信息
 * @return 包含面积和周长的描述字符串
 * @details 返回轮廓的面积和周长信息
 */
QString ContourRenderElement::getDescription()
{
    return QStringLiteral("面积:%1 um^2\r\n周长:%2 um").arg(getArea(), 0, 'f', 1).arg(getPerimeter(), 0, 'f', 1);
}