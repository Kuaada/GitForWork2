/**
 * @file    SnapWidget.h
 * @brief   快照窗口组件类，显示快照图像
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的快照显示功能，包括：
 *          - 快照图像的显示和缩放
 *          - 图像交互和操作
 *          - 图像编辑和调整
 *          - 鼠标和键盘事件处理
 *          - 图像保存和导出
 *          - 自定义绘制和渲染
 *          该类是DSV项目的快照显示组件，为用户提供
 *          快照图像的预览和操作界面。
 *
 * @note    该类继承自QWidget，是DSV项目的快照显示组件
 * @see     SnapDialog, PathologyViewer
 */

#pragma once

#include <QWidget>
#include <QPixmap>

 /**
  * @class  SnapWidget
  * @brief  快照窗口组件类，显示快照图像
  * @details 该类是DSV项目的快照显示组件，负责：
  *          - 显示快照图像
  *          - 处理图像交互操作
  *          - 管理图像保存功能
  *          - 处理鼠标和键盘事件
  *          - 自定义绘制和渲染
  *          - 与快照对话框的通信
  *
  *          主要功能包括：
  *          - 图像显示：显示传入的快照图像
  *          - 图像缩放：支持图像的缩放和适应窗口
  *          - 交互操作：处理鼠标点击和拖拽操作
  *          - 保存功能：提供图像保存到文件的功能
  *          - 自定义绘制：重写绘制事件实现自定义渲染
  *          - 状态管理：管理组件的初始化和更新状态
  *
  * @note   该类继承自QWidget，提供自定义的图像显示界面
  * @example
  *          // 使用示例
  *          SnapWidget* snapWidget = new SnapWidget(parent);
  *
  *          // 设置要显示的图像
  *          snapWidget->setPixmap(capturedPixmap);
  *
  *          // 保存快照
  *          snapWidget->saveSnap();
  *
  *          // 组件会自动处理绘制和交互
  * @see     SnapDialog, PathologyViewer
  */
class SnapWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建快照窗口组件对象并初始化
     *
     * @param   parent 父窗口指针，默认为nullptr
     * @note    构造函数会初始化组件的状态和属性
     * @see     setPixmap
     */
    explicit SnapWidget(QWidget* parent = nullptr);

    /**
     * @brief   设置像素图
     * @details 设置要显示的快照图像
     *
     * @param   pix 要显示的像素图
     * @note    该函数会更新内部存储的图像并触发重绘
     * @see     paintEvent
     */
    void setPixmap(const QPixmap& pix);

public slots:
    /**
     * @brief   保存快照
     * @details 将当前显示的图像保存到文件
     * @note    该槽函数会弹出文件保存对话框，让用户选择保存位置和格式
     * @see     setPixmap
     */
    void saveSnap();

protected:
    /**
     * @brief   绘制事件处理
     * @details 处理绘制事件，显示快照图像
     *
     * @param   event 绘制事件对象
     * @note    该函数会绘制存储的像素图，支持缩放和适应窗口
     * @see     setPixmap
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief   鼠标按下事件处理
     * @details 处理鼠标按下事件，实现交互功能
     *
     * @param   event 鼠标事件对象
     * @note    该函数会处理鼠标点击操作，可能触发保存或其他功能
     * @see     saveSnap
     */
    void mousePressEvent(QMouseEvent* event) override;

signals:
    /**
     * @brief   组件更新信号
     * @details 当组件状态发生变化时发出此信号
     * @note    该信号用于通知其他组件快照窗口的状态变化
     */
    void widgetUpdate();

private:
    /** @brief 存储的像素图 */
    QPixmap m_pixmap;

    /** @brief 初始化状态标志 */
    bool m_InitState;
};