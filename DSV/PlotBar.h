/**
 * @file    PlotBar.h
 * @brief   绘图栏类，提供绘图和测量功能
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的绘图功能，包括：
 *          - 各种绘图工具（直线、矩形、椭圆等）
 *          - 测量工具（距离、角度、面积等）
 *          - 绘图数据的存储和管理
 *          - 绘图结果的显示和导出
 *          - 绘图参数的设置和调整
 *          - 绘图工具栏的布局和管理
 *          该类是DSV项目的绘图功能组件，为用户提供
 *          便捷的绘图和测量工具。
 *
 * @note    该类继承自QWidget，是DSV项目的绘图功能组件
 * @see     MainWin, PathologyViewer, RenderElement
 */

#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QResizeEvent>
#include <QListWidget>
#include <QLabel>


 /**
  * @class  PlotBar
  * @brief  绘图栏类，提供绘图和测量功能
  * @details 该类是DSV项目的绘图工具栏类，负责：
  *          - 提供各种绘图工具按钮
  *          - 管理绘图工具栏的布局
  *          - 处理绘图工具的选择和切换
  *          - 管理绘图数据的显示列表
  *          - 控制绘图栏的显示和隐藏
  *          - 与主窗口和查看器的通信
  *
  *          主要功能包括：
  *          - 绘图工具：直线、矩形、椭圆、文本、轮廓等绘制工具
  *          - 测量工具：距离测量、角度测量、面积计算等测量功能
  *          - 数据管理：存储和管理绘制的图形数据
  *          - 结果显示：在列表中显示绘图结果和测量数据
  *          - 工具切换：处理不同绘图工具之间的切换
  *          - 界面布局：管理工具栏的水平布局和样式
  *
  * @note   该类继承自QWidget，提供水平布局的绘图工具栏
  * @example
  *          // 使用示例
  *          PlotBar* plotBar = new PlotBar(parent);
  *
  *          // 绘图栏会自动创建各种绘图工具按钮
  *          // 并处理用户的工具选择操作
  *
  *          // 绘图栏可以显示/隐藏
  *          plotBar->setVisible(true);
  * @see     MainWin, PathologyViewer, RenderElement
  */
class PlotBar : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建绘图栏对象并初始化绘图工具界面
     *
     * @param   parent 父窗口指针
     * @note    构造函数会调用initUi()和setUpStyle()来初始化界面
     * @see     ~PlotBar, initUi, setUpStyle
     */
    explicit PlotBar(QWidget* parent);

    /**
     * @brief   析构函数
     * @details 清理绘图栏资源
     * @note    析构函数为空，Qt会自动清理资源
     * @see     PlotBar
     */
    ~PlotBar() {};

    //signals:
    //    /**
    //     * @brief   列表可见状态信号
    //     * @details 当绘图结果列表的可见状态改变时发出此信号
    //     * 
    //     * @param   state 列表的可见状态，true显示，false隐藏
    //     * @note    该信号用于通知其他组件列表状态变化
    //     */
    //    void listVisibleState(bool state);

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化绘图工具栏的各个组件
     * @note    该函数会创建各种绘图工具按钮和列表组件
     * @see     setUpStyle, createToolButton
     */
    void initUi();

    /**
     * @brief   创建工具按钮
     * @details 创建带有指定图标的工具按钮
     *
     * @param   iconPath 图标文件路径
     * @return  创建的工具按钮指针
     * @note    该函数会创建一个带有指定图标的QToolButton
     * @see     initUi
     */
    QToolButton* createToolButton(const QString& iconPath);

    /**
     * @brief   设置样式
     * @details 设置绘图工具栏的视觉效果和样式
     * @note    该函数会设置工具栏的颜色、字体、间距等样式
     * @see     initUi
     */
    void setUpStyle();

    /** @brief 可见状态标志 */
    bool m_stateVisible;


public slots:
    void setOneLabel(float Perimeter);
    void setTwoLabel(float Perimeter,float Area);
};