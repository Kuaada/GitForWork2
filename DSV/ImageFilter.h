/**
 * @file    ImageFilter.h
 * @brief   图像过滤器类，提供图像处理功能
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的图像处理功能，包括：
 *          - 图像滤波和增强
 *          - 颜色空间转换
 *          - 图像锐化和模糊
 *          - 噪声去除和降噪
 *          - 对比度和亮度调整
 *          - 图像处理参数设置
 *          该类是DSV项目的图像处理组件，为用户提供
 *          便捷的图像增强和调整功能。
 *
 * @note    该类继承自QDialog，是DSV项目的图像处理组件
 * @see     MainWin, PathologyViewer, SlideColorManagement
 */

#pragma once

#include <QDialog>
#include <QLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>

 /**
  * @class  ImageFilter
  * @brief  图像过滤器类，提供图像处理功能
  * @details 该类是DSV项目的图像处理对话框类，负责：
  *          - 提供图像处理参数设置界面
  *          - 管理图像滤波和增强功能
  *          - 处理图像锐化和模糊操作
  *          - 控制对比度和亮度调整
  *          - 管理图像处理算法的参数
  *          - 与主窗口和查看器的通信
  *
  *          主要功能包括：
  *          - 锐化控制：通过滑块控制图像锐化程度
  *          - 参数输入：通过文本框输入精确的处理参数
  *          - 实时预览：实时显示处理效果
  *          - 参数保存：保存用户设置的处理参数
  *          - 预设管理：提供常用的处理预设
  *          - 界面布局：管理对话框的布局和样式
  *
  * @note   该类继承自QDialog，提供模态的图像处理设置界面
  * @example
  *          // 使用示例
  *          ImageFilter* imageFilter = new ImageFilter(parent);
  *
  *          // 显示图像过滤器对话框
  *          imageFilter->show();
  *
  *          // 用户可以通过滑块和文本框调整参数
  *          // 对话框会自动处理参数变化和应用效果
  * @see     MainWin, PathologyViewer, SlideColorManagement
  */
class ImageFilter : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建图像过滤器对话框对象并初始化界面
     *
     * @param   parent 父窗口指针
     * @note    构造函数会调用initUi()和initConnect()来初始化界面和连接
     * @see     ~ImageFilter, initUi, initConnect
     */
    ImageFilter(QWidget* parent);

    /**
     * @brief   析构函数
     * @details 清理图像过滤器对话框资源
     * @note    析构函数会清理所有UI组件和资源
     * @see     ImageFilter
     */
    ~ImageFilter();

private:
    /**
     * @brief   初始化用户界面
     * @details 创建和初始化图像过滤器对话框的各个组件
     * @note    该函数会创建滑块、文本框、标签等UI组件
     * @see     initConnect
     */
    void initUi();

    /**
     * @brief   初始化信号槽连接
     * @details 建立各个组件之间的信号槽连接
     * @note    该函数会连接滑块值变化、文本框输入等信号
     * @see     initUi
     */
    void initConnect();

    /** @brief 锐化滑块指针，用于控制图像锐化程度 */
    QSlider* m_SharpSlider;

    /** @brief 参数输入文本框指针，用于输入精确的处理参数 */
    QLineEdit* m_LineEdit;
};