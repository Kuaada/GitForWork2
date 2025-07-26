/**
 * @file LineRenderElement.cpp
 * @brief 线条渲染元素实现文件
 * @details 实现数字病理切片查看器中的线条标注功能，包括线条绘制、测量、交互等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "LineRenderElement.h"
#include <cmath>  // 添加数学函数头文件
#include <QGraphicsView>

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
    // 移除 ItemIgnoresTransformations，改用自定义绘制
    //m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    setToolTip(getDescription());
    //m_pTextItem->setFlag(ItemIgnoresTransformations);  // 忽略变换

}

/**
 * @brief 线条渲染元素构造函数（JSON对象）
 * @param json JSON对象，包含线条的序列化数据
 * @details 从JSON对象恢复线条渲染元素的状态
 */
LineRenderElement::LineRenderElement(QJsonObject json) :RenderElement(json)
{
    m_elementType = Line;
    // 移除 ItemIgnoresTransformations，改用自定义绘制
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
    // 移除 ItemIgnoresTransformations，改用自定义绘制
    setToolTip(getDescription());
    m_pTextItem = new QGraphicsSimpleTextItem(this);
    m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));  // 使用微软雅黑字体
    m_pTextItem->setFlag(ItemIgnoresTransformations);  // 忽略变换
    
    // 使用智能单位转换显示长度信息
    m_pTextItem->setText(getDescription());
    
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
 * @details 更新线条的起点和终点位置，并重新计算相关属性
 */
void LineRenderElement::updateLine(const QPointF& pt1, const QPointF& pt2)
{
    setLine(QLineF(pt1, pt2));
    updateContrlPoints();
   
}

/**
 * @brief 获取线条描述信息
 * @return 包含线条信息的描述字符串
 * @details 返回线条的详细信息，只显示长度（周长）
 */
QString LineRenderElement::getDescription()
{
    float perimeter = getPerimeter();
    
    // 线条只显示长度（周长），不显示面积
    QString perimeterText = RenderElement::formatMeasurement(perimeter, false);
    
    return QStringLiteral("长度: %1").arg(perimeterText);
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
 * @details 当鼠标悬停在线条上时，检查是否悬停在控制点上并设置相应光标
 */
void LineRenderElement::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QPointF hoverPos = event->pos();
    
    // 检查是否悬停在控制点上
    if (m_controlPoint1 && m_controlPoint2)
    {
        QPointF control1ScenePos = m_controlPoint1->mapToScene(m_controlPoint1->rect().center());
        QPointF control2ScenePos = m_controlPoint2->mapToScene(m_controlPoint2->rect().center());
        QPointF hoverScenePos = mapToScene(hoverPos);
        
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
        
        // 增加拾取容差，使控制点更容易悬停
        qreal pickTolerance = qMax(controlSize, 12.0); // 增加最小拾取范围到12像素
        
        // 检查悬停位置是否在控制点范围内
        qreal distance1 = std::sqrt(std::pow(hoverScenePos.x() - control1ScenePos.x(), 2) + 
                                    std::pow(hoverScenePos.y() - control1ScenePos.y(), 2));
        qreal distance2 = std::sqrt(std::pow(hoverScenePos.x() - control2ScenePos.x(), 2) + 
                                    std::pow(hoverScenePos.y() - control2ScenePos.y(), 2));
        
        if (distance1 <= pickTolerance || distance2 <= pickTolerance)
        {
            setCursor(Qt::CrossCursor);
            return;
        }
    }
    
    // 如果没有悬停在控制点上，设置默认光标
    setCursor(Qt::ArrowCursor);
    
    // 悬停效果：改变线条颜色为现代蓝色
    QPen hoverPen;
    hoverPen.setWidth(m_pen.width());
    hoverPen.setColor(QColor(0, 120, 215));  // 现代蓝色
    hoverPen.setStyle(Qt::SolidLine);
    setPen(hoverPen);
    
    // 强制重绘以显示悬停效果
    update();
}

/**
 * @brief 鼠标悬停移动事件处理
 * @param event 悬停事件对象
 * @details 当鼠标在线条上移动时，检查是否悬停在控制点上并设置相应光标
 */
void LineRenderElement::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QPointF hoverPos = event->pos();
    
    // 检查是否悬停在控制点上
    if (m_controlPoint1 && m_controlPoint2)
    {
        QPointF control1ScenePos = m_controlPoint1->mapToScene(m_controlPoint1->rect().center());
        QPointF control2ScenePos = m_controlPoint2->mapToScene(m_controlPoint2->rect().center());
        QPointF hoverScenePos = mapToScene(hoverPos);
        
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
        
        // 增加拾取容差，使控制点更容易悬停
        qreal pickTolerance = qMax(controlSize, 12.0); // 增加最小拾取范围到12像素
        
        // 检查悬停位置是否在控制点范围内
        qreal distance1 = std::sqrt(std::pow(hoverScenePos.x() - control1ScenePos.x(), 2) + 
                                    std::pow(hoverScenePos.y() - control1ScenePos.y(), 2));
        qreal distance2 = std::sqrt(std::pow(hoverScenePos.x() - control2ScenePos.x(), 2) + 
                                    std::pow(hoverScenePos.y() - control2ScenePos.y(), 2));
        
        if (distance1 <= pickTolerance || distance2 <= pickTolerance)
        {
            setCursor(Qt::CrossCursor);
            return;
        }
    }
    
    // 如果没有悬停在控制点上，设置默认光标
    setCursor(Qt::ArrowCursor);
}

/**
 * @brief 鼠标悬停离开事件处理
 * @param event 悬停事件对象
 * @details 当鼠标离开线条时，恢复光标样式和线条颜色
 */
void LineRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    // 恢复原始线条样式
    setPen(m_pen);
    
    // 强制重绘以清除悬停效果
    update();
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
        m_controlPoint1 = new ControlPoint(0, 0, m_fControlSize * 2.0, m_fControlSize * 2.0, this);
    }
    if (!m_controlPoint2)
    {
        m_controlPoint2 = new ControlPoint(0, 0, m_fControlSize * 2.0, m_fControlSize * 2.0, this);
    }
    m_controlPoint1->setPos(pt1.x() - m_fControlSize, pt1.y() - m_fControlSize);
    m_controlPoint2->setPos(pt2.x() - m_fControlSize, pt2.y() - m_fControlSize);
}

/**
 * @brief 自定义绘制函数
 * @param painter 绘制器
 * @param option 绘制选项
 * @param widget 绘制目标窗口
 * @details 自定义绘制线条，保持线宽不受缩放影响，正确处理选中状态和悬停效果
 */
void LineRenderElement::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
    
    // 绘制线条
    painter->drawLine(line());
    
    // 如果正在调整大小，绘制调整提示
    if (m_isResizing) {
        QPen resizePen(QColor(255, 140, 0, 180));  // 橙色
        resizePen.setWidthF(2.0 / scale);
        resizePen.setStyle(Qt::DashLine);
        painter->setPen(resizePen);
        painter->drawLine(line());
    }
    
    // 恢复变换
    painter->restore();
}

/**
 * @brief 获取动态字体大小
 * @return 基于屏幕视窗大小的字体大小
 * @details 根据当前视图大小计算合适的字体大小，确保文字在不同缩放级别下都清晰可见
 */
int LineRenderElement::getDynamicFontSize()
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
void LineRenderElement::updateFontSize()
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
QVariant LineRenderElement::itemChange(GraphicsItemChange change, const QVariant& value)
{
    // 当元素变换发生变化时，更新字体大小
    if (change == ItemTransformChange || change == ItemScaleChange) {
        updateFontSize();
    }

    //emit 
    
    return QGraphicsLineItem::itemChange(change, value);
}