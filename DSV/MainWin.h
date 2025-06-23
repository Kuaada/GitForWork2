/**
 * @file    MainWin.h
 * @brief   主窗口类，整合所有UI组件和功能模块
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的主窗口功能，包括：
 *          - 用户界面的整体布局和管理
 *          - 菜单栏、工具栏、状态栏的管理
 *          - 文件操作（打开、保存、导出）
 *          - 与PathologyViewer的集成
 *          - 设置和配置管理
 *          - 各种对话框和组件的协调
 *          该类是DSV项目的UI入口点，整合了所有功能模块，
 *          为用户提供完整的病理图像浏览和标注界面。
 *
 * @note    该类继承自QMainWindow，是DSV项目的主要窗口组件
 * @see     PathologyViewer, CenteredToolBar, FileWidget, SnapDialog
 */

#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QFileDialog>
#include <QStatusBar>
#include "CenteredToolBar.h"
#include "FileWidget.h"
#include "MultiResolutionImageFactory.h"
#include "PathologyViewer.h"
#include "SnapDialog.h"
#include "PlotBar.h"
#include "InputDialog.h"
#include "ImageFilter.h"

 // 前向声明
class MultiResolutionImage;

/**
 * @class  MainWin
 * @brief  主窗口类，整合所有UI组件和功能模块
 * @details 该类是DSV项目的主窗口类，负责：
 *          - 用户界面的整体布局和管理
 *          - 各种UI组件的创建和协调
 *          - 文件操作和图像加载
 *          - 工具栏和菜单的管理
 *          - 状态栏和进度显示
 *          - 对话框和组件的集成
 *
 *          主要功能包括：
 *          - 界面布局：管理主窗口的整体布局和组件排列
 *          - 文件管理：处理图像文件的打开、保存和导出
 *          - 工具栏控制：管理各种工具按钮和功能
 *          - 状态显示：在状态栏显示当前操作状态
 *          - 组件协调：协调各个UI组件之间的交互
 *          - 设置管理：管理应用程序的各种设置
 *
 * @note   该类继承自QMainWindow，是DSV项目的UI入口点
 * @example
 *          // 使用示例
 *          MainWin* mainWindow = new MainWin();
 *          mainWindow->show();
 *
 *          // 主窗口会自动初始化所有UI组件
 *          // 包括工具栏、文件窗口、病理查看器等
 * @see     PathologyViewer, CenteredToolBar, FileWidget, SnapDialog
 */
class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建主窗口对象并初始化所有UI组件
     *
     * @param   parent 父窗口指针，默认为0
     * @note    构造函数会调用initUi()、setupStyle()、initConnect()等函数
     *          来完整初始化主窗口
     * @see     ~MainWin, initUi, setupStyle, initConnect
     */
    explicit MainWin(QWidget* parent = 0);

    /**
     * @brief   析构函数
     * @details 清理主窗口资源
     * @note    析构函数会清理所有UI组件和资源
     * @see     MainWin
     */
    ~MainWin();

protected:
    /**
     * @brief   窗口大小改变事件处理
     * @details 处理窗口大小改变事件，重新调整布局
     *
     * @param   event 大小改变事件对象
     * @note    该函数会重新调整各个组件的布局以适应新的窗口大小
     */
    void resizeEvent(QResizeEvent* event) override;

signals:
    /**
     * @brief   文件窗口状态信号
     * @details 当文件窗口状态改变时发出此信号
     *
     * @param   state 文件窗口的新状态，true显示，false隐藏
     * @note    该信号用于通知其他组件文件窗口的状态变化
     */
    void fileWidgetSate(bool state);

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化所有UI组件
     * @note    该函数会创建工具栏、文件窗口、病理查看器等所有组件
     * @see     setupStyle, initConnect
     */
    void initUi();

    /**
     * @brief   设置界面样式
     * @details 设置主窗口和组件的样式
     * @note    该函数会设置窗口的视觉样式和主题
     * @see     initUi, initConnect
     */
    void setupStyle();

    /**
     * @brief   初始化信号槽连接
     * @details 建立各个组件之间的信号槽连接
     * @note    该函数会连接所有必要的信号和槽函数
     * @see     initUi, setupStyle
     */
    void initConnect();

    /**
     * @brief   设置文件路径树
     * @details 初始化文件路径树结构
     * @note    该函数会设置文件浏览的目录结构
     */
    void setFilePathTree();

    /**
     * @brief   打开文件处理
     * @details 处理文件打开操作
     *
     * @param   fileName 要打开的文件路径
     * @note    该函数会加载指定的图像文件并显示
     * @see     getFileNameAndFactory
     */
    void onOpenFile(const QString& fileName);

    /**
     * @brief   获取文件名和工厂
     * @details 获取支持的文件名列表和对应的工厂
     *
     * @return  支持的文件名列表
     * @note    该函数返回所有支持的文件格式
     * @see     onOpenFile
     */
    QList<QString> getFileNameAndFactory();

    // UI组件成员
    /** @brief 文件按钮指针 */
    QPushButton* m_FileButton;

    /** @brief 居中工具栏指针 */
    CenteredToolBar* m_ToolBar;

    /** @brief 文件窗口指针 */
    FileWidget* m_FileWidget;

    /** @brief 快照对话框指针 */
    SnapDialog* m_SnapDialog;

    /** @brief 绘图栏指针 */
    PlotBar* m_PlotBar;

    /** @brief 图像过滤器指针 */
    ImageFilter* m_ImageFilter;

    // 数据和状态成员
    /** @brief 多分辨率图像对象，用于存储当前图像 */
    std::shared_ptr<MultiResolutionImage> _img;

    /** @brief 缓存最大字节大小 */
    unsigned long long _cacheMaxByteSize;

    /** @brief 状态栏指针 */
    QStatusBar* statusBar;

    /** @brief 中央窗口部件指针，包含病理查看器 */
    QWidget* centralWidget;

    /** @brief 水平布局指针，用于中央窗口部件的布局 */
    QHBoxLayout* horizontalLayout_2;

    /** @brief 病理查看器指针，主要的图像显示组件 */
    PathologyViewer* pathologyView;

public slots:
    /**
     * @brief   设置工具栏启用状态
     * @details 启用或禁用工具栏的功能
     * @note    该槽函数用于控制工具栏的可用性
     */
    void setEnableToolBar();
};