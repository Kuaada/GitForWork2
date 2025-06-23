/**
 * @file    RenderElement.h
 * @brief   渲染元素基类，定义图形渲染对象的基本接口和属性
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的渲染系统基础，包括：
 *          - 渲染元素的基本属性和方法定义
 *          - 几何变换和渲染接口
 *          - 颜色、线宽、透明度等样式管理
 *          - 元素类型和调整方向的枚举定义
 *          - JSON序列化和反序列化支持
 *          - 与图形场景的集成接口
 *          该类是DSV项目中所有图形渲染元素的基类，
 *          为各种几何图形和标注元素提供统一的接口。
 *
 * @note    该类是抽象基类，定义了渲染元素的基本接口
 * @see     LineRenderElement, RectRenderElement, EllipseRenderElement, TextRenderElement
 */

#pragma once

 // 数学常量定义
#define M_PI 3.141592654          // 圆周率π
#define M_PI_4 0.78539816339744831 // π/4

#include <QString>
#include <QPainter>
#include <QImage>
#include <QWidget>
#include <QJsonObject>

/**
 * @class  RenderElement
 * @brief  渲染元素基类，定义图形渲染对象的基本接口和属性
 * @details 该类是DSV项目中所有图形渲染元素的基类，提供了：
 *          - 统一的元素类型定义和枚举
 *          - 基本的几何属性（位置、大小、旋转等）
 *          - 渲染样式属性（颜色、线宽、透明度等）
 *          - 几何计算接口（面积、周长等）
 *          - JSON序列化支持
 *          - 元素调整和变换功能
 *
 *          主要功能包括：
 *          - 元素类型管理：支持矩形、椭圆、多边形、轮廓、文本等多种类型
 *          - 样式管理：颜色、线宽、透明度等渲染属性
 *          - 几何计算：面积、周长等几何属性的计算
 *          - 序列化支持：JSON格式的保存和加载
 *          - 调整功能：支持多种方向的缩放和调整
 *
 * @note   该类是抽象基类，定义了渲染元素的基本接口
 * @example
 *          // 使用示例
 *          RenderElement* element = new RectRenderElement("矩形1");
 *          element->setColor(Qt::red);
 *          element->setLineWidth(2);
 *
 *          // 获取几何属性
 *          float area = element->getArea();
 *          float perimeter = element->getPerimeter();
 *
 *          // 序列化
 *          QJsonObject json = element->toJson();
 * @see     LineRenderElement, RectRenderElement, EllipseRenderElement
 */
class RenderElement
{
public:
    /**
     * @enum   ResizeDirection
     * @brief  元素调整方向枚举，定义元素缩放时的调整方向
     * @details 该枚举定义了元素在交互调整时可能的方向，
     *          用于支持鼠标拖拽调整元素大小的功能。
     */
    enum ResizeDirection {
        None,           // 无调整方向
        RS_TopLeft,     // 左上角调整
        RS_TopRight,    // 右上角调整
        RS_BottomLeft,  // 左下角调整
        RS_BottomRight  // 右下角调整
    };

    /**
     * @enum   ElementType
     * @brief  元素类型枚举，定义所有支持的渲染元素类型
     * @details 该枚举定义了DSV项目中支持的所有图形元素类型，
     *          每种类型都有对应的派生类实现具体的渲染逻辑。
     */
    enum ElementType {
        Undefined = -1, // 未定义类型
        Rectangle = 0,  // 矩形元素
        Ellipse = 1,    // 椭圆元素
        Polygon = 2,    // 多边形元素
        Contour = 3,    // 轮廓元素，由一系列像素点组成
        Text = 4,       // 文本元素
        Comment = 5,    // 标注元素
        Angle = 6,      // 角度元素
        Ruler = 8,      // 标尺元素
        Line = 9,       // 直线元素
        Calibrate = 10, // 定标元素
        Arrow = 11,     // 箭头元素
    };

    /**
     * @brief   构造函数（通过名称）
     * @details 使用指定的名称创建渲染元素对象
     *
     * @param   strName 元素的名称字符串
     * @note    构造函数会初始化基本的元素属性
     * @see     RenderElement(QJsonObject)
     */
    RenderElement(QString strName);

    /**
     * @brief   构造函数（通过JSON对象）
     * @details 使用JSON对象中的数据初始化渲染元素
     *
     * @param   json 包含元素数据的JSON对象
     * @note    该构造函数用于从保存的数据中恢复元素
     * @see     RenderElement(QString), toJson
     */
    RenderElement(QJsonObject json);

    /**
     * @brief   获取元素名称
     * @details 返回元素的当前名称
     *
     * @return  元素名称字符串
     * @note    元素名称用于标识和显示
     * @see     setName
     */
    QString name();

    /**
     * @brief   设置元素名称
     * @details 设置元素的名称
     *
     * @param   strName 新的元素名称
     * @note    元素名称应该具有唯一性和描述性
     * @see     name
     */
    void setName(QString strName);

    /**
     * @brief   设置元素颜色
     * @details 设置元素的渲染颜色
     *
     * @param   color 新的颜色值
     * @note    颜色设置会影响元素的显示效果
     * @see     getColor, setLineWidth
     */
    void setColor(QColor color);

    /**
     * @brief   设置像素大小
     * @details 设置元素的像素大小，用于缩放计算
     *
     * @param   dPixelSize 像素大小值
     * @note    像素大小用于坐标转换和缩放计算
     */
    void setPixelSize(double dPixelSize);

    /**
     * @brief   获取线宽
     * @details 返回当前设置的线宽值
     *
     * @return  线宽值（像素）
     * @note    线宽影响元素的边框和线条粗细
     * @see     setLineWidth, getColor
     */
    int getLineWidth() { return m_pen.width(); }

    /**
     * @brief   获取颜色
     * @details 返回当前设置的颜色值
     *
     * @return  颜色值
     * @note    颜色值用于元素的渲染显示
     * @see     setColor, getLineWidth
     */
    QColor getColor() { return m_pen.color(); }

    /**
     * @brief   设置线宽
     * @details 设置元素的线宽
     *
     * @param   nLineWidth 新的线宽值（像素）
     * @note    线宽设置会影响元素的边框和线条显示
     * @see     getLineWidth, setColor
     */
    void setLineWidth(int nLineWidth);

    /**
     * @brief   获取元素类型
     * @details 返回当前元素的类型
     *
     * @return  元素类型枚举值
     * @note    元素类型用于区分不同的渲染元素
     * @see     ElementType
     */
    ElementType returnType() {
        return m_elementType;
    }

    /**
     * @brief   计算面积（虚函数）
     * @details 计算元素的面积，由派生类实现具体计算逻辑
     *
     * @return  面积值，默认返回0.0
     * @note    该函数是虚函数，派生类应该重写实现具体的面积计算
     * @see     getPerimeter
     */
    virtual float getArea() { return 0.0f; };

    /**
     * @brief   计算周长（虚函数）
     * @details 计算元素的周长，由派生类实现具体计算逻辑
     *
     * @return  周长值，默认返回0.0
     * @note    该函数是虚函数，派生类应该重写实现具体的周长计算
     * @see     getArea
     */
    virtual float getPerimeter() {
        return 0.0f;
    };

    /**
     * @brief   转换为JSON对象（虚函数）
     * @details 将元素数据序列化为JSON格式
     *
     * @return  包含元素数据的JSON对象
     * @note    该函数是虚函数，派生类应该重写实现具体的序列化逻辑
     * @see     RenderElement(QJsonObject)
     */
    virtual QJsonObject toJson();

protected:
    /** @brief 元素类型，标识当前元素的具体类型 */
    ElementType m_elementType;

    /** @brief 元素名称，用于标识和显示 */
    QString m_strName;

    /** @brief 绘制画笔，包含颜色、线宽等样式信息 */
    QPen m_pen;

    /** @brief 像素大小，用于坐标转换和缩放计算 */
    double m_dPixelSize;
};