/**
 * @file    EllipseRenderElement.h
 * @brief   椭圆渲染元素类，实现椭圆的绘制和交互功能
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了DSV项目的椭圆渲染功能，包括：
 *          - 椭圆的绘制和显示
 *          - 椭圆的拖拽和缩放操作
 *          - 椭圆面积和周长计算
 *          - 鼠标事件处理和交互操作
 *          - 控制点管理和视觉反馈
 *          - JSON序列化和反序列化
 *          - 悬停效果和测量信息显示
 *
 * @note    该类继承自QGraphicsEllipseItem和RenderElement，是DSV项目
 *          中常用的渲染元素之一，用于椭圆区域测量、细胞计数等功能
 * @see     RenderElement, QGraphicsEllipseItem, RectRenderElement
 */

#pragma once
//#define M_PI 3.14
#include<QString>
#include<QPainter>
#include<QImage>
#include<QWidget>
#include<QJsonObject>
#include<QGraphicsItem>
#include<QGraphicsEllipseItem>
#include<QGraphicsSceneMouseEvent>
#include"RenderElement.h"
#include<QGraphicsScene>
#include<cmath>
#include"ControlPoint.h"
#include<QGraphicsSimpleTextItem>

/**
 * @brief   椭圆渲染元素类
 * @details 继承自QGraphicsEllipseItem和RenderElement，专门负责椭圆的渲染和交互。
 *          提供椭圆的绘制、拖拽、缩放、面积计算等功能。支持多个控制点，
 *          允许用户通过拖拽控制点来调整椭圆的大小和位置。
 *
 * @note    该类是DSV项目测量工具的核心组件，用于椭圆区域测量、
 *          细胞计数、病灶标注等功能。支持JSON序列化，便于数据保存和恢复。
 *
 * @example
 * @code
 * // 创建椭圆渲染元素
 * EllipseRenderElement* ellipse = new EllipseRenderElement("细胞区域", QRectF(0, 0, 100, 80));
 *
 * // 添加到场景
 * scene->addItem(ellipse);
 *
 * // 获取椭圆面积
 * float area = ellipse->getArea();
 *
 * // 获取椭圆周长
 * float perimeter = ellipse->getPerimeter();
 * @endcode
 */
class EllipseRenderElement:public QObject,public RenderElement,public QGraphicsEllipseItem
{
	Q_OBJECT

public:
	/**
	 * @brief   构造函数（仅名称）
	 * @param   strName    椭圆名称
	 * @param   parent     父图形项指针
	 * @details 创建一个空的椭圆渲染元素，需要后续设置椭圆区域
	 */
	EllipseRenderElement(QString strName, QGraphicsItem *parent=nullptr);
	
	/**
	 * @brief   构造函数（名称和椭圆区域）
	 * @param   strName    椭圆名称
	 * @param   rect       椭圆外接矩形区域
	 * @param   parent     父图形项指针
	 * @details 创建一个指定区域的椭圆渲染元素，自动计算面积和周长
	 */
	EllipseRenderElement(QString strName,const QRectF& rect,QGraphicsItem* parent = nullptr);
	
	/**
	 * @brief   构造函数（JSON数据）
	 * @param   json    JSON对象，包含椭圆的所有属性
	 * @details 通过JSON数据创建椭圆渲染元素，用于数据恢复和加载
	 * 
	 * @note    JSON格式应包含名称、椭圆区域、样式等信息
	 */
	EllipseRenderElement(QJsonObject json);
	
	/**
	 * @brief   更新椭圆区域
	 * @param   rect    新的椭圆外接矩形区域
	 * @details 更新椭圆的区域，重新计算面积、周长和控制点位置
	 */
	void updateRect(QRectF rect);
	
	/**
	 * @brief   析构函数
	 * @details 清理椭圆渲染元素的资源，释放控制点和文本项
	 */
	virtual ~EllipseRenderElement();
	
	/**
	 * @brief   计算椭圆面积
	 * @return  椭圆的像素面积
	 * @details 计算椭圆的面积（π × 长半轴 × 短半轴）
	 * 
	 * @note    该函数是RenderElement虚函数的重写
	 */
	virtual float getArea();
	
	/**
	 * @brief   计算椭圆周长
	 * @return  椭圆的像素周长
	 * @details 计算椭圆的周长，使用近似公式：2π × √((长半轴² + 短半轴²) / 2)
	 * 
	 * @note    该函数是RenderElement虚函数的重写
	 */
	virtual float getPerimeter();
	
	/**
	 * @brief   转换为JSON数据
	 * @return  包含椭圆所有属性的JSON对象
	 * @details 将椭圆渲染元素的所有属性序列化为JSON格式，
	 *          包括名称、椭圆区域、样式等
	 * 
	 * @note    该函数是RenderElement虚函数的重写
	 */
	virtual QJsonObject toJson();
	
	/**
	 * @brief   获取椭圆描述信息
	 * @return  包含椭圆信息的描述字符串
	 * @details 返回椭圆的详细信息，包括名称、位置、大小、面积等
	 */
	QString getDescription();
	
	/**
	 * @brief   鼠标悬停进入事件
	 * @param   event   悬停事件对象
	 * @details 当鼠标悬停在椭圆上时触发，显示控制点和测量信息
	 * 
	 * @note    该函数是QGraphicsItem虚函数的重写
	 */
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	
	/**
	 * @brief   鼠标悬停移动事件
	 * @param   event   悬停事件对象
	 * @details 当鼠标在椭圆上移动时触发，检查是否悬停在控制点上并设置相应光标
	 * 
	 * @note    该函数是QGraphicsItem虚函数的重写
	 */
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
	
	/**
	 * @brief   鼠标悬停离开事件
	 * @param   event   悬停事件对象
	 * @details 当鼠标离开椭圆时触发，隐藏控制点和测量信息
	 * 
	 * @note    该函数是QGraphicsItem虚函数的重写
	 */
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

	/**
	 * @brief   自定义绘制函数
	 * @param   painter     绘制器
	 * @param   option      绘制选项
	 * @param   widget      绘制目标窗口
	 * @details 自定义绘制椭圆，保持线宽不受缩放影响
	 * 
	 * @note    该函数是QGraphicsItem虚函数的重写
	 */
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	/**
	 * @brief   元素变化事件处理
	 * @param   change      变化类型
	 * @param   value       变化值
	 * @details 监听元素变换变化，更新字体大小
	 * 
	 * @note    该函数是QGraphicsItem虚函数的重写
	 */
	QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

protected:
	/**
	 * @brief   鼠标按下事件
	 * @param   event   鼠标事件对象
	 * @details 处理鼠标按下事件，检测是否点击了控制点，
	 *          如果是则开始拖拽或缩放操作
	 * 
	 * @note    该函数是QGraphicsItem虚函数的重写
	 */
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	
	/**
	 * @brief   鼠标移动事件
	 * @param   event   鼠标事件对象
	 * @details 处理鼠标移动事件，如果正在拖拽或缩放，
	 *          则更新椭圆区域和控制点位置
	 * 
	 * @note    该函数是QGraphicsItem虚函数的重写
	 */
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	
	/**
	 * @brief   鼠标释放事件
	 * @param   event   鼠标事件对象
	 * @details 处理鼠标释放事件，结束拖拽或缩放操作，清理状态
	 * 
	 * @note    该函数是QGraphicsItem虚函数的重写
	 */
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	
	/**
	 * @brief   更新控制点位置
	 * @details 根据当前椭圆区域，更新所有控制点的位置
	 */
	void updateControlPoints();
	
	/**
	 * @brief   获取动态字体大小
	 * @return  基于屏幕视窗大小的字体大小
	 * @details 根据当前视图大小计算合适的字体大小
	 */
	int getDynamicFontSize();
	
	/**
	 * @brief   更新文本字体大小
	 * @details 根据当前视图大小更新文本项的字体大小
	 */
	void updateFontSize();
	
	/**
	 * @brief   获取控制点对应的鼠标光标
	 * @param   controlPointIndex    控制点索引
	 * @return  对应的鼠标光标
	 * @details 根据控制点位置返回相应的鼠标光标样式
	 */
	Qt::CursorShape getControlPointCursor(int controlPointIndex);
	
	/** @brief 是否正在调整大小的标志 */
	bool m_isResizing;
	
	/** @brief 上次鼠标位置，用于计算拖拽距离 */
	QPointF m_lastMousePos;
	
	/** @brief 当前拖拽的控制点索引 */
	int m_currentControlPointIndex = -1;
	
	/** @brief 控制点列表，包含椭圆的所有控制点 */
	QList<ControlPoint*> m_controlPoints;
	
	/** @brief 当前选中的控制点指针 */
	ControlPoint* m_selectContrlPoint=nullptr;
	
	/** @brief 控制点的大小（像素） */
	qreal m_fcontrolSize = 10;
	
	/** @brief 文本标签项，显示椭圆面积、周长或名称 */
	QGraphicsSimpleTextItem* m_pTextItem = nullptr;

signals:
	void sendPerimeterAndArea(float Perimeter,float Area);

};
