/**
 * @file QImageGraphicScene.cpp
 * @brief QImage图形场景实现文件
 * @details 该文件实现了基于QImage的图形场景功能，包括：
 *          - 图像显示和场景管理
 *          - 图形项的添加和删除
 *          - 像素尺寸管理
 *          - 场景矩形设置
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "QImageGraphicScene.h"

/**
 * @brief 构造函数：初始化图形场景
 * @param parent 父对象
 * @details 创建空的图形场景，设置默认像素尺寸为1.0
 */
QImageGraphicScene::QImageGraphicScene(QObject *parent)
	: QGraphicsScene(parent)
{
	m_dPixelSize = 1.0;
}

/**
 * @brief 构造函数：通过图像初始化图形场景
 * @param img 要显示的图像
 * @param parent 父对象
 * @details 创建包含指定图像的图形场景，添加像素图项并设置场景矩形
 */
QImageGraphicScene::QImageGraphicScene(QImage& img,QObject* parent)
	: QGraphicsScene(parent)
{
	m_MeasureImage = img;
	m_pPixmapItem=addPixmap(QPixmap::fromImage(img));
	setSceneRect(0, 0, img.width(), img.height());
}

/**
 * @brief 析构函数：清理图形场景资源
 * @details Qt会自动清理场景中的图形项
 */
QImageGraphicScene::~QImageGraphicScene()
{
}

/**
 * @brief 添加图形项
 * @param item 要添加的图形项
 * @details 为图形项设置像素尺寸数据，然后添加到场景中
 */
void QImageGraphicScene::addItem(QGraphicsItem* item)
{
	item->setData(PIXELSIZEINDX, m_dPixelSize);
	QGraphicsScene::addItem(item);
}

/**
 * @brief 删除单个图形项
 * @param item 要删除的图形项
 * @details 从场景中移除指定的图形项
 */
void QImageGraphicScene::deleteItem(QGraphicsItem* item)
{
	removeItem(item);
}

/**
 * @brief 删除多个图形项
 * @param items 要删除的图形项列表
 * @details 遍历图形项列表，逐个从场景中移除
 */
void QImageGraphicScene::deleteItems(QList<QGraphicsItem*> items)
{
	for (int i=0;i<items.size();i++ )
	{
		removeItem(items.at(i));
	}
}

/**
 * @brief 设置像素尺寸
 * @param dPixelSize 像素尺寸值
 * @details 设置场景的像素尺寸，用于后续添加的图形项
 */
void QImageGraphicScene::setPixelSize(double dPixelSize)
{
	m_dPixelSize = dPixelSize;
}