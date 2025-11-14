/**
 * @file    FileWidget.h
 * @brief   文件窗口类，提供文件浏览和管理功能
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的文件管理功能，包括：
 *          - 文件树浏览和路径管理
 *          - 支持的文件格式过滤
 *          - 文件预览和选择
 *          - 最近打开文件列表
 *          - 文件操作（打开、保存、导出）
 *          - 文件系统模型和视图管理
 *          该类是DSV项目的文件管理组件，为用户提供
 *          便捷的文件浏览和选择功能。
 *
 * @note    该类继承自QWidget，是DSV项目的文件管理组件
 * @see     MainWin, MultiResolutionImageFactory
 */

#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QTabWidget>
#include <QFileSystemModel>
#include <QTreeView>
#include "TabStyle.hpp"
#include <QFileInfo>
#include <QListView>
#include <QStandardItemModel>

 /**
  * @class  FileWidget
  * @brief  文件窗口类，提供文件浏览和管理功能
  * @details 该类是DSV项目的文件管理类，负责：
  *          - 提供文件树浏览界面
  *          - 管理文件系统模型和视图
  *          - 处理文件选择和路径传递
  *          - 支持多种文件格式的过滤
  *          - 提供最近打开文件列表
  *          - 管理文件窗口的样式和布局
  *
  *          主要功能包括：
  *          - 文件浏览：使用树形视图显示文件系统结构
  *          - 文件过滤：只显示支持的文件格式（如.svs、.tiff等）
  *          - 文件选择：处理用户的文件选择操作
  *          - 路径管理：管理当前选中的文件路径
  *          - 最近文件：显示最近打开的文件列表
  *          - 界面布局：管理文件窗口的标签页和布局
  *
  * @note   该类继承自QWidget，提供文件浏览和管理界面
  * @example
  *          // 使用示例
  *          FileWidget* fileWidget = new FileWidget(parent);
  *
  *          // 连接文件选择信号
  *          connect(fileWidget, &FileWidget::fileSelected,
  *                  this, &MainWindow::onFileSelected);
  *
  *          // 文件窗口会自动显示文件系统
  *          // 并处理用户的文件选择操作
  * @see     MainWin, MultiResolutionImageFactory, QFileSystemModel
  */
class FileWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建文件窗口对象并初始化文件浏览界面
     *
     * @param   parent 父窗口指针
     * @note    构造函数会调用initUI()和setupStyle()来初始化界面
     * @see     ~FileWidget, initUI, setupStyle
     */
    explicit FileWidget(QWidget* parent);

    /**
     * @brief   析构函数
     * @details 清理文件窗口资源
     * @note    析构函数会清理文件系统模型和视图资源
     * @see     FileWidget
     */
    ~FileWidget();

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化文件浏览界面的各个组件
     * @note    该函数会创建文件树视图、列表视图、标签页等组件
     * @see     setupStyle
     */
    void initUI();

    /**
     * @brief   设置界面样式
     * @details 设置文件窗口的视觉效果和样式
     * @note    该函数会设置窗口的颜色、字体、布局等样式
     * @see     initUI
     */
    void setupStyle();

signals:
    /**
     * @brief   文件选择信号
     * @details 当用户选择文件时发出此信号
     *
     * @param   filePath 选中文件的完整路径
     * @note    该信号用于通知其他组件文件选择事件
     * @see     onFileSelected
     */
    void fileSelected(const QString& filePath);

    /**
     * @brief   文件路径传递信号
     * @details 当需要传递文件路径时发出此信号
     *
     * @param   filePath 要传递的文件路径
     * @note    该信号用于将文件路径传递给其他组件
     * @see     fileSelected
     */
    void filePathTrans(const QString& filePath);

private slots:
    /**
     * @brief   树形视图项目点击槽函数
     * @details 处理树形视图中项目被点击的事件
     *
     * @param   index 被点击项目的模型索引
     * @note    该槽函数会检查点击的项目是否为文件，如果是则发出选择信号
     * @see     onFileSelected, onListViewItemClicked
     */
    void onTreeViewItemClicked(const QModelIndex& index);

    /**
     * @brief   文件选择处理槽函数
     * @details 处理文件选择事件
     *
     * @param   filePath 选中文件的路径
     * @note    该槽函数会验证文件格式并发出路径传递信号
     * @see     onTreeViewItemClicked, onListViewItemClicked
     */
    void onFileSelected(const QString& filePath);

    /**
     * @brief   列表视图项目点击槽函数
     * @details 处理列表视图中项目被点击的事件
     *
     * @param   index 被点击项目的模型索引
     * @note    该槽函数会处理最近文件列表中的项目点击
     * @see     onTreeViewItemClicked, onFileSelected
     */
    void onListViewItemClicked(const QModelIndex& index);
};