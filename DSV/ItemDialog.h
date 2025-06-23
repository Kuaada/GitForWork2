/**
 * @file    ItemDialog.h
 * @brief   标注项对话框类，管理和编辑标注项
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的标注项管理功能，包括：
 *          - 标注项的添加、删除和保存
 *          - 标注内容、字体和线宽的编辑
 *          - 标注项列表的显示和管理
 *          - 对话框的显示与隐藏
 *          该类是DSV项目的标注项管理组件，为用户提供
 *          便捷的标注项编辑和管理界面。
 *
 * @note    该类继承自QDialog，是DSV项目的标注项管理组件
 * @see     MainWin, PathologyViewer
 */

#pragma once

#include <QDialog>
#include <QListWidget>
#include <QToolButton>
#include <QLineEdit>

 /**
  * @class  ItemDialog
  * @brief  标注项对话框类，管理和编辑标注项
  * @details 该类是DSV项目的标注项管理对话框，负责：
  *          - 添加、删除和保存标注项
  *          - 编辑标注内容、字体大小和线宽
  *          - 管理标注项列表的显示
  *          - 控制对话框的显示与隐藏
  *
  *          主要功能包括：
  *          - 标注项管理：添加、删除、保存标注项
  *          - 内容编辑：编辑标注内容、字体大小、线宽等属性
  *          - 列表显示：显示所有标注项
  *          - 界面布局：管理对话框的布局和样式
  *
  * @note   该类继承自QDialog，提供模态的标注项管理界面
  * @example
  *          // 使用示例
  *          ItemDialog* itemDialog = new ItemDialog(parent);
  *
  *          // 添加标注项
  *          itemDialog->addItem(new QListWidgetItem("标注1"));
  *
  *          // 设置对话框可见性
  *          itemDialog->setDialogVisible(true);
  * @see     MainWin, PathologyViewer
  */
class ItemDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建标注项对话框对象并初始化界面
     *
     * @param   parent 父窗口指针
     * @note    构造函数会调用initUi()来初始化界面
     * @see     ~ItemDialog, initUi
     */
    ItemDialog(QWidget* parent);

    /**
     * @brief   析构函数
     * @details 清理标注项对话框资源
     * @note    析构函数会清理所有UI组件和资源
     * @see     ItemDialog
     */
    ~ItemDialog();

    /**
     * @brief   添加标注项
     * @details 向标注项列表中添加新的标注项
     *
     * @param   listItem 要添加的标注项对象
     * @note    该函数会将新项添加到列表并刷新显示
     */
    void addItem(QListWidgetItem* listItem);

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化标注项对话框的各个组件
     * @note    该函数会创建列表、按钮、文本框等UI组件
     */
    void initUi();

    /** @brief 标注项列表控件指针 */
    QListWidget* m_listWidget;

    /** @brief 删除按钮指针 */
    QToolButton* m_DeleteButton;

    /** @brief 保存按钮指针 */
    QToolButton* m_SaveButton;

    /** @brief 内容输入框指针 */
    QLineEdit* m_Content;

    /** @brief 字体大小输入框指针 */
    QLineEdit* m_FontSize;

    /** @brief 线宽输入框指针 */
    QLineEdit* m_LineWidth;

public slots:
    /**
     * @brief   设置对话框可见性
     * @details 控制标注项对话框的显示与隐藏
     *
     * @param   state true显示，false隐藏
     * @note    该槽函数用于外部控制对话框的可见状态
     */
    void setDialogVisible(bool state);

};