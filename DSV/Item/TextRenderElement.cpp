/**
 * @file TextRenderElement.cpp
 * @brief 文本渲染元素实现文件
 * @details 该文件实现了文本渲染元素的功能，包括：
 *          - 文本的显示和编辑
 *          - 鼠标悬停效果
 *          - 多行文本支持
 *          - JSON序列化
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "TextRenderElement.h"
#include <cmath>  // 添加数学函数头文件
#include <QTextDocument>

/**
 * @brief 构造函数：通过名称初始化文本渲染元素
 * @param strName 元素名称
 * @param parent 父图形项
 * @details 初始化文本渲染元素，设置交互标志、字体和默认文本
 */
TextRenderElement::TextRenderElement(QString strName, QGraphicsItem* parent) :RenderElement(strName), QGraphicsTextItem(parent)
{
	m_elementType = Text;
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    setAcceptDrops(true);
    setZValue(10);
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
    setTextInteractionFlags(Qt::NoTextInteraction); // 初始状态不可编辑
    setInputMethodHints(Qt::ImhMultiLine); // 允许多行输入
    QFont font("Microsoft YaHei", 18, QFont::Normal);  // 使用微软雅黑字体，增大字号
    setFont(font);
    setDefaultTextColor(QColor(0, 0, 0));  // 设置默认文字颜色为黑色
    setPlainText(QStringLiteral("双击输入内容"));
    setFlag(ItemIgnoresTransformations);
   // m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    setFlag(QGraphicsItem::ItemAcceptsInputMethod, true);
}

/**
 * @brief 构造函数：通过JSON对象初始化文本渲染元素
 * @param json JSON对象
 * @details 从JSON对象中恢复文本渲染元素的属性
 */
TextRenderElement::TextRenderElement(QJsonObject json):RenderElement(json)
{
    m_elementType = Text;
   // m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    //scene()->addItem(m_pTextItem);
    setFlag(QGraphicsItem::ItemAcceptsInputMethod, true);
    setInputMethodHints(Qt::ImhMultiLine); // 允许多行输入
}

/**
 * @brief 构造函数：通过名称和文本初始化文本渲染元素
 * @param strName 元素名称
 * @param strText 初始文本内容
 * @param parent 父图形项
 * @details 初始化文本渲染元素并设置指定的文本内容
 */
TextRenderElement::TextRenderElement(QString strName, QString strText,QGraphicsItem* parent)
	:RenderElement(strName), QGraphicsTextItem(strText, parent)
{
    m_elementType = Text;
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    setAcceptDrops(true);
    setZValue(10);
    setFlags(ItemIsMovable|ItemIsSelectable|ItemIsFocusable);
    setTextInteractionFlags(Qt::NoTextInteraction); // 初始状态不可编辑
    setInputMethodHints(Qt::ImhMultiLine); // 允许多行输入
    QFont font("Microsoft YaHei", 18, QFont::Normal);  // 使用微软雅黑字体，增大字号
    setFont(font);
    setDefaultTextColor(QColor(0, 0, 0));  // 设置默认文字颜色为黑色
    setFlag(ItemIgnoresTransformations);
    //m_pTextItem = new QGraphicsSimpleTextItem(this);
    //m_pTextItem->setText(QString("面积:%1 周长:%2").arg(getArea(), 0, 'g', 4).arg(getPerimeter(), 0, 'g', 4));
    //m_pTextItem->setPos(mapToScene(pos()));
   // scene()->addItem(m_pTextItem);
    setFlag(QGraphicsItem::ItemAcceptsInputMethod, true);
}

/**
 * @brief 析构函数：清理文本渲染元素资源
 * @details Qt会自动清理图形项资源
 */
TextRenderElement::~TextRenderElement()
{
}

/**
 * @brief 转换为JSON对象
 * @return JSON对象
 * @details 将文本渲染元素的属性序列化为JSON格式
 */
QJsonObject TextRenderElement::toJson()
{
	QJsonObject base = RenderElement::toJson();
	return base;
}

/**
 * @brief 鼠标悬停进入事件
 * @param event 悬停事件
 * @details 鼠标悬停时改变光标样式和文本颜色
 */
void TextRenderElement::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(QCursor(Qt::IBeamCursor));  // 文本编辑光标
    setDefaultTextColor(QColor(0, 120, 215));  // 现代蓝色
}

/**
 * @brief 鼠标悬停离开事件
 * @param event 悬停事件
 * @details 鼠标离开时恢复光标样式和文本颜色
 */
void TextRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    setDefaultTextColor(m_pen.color());
    QGraphicsTextItem::hoverLeaveEvent(event); // 确保调用父类方法
}

/**
 * @brief 自定义绘制函数
 * @param painter 绘制器
 * @param option 绘制选项
 * @param widget 绘制目标窗口
 * @details 自定义绘制文本，提供更好的视觉效果
 */
void TextRenderElement::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // 保存当前变换
    painter->save();
    
    // 获取当前变换矩阵
    QTransform transform = painter->transform();
    
    // 计算缩放因子
    qreal scale = std::sqrt(transform.m11() * transform.m11() + transform.m12() * transform.m12());
    
    // 根据状态设置不同的样式
    if (isSelected()) {
        // 选中状态：绘制蓝色边框
        QPen selectionPen(QColor(0, 120, 215));  // 现代蓝色
        selectionPen.setWidthF(2.0 / scale);
        selectionPen.setStyle(Qt::DashLine);
        painter->setPen(selectionPen);
        painter->setBrush(QColor(0, 120, 215, 20));  // 半透明蓝色填充
        painter->drawRect(boundingRect());
    } else if (hasFocus()) {
        // 焦点状态：绘制深蓝色边框
        QPen focusPen(QColor(0, 100, 180));
        focusPen.setWidthF(1.5 / scale);
        focusPen.setStyle(Qt::SolidLine);
        painter->setPen(focusPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
    
    // 调用父类绘制方法
    QGraphicsTextItem::paint(painter, option, widget);
    
    // 恢复变换
    painter->restore();
}

/**
 * @brief 场景事件处理
 * @param event 场景事件
 * @details 重写场景事件处理，在高放大倍率下防止坐标转换问题导致的视角偏移
 */
bool TextRenderElement::sceneEvent(QEvent* event)
{
    // 对于鼠标事件，在编辑模式下进行特殊处理
    if (event->type() == QEvent::GraphicsSceneMousePress) {
        QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

        // 如果当前在编辑模式
        if (textInteractionFlags() != Qt::NoTextInteraction) {
            // 检查点击位置是否在文本边界内
            if (!isPointInTextArea(mouseEvent->pos())) {
                // 点击在外部，退出编辑模式
                clearFocus();
                setTextInteractionFlags(Qt::NoTextInteraction);
                // 接受事件，防止传递给视图
                event->accept();
                return true;
            }
        }
    }

    // 对于双击事件，也进行特殊处理
    if (event->type() == QEvent::GraphicsSceneMouseDoubleClick) {
        QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

        // 检查双击位置是否在文本边界内
        if (isPointInTextArea(mouseEvent->pos())) {
            // 进入编辑模式
            if (toPlainText() == QStringLiteral("双击输入内容")) {
                setPlainText("");
            }

            // 设置编辑模式
            setupEditMode();

            setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
            setFocus(Qt::MouseFocusReason);
            // 接受事件
            event->accept();
            return true;
        }
    }

    // 其他事件交给父类处理
    return QGraphicsTextItem::sceneEvent(event);
}

/**
 * @brief 检查点是否在有效的文本区域内
 * @param pos 本地坐标系中的点
 * @return 如果点在有效区域内返回true
 * @details 考虑到ItemIgnoresTransformations的影响，提供更精确的边界检测
 */
bool TextRenderElement::isPointInTextArea(const QPointF& pos) const
{
    // 获取文本的边界矩形
    QRectF textBounds = boundingRect();

    // 添加一些容差，使点击更容易
    qreal tolerance = 5.0; // 5像素的容差
    textBounds.adjust(-tolerance, -tolerance, tolerance, tolerance);

    // 检查点是否在扩展的边界内
    return textBounds.contains(pos);
}

/**
 * @brief 键盘按下事件处理
 * @param event 键盘事件
 * @details 处理键盘输入，防止输入时视角偏移
 */
void TextRenderElement::keyPressEvent(QKeyEvent* event)
{
    // 处理回车键确认输入
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        clearFocus(); // 主动释放焦点
        setTextInteractionFlags(Qt::NoTextInteraction);
        event->accept();
        return;
    }

    // 记录当前位置和视图状态
    QPointF currentPos = pos();
    QGraphicsScene* currentScene = scene();
    QGraphicsView* view = nullptr;
    QPointF viewCenter;

    if (currentScene && !currentScene->views().isEmpty()) {
        view = currentScene->views().first();
        if (view) {
            // 记录当前视图中心点
            viewCenter = view->mapToScene(view->viewport()->rect().center());
        }
    }

    // 调用父类处理键盘输入
    QGraphicsTextItem::keyPressEvent(event);

    // 检查位置是否发生了变化
    if (pos() != currentPos) {
        // 恢复原始位置
        setPos(currentPos);
    }

    // 检查视图是否发生了偏移
    if (view && currentScene) {
        QPointF newViewCenter = view->mapToScene(view->viewport()->rect().center());
        if ((newViewCenter - viewCenter).manhattanLength() > 1.0) {
            // 视图发生了偏移，使用更温和的方式恢复
            view->centerOn(viewCenter);
        }
    }
}

/**
 * @brief 输入法事件处理
 * @param event 输入法事件
 * @details 处理输入法事件，防止输入时视角偏移
 */
void TextRenderElement::inputMethodEvent(QInputMethodEvent* event)
{
    // 记录当前位置和视图状态
    QPointF currentPos = pos();
    QGraphicsScene* currentScene = scene();
    QGraphicsView* view = nullptr;
    QPointF viewCenter;

    if (currentScene && !currentScene->views().isEmpty()) {
        view = currentScene->views().first();
        if (view) {
            // 记录当前视图中心点
            viewCenter = view->mapToScene(view->viewport()->rect().center());
        }
    }

    // 调用父类处理输入法事件
    QGraphicsTextItem::inputMethodEvent(event);

    // 检查位置是否发生了变化
    if (pos() != currentPos) {
        // 恢复原始位置
        setPos(currentPos);
    }

    // 检查视图是否发生了偏移
    if (view && currentScene) {
        QPointF newViewCenter = view->mapToScene(view->viewport()->rect().center());
        if ((newViewCenter - viewCenter).manhattanLength() > 1.0) {
            // 视图发生了偏移，使用更温和的方式恢复
            view->centerOn(viewCenter);
        }
    }
}

/**
 * @brief 焦点失去事件处理
 * @param event 焦点事件
 * @details 处理焦点失去，退出编辑模式并恢复ItemIgnoresTransformations
 */
void TextRenderElement::focusOutEvent(QFocusEvent* event)
{
    // 先调用父类方法
    QGraphicsTextItem::focusOutEvent(event);

    // 如果文本为空，设置默认提示文本
    if (toPlainText().isEmpty()) {
        setPlainText(QStringLiteral("双击输入内容"));
    }

    // 退出编辑模式
    setTextInteractionFlags(Qt::NoTextInteraction);

    // 恢复ItemIgnoresTransformations标志
    setFlag(ItemIgnoresTransformations, true);

    // 清除文本宽度限制
    setTextWidth(-1);

    // 重置变换
    setTransform(QTransform());

    // 恢复原始字体大小
    QFont currentFont = font();
    currentFont.setPointSize(18); // 恢复到原始大小
    setFont(currentFont);

    qDebug() << "Exit edit mode - Font size restored to:" << currentFont.pointSize();
}

/**
 * @brief 计算编辑模式下的最佳字体大小
 * @return 适合当前缩放级别的字体大小
 * @details 确保编辑时字体大小看起来和非编辑时一致
 */
int TextRenderElement::calculateEditFontSize() const
{
    if (!scene() || scene()->views().isEmpty()) {
        return 12; // 默认大小
    }

    QGraphicsView* view = scene()->views().first();
    if (!view) {
        return 12;
    }

    // 获取视图的屏幕尺寸信息
    QSize viewportSize = view->viewport()->size();
    int screenHeight = viewportSize.height();

    // 基于屏幕高度计算字体大小
    // 目标：让字体在屏幕上占据合理的大小（约为屏幕高度的1/40到1/60）
    int targetScreenFontSize;

    if (screenHeight > 1000) {
        // 高分辨率屏幕
        targetScreenFontSize = screenHeight / 50;  // 约20像素 for 1080p
    } else if (screenHeight > 600) {
        // 中等分辨率屏幕
        targetScreenFontSize = screenHeight / 45;  // 约15像素 for 720p
    } else {
        // 低分辨率屏幕
        targetScreenFontSize = screenHeight / 40;  // 约12像素 for 480p
    }

    // 限制字体大小在合理范围内
    targetScreenFontSize = qBound(10, targetScreenFontSize, 24);

    // 现在我们需要将这个屏幕字体大小转换为场景字体大小
    // 由于我们禁用了ItemIgnoresTransformations，需要考虑缩放
    QTransform transform = view->transform();
    qreal scaleFactor = transform.m11();

    // 计算场景中需要的字体大小，使其在屏幕上显示为目标大小
    int sceneFontSize = static_cast<int>(targetScreenFontSize / scaleFactor);

    // 最终限制范围
    sceneFontSize = qBound(6, sceneFontSize, 32);

    qDebug() << "calculateEditFontSize - Screen height:" << screenHeight
             << "Target screen font:" << targetScreenFontSize
             << "Scale factor:" << scaleFactor
             << "Scene font size:" << sceneFontSize;

    return sceneFontSize;
}

/**
 * @brief 设置编辑模式的文本框大小
 * @details 直接控制文本框的显示大小，使其相对于屏幕大小合理
 */
void TextRenderElement::setupEditMode()
{
    if (!scene() || scene()->views().isEmpty()) {
        return;
    }

    QGraphicsView* view = scene()->views().first();
    if (!view) {
        return;
    }

    // 禁用ItemIgnoresTransformations以允许正常的文本编辑
    setFlag(ItemIgnoresTransformations, false);

    // 设置合适的字体大小
    QFont currentFont = font();
    int editFontSize = calculateEditFontSize();
    currentFont.setPointSize(editFontSize);
    setFont(currentFont);

    // 设置文本框的最大宽度，基于屏幕宽度
    QSize viewportSize = view->viewport()->size();
    int maxScreenWidth = qMin(viewportSize.width() / 4, 300); // 最大占屏幕宽度的1/4，但不超过300像素

    // 将屏幕宽度转换为场景坐标
    QTransform transform = view->transform();
    qreal scaleFactor = transform.m11();
    int maxSceneWidth = static_cast<int>(maxScreenWidth / scaleFactor);

    // 确保最小宽度
    maxSceneWidth = qMax(maxSceneWidth, 50);

    // 设置文本宽度限制
    setTextWidth(maxSceneWidth);

    // 设置文档边距，使文本看起来更整洁
    document()->setDocumentMargin(2);

    // 如果字体仍然太大，使用变换来进一步缩小
    if (scaleFactor > 5.0) {
        qreal additionalScale = 1.0 / (scaleFactor / 5.0);
        additionalScale = qBound(0.3, additionalScale, 1.0);
        setTransform(QTransform::fromScale(additionalScale, additionalScale));

        qDebug() << "Applied additional scale:" << additionalScale;
    } else {
        // 重置变换
        setTransform(QTransform());
    }

    qDebug() << "setupEditMode - Viewport size:" << viewportSize
             << "Max screen width:" << maxScreenWidth
             << "Scale factor:" << scaleFactor
             << "Max scene width:" << maxSceneWidth
             << "Font size:" << editFontSize;
}
