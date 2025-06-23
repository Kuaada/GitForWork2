/**
 * @file ItemDialog.cpp
 * @brief 项目对话框实现文件
 * @details 实现数字病理切片查看器的项目编辑对话框，用于编辑标注项目的属性
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "ItemDialog.h"
#include <QLabel>
#include <QLayout>
#include <QScrollBar>

/**
 * @brief 项目对话框构造函数
 * @param parent 父窗口指针
 * @details 初始化项目对话框，设置界面布局
 */
ItemDialog::ItemDialog(QWidget *parent)
	: QDialog(parent)
{
	initUi();  // 初始化用户界面
}

/**
 * @brief 项目对话框析构函数
 * @details 清理资源，释放内存
 */
ItemDialog::~ItemDialog()
{

}

/**
 * @brief 添加列表项
 * @param listItem 要添加的列表项
 * @details 向列表控件中添加新的项目
 */
void ItemDialog::addItem(QListWidgetItem* listItem)
{
	m_listWidget->addItem(listItem);
}

/**
 * @brief 设置对话框可见性
 * @param state 可见状态（true为隐藏，false为显示）
 * @details 控制对话框的显示和隐藏
 */
void ItemDialog::setDialogVisible(bool state)
{
	this->setHidden(state);
}

/**
 * @brief 初始化用户界面
 * @details 创建和配置对话框的所有UI组件，包括列表控件和编辑控件
 */
void ItemDialog::initUi()
{
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setWindowTitle(QStringLiteral("图像项目列表"));

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	m_listWidget = new QListWidget(this);
	m_listWidget->setVerticalScrollBar(new QScrollBar());
	
	QGridLayout* Left = new QGridLayout(this);

	// 第一行：内容标签和内容编辑框
	QLabel* contentLabel = new QLabel(QStringLiteral("内容"), this);
	QLineEdit* contentEdit = new QLineEdit(this);
	Left->addWidget(contentLabel, 0, 0);  // 放在第0行，第0列
	Left->addWidget(contentEdit, 0, 1);   // 放在第0行，第1列

	// 第二行：字体标签和字体编辑框
	QLabel* fontLabel = new QLabel(QStringLiteral("字体"), this);
	QLineEdit* fontEdit = new QLineEdit(this);
	Left->addWidget(fontLabel, 1, 0);  // 放在第1行，第0列
	Left->addWidget(fontEdit, 1, 1);   // 放在第1行，第1列

	// 第三行：线宽标签和线宽编辑框
	QLabel* lineWidthLabel = new QLabel(QStringLiteral("线宽"), this);
	QLineEdit* lineWidthEdit = new QLineEdit(this);
	Left->addWidget(lineWidthLabel, 2, 0);  // 放在第2行，第0列
	Left->addWidget(lineWidthEdit, 2, 1);   // 放在第2行，第1列

	// 第四行：删除按钮
	QToolButton* deleteButton = new QToolButton(this);
	deleteButton->setText(QStringLiteral("删除"));
	Left->addWidget(deleteButton, 3, 0, 1, 2);  // 放在第3行，跨2列

	// 第五行：保存按钮
	QToolButton* saveButton = new QToolButton(this);
	saveButton->setText(QStringLiteral("保存"));
	Left->addWidget(saveButton, 4, 0, 1, 2);  // 放在第4行，跨2列
	

	mainLayout->addWidget(m_listWidget);
	mainLayout->addLayout(Left);


}
