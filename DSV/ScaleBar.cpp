/**
 * @file ScaleBar.cpp
 * @brief 比例尺实现文件
 * @details 实现数字病理切片查看器的比例尺显示功能，用于显示图像的实际尺寸比例
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "ScaleBar.h"  
#include <QPainter>
#include <QSizePolicy>
#include <QDebug>

/**
 * @brief 比例尺构造函数
 * @param resolution 图像分辨率（微米/像素）
 * @param parent 父窗口指针
 * @details 初始化比例尺，设置尺寸策略和初始参数
 */
ScaleBar::ScaleBar(float resolution, QWidget* parent)
    : QWidget(parent),
    _resolution(resolution),
    _currentWidth(300)
{
    // 设置尺寸策略
    QSizePolicy policy;
    policy.setHeightForWidth(true);           // 高度根据宽度调整
    policy.setHorizontalPolicy(QSizePolicy::Fixed);  // 水平固定
    policy.setVerticalPolicy(QSizePolicy::Fixed);    // 垂直固定
    setSizePolicy(policy);
}

/**
 * @brief 比例尺析构函数
 * @details 清理资源，释放内存
 */
ScaleBar::~ScaleBar() {
    // 析构函数实现
}

/**
 * @brief 计算比例尺
 * @param pixelsPerScreenPixel 每个屏幕像素对应的图像像素数
 * @param useMicrons 是否使用微米单位
 * @details 根据当前缩放比例计算合适的比例尺长度和标签
 */
void ScaleBar::calculateScale(float pixelsPerScreenPixel, bool useMicrons) {
    unsigned int increment = 1000;  // 初始增量
    
    // 从大到小尝试不同的比例尺长度
    for (int i = 10000; i > 0; i -= increment) {
        if (i <= 1000) {
            increment = 250;  // 调整增量
        }
        if (i <= 250) {
            increment = 25;   // 调整增量
        }
        
        // 如果当前长度在合理范围内（小于300像素）
        if (i / pixelsPerScreenPixel < 300)
        {
            _currentWidth = i / pixelsPerScreenPixel;  // 设置当前宽度
            updateGeometry();  // 更新几何信息
            
            if (useMicrons) {
                emit scaleParametre(QString::number(i));  // 发送比例参数信号
                
                // 根据大小选择合适的单位
                if (i >= 1000) {
                    _currentLabel = QString::number(i / 1000) + " mm";  // 毫米
                }
                else {
                    _currentLabel = QString::number(i) + " um";  // 微米
                }
            }
            else {
                _currentLabel = QString::number(i) + " pixels";  // 像素
            }
            break;
        }
    }
}

/**
 * @brief 根据视场更新比例尺
 * @param fieldOfView 当前视场范围
 * @details 根据当前视场和分辨率计算并更新比例尺显示
 */
void ScaleBar::updateForFieldOfView(const QRectF& fieldOfView) {
    _lastFieldOfView = fieldOfView;  // 保存视场信息
    
    QWidget* parent = this->parentWidget();
    if (parent) {
        float parentWidth = parent->width();           // 父窗口宽度
        float FOVWidth = fieldOfView.width();          // 视场宽度
        float imagePixelsPerScreenPixel = FOVWidth / parentWidth;  // 计算像素比例
        
        if (this->_resolution > 0) {
            // 有分辨率信息，使用微米单位
            float micronsPerScreenPixel = imagePixelsPerScreenPixel * this->_resolution;
            calculateScale(micronsPerScreenPixel, true);
        }
        else {
            // 无分辨率信息，使用像素单位
            calculateScale(imagePixelsPerScreenPixel, false);
        }
    }
}

/**
 * @brief 绘制事件处理
 * @param event 绘制事件对象
 * @details 绘制比例尺的视觉效果
 */
void ScaleBar::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(QPen(QColor("black")));                    // 设置黑色画笔
    painter.setBrush(QColor(255, 255, 255, 80));             // 设置半透明白色填充
    
    // 绘制比例尺矩形
    QRectF bar = QRectF(0, 0, _currentWidth, 20);
    painter.drawRect(bar);
    
    // 绘制比例尺标签
    painter.drawText(bar, Qt::AlignCenter, _currentLabel);
}

/**
 * @brief 获取建议尺寸
 * @return 建议的尺寸
 * @details 返回比例尺的建议尺寸
 */
QSize ScaleBar::sizeHint() const {
    QSize size(_currentWidth + 1, 21);
    return size;
}

/**
 * @brief 根据宽度计算高度
 * @param w 宽度
 * @return 高度
 * @details 固定高度为21像素
 */
int ScaleBar::heightForWidth(int w) const {
    return 21;
}

/**
 * @brief 设置分辨率
 * @param resolution 新的分辨率（微米/像素）
 * @details 更新分辨率并重新计算比例尺
 */
void ScaleBar::setResolution(float resolution) {
    if (_resolution != resolution) {
        _resolution = resolution;
        
        // 如果有有效的视场信息，重新计算比例尺
        if (!_lastFieldOfView.isEmpty()) {
            updateForFieldOfView(_lastFieldOfView);
        }
        update();  // 触发重绘
    }
}