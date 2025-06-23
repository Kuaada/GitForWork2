/**
 * @file    QImageGraphicScene.h
 * @brief   Qt图像图形场景类，管理图像显示和渲染元素的交互
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的图像图形场景管理，包括：
 *          - 图像背景的显示和管理
 *          - 渲染元素的添加、删除和更新
 *          - 像素大小和坐标系统的管理
 *          - 与Qt图形视图框架的集成
 *          - 测量图像的支持
 *          该类是DSV项目中图像显示和交互的核心组件，
 *          为病理图像浏览提供图形场景支持。
 *
 * @note    该类继承自QGraphicsScene，扩展了Qt的图形场景功能
 * @see     RenderElement, PathologyViewer, QGraphicsPixmapItem
 */

#pragma once

#include <QGraphicsScene>
#include <QGraphicsItem>

 /** @brief 像素大小索引常量，用于标识像素大小相关的数据 */
#define PIXELSIZEINDX 0

/**
 * @class  QImageGraphicScene
 * @brief  Qt图像图形场景类，管理图像显示和渲染元素的交互
 * @details 该类是DSV项目中图像显示的核心场景类，负责：
 *          - 管理图像背景的显示
 *          - 处理渲染元素的添加、删除和更新
 *          - 维护像素大小和坐标系统
 *          - 提供与Qt图形视图框架的集成
 *          - 支持测量和标注功能
 *
 *          主要功能包括：
 *          - 图像背景管理：设置和显示图像背景
 *          - 元素管理：添加、删除、更新渲染元素
 *          - 坐标系统：管理像素大小和坐标转换
 *          - 交互支持：提供鼠标和键盘交互的基础
 *          - 测量支持：支持图像测量和标注功能
 *
 * @note   该类继承自QGraphicsScene，支持Qt的图形场景功能
 * @example
 *          // 使用示例
 *          QImage image("pathology_image.tif");
 *          QImageGraphicScene* scene = new QImageGraphicScene(image, parent);
 *
 *          // 设置像素大小
 *          scene->setPixelSize(0.5);
 *
 *          // 添加渲染元素
 *          RenderElement* element = new RectRenderElement("标注1");
 *          scene->addItem(element);
 *
 *          // 获取背景图像项
 *          QGraphicsPixmapItem* pixmapItem = scene->getPixmapItem();
 * @see     RenderElement, PathologyViewer, QGraphicsPixmapItem
 */
class QImageGraphicScene : public QGraphicsScene
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数（无图像）
     * @details 创建空的图像图形场景对象
     *
     * @param   parent 父对象指针
     * @note    该构造函数创建的场景没有背景图像
     * @see     QImageGraphicScene(QImage&, QObject*)
     */
    QImageGraphicScene(QObject* parent);

    /**
     * @brief   构造函数（带图像）
     * @details 使用指定的图像创建图像图形场景对象
     *
     * @param   img 要显示的图像对象
     * @param   parent 父对象指针
     * @note    该构造函数会设置图像作为场景的背景
     * @see     QImageGraphicScene(QObject*)
     */
    QImageGraphicScene(QImage& img, QObject* parent);

    /**
     * @brief   析构函数
     * @details 清理图像图形场景资源
     * @note    析构函数会自动清理所有相关的资源
     */
    ~QImageGraphicScene();

    /**
     * @brief   添加图形项
     * @details 向场景中添加一个图形项（渲染元素）
     *
     * @param   item 要添加的图形项指针
     * @note    该函数会将图形项添加到场景中并管理其生命周期
     * @see     deleteItem, deleteItems
     */
    void addItem(QGraphicsItem* item);

    /**
     * @brief   删除单个图形项
     * @details 从场景中删除指定的图形项
     *
     * @param   item 要删除的图形项指针
     * @note    该函数会从场景中移除图形项并释放相关资源
     * @see     addItem, deleteItems
     */
    void deleteItem(QGraphicsItem* item);

    /**
     * @brief   删除多个图形项
     * @details 从场景中删除指定的图形项列表
     *
     * @param   items 要删除的图形项列表
     * @note    该函数会批量删除图形项，提高删除效率
     * @see     addItem, deleteItem
     */
    void deleteItems(QList<QGraphicsItem*> items);

    /**
     * @brief   获取背景图像项
     * @details 返回场景中的背景图像项指针
     *
     * @return  背景图像项指针
     * @note    背景图像项用于显示图像背景
     * @see     setPixelSize
     */
    QGraphicsPixmapItem* getPixmapItem() { return m_pPixmapItem; }

    /**
     * @brief   设置像素大小
     * @details 设置场景的像素大小，用于坐标转换和测量
     *
     * @param   dPixelSize 像素大小值
     * @note    像素大小用于将像素坐标转换为实际物理坐标
     * @see     getPixmapItem
     */
    void setPixelSize(double dPixelSize);

private:
    /** @brief 测量图像，用于显示和测量功能 */
    QImage m_MeasureImage;

    /** @brief 背景图像项指针，用于显示图像背景 */
    QGraphicsPixmapItem* m_pPixmapItem;

    /** @brief 像素大小，用于坐标转换和测量计算 */
    double m_dPixelSize;
};