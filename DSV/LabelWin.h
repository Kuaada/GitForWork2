/**
 * @file    LabelWin.h
 * @brief   标签窗口类，显示和管理标签图像
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的标签显示功能，包括：
 *          - 标签图像的显示和更新
 *          - 标签窗口的布局和管理
 *          - 标签图像的自适应缩放
 *          该类是DSV项目的标签显示组件，为用户提供
 *          标签图像的预览和交互界面。
 *
 * @note    该类继承自QWidget，是DSV项目的标签显示组件
 * @see     MainWin, PathologyViewer
 */

#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>

 /**
  * @class  LabelWin
  * @brief  标签窗口类，显示和管理标签图像
  * @details 该类是DSV项目的标签显示窗口，负责：
  *          - 显示标签图像
  *          - 管理标签窗口的布局
  *          - 处理标签图像的自适应缩放
  *          - 提供标签图像的更新接口
  *
  *          主要功能包括：
  *          - 标签显示：显示传入的标签图像
  *          - 图像更新：支持动态更新标签内容
  *          - 窗口自适应：根据窗口大小自适应缩放标签图像
  *          - 界面布局：管理窗口的布局和样式
  *
  * @note   该类继承自QWidget，提供自定义的标签显示界面
  * @example
  *          // 使用示例
  *          LabelWin* labelWin = new LabelWin(parent, labelImage);
  *
  *          // 设置新的标签图像
  *          labelWin->setLabel(newLabelImage);
  *
  *          // 标签窗口会自动适应窗口大小
  * @see     MainWin, PathologyViewer
  */
class LabelWin : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建标签窗口对象并初始化界面
     *
     * @param   parent 父窗口指针，默认为nullptr
     * @param   img 初始标签图像，默认为空
     * @note    构造函数会调用initUi()来初始化界面
     * @see     ~LabelWin, initUi
     */
    explicit LabelWin(QWidget* parent = nullptr, const QImage& img = QImage());

    /**
     * @brief   析构函数
     * @details 清理标签窗口资源
     * @note    析构函数会清理所有UI组件和资源
     * @see     LabelWin
     */
    ~LabelWin();

    /**
     * @brief   设置标签图像
     * @details 更新并显示新的标签图像
     *
     * @param   Label 新的标签图像
     * @note    该函数会更新内部图像并触发重绘
     * @see     resizeEvent
     */
    void setLabel(const QImage& Label);

protected:
    /**
     * @brief   窗口大小调整事件
     * @details 处理窗口大小变化，实现标签图像自适应缩放
     *
     * @param   event 窗口大小调整事件对象
     * @note    该函数会根据窗口大小调整标签图像的显示
     * @see     setLabel
     */
    void resizeEvent(QResizeEvent* event);

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化标签窗口的各个组件
     * @note    该函数会创建标签图像显示控件并设置布局
     */
    void initUi();

    /** @brief 标签图像显示控件指针 */
    QLabel* m_LabelPic;

    /** @brief 当前显示的标签图像 */
    QImage m_Image;
};