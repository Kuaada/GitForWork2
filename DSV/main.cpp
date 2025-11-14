/**
 * @file main.cpp
 * @brief DSV数字病理切片查看器主程序入口
 * @details 该文件是DSV应用程序的主入口点，负责：
 *          - 初始化Qt应用程序
 *          - 创建并显示主窗口
 *          - 启动应用程序事件循环
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include <QApplication>
#include "MainWin.h"

/**
 * @brief 主函数：应用程序入口点
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 应用程序退出码
 * @details 创建Qt应用程序实例，初始化主窗口并启动事件循环
 */
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    w.showMaximized();
    return a.exec();
}
