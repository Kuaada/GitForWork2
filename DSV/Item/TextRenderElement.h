/**
 * @file    TextRenderElement.h
 * @brief   文本渲染元素类，实现文本的绘制和编辑功能
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了DSV项目的文本渲染功能，包括：
 *          - 文本的绘制和显示
 *          - 文本的拖拽和编辑操作
 *          - 双击编辑和焦点管理
 *          - 键盘事件处理和输入确认
 *          - 悬停效果和视觉反馈
 *          - JSON序列化和反序列化
 *          - 文本交互标志管理
 * 
 * @note    该类是DSV项目标注工具的重要组件，用于文本标注、
 *          说明文字、测量结果标注等功能
 * @see     RenderElement, QGraphicsTextItem, EllipseRenderElement
 */

#pragma once
//#define M_PI 3.14
#include<QString>
#include<QPainter>
#include<QImage>
#include<QWidget>
#include<QJsonObject>
#include<QGraphicsItem>
#include<QGraphicsLineItem>
#include<QGraphicsSceneMouseEvent>
#include"RenderElement.h"
#include<QGraphicsTextItem>
#include<QGraphicsScene>
#include<QGraphicsView>
#include <QKeyEvent>
#include <QEvent>
#include <QInputMethodEvent>
#include <QFocusEvent>

/**
 * @brief   文本渲染元素类
 * @details 继承自QGraphicsTextItem和RenderElement，专门负责文本的渲染和编辑。
 *          提供文本的绘制、拖拽、双击编辑、键盘输入等功能。支持焦点管理，
 *          允许用户通过双击进入编辑模式，通过回车键确认输入。
 * 
 * @note    该类是DSV项目标注工具的重要组件，用于文本标注、
 *          说明文字、测量结果标注等功能。支持JSON序列化，便于数据保存和恢复。
 * 
 * @example
 * @code
 * // 创建文本渲染元素
 * TextRenderElement* text = new TextRenderElement("标注", "双击输入内容");
 * 
 * // 添加到场景
 * scene->addItem(text);
 * 
 * // 双击编辑文本
 * // 用户双击文本项进入编辑模式
 * // 按回车键确认输入
 * @endcode
 */
class TextRenderElement :public QGraphicsTextItem, public RenderElement
{
public:
    /**
     * @brief   构造函数（仅名称）
     * @param   strName    文本元素名称
     * @param   parent     父图形项指针
     * @details 创建一个空的文本渲染元素，默认显示"双击输入内容"
     */
    TextRenderElement(QString strName, QGraphicsItem *parent=nullptr);
    
    /**
     * @brief   构造函数（名称和文本内容）
     * @param   strName    文本元素名称
     * @param   strText    文本内容
     * @param   parent     父图形项指针
     * @details 创建一个指定内容的文本渲染元素
     */
    TextRenderElement(QString strName,QString strText,QGraphicsItem* parent = nullptr);
    
    /**
     * @brief   构造函数（JSON数据）
     * @param   json    JSON对象，包含文本元素的所有属性
     * @details 通过JSON数据创建文本渲染元素，用于数据恢复和加载
     * 
     * @note    JSON格式应包含名称、文本内容、样式等信息
     */
    TextRenderElement(QJsonObject json);
    
    /**
     * @brief   析构函数
     * @details 清理文本渲染元素的资源
     */
    virtual ~TextRenderElement();
    
    /**
     * @brief   转换为JSON数据
     * @return  包含文本元素所有属性的JSON对象
     * @details 将文本渲染元素的所有属性序列化为JSON格式，
     *          包括名称、文本内容、样式等
     * 
     * @note    该函数是RenderElement虚函数的重写
     */
    virtual QJsonObject toJson();
    
    /**
     * @brief   鼠标双击事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标双击事件，进入文本编辑模式：
     *          - 如果当前文本是默认提示，则清空文本
     *          - 设置文本为可编辑和可选择状态
     *          - 设置光标样式
     * 
     * @note    该函数是QGraphicsTextItem虚函数的重写
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override
    {
        // 双击事件现在主要由sceneEvent处理，这里只做基本处理
        event->accept();
    }
    
    /**
     * @brief   鼠标按下事件
     * @param   event   鼠标事件对象
     * @details 处理鼠标按下事件：
     *          - 如果点击在文本区域外且当前处于编辑状态，则清除焦点
     *          - 结束编辑模式
     * 
     * @note    该函数是QGraphicsTextItem虚函数的重写
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        // 鼠标按下事件现在主要由sceneEvent处理，这里只做基本处理
        if (textInteractionFlags() != Qt::NoTextInteraction) {
            // 在编辑模式下，只处理文本区域内的点击
            if (isPointInTextArea(event->pos())) {
                QGraphicsTextItem::mousePressEvent(event);
            } else {
                event->accept();
            }
        } else {
            QGraphicsTextItem::mousePressEvent(event);
        }
    }
    
    /**
     * @brief   焦点失去事件
     * @param   event   焦点事件对象
     * @details 处理焦点失去事件：
     *          - 如果文本为空，则设置为默认提示文本
     *          - 禁用文本交互，退出编辑模式
     *
     * @note    该函数是QGraphicsTextItem虚函数的重写
     */
    virtual void focusOutEvent(QFocusEvent* event) override;
    
    /**
     * @brief   鼠标悬停进入事件
     * @param   event   悬停事件对象
     * @details 当鼠标悬停在文本上时触发，显示编辑提示或高亮效果
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    
    /**
     * @brief   鼠标悬停离开事件
     * @param   event   悬停事件对象
     * @details 当鼠标离开文本时触发，隐藏编辑提示或高亮效果
     * 
     * @note    该函数是QGraphicsItem虚函数的重写
     */
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    
    /**
     * @brief   自定义绘制函数
     * @param   painter     绘制器
     * @param   option      绘制选项
     * @param   widget      绘制目标窗口
     * @details 自定义绘制文本，提供更好的视觉效果
     *
     * @note    该函数是QGraphicsTextItem虚函数的重写
     */
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    /**
     * @brief   检查点是否在有效的文本区域内
     * @param   pos   本地坐标系中的点
     * @return  如果点在有效区域内返回true
     * @details 考虑到ItemIgnoresTransformations的影响，提供更精确的边界检测
     */
    bool isPointInTextArea(const QPointF& pos) const;

    /**
     * @brief   计算编辑模式下的最佳字体大小
     * @return  适合当前缩放级别的字体大小
     * @details 基于屏幕尺寸计算合适的字体大小，确保输入框大小合理
     */
    int calculateEditFontSize() const;

    /**
     * @brief   设置编辑模式的文本框大小
     * @details 直接控制文本框的显示大小，使其相对于屏幕大小合理
     */
    void setupEditMode();

protected:
    /**
     * @brief   场景事件处理
     * @param   event   场景事件
     * @details 重写场景事件处理，防止在ItemIgnoresTransformations模式下的坐标问题
     */
    bool sceneEvent(QEvent* event) override;

private:

    
    /**
     * @brief   键盘按下事件
     * @param   event   键盘事件对象
     * @details 处理键盘按下事件：
     *          - 如果按下回车键，则清除焦点，确认输入
     *          - 其他按键按默认方式处理
     *
     * @note    该函数是QGraphicsTextItem虚函数的重写
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief   输入法事件处理
     * @param   event   输入法事件对象
     * @details 处理输入法事件，防止输入时视角偏移
     */
    void inputMethodEvent(QInputMethodEvent* event) override;
};
