/**
 * @file    IOThread.h
 * @brief   IO线程类，负责异步瓦片加载和渲染任务的管理
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的异步IO处理系统，包括：
 *          - 线程任务的定义和管理
 *          - 异步瓦片加载任务调度
 *          - 异步瓦片渲染任务处理
 *          - 多线程工作队列管理
 *          - 背景和前景图像的处理
 *          - 线程同步和协调机制
 *          该类是DSV项目中性能优化的关键组件，
 *          通过多线程异步处理实现流畅的图像浏览体验。
 *
 * @note    该类继承自QObject，支持Qt的信号槽机制
 * @see     IOWorker, MultiResolutionImage, TileManager
 */

#pragma once

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include "SlideColorManagement.h"

 // 前向声明
class MultiResolutionImage;
class IOWorker;
class ImageSource;

/**
 * @class  ThreadJob
 * @brief  线程任务基类，定义瓦片处理任务的基本信息
 * @details 该类是所有线程任务的基础类，包含瓦片处理所需的基本参数：
 *          瓦片大小、图像位置坐标、层级信息等。IOJob和RenderJob都继承自此类。
 *
 * @note   该类是线程安全的，可以被多个工作线程访问
 * @see     IOJob, RenderJob
 */
class ThreadJob {
public:
    /** @brief 瓦片大小（像素） */
    unsigned int _tileSize;

    /** @brief 瓦片在图像中的X坐标位置 */
    long long _imgPosX;

    /** @brief 瓦片在图像中的Y坐标位置 */
    long long _imgPosY;

    /** @brief 瓦片所属的层级索引 */
    unsigned int _level;

    /**
     * @brief   构造函数
     * @details 创建线程任务对象，初始化瓦片处理的基本参数
     *
     * @param   tileSize 瓦片大小（像素）
     * @param   imgPosX 瓦片在图像中的X坐标
     * @param   imgPosY 瓦片在图像中的Y坐标
     * @param   level 瓦片所属的层级索引
     * @note    构造函数会初始化所有成员变量
     */
    ThreadJob(unsigned int tileSize, long long imgPosX, long long imgPosY, unsigned int level) :
        _tileSize(tileSize), _imgPosX(imgPosX), _imgPosY(imgPosY), _level(level)
    {
        // 构造函数体为空，所有初始化在初始化列表中完成
    }

    /**
     * @brief   虚析构函数
     * @details 确保正确释放派生类对象的内存
     * @note    虚析构函数允许通过基类指针删除派生类对象
     */
    virtual ~ThreadJob()
    {
        // 析构函数体为空，基类无需特殊清理
    }
};

/**
 * @class  IOJob
 * @brief  IO任务类，用于瓦片数据加载任务
 * @details 该类继承自ThreadJob，专门用于表示瓦片数据加载任务。
 *          工作线程会处理此类任务，从多分辨率图像中加载指定位置的瓦片数据。
 *
 * @note   该类是IOThread系统中最常用的任务类型
 * @see     ThreadJob, RenderJob
 */
class IOJob : public ThreadJob
{
public:
    /**
     * @brief   构造函数
     * @details 创建IO任务对象，调用基类构造函数初始化基本参数
     *
     * @param   tileSize 瓦片大小（像素）
     * @param   imgPosX 瓦片在图像中的X坐标
     * @param   imgPosY 瓦片在图像中的Y坐标
     * @param   level 瓦片所属的层级索引
     * @note    该构造函数直接调用基类构造函数
     */
    IOJob(unsigned int tileSize, long long imgPosX, long long imgPosY, unsigned int level) :
        ThreadJob(tileSize, imgPosX, imgPosY, level)
    {
        // 构造函数体为空，所有初始化在基类构造函数中完成
    }
};

/**
 * @class  RenderJob
 * @brief  渲染任务类，用于瓦片前景渲染任务
 * @details 该类继承自ThreadJob，专门用于表示瓦片前景渲染任务。
 *          工作线程会处理此类任务，将前景瓦片与背景瓦片进行合成渲染。
 *
 * @note   该类包含前景瓦片的图像源信息
 * @see     ThreadJob, IOJob
 */
class RenderJob : public ThreadJob {
public:
    /** @brief 前景瓦片的图像源指针 */
    ImageSource* _foregroundTile;

    /**
     * @brief   构造函数
     * @details 创建渲染任务对象，初始化瓦片处理参数和前景瓦片信息
     *
     * @param   tileSize 瓦片大小（像素）
     * @param   imgPosX 瓦片在图像中的X坐标
     * @param   imgPosY 瓦片在图像中的Y坐标
     * @param   level 瓦片所属的层级索引
     * @param   foregroundTile 前景瓦片的图像源指针
     * @note    该构造函数会初始化所有成员变量，包括前景瓦片信息
     */
    RenderJob(unsigned int tileSize, long long imgPosX, long long imgPosY, unsigned int level, ImageSource* foregroundTile) :
        ThreadJob(tileSize, imgPosX, imgPosY, level),
        _foregroundTile(foregroundTile)
    {
        // 构造函数体为空，所有初始化在初始化列表中完成
    }
};

/**
 * @class  IOThread
 * @brief  IO线程管理器类，负责异步瓦片加载和渲染任务的管理
 * @details 该类是DSV项目中异步IO处理的核心组件，负责：
 *          - 管理多线程工作队列
 *          - 调度瓦片加载和渲染任务
 *          - 协调背景和前景图像的处理
 *          - 提供线程同步和通信机制
 *          - 支持LUT和通道切换的动态更新
 *
 *          主要功能包括：
 *          - 任务队列管理：添加、获取、清空任务
 *          - 工作线程管理：创建、监控、关闭工作线程
 *          - 图像源管理：设置背景和前景图像
 *          - 动态配置：支持LUT和通道的动态切换
 *          - 线程同步：使用互斥锁和条件变量确保线程安全
 *
 * @note   该类继承自QObject，支持Qt的信号槽机制
 * @example
 *          // 使用示例
 *          IOThread* ioThread = new IOThread(parent, 4); // 创建4个工作线程
 *
 *          // 设置背景图像
 *          ioThread->setBackgroundImage(backgroundImage);
 *
 *          // 添加瓦片加载任务
 *          ioThread->addJob(256, 0, 0, 0);
 *
 *          // 添加瓦片渲染任务
 *          ioThread->addJob(256, 0, 0, 0, foregroundTile);
 * @see     IOWorker, MultiResolutionImage, ThreadJob
 */
class IOThread : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建IO线程管理器，初始化工作线程池和任务队列
     *
     * @param   parent 父对象指针
     * @param   nrThreads 工作线程数量，默认为2
     * @note    构造函数会创建指定数量的IOWorker线程
     */
    IOThread(QObject* parent, unsigned int nrThreads = 2);

    /**
     * @brief   析构函数
     * @details 清理IO线程管理器资源，包括停止所有工作线程和清理任务队列
     * @note    析构函数会调用shutdown()确保所有线程正确停止
     */
    ~IOThread();

    /**
     * @brief   添加任务到队列
     * @details 根据参数创建相应的任务对象并添加到任务队列中
     *
     * @param   tileSize 瓦片大小（像素）
     * @param   imgPosX 瓦片在图像中的X坐标
     * @param   imgPosY 瓦片在图像中的Y坐标
     * @param   level 瓦片所属的层级索引
     * @param   foregroundTile 前景瓦片图像源指针，NULL表示IO任务，非NULL表示渲染任务
     * @note    该函数是线程安全的，支持多线程并发调用
     * @see     getJob, clearJobs
     */
    void addJob(const unsigned int tileSize, const long long imgPosX, const long long imgPosY, const unsigned int level, ImageSource* foregroundTile = NULL);

    /**
     * @brief   设置背景图像
     * @details 设置用于瓦片加载的背景多分辨率图像
     *
     * @param   bck_img 背景图像的弱引用指针
     * @note    使用弱引用避免循环引用，图像对象由外部管理
     * @see     setForegroundImage
     */
    void setBackgroundImage(std::weak_ptr<MultiResolutionImage> bck_img);

    /**
     * @brief   设置前景图像
     * @details 设置用于瓦片渲染的前景多分辨率图像
     *
     * @param   for_img 前景图像的弱引用指针
     * @param   scale 前景图像的缩放因子，默认为1.0
     * @note    使用弱引用避免循环引用，图像对象由外部管理
     * @see     setBackgroundImage
     */
    void setForegroundImage(std::weak_ptr<MultiResolutionImage> for_img, float scale = 1.);

    /**
     * @brief   从队列中获取一个任务
     * @details 从任务队列中取出一个待处理的任务，如果队列为空则阻塞等待
     *
     * @return  任务对象指针，如果线程被中止则返回NULL
     * @note    该函数是线程安全的，支持多线程并发调用
     * @see     addJob, clearJobs
     */
    ThreadJob* getJob();

    /**
     * @brief   清空任务队列
     * @details 清空所有待处理的任务，释放任务对象的内存
     * @note    该函数是线程安全的，会等待所有任务处理完成
     * @see     addJob, getJob
     */
    void clearJobs();

    /**
     * @brief   获取队列中的任务数量
     * @details 返回当前任务队列中待处理任务的数量
     *
     * @return  任务数量
     * @note    该函数是线程安全的，返回的是调用时刻的快照
     * @see     getWaitingThreads
     */
    unsigned int numberOfJobs();

    /**
     * @brief   关闭所有工作线程
     * @details 停止所有工作线程，清空任务队列，释放相关资源
     * @note    该函数会等待所有线程正确停止后再返回
     * @see     ~IOThread
     */
    void shutdown();

    /**
     * @brief   获取所有工作线程
     * @details 返回所有工作线程的指针数组
     *
     * @return  工作线程指针数组
     * @note    返回的指针数组可用于监控线程状态
     * @see     getWaitingThreads
     */
    std::vector<IOWorker*> getWorkers();

    /**
     * @brief   获取等待中的线程数量
     * @details 返回当前正在等待任务的工作线程数量
     *
     * @return  等待中的线程数量
     * @note    该函数用于监控线程池的负载情况
     * @see     numberOfJobs, getWorkers
     */
    unsigned int getWaitingThreads();

public slots:
    /**
     * @brief   背景通道改变槽函数
     * @details 当背景图像的显示通道改变时调用此槽函数
     *
     * @param   channel 新的通道索引
     * @note    该槽函数会通知所有工作线程更新背景通道设置
     * @see     onForegroundChannelChanged, onLUTChanged
     */
    void onBackgroundChannelChanged(int channel);

    /**
     * @brief   前景通道改变槽函数
     * @details 当前景图像的显示通道改变时调用此槽函数
     *
     * @param   channel 新的通道索引
     * @note    该槽函数会通知所有工作线程更新前景通道设置
     * @see     onBackgroundChannelChanged, onLUTChanged
     */
    void onForegroundChannelChanged(int channel);

    /**
     * @brief   LUT改变槽函数
     * @details 当颜色查找表（LUT）改变时调用此槽函数
     *
     * @param   LUTname 新的LUT对象
     * @note    该槽函数会通知所有工作线程更新LUT设置
     * @see     onBackgroundChannelChanged, onForegroundChannelChanged
     */
    void onLUTChanged(const SlideColorManagement::LUT& LUTname);

private:
    /** @brief 线程中止标志，true表示需要停止所有线程 */
    bool _abort;

    /** @brief 保护任务队列的互斥锁 */
    QMutex _jobListMutex;

    /** @brief 线程同步条件变量，用于线程间的等待和通知 */
    QWaitCondition _condition;

    /** @brief 背景图像的弱引用指针 */
    std::weak_ptr<MultiResolutionImage> _bck_img;

    /** @brief 前景图像的弱引用指针 */
    std::weak_ptr<MultiResolutionImage> _for_img;

    /** @brief 任务队列，存储待处理的线程任务 */
    std::list<ThreadJob*> _jobList;

    /** @brief 工作线程数组，存储所有IOWorker线程指针 */
    std::vector<IOWorker*> _workers;

    /** @brief 等待中的线程数量计数器 */
    unsigned int _threadsWaiting;
};