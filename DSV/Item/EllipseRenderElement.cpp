/**
 * @file EllipseRenderElement.cpp
 * @brief 椭圆渲染元素实现文件
 * @details 实现数字病理切片查看器中的椭圆标注功能，包括椭圆绘制、测量、交互等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "EllipseRenderElement.h"
#include <cmath>  // 添加数学函数头文件
#include <QGraphicsView>
#include <QTextDocument>

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
    // 移除 ItemIgnoresTransformations，改用自定义绘制
    setZValue(10);                 // 设置Z轴值
    
    // 初始化成员变量
    m_isResizing = false;
    m_currentControlPointIndex = -1;
    
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
    // 移除 ItemIgnoresTransformations，改用自定义绘制
    
    // 初始化成员变量
    m_isResizing = false;
    m_currentControlPointIndex = -1;
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
    // 移除 ItemIgnoresTransformations，改用自定义绘制
    setZValue(10);                 // 设置Z轴值
    setToolTip(getDescription());  // 设置工具提示
    
    // 初始化成员变量
    m_isResizing = false;
    m_currentControlPointIndex = -1;
    
    //// 创建文本项显示描述信息
    //if (!m_pTextItem)
    //{
    //    m_pTextItem = new QGraphicsSimpleTextItem(this);
    //    m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));
    //    m_pTextItem->setFlag(ItemIgnoresTransformations);
    //    m_pTextItem->setPen(m_pen); // 使用元素主色描边
    //}
    //m_pTextItem->setText(getDescription());
    //m_pTextItem->setPos(rect().center());
    updateControlPoints();  // 初始化控制点
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
    
    //// 更新文本项
    //if (!m_pTextItem)
    //{
    //    m_pTextItem = new QGraphicsSimpleTextItem(this);
    //    m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));
    //    m_pTextItem->setFlag(ItemIgnoresTransformations);
    //    m_pTextItem->setPen(m_pen); // 使用元素主色描边
    //}
    //m_pTextItem->setText(getDescription());
    //m_pTextItem->setPos(rect().center());
}

/**
 * @brief 获取椭圆描述信息
 * @return 包含椭圆信息的描述字符串
 * @details 返回椭圆的详细信息，包括面积和周长，使用与直线一致的格式
 */
QString EllipseRenderElement::getDescription()
{
    float area = getArea();
    float perimeter = getPerimeter();
    
    // 使用与直线一致的格式，分行显示
    QString areaText = RenderElement::formatMeasurement(area, true);
    QString perimeterText = RenderElement::formatMeasurement(perimeter, false);
    
    return QStringLiteral("面积: %1\n周长: %2").arg(areaText).arg(perimeterText);
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
 * @details 当鼠标悬停在椭圆上时，检查是否悬停在控制点上并设置相应光标
 */
void EllipseRenderElement::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QPointF hoverPos = event->pos();
    
    // 检查是否悬停在控制点上
    for (int i = 0; i < m_controlPoints.size(); ++i)
    {
        QPointF controlScenePos = m_controlPoints[i]->mapToScene(m_controlPoints[i]->rect().center());
        QPointF hoverScenePos = mapToScene(hoverPos);
        
        // 计算控制点大小（考虑缩放）
        qreal controlSize = m_fcontrolSize;
        if (scene()) {
            QList<QGraphicsView*> views = scene()->views();
            if (!views.isEmpty()) {
                QGraphicsView* view = views.first();
                QTransform viewTransform = view->transform();
                qreal scale = std::sqrt(viewTransform.m11() * viewTransform.m11() + viewTransform.m12() * viewTransform.m12());
                controlSize = m_fcontrolSize / scale;
            }
        }
        
        // 增加拾取容差，使控制点更容易悬停
        qreal pickTolerance = qMax(controlSize, 12.0); // 增加最小拾取范围到12像素
        
        // 检查悬停位置是否在控制点范围内
        qreal distance = std::sqrt(std::pow(hoverScenePos.x() - controlScenePos.x(), 2) + 
                                   std::pow(hoverScenePos.y() - controlScenePos.y(), 2));
        
        if (distance <= pickTolerance)
        {
            setCursor(getControlPointCursor(i));
            return;
        }
    }
    
    // 如果没有悬停在控制点上，设置默认光标
    setCursor(Qt::ArrowCursor);
    
    // 悬停效果：改变椭圆颜色为现代蓝色
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
 * @details 当鼠标在椭圆上移动时，检查是否悬停在控制点上并设置相应光标
 */
void EllipseRenderElement::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QPointF hoverPos = event->pos();
    
    // 检查是否悬停在控制点上
    for (int i = 0; i < m_controlPoints.size(); ++i)
    {
        QPointF controlScenePos = m_controlPoints[i]->mapToScene(m_controlPoints[i]->rect().center());
        QPointF hoverScenePos = mapToScene(hoverPos);
        
        // 计算控制点大小（考虑缩放）
        qreal controlSize = m_fcontrolSize;
        if (scene()) {
            QList<QGraphicsView*> views = scene()->views();
            if (!views.isEmpty()) {
                QGraphicsView* view = views.first();
                QTransform viewTransform = view->transform();
                qreal scale = std::sqrt(viewTransform.m11() * viewTransform.m11() + viewTransform.m12() * viewTransform.m12());
                controlSize = m_fcontrolSize / scale;
            }
        }
        
        // 增加拾取容差，使控制点更容易悬停
        qreal pickTolerance = qMax(controlSize, 12.0); // 增加最小拾取范围到12像素
        
        // 检查悬停位置是否在控制点范围内
        qreal distance = std::sqrt(std::pow(hoverScenePos.x() - controlScenePos.x(), 2) + 
                                   std::pow(hoverScenePos.y() - controlScenePos.y(), 2));
        
        if (distance <= pickTolerance)
        {
            setCursor(getControlPointCursor(i));
            return;
        }
    }
    
    // 如果没有悬停在控制点上，设置默认光标
    setCursor(Qt::ArrowCursor);
}

/**
 * @brief 鼠标悬停离开事件处理
 * @param event 悬停事件对象
 * @details 当鼠标离开椭圆时，恢复椭圆颜色
 */
void EllipseRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    // 恢复原始椭圆样式
    setPen(m_pen);
    
    // 强制重绘以清除悬停效果
    update();
}

/**
 * @brief 鼠标按下事件处理
 * @param event 鼠标事件对象
 * @details 检测是否点击在控制点上，如果是则开始调整大小
 */
void EllipseRenderElement::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF clickPos = event->pos();
    
    // 遍历控制点，判断是否点击在控制点上
    for (int i = 0; i < m_controlPoints.size(); ++i)
    {
        // 获取控制点在场景中的实际位置
        QPointF controlScenePos = m_controlPoints[i]->mapToScene(m_controlPoints[i]->rect().center());
        QPointF clickScenePos = mapToScene(clickPos);
        
        // 计算控制点大小（考虑缩放）
        qreal controlSize = m_fcontrolSize;
        if (scene()) {
            QList<QGraphicsView*> views = scene()->views();
            if (!views.isEmpty()) {
                QGraphicsView* view = views.first();
                QTransform viewTransform = view->transform();
                qreal scale = std::sqrt(viewTransform.m11() * viewTransform.m11() + viewTransform.m12() * viewTransform.m12());
                controlSize = m_fcontrolSize / scale;
            }
        }
        
        // 增加拾取容差，使控制点更容易点击
        qreal pickTolerance = qMax(controlSize, 12.0); // 增加最小拾取范围到12像素
        
        // 检查点击位置是否在控制点范围内
        qreal distance = std::sqrt(std::pow(clickScenePos.x() - controlScenePos.x(), 2) + 
                                   std::pow(clickScenePos.y() - controlScenePos.y(), 2));
        
        if (distance <= pickTolerance)
        {
            m_isResizing = true;
            m_currentControlPointIndex = i;
            m_lastMousePos = clickPos;
            
            setFocus();
            
            // 设置相应的鼠标光标
            setCursor(getControlPointCursor(i));
            
            event->accept();
            return; // 直接返回，不调用基类事件
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
    if (m_isResizing && m_currentControlPointIndex >= 0)
    {
        QPointF currentPos = event->pos();
        qreal dx = currentPos.x() - m_lastMousePos.x();
        qreal dy = currentPos.y() - m_lastMousePos.y();
        
        QRectF currentRect = rect();
        qreal newX = currentRect.x();
        qreal newY = currentRect.y();
        qreal newWidth = currentRect.width();
        qreal newHeight = currentRect.height();

        // 根据不同的控制点实现不同的调整行为
        switch (m_currentControlPointIndex)
        {
        case 0: // 上控制点 - 调整上边
            newY += dy;
            newHeight -= dy;
            break;
        case 1: // 下控制点 - 调整下边
            newHeight += dy;
            break;
        case 2: // 左控制点 - 调整左边
            newX += dx;
            newWidth -= dx;
            break;
        case 3: // 右控制点 - 调整右边
            newWidth += dx;
            break;
        }
        
        // 确保新的宽度和高度为正数
        if (newWidth > 10 && newHeight > 10)
        {
            // 只更新椭圆项的矩形区域，不修改pos()
            setRect(QRectF(newX, newY, newWidth, newHeight));

            // 更新控制点位置
            updateControlPoints();
            
            // 更新工具提示和文本显示
            setToolTip(getDescription());
            if (m_pTextItem) {
                m_pTextItem->setText(getDescription());
                m_pTextItem->setPos(rect().center());  // 更新文本位置
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
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

/**
 * @brief 鼠标释放事件处理
 * @param event 鼠标事件对象
 * @details 结束调整大小模式，恢复默认光标
 */
void EllipseRenderElement::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_isResizing) {
        m_isResizing = false;
        m_currentControlPointIndex = -1;
        // 恢复默认光标
        emit sendPerimeterAndArea(getPerimeter(), getArea());
        setCursor(Qt::ArrowCursor);
        event->accept();
        return; // 直接返回，不调用基类事件
    }
    
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

/**
 * @brief 更新控制点位置
 * @details 在椭圆的上下左右四个方向创建或更新控制点
 */
void EllipseRenderElement::updateControlPoints()
{
    // 如果控制点数量不足，创建新的控制点
    if (m_controlPoints.size() < 4)
    {
        // 上控制点
        ControlPoint* topPoint = new ControlPoint(0, 0, 2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize, this);
        topPoint->setPos(rect().center().x() - m_fcontrolSize, rect().top() - m_fcontrolSize);
        m_controlPoints.append(topPoint);
        
        // 下控制点
        ControlPoint* bottomPoint = new ControlPoint(0, 0, 2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize, this);
        bottomPoint->setPos(rect().center().x() - m_fcontrolSize, rect().bottom() - m_fcontrolSize);
        m_controlPoints.append(bottomPoint);
        
        // 左控制点
        ControlPoint* LeftPoint = new ControlPoint(0, 0, 2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize, this);
        LeftPoint->setPos(rect().left() - m_fcontrolSize, rect().center().y() - m_fcontrolSize);
        m_controlPoints.append(LeftPoint);
        
        // 右控制点
        ControlPoint* RightPoint = new ControlPoint(0, 0, 2.0 * m_fcontrolSize, 2.0 * m_fcontrolSize, this);
        RightPoint->setPos(rect().right() - m_fcontrolSize, rect().center().y() - m_fcontrolSize);
        m_controlPoints.append(RightPoint);
    }
    else
    {
        // 更新现有控制点的位置 - 使用setPos，保持原有的定位逻辑
        // 控制点位置是相对于Item的本地坐标
        m_controlPoints[0]->setPos(rect().center().x() - m_fcontrolSize, rect().top() - m_fcontrolSize);
        m_controlPoints[1]->setPos(rect().center().x() - m_fcontrolSize, rect().bottom() - m_fcontrolSize);
        m_controlPoints[2]->setPos(rect().left() - m_fcontrolSize, rect().center().y() - m_fcontrolSize);
        m_controlPoints[3]->setPos(rect().right() - m_fcontrolSize, rect().center().y() - m_fcontrolSize);
    }
}

/**
 * @brief 自定义绘制函数
 * @param painter 绘制器
 * @param option 绘制选项
 * @param widget 绘制目标窗口
 * @details 自定义绘制椭圆，保持线宽不受缩放影响，正确处理选中状态和悬停效果
 */
void EllipseRenderElement::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
    
    // 绘制椭圆
    painter->drawEllipse(rect());
    
    // 如果正在调整大小，绘制调整提示
    if (m_isResizing) {
        QPen resizePen(QColor(255, 140, 0, 180));  // 橙色
        resizePen.setWidthF(2.0 / scale);
        resizePen.setStyle(Qt::DashLine);
        painter->setPen(resizePen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(rect());
    }
    
    // 恢复变换
    painter->restore();
}

/**
 * @brief 获取动态字体大小
 * @return 基于屏幕视窗大小的字体大小
 * @details 根据当前视图大小计算合适的字体大小，确保文字在不同缩放级别下都清晰可见
 */
int EllipseRenderElement::getDynamicFontSize()
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
void EllipseRenderElement::updateFontSize()
{
    if (m_pTextItem) {
        m_pTextItem->setFont(QFont("Microsoft YaHei", getDynamicFontSize(), QFont::Normal));  // 使用微软雅黑字体
    }
}

/**
 * @brief 获取控制点对应的鼠标光标
 * @param controlPointIndex 控制点索引
 * @return 对应的鼠标光标
 * @details 根据控制点位置返回相应的鼠标光标样式
 */
Qt::CursorShape EllipseRenderElement::getControlPointCursor(int controlPointIndex)
{
    switch (controlPointIndex)
    {
    case 0: // 上控制点 - 垂直调整
        return Qt::SizeVerCursor;
    case 1: // 下控制点 - 垂直调整
        return Qt::SizeVerCursor;
    case 2: // 左控制点 - 水平调整
        return Qt::SizeHorCursor;
    case 3: // 右控制点 - 水平调整
        return Qt::SizeHorCursor;
    default:
        return Qt::ArrowCursor;
    }
}

/**
 * @brief 元素变化事件处理
 * @param change 变化类型
 * @param value 变化值
 * @return 处理后的值
 * @details 监听元素变换变化，更新控制点位置和字体大小
 */
QVariant EllipseRenderElement::itemChange(GraphicsItemChange change, const QVariant& value)
{
    // 当元素变换发生变化时，更新控制点位置和字体大小
    if (change == ItemTransformChange || change == ItemScaleChange) {
        updateControlPoints();
        updateFontSize();
    }
    
    return QGraphicsEllipseItem::itemChange(change, value);
}

