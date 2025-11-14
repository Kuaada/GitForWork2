#include "SnapDialog.h"


SnapDialog::SnapDialog(QWidget *parent)
	: QDialog(parent)
{
	initUi();
	setupStyle();
	fadeInAnimation = new QPropertyAnimation(this, "windowOpacity");
	fadeInAnimation->setDuration(500); // 动画持续时间，单位毫秒
	fadeInAnimation->setStartValue(0.0);
	fadeInAnimation->setEndValue(1.0);

	// 创建淡出动画
	fadeOutAnimation = new QPropertyAnimation(this, "windowOpacity");
	fadeOutAnimation->setDuration(500);
	fadeOutAnimation->setStartValue(1.0);
	fadeOutAnimation->setEndValue(0.0);

	connect(fadeOutAnimation, &QPropertyAnimation::finished, this, &SnapDialog::accept);

	// 设置对话框初始透明度为0
	setWindowOpacity(0.0);
	QPushButton* save = this->findChild<QPushButton*>("SavePic");
	connect(save, &QPushButton::clicked, m_SnapWidget, &SnapWidget::saveSnap);
	QPushButton* saveTiff = this->findChild<QPushButton*>("SaveTIFF");
	connect(saveTiff, &QPushButton::clicked, m_SnapWidget, &SnapWidget::saveTIFFSnap);
}

void SnapDialog::drawPic(const QPixmap& pix)
{
	m_SnapWidget->setPixmap(pix);
}

void SnapDialog::showEvent(QShowEvent* event)
{
	QDialog::showEvent(event);
	fadeInAnimation->start();
}

SnapDialog::~SnapDialog()
{



}

void SnapDialog::fadeOutAndHide()
{
	fadeOutAnimation->start();
}

void SnapDialog::initUi()
{
	//关闭问号提示
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setWindowTitle(QStringLiteral("快照"));
	this->setFixedSize(800, 400);

	//布局：
	//___________________
	//|        |         |
	//|        |         |
	//|  left0 |         |
	//|————|  Widget |
	//|        |         |
	//|  left1 |         |
	//|        |         |
	//——————————
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins(10, 10, 10, 10);
	QVBoxLayout* mainLeft = new QVBoxLayout();
	mainLeft->setContentsMargins(0, 10, 0, 30);
	
	//left0布局
	QVBoxLayout* left0 = new QVBoxLayout();
	QLabel* labelInfo0 = new QLabel(QStringLiteral("快照信息"), this);
	labelInfo0->setFixedHeight(60);
	QCheckBox* label = new QCheckBox(QStringLiteral("显示切片标签"), this);
	label->setObjectName(QStringLiteral("labelInfo"));
	label->setFixedHeight(50);
	QCheckBox* scalebar = new QCheckBox(QStringLiteral("显示比例尺"),this);
	scalebar->setObjectName(QStringLiteral("scaleBar"));
	scalebar->setChecked(true);
	scalebar->setFixedHeight(50);
	QCheckBox* minimap = new QCheckBox(QStringLiteral("显示迷你图"), this);
	minimap->setObjectName(QStringLiteral("miniMap"));
	minimap->setChecked(true);
	minimap->setFixedHeight(50);

	QPushButton* changePos = new QPushButton(QStringLiteral("位置"),this);
	changePos->setObjectName(QStringLiteral("changePos"));
	changePos->setFixedHeight(30);
	changePos->setFixedWidth(75);

	left0->addWidget(labelInfo0,1);
	left0->addWidget(scalebar,1);
	left0->addWidget(label,1);
	left0->addWidget(minimap,1);
	left0->addSpacing(2);
	left0->addWidget(changePos, 0.8);
	//left1布局
	QVBoxLayout* left1 = new QVBoxLayout();
	QLabel* labelInfo1 = new QLabel(QStringLiteral("保存"), this);
	labelInfo1->setFixedHeight(60);
	QHBoxLayout* H0 = new QHBoxLayout(this);
	QPushButton* Save = new QPushButton(this);
	Save->setObjectName(QStringLiteral("SavePic"));
	QLabel* label0 = new QLabel(QStringLiteral("保存视图"),this);
	H0->addWidget(Save);
	H0->addWidget(label0);

	QHBoxLayout* H1 = new QHBoxLayout(this);
	QPushButton* SaveTIFF = new QPushButton(this);
	SaveTIFF->setObjectName(QStringLiteral("SaveTIFF"));
	QLabel* label1 = new QLabel(QStringLiteral("保存视图为TIFF"), this);
	SaveTIFF->setEnabled(true);
	label1->setEnabled(true);
	H1->addWidget(SaveTIFF);
	H1->addWidget(label1);

	left1->addWidget(labelInfo1,1);
	left1->addLayout(H0,1);
	left1->addLayout(H1,1);

	//left布局
	mainLeft->addLayout(left0,1);
	mainLeft->addLayout(left1,1);

	m_SnapWidget = new SnapWidget(this);
	m_SnapWidget->setObjectName(QStringLiteral("SnapWidget"));
	mainLayout->addLayout(mainLeft, 1);
	mainLayout->addWidget(m_SnapWidget, 4);
	

	
	//设置button图标及大小
	Save->setFixedSize(40, 40);
	Save->setIcon(QIcon(":/resources/save.png"));
	SaveTIFF->setFixedSize(40, 40);
	SaveTIFF->setIcon(QIcon(":/resources/saveTIFF.png"));

}

void SnapDialog::setupStyle()
{

	QString styleSheet =
		"QDialog {"
		"border-radius: 10px; /* 设置对话框的圆角半径 */"
		"}"
		"QLabel {"
		"font-size: 14pt; /* 设置 QLabel 的字体大小 */"
		"padding: 0; "
		"margin: 0; "
		"}"
		"QCheckBox {"
		"font-size: 14pt; /* 设置 QCheckBox 的字体大小 */"
		"padding: 0; "
		"margin: 0; "
		"}"
		"QPushButton {"
		"font-size: 14pt; /* 设置 QPushButton 的字体大小 */"
		"padding: 0; "
		"margin: 0; "
		"}";
	this->setStyleSheet(styleSheet);


}
