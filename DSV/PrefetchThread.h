/**
 * @file    PrefetchThread.h
 * @brief   预取线程类，负责异步预加载图像瓦片
 * @author  [JianZhang]
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该类实现了DSV项目的图像瓦片预取功能，包括：
 *          - 异步预加载视场范围内的图像瓦片
 *          - 多分辨率级别的瓦片预取
 *          - 线程安全的视场变化处理
 *          - 智能的预取策略和缓存管理
 *          - 与主线程的协调机制
 *
 * @note    该类是DSV项目性能优化的关键组件，通过预取机制
 *          减少用户等待时间，提升图像浏览体验
 * @see     MultiResolutionImage, TileManager, IOThread
 */

#pragma once
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QRect>

class MultiResolutionImage;

/**
 * @brief   预取线程类
 * @details 继承自QThread，专门负责在后台预加载用户可能需要的图像瓦片。
 *          当用户改变视场（FOV）或缩放级别时，该线程会智能地预取
 *          周围区域的瓦片，以提供流畅的浏览体验。
 *
 * @note    该线程采用生产者-消费者模式，通过信号槽机制接收
 *          视场变化通知，并在后台异步执行预取任务
 *
 * @example
 * @code
 * // 创建预取线程
 * PrefetchThread* prefetchThread = new PrefetchThread(this);
 * prefetchThread->start();
 *
 * // 连接视场变化信号
 * connect(viewer, &PathologyViewer::FOVChanged,
 *         prefetchThread, &PrefetchThread::FOVChanged);
 * @endcode
 */
class PrefetchThread : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @param   parent  父对象指针，默认为nullptr
     * @details 初始化预取线程，设置线程状态和同步机制
     */
    PrefetchThread(QObject* parent = 0);

    /**
     * @brief   析构函数
     * @details 安全地停止线程并清理资源，确保线程正确退出
     */
    ~PrefetchThread();

public slots:
    /**
     * @brief   视场变化处理槽函数
     * @param   img     多分辨率图像对象指针
     * @param   FOV     新的视场矩形（浮点坐标）
     * @param   level   目标分辨率级别
     * @details 当用户改变视场或缩放级别时，该槽函数会被调用。
     *          线程会根据新的视场信息，在后台预取相关的图像瓦片。
     *          支持多分辨率级别的智能预取策略。
     *
     * @note    该函数是线程安全的，使用互斥锁保护共享数据
     * @see     MultiResolutionImage::getTilesInRect
     */
    void FOVChanged(MultiResolutionImage* img, const QRectF& FOV, const unsigned int level);

protected:
    /**
     * @brief   线程运行函数
     * @details 线程的主要执行循环，负责：
     *          - 等待视场变化通知
     *          - 执行瓦片预取任务
     *          - 处理线程暂停和恢复
     *          - 管理预取队列和优先级
     *
     * @note    该函数在独立线程中运行，不应直接调用
     */
    void run();

private:
    /** @brief 线程重启标志，用于重新启动预取任务 */
    bool _restart;

    /** @brief 线程中止标志，用于安全停止线程 */
    bool _abort;

    /** @brief 互斥锁，保护共享数据的线程安全 */
    QMutex _mutex;

    /** @brief 等待条件，用于线程同步和唤醒 */
    QWaitCondition _condition;

    /** @brief 当前视场矩形，记录用户查看的区域 */
    QRectF _FOV;

    /** @brief 当前分辨率级别，记录用户选择的缩放级别 */
    unsigned int _level;

    /** @brief 多分辨率图像对象指针，用于访问图像数据和瓦片 */
    MultiResolutionImage* _img;
};