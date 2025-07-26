/**
 * @file RectRenderElement.cpp
 * @brief 矩形渲染元素实现文件
 * @details 实现数字病理切片查看器中的矩形标注功能，包括矩形绘制、测量、交互等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "RectRenderElement.h"
#include <cmath>  // 添加数学函数头文件
#include <QTextDocument>


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
    setToolTip(getDescription());
    createControlPoints();
    updateControlPointsPosition(); // 确保控制点位置正确初始化
}

/**
 * @brief 矩形渲染元素构造函数（JSON对象）
 * @param json JSON对象，包含矩形的序列化数据
 * @details 从JSON对象恢复矩形渲染元素的状态
 */
RectRenderElement::RectRenderElement(QJsonObject json):RenderElement(json)
{
    m_elementType = Rectangle;
    // 移除 ItemIgnoresTransformations，改用自定义绘制
   // m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    //scene()->addItem(m_pTextItem);
    createControlPoints();
    updateControlPointsPosition(); // 确保控制点位置正确初始化
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
    m_elementType = Rectangle;  // 设置元素类型为矩形
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    setAcceptDrops(true);
    setFlags(ItemIsMovable|ItemIsSelectable|ItemIsFocusable);
    setToolTip(getDescription());
    createControlPoints();
    updateControlPointsPosition(); // 确保控制点位置正确初始化
    setZValue(10);
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
    QGraphicsRectItem::setRect(QRectF(0, 0, rect.width(), rect.height()));
    setPos(rect.topLeft());
    setToolTip(getDescription());
    updateControlPointsPosition();
    // 延迟创建文本项到itemChange，不在这里创建
    //if (m_pTextItem) {
    //    m_pTextItem->setPlainText(getDescription());
    //    m_pTextItem->setPos(10, 10);
    //}
    
    //// 创建文本项显示描述信息
    //if (!m_pTextItem) {
    //    m_pTextItem = new QGraphicsTextItem(this);
    //    m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));
    //    m_pTextItem->setFlag(ItemIgnoresTransformations);
    //    m_pTextItem->setDefaultTextColor(m_pen.color()); // 使用元素主色作为文字颜色
    //}
    //        m_pTextItem->setPlainText(getDescription());
    //m_pTextItem->setPos(10, 10);
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
 * @details 当鼠标悬停在矩形上时，显示悬停效果
 */
void RectRenderElement::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    // 悬停效果：改变矩形颜色为现代蓝色
    QPen hoverPen;
    hoverPen.setWidth(m_pen.width());
    hoverPen.setColor(QColor(0, 120, 215));  // 现代蓝色
    hoverPen.setStyle(Qt::SolidLine);
    setPen(hoverPen);
    
    // 强制重绘以显示悬停效果
    update();
    
    QGraphicsRectItem::hoverEnterEvent(event);
}

/**
 * @brief 鼠标悬停移动事件处理
 * @param event 悬停事件对象
 * @details 当鼠标在矩形上移动时，处理悬停移动事件
 */
void RectRenderElement::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverMoveEvent(event);
}

/**
 * @brief 鼠标悬停离开事件处理
 * @param event 悬停事件对象
 * @details 当鼠标离开矩形时，恢复光标样式和矩形颜色
 */
void RectRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    // 恢复原始矩形样式
    setPen(m_pen);
    
    // 强制重绘以清除悬停效果
    update();
}

/**
 * @brief 鼠标按下事件处理
 * @param event 鼠标事件对象
 * @details 检测是否点击在控制点上，如果是则开始调整大小
 */
void RectRenderElement::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF clickPos = event->pos();
    bool controlPointClicked = false;

    // 遍历控制点，判断是否点击在控制点上
    for (int i = 0; i < m_controlPoints.size(); ++i)
    {
        // 使用控制点的shape()函数来判断点击
        if (m_controlPoints[i]->shape().contains(m_controlPoints[i]->mapFromParent(clickPos)))
        {
            m_isResizing = true;
            m_currentControlPointIndex = i;
            m_lastMousePos = clickPos;
            setFocus();
            
            // 设置相应的鼠标光标
            setCursor(getControlPointCursor(i));
            
            controlPointClicked = true;
            event->accept();
            break;
        }
    }

    // 如果没有点击控制点，则调用基类处理移动和选择
    if (!controlPointClicked) {
        QGraphicsRectItem::mousePressEvent(event);
    }
}

/**
 * @brief 鼠标移动事件处理
 * @param event 鼠标事件对象
 * @details 在调整大小模式下，根据鼠标移动调整矩形大小
 */
void RectRenderElement::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_isResizing && m_currentControlPointIndex >= 0)
    {
        QPointF currentPos = event->pos();
        qreal dx = currentPos.x() - m_lastMousePos.x();
        qreal dy = currentPos.y() - m_lastMousePos.y();
        
        QPointF currentItemPos = pos();
        QRectF currentRect = rect();
        qreal newItemX = currentItemPos.x();
        qreal newItemY = currentItemPos.y();
        qreal newWidth = currentRect.width();
        qreal newHeight = currentRect.height();

        // 根据不同的控制点实现不同的调整行为
        switch (m_currentControlPointIndex)
        {
        case 0: // 左上角 - 调整左上角
            newItemX += dx;
            newItemY += dy;
            newWidth -= dx;
            newHeight -= dy;
            break;
        case 1: // 右上角 - 调整右上角
            newItemY += dy;
            newWidth += dx;
            newHeight -= dy;
            break;
        case 2: // 左下角 - 调整左下角
            newItemX += dx;
            newWidth -= dx;
            newHeight += dy;
            break;
        case 3: // 右下角 - 调整右下角
            newWidth += dx;
            newHeight += dy;
            break;
        case 4: // 左边中间 - 调整左边
            newItemX += dx;
            newWidth -= dx;
            break;
        case 5: // 右边中间 - 调整右边
            newWidth += dx;
            break;
        case 6: // 上边中间 - 调整上边
            newItemY += dy;
            newHeight -= dy;
            break;
        case 7: // 下边中间 - 调整下边
            newHeight += dy;
            break;
        }

        // 确保最小尺寸限制
        if (newWidth > 10 && newHeight > 10)
        {
            // 更新矩形项的位置和大小
            setPos(newItemX, newItemY);
            setRect(QRectF(0, 0, newWidth, newHeight));
            
            // 更新控制点位置
            updateControlPointsPosition();
            
            // 更新工具提示和文本显示
        setToolTip(getDescription());
            if (m_pTextItem) {
        m_pTextItem->setPlainText(getDescription());
            }
            
            // 更新最后鼠标位置
            m_lastMousePos = currentPos;
            
            // 强制重绘
            update();
        }
        emit sendPerimeterAndArea(getPerimeter(), getArea());
        event->accept();
        return;
    }

    QGraphicsRectItem::mouseMoveEvent(event);
}

/**
 * @brief 鼠标释放事件处理
 * @param event 鼠标事件对象
 * @details 结束调整大小模式，恢复默认光标
 */
void RectRenderElement::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_isResizing) {
        m_isResizing = false;
        m_currentControlPointIndex = -1;
        // 恢复默认光标
        emit sendPerimeterAndArea(getPerimeter(), getArea());
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    
    QGraphicsRectItem::mouseReleaseEvent(event);
}

/**
 * @brief 获取矩形描述信息
 * @return 包含矩形信息的描述字符串
 * @details 返回矩形的详细信息，包括名称、位置、大小、面积等
 */
QString RectRenderElement::getDescription()
{
    float area = getArea();
    float perimeter = getPerimeter();
    
    // 使用与直线一致的格式，分行显示
    QString areaText = RenderElement::formatMeasurement(area, true);
    QString perimeterText = RenderElement::formatMeasurement(perimeter, false);
    
    return QStringLiteral("面积: %1\n周长: %2").arg(areaText).arg(perimeterText);
}

/**
 * @brief 获取控制点对应的鼠标光标
 * @param controlPointIndex 控制点索引
 * @return 对应的鼠标光标
 * @details 根据控制点位置返回相应的鼠标光标样式
 */
Qt::CursorShape RectRenderElement::getControlPointCursor(int controlPointIndex)
{
    switch (controlPointIndex)
    {
    case 0: // 左上角 - 对角线调整
        return Qt::SizeFDiagCursor;
    case 1: // 右上角 - 对角线调整
        return Qt::SizeBDiagCursor;
    case 2: // 左下角 - 对角线调整
        return Qt::SizeBDiagCursor;
    case 3: // 右下角 - 对角线调整
        return Qt::SizeFDiagCursor;
    case 4: // 左边中间 - 水平调整
        return Qt::SizeHorCursor;
    case 5: // 右边中间 - 水平调整
        return Qt::SizeHorCursor;
    case 6: // 上边中间 - 垂直调整
        return Qt::SizeVerCursor;
    case 7: // 下边中间 - 垂直调整
        return Qt::SizeVerCursor;
    default:
        return Qt::ArrowCursor;
    }
}

/**
 * @brief 创建控制点
 * @details 在矩形的各个关键位置创建控制点，用于调整矩形大小和形状
 */
void RectRenderElement::createControlPoints()
{
    qreal rectWidth = rect().width();
    qreal rectHeight = rect().height();
    
    // 左上角控制点 - 相对于矩形项的位置
    ControlPoint* topLeftPoint = new ControlPoint(-m_fControlSize, -m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize, this);
    m_controlPoints.append(topLeftPoint);

    // 右上角控制点 - 相对于矩形项的位置
    ControlPoint* topRightPoint = new ControlPoint(rectWidth - m_fControlSize, -m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize, this);
    m_controlPoints.append(topRightPoint);
                               
    // 左下角控制点 - 相对于矩形项的位置
    ControlPoint* bottomLeftPoint = new ControlPoint(-m_fControlSize, rectHeight - m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize, this);
    m_controlPoints.append(bottomLeftPoint);

    // 右下角控制点 - 相对于矩形项的位置
    ControlPoint* bottomRightPoint = new ControlPoint(rectWidth - m_fControlSize, rectHeight - m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize, this);
    m_controlPoints.append(bottomRightPoint);

    // 左边中间控制点 - 相对于矩形项的位置
    ControlPoint* leftMidPoint = new ControlPoint(-m_fControlSize, rectHeight / 2 - m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0, this);
    m_controlPoints.append(leftMidPoint);

    // 右边中间控制点 - 相对于矩形项的位置
    ControlPoint* rightMidPoint = new ControlPoint(rectWidth - m_fControlSize, rectHeight / 2 - m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0, this);
    m_controlPoints.append(rightMidPoint);

    // 上边中间控制点 - 相对于矩形项的位置
    ControlPoint* topMidPoint = new ControlPoint(rectWidth / 2 - m_fControlSize, -m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0, this);
    m_controlPoints.append(topMidPoint);

    // 下边中间控制点 - 相对于矩形项的位置
    ControlPoint* bottomMidPoint = new ControlPoint(rectWidth / 2 - m_fControlSize, rectHeight - m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0, this);
    m_controlPoints.append(bottomMidPoint);
}

/**
 * @brief 更新控制点位置
 * @details 根据当前矩形区域，更新所有控制点的位置
 */
void RectRenderElement::updateControlPointsPosition()
{
    qreal rectWidth = rect().width();
    qreal rectHeight = rect().height();

    // 更新所有控制点的位置 - 使用setRect来设置位置和大小
    // 左上角控制点
    m_controlPoints[0]->setRect(-m_fControlSize, -m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize);
    
    // 右上角控制点
    m_controlPoints[1]->setRect(rectWidth - m_fControlSize, -m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize);
    
    // 左下角控制点
    m_controlPoints[2]->setRect(-m_fControlSize, rectHeight - m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize);
    
    // 右下角控制点
    m_controlPoints[3]->setRect(rectWidth - m_fControlSize, rectHeight - m_fControlSize, 2.0* m_fControlSize, 2.0* m_fControlSize);

    // 左边中间控制点
    m_controlPoints[4]->setRect(-m_fControlSize, rectHeight / 2 - m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0);
    
    // 右边中间控制点
    m_controlPoints[5]->setRect(rectWidth - m_fControlSize, rectHeight / 2 - m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0);

    // 上边中间控制点
    m_controlPoints[6]->setRect(rectWidth / 2 - m_fControlSize, -m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0);
    
    // 下边中间控制点
    m_controlPoints[7]->setRect(rectWidth / 2 - m_fControlSize, rectHeight - m_fControlSize, m_fControlSize *2.0, m_fControlSize *2.0);
}

/**
 * @brief 元素变化事件处理
 * @param change 变化类型
 * @param value 变化值
 * @return 处理后的值
 * @details 监听元素变换变化，更新控制点位置
 */
QVariant RectRenderElement::itemChange(GraphicsItemChange change, const QVariant& value)
{
    // 当元素变换发生变化时，更新控制点位置和字体大小
    if (change == ItemTransformChange || change == ItemScaleChange) {
        if (!m_controlPoints.isEmpty()) {
            updateControlPointsPosition();
        }
        if (m_pTextItem) {
            m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

/**
 * @brief 更新控制点拾取区域
 * @details 根据当前视图缩放状态更新所有控制点的拾取区域
 */
void RectRenderElement::updateControlPointsPickupArea()
{
    for (ControlPoint* controlPoint : m_controlPoints) {
        controlPoint->updatePickupArea();
    }
}



/**
 * @brief 自定义绘制函数
 * @param painter 绘制器
 * @param option 绘制选项
 * @param widget 绘制目标窗口
 * @details 自定义绘制矩形，保持线宽不受缩放影响，正确处理选中状态和悬停效果
 */
void RectRenderElement::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
    
    // 绘制矩形
    painter->drawRect(rect());
    
    // 如果正在调整大小，绘制调整提示
    if (m_isResizing) {
        QPen resizePen(QColor(255, 140, 0, 180));  // 橙色
        resizePen.setWidthF(2.0 / scale);
        resizePen.setStyle(Qt::DashLine);
        painter->setPen(resizePen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect());
    }
    
    // 恢复变换
    painter->restore();
}

/**
 * @brief 获取动态字体大小
 * @return 基于屏幕视窗大小的字体大小
 * @details 根据当前视图大小计算合适的字体大小，确保文字在不同缩放级别下都清晰可见
 */
int RectRenderElement::getDynamicFontSize()
{
    // 默认字体大小（像素）
    int defaultSize = 14;  // 与椭圆元素保持一致
    
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
void RectRenderElement::updateFontSize()
{
    if (m_pTextItem) {
        m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));  // 使用微软雅黑字体
    }
}