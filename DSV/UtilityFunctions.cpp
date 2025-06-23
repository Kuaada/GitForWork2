/**
 * @file UtilityFunctions.cpp
 * @brief 工具函数和常量定义实现文件
 * @details 实现数字病理切片查看器中使用的工具函数和字符串常量
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "UtilityFunctions.h"

// 字符串常量定义
namespace StringConstants {
    // 工具栏相关
    const QString SLICE_INFO = QString("切片信息");
    const QString ROTATE_IMAGE = QString("旋转图像");
    const QString EFFECT_ADJUST = QString("效果调整");
    const QString IMAGE_PROCESS = QString("图像处理");
    const QString LABEL_IMAGE = QString("标签图");
    const QString MINI_MAP = QString("小地图");
    const QString SCALE_BAR = QString("比例尺");
    const QString ANNOTATION = QString("标注");
    const QString SCREENSHOT = QString("截图");
    const QString RESET_IMAGE = QString("重置图像");
    const QString ADD_MORE = QString("添加更多");
    
    // 缩放相关
    const QString X2_ZOOM = QString("X2缩放");
    const QString X5_ZOOM = QString("X5缩放");
    const QString X10_ZOOM = QString("X10缩放");
    const QString X20_ZOOM = QString("X20缩放");
    const QString X40_ZOOM = QString("X40缩放");
    
    // 文件相关
    const QString FILE_TREE = QString("文件树");
    const QString UNSUPPORTED_FORMAT = QString("不支持的文件格式");
    const QString INVALID_FILE = QString("无效的文件");
    const QString FILES = QString("文   件");
    const QString RECENT = QString("最近");
    
    // 对话框相关
    const QString IMAGE_ITEM_LIST = QString("图像项目列表");
    const QString CONTENT = QString("内容");
    const QString FONT = QString("字体");
    const QString LINE_WIDTH = QString("线宽");
    const QString DELETE = QString("删除");
    const QString SAVE = QString("保存");
    const QString LABEL_PIC = QString("标签图片");
    
    // 测量相关
    const QString AREA_PERIMETER = QString("面积:%1 um^2 周长:%2 um");
    const QString AREA_PERIMETER_NEWLINE = QString("面积:%1 um^2\r\n周长:%2 um");
    const QString LENGTH = QString("长度:%2");
    const QString DOUBLE_CLICK_INPUT = QString("双击输入内容");
    
    // 状态相关
    const QString CURRENT_POSITION = QString("当前位置: (");
    const QString DELETE_SELECTION = QString("删除选择");
}

// 工具函数实现
namespace UtilityFunctions {
    QString formatAreaPerimeter(double area, double perimeter) {
        return StringConstants::AREA_PERIMETER.arg(area, 0, 'f', 1).arg(perimeter, 0, 'f', 1);
    }
    
    QString formatLength(double length) {
        return StringConstants::LENGTH.arg(length, 0, 'g', 4);
    }
    
    QString formatPosition(double x, double y) {
        return StringConstants::CURRENT_POSITION + QString::number(x) + QString(", ") + QString::number(y) + QString(")");
    }
} 