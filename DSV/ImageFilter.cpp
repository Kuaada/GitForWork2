#include "ImageFilter.h"

ImageFilter::ImageFilter(QWidget *parent)
	: QDialog(parent)
{
	initUi();
	initConnect();
}

ImageFilter::~ImageFilter()
{


}

void ImageFilter::initUi()
{
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setWindowTitle(QStringLiteral("图像滤波"));
	this->setMinimumSize(300, 400);
	this->setMaximumSize(300, 700);
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QCheckBox* Guass = new QCheckBox(QStringLiteral("高斯滤波"),this);
	Guass->setObjectName(QStringLiteral("GuassFilter"));
	Guass->setFixedHeight(30);
	QFont fontsize = Guass->font();
	fontsize.setPointSizeF(20);
	Guass->setFont(fontsize);

	
	QCheckBox* Sharp = new QCheckBox(QStringLiteral("锐化"), this);
	Sharp->setObjectName(QStringLiteral("SharpFilter"));
	Sharp->setFixedHeight(30);
	Sharp->setFont(fontsize);


	QHBoxLayout* H3 = new QHBoxLayout(this);

	m_SharpSlider = new QSlider(this);
	m_SharpSlider->setObjectName(QStringLiteral("SharpSlider"));
	m_SharpSlider->setOrientation(Qt::Horizontal);
	m_SharpSlider->setFixedHeight(30);
	
	//m_SharpSlider->setEnabled(false);
	m_SharpSlider->setRange(0, 200);
	m_LineEdit = new QLineEdit(this);
	m_LineEdit->setObjectName(QStringLiteral("ValueOfSharp"));
	//m_LineEdit->setEnabled(false);
	m_LineEdit->setFixedHeight(30);
	m_LineEdit->setStyleSheet(R"(
        QLineEdit {
            border: none;          /* 去除默认边框 */
            border-bottom: 1px solid #ccc; /* 可选择性地保留底部细边框 */
            padding: 2px;          /* 调整内边距，让文本看起来更居中 */
        }
        QLineEdit:focus {
            border-bottom: 1px solid #4a90e2; /* 聚焦时突出显示底部边框 */
        }
    )");

	H3->addWidget(m_SharpSlider,3);
	H3->addWidget(m_LineEdit,1);

	mainLayout->addWidget(Guass);
	mainLayout->addWidget(Sharp);
	mainLayout->addLayout(H3);
	// 添加垂直伸缩项，让H1和H2下方有空间
	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	mainLayout->addItem(verticalSpacer);

	this->setLayout(mainLayout);
}

void ImageFilter::initConnect()
{
	connect(m_SharpSlider, &QSlider::valueChanged, [this](int num) {
		float valueToSingle = (float)num / 100;
		m_LineEdit->setText(QString::number(valueToSingle));
		});
	connect(m_LineEdit, &QLineEdit::editingFinished, [this]() {
		QString num = m_LineEdit->text();
		bool ok;
		float floatValue = num.toFloat(&ok);
		if (ok) {
			int sliderNum = static_cast<int>(floatValue * 100);
			m_SharpSlider->setValue(sliderNum);
		}
		});
}

