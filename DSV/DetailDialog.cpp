#include "DetailDialog.h"
#include <QDebug>
DetailDialog::DetailDialog(QWidget *parent, const std::vector<SlideColorManagement::PropertyInfo>& properties)
	: QDialog(parent),m_properties(properties)
{
	initUi();
}

DetailDialog::~DetailDialog()
{

}

void DetailDialog::initUi()
{
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowTitle(QStringLiteral("详情信息"));
    this->setFixedSize(200, 200);
    // 创建垂直布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 用于记录是否已经添加了某些信息的标志
    bool addedWidth = false;
    bool addedHeight = false;

    // 遍历属性信息
    for (const auto& prop : m_properties) {
        // 寻找对应的比较重要的参数
        if (prop.name == "aperio.AppMag") {
            QString data = QStringLiteral("增益：") + QString::number(prop.numericValue);
            QLabel* appMag = new QLabel(data, this);
            mainLayout->addWidget(appMag);
        }
        else if (prop.name == "aperio.MPP") {
            QString data = QStringLiteral("每像素微米数：") + QString::number(prop.numericValue);
            m_MPP = prop.numericValue;
            QLabel* appMag = new QLabel(data, this);
            mainLayout->addWidget(appMag);
        }
        else if ((prop.name == "aperio.OriginalWidth" || prop.name == "openslide.level[0].width") && !addedWidth) {
            QString data = QStringLiteral("图像宽度：") + QString::number(prop.numericValue);
            QLabel* appMag = new QLabel(data, this);
            mainLayout->addWidget(appMag);
            addedWidth = true; // 标记已添加宽度信息
        }
        else if ((prop.name == "aperio.Originalheight" || prop.name == "openslide.level[0].height") && !addedHeight) {
            QString data = QStringLiteral("图像高度：") + QString::number(prop.numericValue);
            QLabel* appMag = new QLabel(data, this);
            mainLayout->addWidget(appMag);
            addedHeight = true; // 标记已添加高度信息
        }
        else if (prop.name == "openslide.level-count") {
            QString data = QStringLiteral("图像层级：") + QString::number(prop.numericValue);
            QLabel* appMag = new QLabel(data, this);
            mainLayout->addWidget(appMag);
        }
    }

    // 设置主布局
    this->setLayout(mainLayout);
}

