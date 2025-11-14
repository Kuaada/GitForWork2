#include "SnapWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QFileDialog>


SnapWidget::SnapWidget(QWidget* parent) : QWidget(parent) {}

void SnapWidget::setPixmap(const QPixmap& pix) {
    m_pixmap = pix;
    update(); // 触发重绘
}

void SnapWidget::saveTIFFSnap()
{
    QString filePath = QFileDialog::getSaveFileName(this,
        QStringLiteral("保存图像"),
        "",//路径
        QStringLiteral("Images (*tiff)"));

    if (!filePath.isEmpty()) {
        // 保存QPixmap到指定路径
        if (m_pixmap.save(filePath)) {
            // 保存成功
            qDebug() << "Pixmap saved successfully to:" << filePath;
        }
        else {
            // 保存失败
            qDebug() << "Failed to save pixmap to:" << filePath;
        }
    }
}

void SnapWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    if (!m_pixmap.isNull()) {
        // 绘制 QPixmap
        painter.drawPixmap(0, 0, m_pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

}

void SnapWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit widgetUpdate();
    }
   
    QWidget::mousePressEvent(event);
}

void SnapWidget::saveSnap()
{
    QString filePath = QFileDialog::getSaveFileName(this,
        QStringLiteral("保存图像"),
        "",//路径
        QStringLiteral("Images (*.png *.jpg *.bmp)"));

    if (!filePath.isEmpty()) {
        // 保存QPixmap到指定路径
        if (m_pixmap.save(filePath)) {
            // 保存成功
            qDebug() << "Pixmap saved successfully to:" << filePath;
        }
        else {
            // 保存失败
            qDebug() << "Failed to save pixmap to:" << filePath;
        }
    }


}