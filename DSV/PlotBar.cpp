/**
 * @file PlotBar.cpp
 * @brief 绘图工具栏实现文件
 * @details 实现数字病理切片查看器的绘图工具栏，包含线条、矩形、圆形、文本、路径等绘图工具
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "PlotBar.h"
#include <QDebug>

/**
 * @brief 绘图工具栏构造函数
 * @param parent 父窗口指针
 * @details 初始化绘图工具栏，设置界面布局和样式
 */
PlotBar::PlotBar(QWidget *parent)
	: QWidget(parent)
{
    this->setObjectName(QStringLiteral("PlotBarWidget"));  // 设置对象名称
    initUi();              // 初始化用户界面
    setUpStyle();          // 设置样式
    setAttribute(Qt::WA_StyledBackground);  // 启用样式背景
    m_stateVisible = true; // 设置可见状态
}

/**
 * @brief 初始化用户界面
 * @details 创建和配置绘图工具栏的所有UI组件，包括各种绘图工具按钮
 */
void PlotBar::initUi()
{
    // 创建水平布局
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10,10,10,10);
    mainLayout->setMargin(10);
    
    // 创建各种绘图工具按钮
    QToolButton* line = createToolButton(":/resources/line.png");        // 线条工具
    line->setObjectName(QStringLiteral("LineTool"));
    
    QToolButton* rectangle = createToolButton(":/resources/rectangle.png");  // 矩形工具
    rectangle->setObjectName(QStringLiteral("RectTool"));
    
    QToolButton* circle = createToolButton(":/resources/circle.png");     // 圆形工具
    circle->setObjectName(QStringLiteral("CircleTool"));
    
    QToolButton* text = createToolButton(":/resources/text.png");         // 文本工具
    text->setObjectName(QStringLiteral("TextTool"));
    
    QToolButton* path = createToolButton(":/resources/brush.png");        // 路径工具
    path->setObjectName(QStringLiteral("PathTool"));
    
    QToolButton* cancel = createToolButton(":/resources/cancel.png");     // 取消工具
    cancel->setObjectName(QStringLiteral("Cancel"));
    
    // 将按钮添加到布局中
    mainLayout->addWidget(line);
    mainLayout->addWidget(rectangle);
    mainLayout->addWidget(circle);
    mainLayout->addWidget(text);
    mainLayout->addWidget(path);
    mainLayout->addWidget(cancel);

    setLayout(mainLayout);
    
    // 设置固定尺寸
    int fixedWidth = 6 * 35 + 20 + 5 * 10;   // 6个按钮 * 35像素 + 边距
    int fixedHeight = 35 + 10 * 2;           // 按钮高度 + 上下边距
    setFixedHeight(fixedHeight);
    setFixedWidth(fixedWidth);
    setWindowOpacity(1.0);                   // 设置窗口透明度
}

/**
 * @brief 创建工具按钮
 * @param iconPath 图标路径
 * @return 创建的工具按钮指针
 * @details 根据指定的图标路径创建标准化的工具按钮
 */
QToolButton* PlotBar::createToolButton(const QString& iconPath)
{
    QToolButton* button = new QToolButton(this);
    button->setIcon(QIcon(iconPath));        // 设置按钮图标
    button->setFixedSize(35, 35);            // 设置固定大小
    return button;
}

/**
 * @brief 设置样式
 * @details 配置绘图工具栏的视觉样式，包括背景色、圆角、按钮样式等
 */
void PlotBar::setUpStyle()
{
    QString style =
        "#PlotBarWidget {"
        "   background-color:white /*#DCDCDC*/;"       // 白色背景
        "   border-radius: 10px;"             // 圆角半径
        "}"

        "QToolButton {"
        "   border: none;"                    // 无边框
        "   background: #e0e0e0;"        // 透明背景
        "   icon-size: 20px 20px; /* 精确设置图标大小 */"
        "   border-radius: 10px;"             // 圆角半径
        "}"

        "QToolButton:hover {"
        "   background-color: #c0c0c0;" // 悬停时的透明背景色
        "}"

        "QToolButton:pressed {"
        "   background-color: #a0a0a0;" // 按下时的深一点的白色
        "}";
    setStyleSheet(style);
}


