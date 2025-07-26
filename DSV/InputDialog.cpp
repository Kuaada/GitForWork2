#include "InputDialog.h"

InputDialog::InputDialog(QWidget *parent)
	: QDialog(parent)
{
	initUi();
	setUpStyle();
	connect(m_confirm, &QToolButton::clicked, this, &InputDialog::onConfirmClicked);
	connect(m_cancel, &QToolButton::clicked, [this]() {hide();});
}

InputDialog::~InputDialog(){}

QString InputDialog::getInputText()
{
	return m_strEdit->text();
}


void InputDialog::initUi()
{
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setWindowTitle(QStringLiteral("文本输入"));

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	m_strEdit = new QLineEdit(this);
	m_strEdit->setObjectName(QStringLiteral("STREdit"));
	mainLayout->addWidget(m_strEdit);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);

	m_confirm = new QToolButton(this);
	m_confirm->setText(QStringLiteral("确认"));
	m_confirm->setObjectName(QStringLiteral("Confirm"));
	m_cancel = new QToolButton(this);
	m_cancel->setObjectName(QStringLiteral("Cancel"));
	m_cancel->setText(QStringLiteral("取消"));
	buttonLayout->addWidget(m_confirm);
	buttonLayout->addWidget(m_cancel);

	mainLayout->addLayout(buttonLayout);

	this->setLayout(mainLayout);

}

void InputDialog::setUpStyle()
{




}
void InputDialog::onConfirmClicked()
{
	QString inputText = m_strEdit->text();
	if (inputText.isEmpty()) {
		QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("输入内容不能为空，请重新输入！"));
		return;
	}
	accept(); // 关闭对话框并返回 QDialog::Accepted
}