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
   
    //setTwoLabel(120.,1209);
}

/**
 * @brief 初始化用户界面
 * @details 创建和配置绘图工具栏的所有UI组件，包括各种绘图工具按钮
 */
void PlotBar::initUi()
{
    QVBoxLayout* mainLayout1 = new QVBoxLayout(this);
    mainLayout1->setMargin(10);

    // 创建水平布局
    QHBoxLayout* mainLayout0 = new QHBoxLayout();
    mainLayout0->setContentsMargins(10,10,10,10);
    mainLayout0->setMargin(0);
    
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

    QToolButton* deletebutton = createToolButton(":/resources/delete.png");        // 路径工具
    deletebutton->setObjectName(QStringLiteral("DeleteTool"));
    
    QToolButton* cancel = createToolButton(":/resources/cancel.png");     // 取消工具
    cancel->setObjectName(QStringLiteral("Cancel"));
    

    QLabel* PerimeterLabel = new QLabel();
    PerimeterLabel->setObjectName(QStringLiteral("PerimeterLabel"));
    PerimeterLabel->setFixedHeight(40);

    QLabel* AreaLabel = new QLabel();
    AreaLabel->setObjectName(QStringLiteral("AreaLabel"));
    PerimeterLabel->setFixedHeight(40);


    // 将按钮添加到布局中
    mainLayout0->addWidget(line);
    mainLayout0->addWidget(rectangle);
    mainLayout0->addWidget(circle);
    mainLayout0->addWidget(text);
    mainLayout0->addWidget(path);
    mainLayout0->addWidget(deletebutton);
    mainLayout0->addWidget(cancel);


    mainLayout1->addLayout(mainLayout0);
    mainLayout1->addWidget(PerimeterLabel);
    mainLayout1->addWidget(AreaLabel);

    setLayout(mainLayout1);
    
    // 设置固定尺寸
    int fixedWidth = 7 * 35 + 20 + 5 * 10;   // 6个按钮 * 35像素 + 边距
    int fixedHeight = 40*2 + 35 + 10 * 2;           // 按钮高度 + 上下边距
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


void PlotBar::setOneLabel(float Perimeter)
{
    QLabel* PerimeterLabel = this->findChild<QLabel*>(QStringLiteral("PerimeterLabel"));
    if (!PerimeterLabel) return; // 建议保留空指针检查

    // 使用 QStringLiteral 包装每个字符串部分
    QString showText = QStringLiteral("长度：") +
        QString::number(Perimeter) +
        QStringLiteral("um");

    PerimeterLabel->setText(showText);
}

void PlotBar::setTwoLabel(float Perimeter, float Area)
{
    QLabel* PerimeterLabel = this->findChild<QLabel*>(QStringLiteral("PerimeterLabel"));
    if (!PerimeterLabel) return;

    QString showText0 = QStringLiteral("周长：") +
        QString::number(Perimeter) +
        QStringLiteral(" um");

    PerimeterLabel->setText(showText0);

    QLabel* AreaLabel = this->findChild<QLabel*>(QStringLiteral("AreaLabel"));
    if (!AreaLabel) return;

    QString showText1 = QStringLiteral("面积：") +
        QString::number(Area) +
        QStringLiteral(" um^2");

    AreaLabel->setText(showText1);
}