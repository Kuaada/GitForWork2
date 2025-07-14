/**
 * @file ContourRenderElement.cpp
 * @brief 轮廓渲染元素实现文件
 * @details 实现数字病理切片查看器中的轮廓标注功能，包括轮廓绘制、测量、交互等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "ContourRenderElement.h"
#include <cmath>  // 添加数学函数头文件
#include <QTextDocument>

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
    // 移除 ItemIgnoresTransformations，改用自定义绘制
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
    // 移除 ItemIgnoresTransformations，改用自定义绘制
 
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
    // 移除 ItemIgnoresTransformations，改用自定义绘制
    
    // 创建文本项显示描述信息
    m_pTextItem = new QGraphicsTextItem(this);
    m_pTextItem->setDefaultTextColor(m_pen.color());
    m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));
    m_pTextItem->setFlag(ItemIgnoresTransformations);
    m_pTextItem->setDefaultTextColor(QColor(0, 0, 0));
    m_pTextItem->document()->setDefaultStyleSheet("body { background-color: rgba(255, 255, 255, 0.8); padding: 2px; }");
    m_pTextItem->setHtml(getDescription());
    m_pTextItem->setPos(boundingRect().center());
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
        m_pTextItem = new QGraphicsTextItem(this);
        m_pTextItem->setDefaultTextColor(QColor(0, 0, 0));
        m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));
        m_pTextItem->setFlag(ItemIgnoresTransformations);
        m_pTextItem->document()->setDefaultStyleSheet("body { background-color: rgba(255, 255, 255, 0.8); padding: 2px; }");
    }
    m_pTextItem->setHtml(getDescription());
    m_pTextItem->setPos(boundingRect().center());
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
    
    // 悬停效果：改变轮廓颜色为现代蓝色
    QPen hoverPen;
    hoverPen.setWidth(m_pen.width());
    hoverPen.setColor(QColor(0, 120, 215));  // 现代蓝色
    hoverPen.setStyle(Qt::SolidLine);
    setPen(hoverPen);
    
    // 强制重绘以显示悬停效果
    update();
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
    // 恢复原始轮廓样式
    setPen(m_pen);
    
    // 强制重绘以清除悬停效果
    update();
}

/**
 * @brief 获取描述信息
 * @return 包含面积和周长的描述字符串
 * @details 返回轮廓的面积和周长信息，使用更友好的格式
 */
QString ContourRenderElement::getDescription()
{
    float area = getArea();
    float perimeter = getPerimeter();
    
    // 使用标准格式显示测量信息
    QString areaText = QString::number(area, 'f', 1);
    QString perimeterText = QString::number(perimeter, 'f', 1);
    
    return QStringLiteral("面积: %1 μm<sup>2</sup><br>周长: %2 μm").arg(areaText).arg(perimeterText);
}

/**
 * @brief 自定义绘制函数
 * @param painter 绘制器
 * @param option 绘制选项
 * @param widget 绘制目标窗口
 * @details 自定义绘制轮廓，保持线宽不受缩放影响，正确处理选中状态和悬停效果
 */
void ContourRenderElement::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // 保存当前变换
    painter->save();
    
    // 获取当前变换矩阵
    QTransform transform = painter->transform();
    
    // 计算缩放因子
    qreal scale = std::sqrt(transform.m11() * transform.m11() + transform.m12() * transform.m12());
    
    // 创建新的画笔，调整线宽以抵消缩放
    QPen pen = m_pen;
    pen.setWidthF(pen.widthF() / scale);
    
    // 根据状态设置不同的样式
    if (isSelected()) {
        // 选中状态：蓝色边框，线宽加倍
        pen.setColor(QColor(0, 120, 215));  // 现代蓝色
        pen.setWidthF(pen.widthF() * 2.5);
        pen.setStyle(Qt::SolidLine);
    } else if (hasFocus()) {
        // 焦点状态：深蓝色边框，线宽适中
        pen.setColor(QColor(0, 100, 180));
        pen.setWidthF(pen.widthF() * 1.8);
        pen.setStyle(Qt::SolidLine);
    } else {
        // 普通状态：使用原始颜色，但稍微加粗
        pen.setColor(m_pen.color());
        pen.setWidthF(pen.widthF() * 1.2);
        pen.setStyle(Qt::SolidLine);
    }
    
    painter->setPen(pen);
    
    // 如果选中，绘制半透明填充
    if (isSelected()) {
        QBrush fillBrush(QColor(0, 120, 215, 30));  // 半透明蓝色填充
        painter->setBrush(fillBrush);
    } else {
        painter->setBrush(Qt::NoBrush);
    }
    
    // 绘制轮廓
    painter->drawPolygon(polygon());
    
    // 恢复变换
    painter->restore();
}

/**
 * @brief 获取动态字体大小
 * @return 基于屏幕视窗大小的字体大小
 * @details 根据当前视图大小计算合适的字体大小，确保文字在不同缩放级别下都清晰可见
 */
int ContourRenderElement::getDynamicFontSize()
{
    // 默认字体大小（像素）
    int defaultSize = 14;  // 稍微增大默认字体大小
    
    // 尝试获取视图
    if (scene()) {
        QList<QGraphicsView*> views = scene()->views();
        if (!views.isEmpty()) {
            QGraphicsView* view = views.first();
            QSize viewSize = view->size();
            
            // 计算基于视图大小的字体大小
            // 使用视图对角线长度的比例
            qreal diagonal = std::sqrt(viewSize.width() * viewSize.width() + viewSize.height() * viewSize.height());
            
            // 字体大小约为视图对角线的1.0%（稍微增大比例）
            qreal screenBasedSize = diagonal * 0.01;
            
            // 设置最小和最大限制
            screenBasedSize = qMax(screenBasedSize, 10.0);   // 最小10像素
            screenBasedSize = qMin(screenBasedSize, 28.0);   // 最大28像素
            
            return static_cast<int>(screenBasedSize);
        }
    }
    
    // 如果无法获取视图，返回默认大小
    return defaultSize;
}

/**
 * @brief 更新文本字体大小
 * @details 根据当前视图大小更新文本项的字体大小
 */
void ContourRenderElement::updateFontSize()
{
    if (m_pTextItem) {
        m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));  // 使用微软雅黑字体
    }
}

/**
 * @brief 元素变化事件处理
 * @param change 变化类型
 * @param value 变化值
 * @return 处理后的值
 * @details 监听元素变换变化，更新字体大小
 */
QVariant ContourRenderElement::itemChange(GraphicsItemChange change, const QVariant& value)
{
    // 当元素变换发生变化时，更新字体大小
    if (change == ItemTransformChange || change == ItemScaleChange) {
        updateFontSize();
    }
    
    return QGraphicsPolygonItem::itemChange(change, value);
}

