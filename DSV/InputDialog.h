/**
 * @file    InputDialog.h
 * @brief   输入对话框类，提供文本输入功能
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的文本输入功能，包括：
 *          - 文本输入和编辑
 *          - 输入验证和格式化
 *          - 对话框布局管理
 *          - 用户交互处理
 *          - 输入结果返回
 *          - 确认和取消操作
 *          该类是DSV项目的文本输入组件，为用户提供
 *          便捷的文本输入和编辑界面。
 *
 * @note    该类继承自QDialog，是DSV项目的文本输入组件
 * @see     MainWin, PathologyViewer
 */

#pragma once

#include <QDialog>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QToolButton>

 /**
  * @class  InputDialog
  * @brief  输入对话框类，提供文本输入功能
  * @details 该类是DSV项目的文本输入对话框类，负责：
  *          - 提供文本输入界面
  *          - 处理用户文本输入
  *          - 管理确认和取消操作
  *          - 验证输入内容
  *          - 返回输入结果
  *          - 与主窗口的通信
  *
  *          主要功能包括：
  *          - 文本输入：通过文本框接收用户输入
  *          - 输入验证：验证输入内容的有效性
  *          - 确认操作：处理用户确认输入的操作
  *          - 取消操作：处理用户取消输入的操作
  *          - 结果返回：将输入结果返回给调用者
  *          - 界面布局：管理对话框的布局和样式
  *
  * @note   该类继承自QDialog，提供模态的文本输入界面
  * @example
  *          // 使用示例
  *          InputDialog* inputDialog = new InputDialog(parent);
  *
  *          // 显示输入对话框
  *          inputDialog->show();
  *
  *          // 获取用户输入的文本
  *          QString inputText = inputDialog->getInputText();
  *
  *          // 用户可以通过确认或取消按钮完成操作
  * @see     MainWin, PathologyViewer
  */
class InputDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建输入对话框对象并初始化界面
     *
     * @param   parent 父窗口指针，默认为nullptr
     * @note    构造函数会调用initUi()和setUpStyle()来初始化界面
     * @see     ~InputDialog, initUi, setUpStyle
     */
    explicit InputDialog(QWidget* parent = nullptr);

    /**
     * @brief   析构函数
     * @details 清理输入对话框资源
     * @note    析构函数会清理所有UI组件和资源
     * @see     InputDialog
     */
    ~InputDialog();

    /**
     * @brief   获取输入文本
     * @details 返回用户输入的文本内容
     *
     * @return  用户输入的文本字符串
     * @note    该函数会返回当前文本框中的内容
     * @see     onConfirmClicked
     */
    QString getInputText();

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化输入对话框的各个组件
     * @note    该函数会创建文本框、确认按钮、取消按钮等组件
     * @see     setUpStyle
     */
    void initUi();

    /**
     * @brief   设置样式
     * @details 设置输入对话框的视觉效果和样式
     * @note    该函数会设置对话框的颜色、字体、布局等样式
     * @see     initUi
     */
    void setUpStyle();

    /**
     * @brief   确认按钮点击处理
     * @details 处理用户点击确认按钮的操作
     * @note    该函数会验证输入内容并关闭对话框
     * @see     getInputText
     */
    void onConfirmClicked();

    /** @brief 文本输入框指针，用于接收用户输入 */
    QLineEdit* m_strEdit;

    /** @brief 确认按钮指针，用于确认输入操作 */
    QToolButton* m_confirm;

    /** @brief 取消按钮指针，用于取消输入操作 */
    QToolButton* m_cancel;
};