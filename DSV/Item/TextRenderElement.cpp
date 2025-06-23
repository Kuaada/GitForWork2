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
    setTextInteractionFlags(Qt::TextEditable|Qt::TextSelectableByKeyboard);
    setInputMethodHints(Qt::ImhMultiLine); // 允许多行输入
    QFont font("宋体",16);
    setFont(font);
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
    setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    setInputMethodHints(Qt::ImhMultiLine); // 允许多行输入
    QFont font("宋体", 16);
    setFont(font);
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
    setCursor(QCursor(Qt::ArrowCursor));
    setDefaultTextColor(Qt::red);
}

/**
 * @brief 鼠标悬停离开事件
 * @param event 悬停事件
 * @details 鼠标离开时恢复光标样式和文本颜色
 */
void TextRenderElement::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(QCursor(Qt::PointingHandCursor));
    setDefaultTextColor(m_pen.color());
    /*setTextInteractionFlags(Qt::NoTextInteraction);*/
    QGraphicsTextItem::hoverLeaveEvent(event); // 确保调用父类方法
}
