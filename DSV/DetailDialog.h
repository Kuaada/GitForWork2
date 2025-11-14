/**
 * @file    DetailDialog.h
 * @brief   详情对话框类，显示切片属性和详细信息
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的切片详情显示功能，包括：
 *          - 切片属性信息的展示
 *          - 详细参数的显示与管理
 *          - MPP（微米每像素）等关键信息的获取
 *          该类是DSV项目的切片详情组件，为用户提供
 *          切片属性的详细查看界面。
 *
 * @note    该类继承自QDialog，是DSV项目的详情显示组件
 * @see     MainWin, PathologyViewer, SlideColorManagement
 */
#pragma once

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QDebug>
#include "SlideColorManagement.h"

 /**
  * @class  DetailDialog
  * @brief  详情对话框类，显示切片属性和详细信息
  * @details 该类是DSV项目的切片详情对话框，负责：
  *          - 展示切片的属性信息
  *          - 管理和显示详细参数
  *          - 提供MPP（微米每像素）等关键信息的获取接口
  *
  *          主要功能包括：
  *          - 属性展示：显示传入的切片属性信息
  *          - 参数管理：管理和显示详细参数
  *          - MPP获取：提供获取微米每像素值的接口
  *          - 界面布局：管理对话框的布局和样式
  *
  * @note   该类继承自QDialog，提供模态的详情显示界面
  * @example
  *          // 使用示例
  *          DetailDialog* detailDialog = new DetailDialog(parent, properties);
  *
  *          // 获取切片的MPP值
  *          float mpp = detailDialog->retMpp();
  *
  *          // 详情对话框会自动显示所有属性信息
  * @see     MainWin, PathologyViewer, SlideColorManagement
  */

class DetailDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建详情对话框对象并初始化界面
     *
     * @param   parent 父窗口指针
     * @param   properties 切片属性信息列表
     * @note    构造函数会调用initUi()来初始化界面并显示属性
     * @see     ~DetailDialog, initUi
     */
    explicit DetailDialog(QWidget* parent, const std::vector<SlideColorManagement::PropertyInfo>& properties);

    /**
     * @brief   析构函数
     * @details 清理详情对话框资源
     * @note    析构函数会清理所有UI组件和资源
     * @see     DetailDialog
     */
    ~DetailDialog();

    /**
     * @brief   获取微米每像素值
     * @details 返回当前切片的MPP（微米每像素）值
     *
     * @return  当前切片的MPP值
     * @note    该函数用于获取切片的空间分辨率
     */
    float retMpp() { return m_MPP; }

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化详情对话框的各个组件
     * @note    该函数会根据属性信息动态生成界面内容
     */
    void initUi();

    /** @brief 切片属性信息列表 */
    std::vector<SlideColorManagement::PropertyInfo> m_properties;

    /** @brief 当前切片的微米每像素值 */
    float m_MPP;
};