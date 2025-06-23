#include "CenteredToolBar.h"
#include <QDebug>
CenteredToolBar::CenteredToolBar(QWidget* parent = nullptr)
    : QToolBar(parent), m_iToolBarHeight(50)
{
    setMovable(false);
    setFloatable(false);
    setupStyle();
    setupAction();
    adjustSize();
    setFixedHeight(m_iToolBarHeight);
    setEnabled(false);
    connect(ScaleDisplayLineEdit, &QLineEdit::editingFinished, [this]()
        {
            emit TransChangedMpp(ScaleDisplayLineEdit->text().toFloat());
        });
}

CenteredToolBar::~CenteredToolBar()
{


}

void CenteredToolBar::setFactor(float factor)
{
    zoomDisplayLineEdit->setText(QString::number(factor, 'f', 2));
}

void CenteredToolBar::setMpp(float mpp)
{
    ScaleDisplayLineEdit->setText(QString::number(mpp));
}

void CenteredToolBar::setupStyle()
{
    // 设置样式表
    QString style = QString(
        "QToolBar {"
        "    background-color: #DCDCDC; /* 设置背景颜色 */"
        "    border-top-left-radius: 5px; /* 设置左上角圆角 */"
        "    border-top-right-radius: 5px; /* 设置右上角圆角 */"
        "    border-bottom-left-radius: 5px; /* 左下角无圆角 */"
        "    border-bottom-right-radius: 5px; /* 右下角无圆角 */"
        "    padding-left: 10px; /* 设置工具栏左内边距 */"
        "}"
        "QToolBar::separator {"
        "    background: white; /* 设置分隔符颜色 */"
        "    width: 2px; /* 设置分隔符宽度 */"
        "    margin-right: 5px; /* 设置分隔符边距 */"
        "    margin-left:  1px;"
        "}"
        "QToolButton {"
        "    margin-right: 5px; /* 设置 QToolButton 右侧外边距，增加按钮间间距 */"
        "    icon-size: 30px 30px; /* 明确设置图标大小 */"
        "    min-width: 30px; /* 设置按钮最小宽度 */"
        "    min-height: 30px; /* 设置按钮最小高度 */"
        "    max-width: 30px; /* 设置按钮最小宽度 */"
        "    max-height: 30px; /* 设置按钮最小高度 */"
        "    font-size: 20px; /* 设置字体大小 */"
        "    text-align: center; /* 文本居中 */"
        "    background-color: #e0e0e0; /* 正常状态背景颜色 */"
        "    border: none; "
        "    border-radius: 5px; "
        "}"

        "QToolButton:hover {"
        "    background-color: #c0c0c0; /* 悬停状态背景颜色 */"
        "}"
        "QToolButton:pressed {"
        "    background-color: #a0a0a0; /* 按下状态背景颜色 */"
        "}"

        "QLineEdit {"
        "    font-size: 20px; "
        "    padding: 0; "
        "    margin-right: 5px; "
        "    border:none;"
        "    background-color: transparent; "
        "}"
        "QLabel {"
        "    font-size: 20px; /* 设置 QLabel 的字体大小 */"
        "    padding: 0; "
        "    margin: 0; "
        "}"
        "QWidget#ScaleDisplayWidget {"
        "    background-color:  #e0e0e0; /* 设置背景颜色 */"
        "    border-radius: 5px; /* 设置圆角 */"
        "    margin: 2px; /* 设置外边距 */"
        "}"
        "QWidget#ZoomDisplayWidget {"
        "    background-color:  #e0e0e0; /* 设置背景颜色 */"
        "    border-radius: 5px; /* 设置圆角 */"
        "    margin: 2px; /* 设置外边距 */"
        "}"


    );
    setStyleSheet(style);

}

void CenteredToolBar::setupAction()
{
    QIcon icon1(":/resources/detail.png");
    QIcon icon2(":/resources/rotate.png");
    QIcon icon3(":/resources/color.png");
    QIcon icon4(":/resources/deal.png");
    QIcon icon5(":/resources/label.png");
    QIcon icon6(":/resources/mini.png");
    QIcon icon7(":/resources/scale.png");
    QIcon icon8(":/resources/tag.png");
    QIcon icon9(":/resources/snap.png");

    QAction* detail = new QAction(icon1, "detailAction", this);
    detail->setObjectName(QStringLiteral("detailAction"));
    detail->setToolTip(QStringLiteral("detailAction"));
    addAction(detail);
    QAction* rotate = new QAction(icon2, "rotateAction", this);
    rotate->setObjectName(QStringLiteral("rotateAction"));
    rotate->setToolTip(QStringLiteral("rotateAction"));
    rotate->setEnabled(false);
    addAction(rotate);
    QAction* color = new QAction(icon3, "colorAction", this);
    color->setObjectName(QStringLiteral("colorAction"));
    color->setToolTip(QStringLiteral("colorAction"));
    color->setEnabled(false);
    addAction(color);
    QAction* deal = new QAction(icon4, "dealAction", this);
    deal->setObjectName(QStringLiteral("dealAction"));
    deal->setToolTip(QStringLiteral("dealAction"));
    deal->setEnabled(false);
    addAction(deal);
    QAction* label = new QAction(icon5, "labelAction", this);
    label->setObjectName(QStringLiteral("labelAction"));
    label->setToolTip(QStringLiteral("labelAction"));

    addAction(label);
    QAction* mini = new QAction(icon6, "miniAction", this);
    mini->setObjectName(QStringLiteral("miniAction"));
    mini->setCheckable(true);
    mini->setToolTip(QStringLiteral("miniAction"));
    addAction(mini);
    QAction* scale = new QAction(icon7, "scaleAction", this);
    scale->setObjectName(QStringLiteral("scaleAction"));
    scale->setToolTip(QStringLiteral("scaleAction"));
    scale->setEnabled(false);
    addAction(scale);
    QAction* tag = new QAction(icon8, "tagAction", this);
    tag->setObjectName(QStringLiteral("tagAction"));
    tag->setToolTip(QStringLiteral("tagAction"));
    addAction(tag);
    QAction* snap = new QAction(icon9, "snapAction", this);
    snap->setObjectName(QStringLiteral("snapAction"));
    snap->setToolTip(QStringLiteral("snapAction"));
    addAction(snap);
    //添加分割符
    addSeparator();

    QIcon icon10(":/resources/reset.png");
    QAction* reset = new QAction(icon10, "resetAction", this);
    reset->setObjectName(QStringLiteral("resetAction"));
    reset->setToolTip(QStringLiteral("resetAction"));
    addAction(reset);

    QAction* X2 = new QAction("X2", this);
    X2->setObjectName(QStringLiteral("X2Action"));
    X2->setToolTip(QStringLiteral("X2Action"));
    addAction(X2);

    QAction* X5 = new QAction("X5", this);
    X5->setObjectName(QStringLiteral("X5Action"));
    X5->setToolTip(QStringLiteral("X5Action"));
    addAction(X5);

    QAction* X10 = new QAction("X10", this);
    X10->setObjectName(QStringLiteral("X10Action"));
    X10->setToolTip(QStringLiteral("X10Action"));
    addAction(X10);

    QAction* X20 = new QAction("X20", this);
    X20->setObjectName(QStringLiteral("X20Action"));
    X20->setToolTip(QStringLiteral("X20Action"));
    addAction(X20);

    QAction* X40 = new QAction("X40", this);
    X40->setObjectName(QStringLiteral("X40Action"));
    X40->setToolTip(QStringLiteral("X40Action"));
    addAction(X40);

    QAction* R1 = new QAction("1:1", this);
    R1->setObjectName(QStringLiteral("R1Action"));
    R1->setToolTip(QStringLiteral("1:1"));
    addAction(R1);

    addSeparator();

    QIcon icon11(":/resources/zoom.png");
    QWidgetAction* valueAction1 = createZoomDisplayAction(icon11, "X");
    valueAction1->setObjectName(QStringLiteral("ValueAction1"));
    addAction(valueAction1);

    addSeparator();

    QWidgetAction* valueAction2 = createScaleDisplayAction("um");
    valueAction2->setObjectName(QStringLiteral("ValueAction2"));
    addAction(valueAction2);

    addSeparator();

    QAction* Addition = new QAction(">>>", this);
    Addition->setObjectName(QStringLiteral("Addition"));
    Addition->setToolTip(QStringLiteral("Addition"));
    addAction(Addition);


}



QWidgetAction* CenteredToolBar::createZoomDisplayAction(const QIcon& icon, const QString& prefix)
{
    QWidgetAction* action = new QWidgetAction(this);
    QWidget* widget = new QWidget(this);
    widget->setObjectName(QStringLiteral("ZoomDisplayWidget"));
    QHBoxLayout* layout = new QHBoxLayout(widget);

    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(0);

    QLabel* iconLabel = new QLabel(widget);
    iconLabel->setPixmap(icon.pixmap(25, 25));
    iconLabel->setFixedSize(25, 25);
    layout->addWidget(iconLabel, 0);

    zoomDisplayLineEdit = new QLineEdit(widget);
    zoomDisplayLineEdit->setObjectName(QStringLiteral("ZoomDisplay"));
    zoomDisplayLineEdit->setFixedWidth(80);
    zoomDisplayLineEdit->setText(" "); // 默认值
    zoomDisplayLineEdit->setReadOnly(true);
    zoomDisplayLineEdit->setAlignment(Qt::AlignRight);
    layout->addWidget(zoomDisplayLineEdit, 0);

    QLabel* prefixLabel = new QLabel(prefix, widget);
    layout->addWidget(prefixLabel, 0);

    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    zoomDisplayLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    prefixLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    action->setDefaultWidget(widget);
    return action;
}

QWidgetAction* CenteredToolBar::createScaleDisplayAction(const QString& prefix)
{
    QWidgetAction* action = new QWidgetAction(this);
    QWidget* widget = new QWidget(this);
    widget->setObjectName(QStringLiteral("ScaleDisplayWidget"));
    QHBoxLayout* layout = new QHBoxLayout(widget);

    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(0);

    ScaleDisplayLineEdit = new QLineEdit(widget);
    ScaleDisplayLineEdit->setObjectName(QStringLiteral("SacleDisplay"));
    ScaleDisplayLineEdit->setFixedWidth(80);
    ScaleDisplayLineEdit->setText(" "); // 默认值
    ScaleDisplayLineEdit->setAlignment(Qt::AlignRight);
    layout->addWidget(ScaleDisplayLineEdit, 0);

    QLabel* prefixLabel = new QLabel(prefix, widget);
    layout->addWidget(prefixLabel, 0);

    ScaleDisplayLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    prefixLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    action->setDefaultWidget(widget);
    return action;
}

