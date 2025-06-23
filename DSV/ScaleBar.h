/**
 * @file    ScaleBar.h
 * @brief   比例尺组件类，显示当前视图的物理比例尺
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的比例尺显示功能，包括：
 *          - 当前视图物理比例尺的绘制
 *          - 分辨率设置与动态更新
 *          - 随视场变化自动调整比例尺长度和标签
 *          该类是DSV项目的比例尺显示组件，为用户提供
 *          直观的物理距离参考。
 *
 * @note    该类继承自QWidget，是DSV项目的比例尺显示组件
 * @see     PathologyViewer
 */

#pragma once

#include <QWidget>

 /**
  * @class  ScaleBar
  * @brief  比例尺组件类，显示当前视图的物理比例尺
  * @details 该类是DSV项目的比例尺显示组件，负责：
  *          - 绘制当前视图的物理比例尺
  *          - 根据分辨率动态调整比例尺长度和标签
  *          - 响应视场变化自动更新比例尺
  *          - 提供分辨率设置和获取接口
  *
  *          主要功能包括：
  *          - 比例尺绘制：根据当前分辨率绘制物理比例尺
  *          - 分辨率设置：支持动态设置和更新分辨率
  *          - 视场联动：根据视场变化自动调整比例尺
  *          - 标签显示：显示当前比例尺的物理长度标签
  *          - 界面自适应：根据窗口大小自适应绘制
  *
  * @note   该类继承自QWidget，提供自定义的比例尺显示界面
  * @example
  *          // 使用示例
  *          ScaleBar* scaleBar = new ScaleBar(0.25f, parent);
  *
  *          // 设置分辨率
  *          scaleBar->setResolution(0.5f);
  *
  *          // 响应视场变化
  *          scaleBar->updateForFieldOfView(fieldOfViewRect);
  * @see     PathologyViewer
  */
class ScaleBar : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建比例尺对象并初始化分辨率
     *
     * @param   resolution 初始分辨率（微米/像素）
     * @param   parent 父窗口指针
     * @note    构造函数会初始化比例尺参数
     * @see     setResolution
     */
    ScaleBar(float resolution, QWidget* parent);

    /**
     * @brief   析构函数
     * @details 清理比例尺组件资源
     * @note    析构函数会清理所有UI组件和资源
     * @see     ScaleBar
     */
    ~ScaleBar();

    /**
     * @brief   推荐尺寸
     * @details 返回比例尺的推荐显示尺寸
     *
     * @return  推荐的QSize尺寸
     * @note    用于界面布局自适应
     */
    QSize sizeHint() const;

    /**
     * @brief   按宽度自适应高度
     * @details 根据给定宽度返回合适的高度
     *
     * @param   w 目标宽度
     * @return  计算得到的高度
     * @note    用于界面布局自适应
     */
    int heightForWidth(int w) const;

    /**
     * @brief   设置分辨率
     * @details 设置比例尺的分辨率（微米/像素）
     *
     * @param   resolution 新的分辨率
     * @note    该函数会触发比例尺重绘
     */
    void setResolution(float resolution);

signals:
    /**
     * @brief   比例尺参数信号
     * @details 当比例尺参数变化时发出该信号
     *
     * @param   i 当前比例尺标签字符串
     * @note    用于通知主视图或其他组件
     */
    void scaleParametre(QString i);

public slots:
    /**
     * @brief   视场更新槽
     * @details 当视场变化时更新比例尺显示
     *
     * @param   fieldOfView 当前视场矩形
     * @note    该槽函数用于主视图联动
     */
    void updateForFieldOfView(const QRectF& fieldOfView);

protected:
    /**
     * @brief   绘制事件
     * @details 处理比例尺的绘制，包括线条和标签
     *
     * @param   event 绘制事件对象
     * @note    该函数会根据当前分辨率和视场绘制比例尺
     */
    void paintEvent(QPaintEvent* event);

private:
    /** @brief 当前分辨率（微米/像素） */
    float _resolution;

    /** @brief 当前比例尺标签 */
    QString _currentLabel;

    /** @brief 当前比例尺长度（像素） */
    float _currentWidth;

    /**
     * @brief   计算比例尺参数
     * @details 根据屏幕像素密度和单位选择计算比例尺长度和标签
     *
     * @param   pixelsPerScreenPixel 屏幕像素密度
     * @param   useMicrons 是否使用微米单位
     * @note    该函数会更新_currentLabel和_currentWidth
     */
    void calculateScale(float pixelsPerScreenPixel, bool useMicrons);

    /** @brief 上一次的视场范围 */
    QRectF _lastFieldOfView;

};