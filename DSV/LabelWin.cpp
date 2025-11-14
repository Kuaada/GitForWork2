/**
 * @file LabelWin.cpp
 * @brief 标签窗口实现文件
 * @details 实现数字病理切片查看器的标签显示窗口，用于显示标签图像
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "LabelWin.h"

/**
 * @brief 标签窗口构造函数
 * @param parent 父窗口指针
 * @param img 要显示的标签图像
 * @details 初始化标签窗口，设置界面布局并显示标签图像
 */
LabelWin::LabelWin(QWidget *parent, const QImage& img)
	: QWidget(parent)
{
	initUi();      // 初始化用户界面
    setLabel(img); // 设置标签图像
}

/**
 * @brief 标签窗口析构函数
 * @details 清理资源，释放内存
 */
LabelWin::~LabelWin()
{
    // 析构函数实现
}

/**
 * @brief 设置标签图像
 * @param Label 要显示的标签图像
 * @details 更新标签窗口中显示的图像
 */
void LabelWin::setLabel(const QImage& Label)
{
	m_Image = Label;  // 保存图像
	// 将图像转换为像素图并缩放到标签控件大小，保持宽高比
	m_LabelPic->setPixmap(QPixmap::fromImage(m_Image).scaled(m_LabelPic->size(),Qt::KeepAspectRatio));
}

/**
 * @brief 初始化用户界面
 * @details 创建和配置标签窗口的所有UI组件
 */
void LabelWin::initUi()
{
    /*this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowTitle(QStringLiteral("标签图片"));*/
    QGridLayout* mainLayout = new QGridLayout(this);
    m_LabelPic = new QLabel(this);
    m_LabelPic->setScaledContents(false); // 不自动缩放内容
    m_LabelPic->setAlignment(Qt::AlignCenter); // 图像居中显示
    mainLayout->addWidget(m_LabelPic);
    this->setLayout(mainLayout);
    this->setMinimumSize(300, 300);
}

/**
 * @brief 窗口大小改变事件处理
 * @param event 大小改变事件对象
 * @details 当窗口大小改变时，重新调整图像显示
 */
void LabelWin::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);  // 未使用的事件参数
    if (!m_Image.isNull()) {
        m_LabelPic->setPixmap(QPixmap::fromImage(m_Image).scaled(m_LabelPic->size(), Qt::KeepAspectRatio));
    }
}