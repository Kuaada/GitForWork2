/**
 * @file RenderElement.cpp
 * @brief 渲染元素基类实现文件
 * @details 该文件实现了渲染元素基类的功能，包括：
 *          - 渲染元素的基本属性管理
 *          - JSON序列化和反序列化
 *          - 颜色和线宽设置
 *          - 像素尺寸管理
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "RenderElement.h"

/**
 * @brief 构造函数：通过名称初始化渲染元素
 * @param strName 元素名称
 * @details 初始化渲染元素的基本属性，设置默认的黑色画笔和线宽
 */
RenderElement::RenderElement(QString strName)
{
	m_elementType = Undefined;
	m_strName = strName;
	m_pen.setColor(Qt::black);
	m_pen.setWidth(2);
}

/**
 * @brief 构造函数：通过JSON对象初始化渲染元素
 * @param json JSON对象
 * @details 从JSON对象中解析渲染元素的属性，包括类型、颜色、线宽、名称和像素尺寸
 */
RenderElement::RenderElement(QJsonObject json)
{
	QJsonObject base = json["base"].toObject();
	m_elementType = (ElementType)base["type"].toInt();
	int red= base["r"].toInt();
	int green = base["g"].toInt();
	int blue = base["b"].toInt();
	m_pen.setColor(QColor(red, green, blue));
	int nLineWidth = base["width"].toInt();
	m_pen.setWidth(nLineWidth);
	m_strName = base["name"].toString();
	m_dPixelSize = base["pixelSize"].toDouble();
}

/**
 * @brief 设置像素尺寸
 * @param dPixelSize 像素尺寸值
 * @details 设置渲染元素的像素尺寸，用于缩放计算
 */
void RenderElement::setPixelSize(double dPixelSize)
{
	m_dPixelSize = dPixelSize;
}

/**
 * @brief 获取元素名称
 * @return 元素名称字符串
 */
QString RenderElement::name() 
{
	return m_strName; 
}

/**
 * @brief 设置元素名称
 * @param strName 新的元素名称
 */
void RenderElement::setName(QString strName)
{ 
	m_strName = strName;
}

/*
 * @brief 转换为JSON对象
 * @return JSON对象
 * @details 将渲染元素的属性序列化为JSON格式，包括类型、颜色、线宽、名称和像素尺寸
 */
QJsonObject RenderElement::toJson()
{
	QColor color = m_pen.color();
	return QJsonObject{
		{"type",m_elementType},
		{"r",color.red()},
		{"g",color.green()},
		{"b",color.blue()},
		{"lineWidth",m_pen.width()},
		{"name",m_strName},
		{"pixelSize",m_dPixelSize}
	};
}

/**
 * @brief 设置颜色
 * @param color 新的颜色
 * @details 设置渲染元素的画笔颜色
 */
void RenderElement::setColor(QColor color) 
{ 
	m_pen.setColor(color);
}

/**
 * @brief 设置线宽
 * @param nLineWidth 新的线宽
 * @details 设置渲染元素的画笔线宽
 */
void RenderElement::setLineWidth(int nLineWidth) 
{
	m_pen.setWidth(nLineWidth);
}

/**
 * @brief 智能单位转换工具函数
 * @param value 原始值（微米）
 * @param isArea 是否为面积值（决定使用平方单位）
 * @return 包含转换后值和单位的字符串
 * @details 根据数值大小自动选择合适的单位：
 *          - 小于1000 μm: 使用 μm
 *          - 1000-999999 μm: 使用 mm
 *          - 大于等于1000000 μm: 使用 cm
 *          面积值会自动使用对应的平方单位
 */
QString RenderElement::formatMeasurement(float value, bool isArea)
{
    QString unit;
    float convertedValue;
    int precision;
    
    if (value < 1000.0f) {
        // 小于1000，使用微米
        convertedValue = value;
        unit = isArea ? "um2" : "um";  // 使用更兼容的字符
        precision = (value < 10.0f) ? 2 : 1; // 小数值使用更多精度
    } else if (value < 1000000.0f) {
        // 1000-999999，使用毫米
        convertedValue = value / 1000.0f;
        unit = isArea ? "mm2" : "mm";  // 使用更兼容的字符
        precision = (convertedValue < 10.0f) ? 2 : 1;
    } else {
        // 大于等于1000000，使用厘米
        convertedValue = value / 10000.0f;
        unit = isArea ? "cm2" : "cm";  // 使用更兼容的字符
        precision = (convertedValue < 10.0f) ? 2 : 1;
    }
    
    // 格式化数值，避免科学计数法
    QString valueStr = QString::number(convertedValue, 'f', precision);
    
    // 移除末尾的0（如果小数点后全是0，则移除小数点）
    if (valueStr.contains('.')) {
        while (valueStr.endsWith('0') && valueStr.contains('.')) {
            valueStr.chop(1);
        }
        if (valueStr.endsWith('.')) {
            valueStr.chop(1);
        }
    }
    
    return QString("%1 %2").arg(valueStr).arg(unit);
}
