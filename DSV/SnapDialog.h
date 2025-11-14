/**
 * @file    SnapDialog.h
 * @brief   快照对话框类，提供图像快照功能
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的快照功能，包括：
 *          - 当前视图的快照生成
 *          - 快照图像的预览和编辑
 *          - 快照图像的保存和导出
 *          - 快照参数设置（分辨率、格式等）
 *          - 快照历史记录管理
 *          - 淡入淡出动画效果
 *          该类是DSV项目的快照功能组件，为用户提供
 *          便捷的图像快照和保存功能。
 *
 * @note    该类继承自QDialog，是DSV项目的快照功能组件
 * @see     MainWin, PathologyViewer, SnapWidget
 */

#pragma once

#include <QDialog>
#include <QPropertyAnimation>

#include <QLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

#include <QTimer>
#include "SnapWidget.h"

 /**
  * @class  SnapDialog
  * @brief  快照对话框类，提供图像快照功能
  * @details 该类是DSV项目的快照对话框类，负责：
  *          - 显示快照预览界面
  *          - 处理快照图像的生成和显示
  *          - 管理快照对话框的动画效果
  *          - 提供快照保存和导出选项
  *          - 处理用户交互和操作
  *
  *          主要功能包括：
  *          - 快照预览：显示当前视图的快照图像
  *          - 动画效果：提供淡入淡出的显示动画
  *          - 图像处理：支持快照图像的编辑和调整
  *          - 保存功能：提供多种格式的保存选项
  *          - 参数设置：允许用户设置快照参数
  *          - 界面管理：管理对话框的布局和样式
  *
  * @note   该类继承自QDialog，提供模态对话框界面
  * @example
  *          // 使用示例
  *          SnapDialog* snapDialog = new SnapDialog(parent);
  *
  *          // 显示快照图像
  *          snapDialog->drawPic(currentPixmap);
  *
  *          // 显示对话框
  *          snapDialog->show();
  *
  *          // 淡出并隐藏
  *          snapDialog->fadeOutAndHide();
  * @see     MainWin, PathologyViewer, SnapWidget
  */
class SnapDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建快照对话框对象并初始化界面
     *
     * @param   parent 父窗口指针，默认为nullptr
     * @note    构造函数会调用initUi()和setupStyle()来初始化界面
     * @see     ~SnapDialog, initUi, setupStyle
     */
    explicit SnapDialog(QWidget* parent = nullptr);

    /**
     * @brief   析构函数
     * @details 清理快照对话框资源
     * @note    析构函数会清理动画对象和其他资源
     * @see     SnapDialog
     */
    ~SnapDialog();

    /**
     * @brief   淡出并隐藏
     * @details 执行淡出动画后隐藏对话框
     * @note    该函数会播放淡出动画，动画完成后自动隐藏对话框
     * @see     showEvent
     */
    void fadeOutAndHide();

private slots:
    /**
     * @brief   显示事件处理
     * @details 处理对话框显示事件，播放淡入动画
     *
     * @param   event 显示事件对象
     * @note    该槽函数会在对话框显示时播放淡入动画
     * @see     fadeOutAndHide
     */
    void showEvent(QShowEvent* event) override;

public slots:
    /**
     * @brief   绘制图像
     * @details 在快照窗口中显示指定的图像
     *
     * @param   pix 要显示的像素图
     * @note    该槽函数会将图像传递给SnapWidget进行显示
     * @see     SnapWidget
     */
    void drawPic(const QPixmap& pix);

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化快照对话框的各个组件
     * @note    该函数会创建SnapWidget、按钮、标签等组件
     * @see     setupStyle
     */
    void initUi();

    /**
     * @brief   设置界面样式
     * @details 设置快照对话框的视觉效果和样式
     * @note    该函数会设置对话框的颜色、字体、布局等样式
     * @see     initUi
     */
    void setupStyle();

    /** @brief 快照窗口组件指针 */
    SnapWidget* m_SnapWidget;

    /** @brief 淡入动画对象指针 */
    QPropertyAnimation* fadeInAnimation;

    /** @brief 淡出动画对象指针 */
    QPropertyAnimation* fadeOutAnimation;
};