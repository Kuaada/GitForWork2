#pragma once

#include <QProxyStyle>
#include <QStyleOptionTab>
#include <QStylePainter>
#include <QPainter>
#include <QTabBar>
#include <QTabWidget>

class TabStyle : public QProxyStyle {
    Qt::Orientation m_orientation; // 新增成员变量用于存储方向
public:
    // 构造函数，接收一个QStyle指针和方向作为参数
    TabStyle(QStyle* style = nullptr, Qt::Orientation orientation = Qt::Horizontal)
        : QProxyStyle(style), m_orientation(orientation) {
    }

    void drawControl(QStyle::ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override {
        QProxyStyle::drawControl(element, option, painter, widget);

        if (element == QStyle::CE_TabBarTabLabel) {
            if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(option)) {
                QRect controlRect = tab->rect;
                // 确保绘制区域不会超出 tab 范围
                painter->setClipRect(controlRect);

                QString tabText;
                if (m_orientation == Qt::Vertical) {
                    for (int i = 0; i < tab->text.length(); i++) {
                        tabText.append(tab->text.at(i));
                        tabText.append('\n');
                    }
                    if (tabText.length() > 1)
                        tabText = tabText.mid(0, tabText.length() - 1);
                }
                else {
                    tabText = tab->text;
                }

                QTextOption textOption;
                textOption.setAlignment(Qt::AlignCenter);
                QPen pen = painter->pen();
                pen.setColor(tab->palette.color(QPalette::WindowText));
                painter->setPen(pen);
                painter->drawText(controlRect, tabText, textOption);

                painter->setClipping(false); // 恢复裁剪状态
            }
        }
    }
    // 重写drawItemText函数，函数体为空
    void drawItemText(QPainter* painter, const QRect& rect, int flags, const QPalette& pal, bool enabled, const QString& text, QPalette::ColorRole textRole = QPalette::NoRole) const override {
        // 不进行文本绘制，将绘制工作放到drawControl中
      
    }

    // 重写sizeFromContents函数，用于计算控件内容所需的大小
    QSize sizeFromContents(QStyle::ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget = nullptr) const override {
        QSize size = contentsSize;
        if (type == QStyle::CT_TabBarTab) {
            if (m_orientation == Qt::Vertical) {
                // 更小的调整幅度
                size.rwidth() += 2;
                size.rheight() += 5;
            }
            else {
                size.transpose();
                size.rwidth() += 2;
                size.rheight() += 5;
            }
        }
        return size;
    }
};