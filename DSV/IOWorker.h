/**
 * @file    IOWorker.h
 * @brief   IO工作线程类，负责具体的瓦片加载和渲染任务执行
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的具体IO处理工作线程，包括：
 *          - 瓦片数据的实际加载和缓存
 *          - 瓦片渲染的具体实现和合成
 *          - 背景和前景图像的处理
 *          - 颜色通道和LUT的应用
 *          - 与IOThread的协调和通信
 *          - 模板化的图像处理算法
 *          该类是DSV项目中性能优化的执行单元，
 *          通过多线程并行处理实现高效的图像浏览。
 *
 * @note    该类继承自QThread，运行在独立的工作线程中
 * @see     IOThread, ThreadJob, MultiResolutionImage, Patch
 */

#pragma once

#include <QThread>
#include <QMutex>
#include <QPixmap>
#include "SlideColorManagement.h"
#include "Patch.h"

 // 前向声明
class MultiResolutionImage;
class IOJob;
class RenderJob;
class IOThread;

/**
 * @class  IOWorker
 * @brief  IO工作线程类，负责具体的瓦片加载和渲染任务执行
 * @details 该类是DSV项目中异步IO处理的具体执行单元，负责：
 *          - 从任务队列中获取并执行瓦片处理任务
 *          - 从多分辨率图像中加载指定位置的瓦片数据
 *          - 将瓦片数据渲染为QPixmap格式
 *          - 处理背景和前景图像的合成渲染
 *          - 应用颜色通道和LUT设置
 *          - 通过信号机制与主线程通信
 *
 *          主要功能包括：
 *          - 任务执行：处理IOJob和RenderJob两种任务类型
 *          - 图像加载：从多分辨率图像中提取瓦片数据
 *          - 图像渲染：将原始数据转换为显示格式
 *          - 图像合成：背景和前景图像的叠加处理
 *          - 动态配置：支持通道和LUT的动态切换
 *          - 线程安全：使用互斥锁保护共享资源
 *
 * @note   该类继承自QThread，运行在独立的工作线程中
 * @example
 *          // 使用示例
 *          IOThread* ioThread = new IOThread(parent, 4);
 *          IOWorker* worker = new IOWorker(ioThread);
 *
 *          // 设置图像源
 *          worker->setBackgroundImage(backgroundImage);
 *          worker->setForegroundImage(foregroundImage, 0.5);
 *
 *          // 设置显示参数
 *          worker->setBackgroundChannel(0);
 *          worker->setLUT(SlideColorManagement::LUT::HOT);
 *
 *          // 启动工作线程
 *          worker->start();
 * @see     IOThread, ThreadJob, MultiResolutionImage, Patch
 */
class IOWorker : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief   构造函数
     * @details 创建IO工作线程对象，初始化线程参数和资源
     *
     * @param   thread IO线程管理器指针，用于获取任务和配置
     * @note    构造函数会初始化所有成员变量，但不会启动线程
     * @see     ~IOWorker, start
     */
    IOWorker(IOThread* thread);

    /**
     * @brief   析构函数
     * @details 清理IO工作线程资源，确保线程正确停止
     * @note    析构函数会调用abort()确保线程安全停止
     * @see     abort
     */
    ~IOWorker();

    /**
     * @brief   中止线程执行
     * @details 设置中止标志，通知线程停止处理任务
     * @note    该函数会等待线程完成当前任务后停止
     * @see     ~IOWorker
     */
    void abort();

    /**
     * @brief   设置背景图像通道
     * @details 设置背景图像的显示通道索引
     *
     * @param   channel 通道索引
     * @note    该设置会影响后续的瓦片渲染
     * @see     setForegroundChannel, setLUT
     */
    void setBackgroundChannel(int channel);

    /**
     * @brief   设置前景图像通道
     * @details 设置前景图像的显示通道索引
     *
     * @param   channel 通道索引
     * @note    该设置会影响后续的瓦片渲染
     * @see     setBackgroundChannel, setLUT
     */
    void setForegroundChannel(int channel);

    /**
     * @brief   设置颜色查找表（LUT）
     * @details 设置用于图像颜色映射的查找表
     *
     * @param   LUTname LUT对象，定义颜色映射规则
     * @note    该设置会影响后续的瓦片渲染颜色
     * @see     setBackgroundChannel, setForegroundChannel
     */
    void setLUT(const SlideColorManagement::LUT& LUTname);

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
     * @note    前景图像只能与背景图像相同大小或更小
     * @see     setBackgroundImage
     */
    void setForegroundImage(std::weak_ptr<MultiResolutionImage> for_img, float scale = 1.);

signals:
    /**
     * @brief   瓦片加载完成信号
     * @details 当瓦片数据加载完成时发出此信号
     *
     * @param   tile 加载完成的瓦片像素图
     * @param   tileX 瓦片X坐标
     * @param   tileY 瓦片Y坐标
     * @param   tileSize 瓦片大小
     * @param   tileByteSize 瓦片字节大小
     * @param   tileLevel 瓦片层级
     * @param   foregroundTile 前景瓦片图像源指针，默认为NULL
     * @param   foregroundPixmap 前景瓦片像素图，默认为NULL
     * @note    该信号由主线程接收，用于更新UI显示
     * @see     foregroundTileRendered
     */
    void tileLoaded(QPixmap* tile, unsigned int tileX, unsigned int tileY, unsigned int tileSize, unsigned int tileByteSize, unsigned int tileLevel, ImageSource* foregroundTile = NULL, QPixmap* foregroundPixmap = NULL);

    /**
     * @brief   前景瓦片渲染完成信号
     * @details 当前景瓦片渲染完成时发出此信号
     *
     * @param   tile 渲染完成的前景瓦片像素图
     * @param   tileX 瓦片X坐标
     * @param   tileY 瓦片Y坐标
     * @param   tileLevel 瓦片层级
     * @note    该信号由主线程接收，用于更新前景显示
     * @see     tileLoaded
     */
    void foregroundTileRendered(QPixmap* tile, unsigned int tileX, unsigned int tileY, unsigned int tileLevel);

protected:
    /**
     * @brief   线程主执行函数
     * @details 线程启动后执行的主循环，负责从任务队列获取任务并执行
     * @note    该函数是QThread的虚函数，由start()调用
     * @see     abort, executeIOJob, executeRenderJob
     */
    void run();

private:
    /** @brief 保护共享资源的互斥锁 */
    QMutex mutex;

    /** @brief 背景图像的弱引用指针 */
    std::weak_ptr<MultiResolutionImage> _bck_img;

    /** @brief 前景图像的弱引用指针 */
    std::weak_ptr<MultiResolutionImage> _for_img;

    /** @brief 线程中止标志，true表示需要停止线程 */
    bool _abort;

    /** @brief 背景图像当前显示的通道索引 */
    int _backgroundChannel;

    /** @brief 前景图像当前显示的通道索引 */
    int _foregroundChannel;

    /**
     * @brief 前景图像相对于背景图像的缩放因子
     * @details 前景图像只能与背景图像相同大小或更小，因此该值范围为1到正无穷
     * @note    该值用于计算前景瓦片在背景瓦片中的位置和大小
     */
    float _foregroundImageScale;

    /** @brief 当前使用的颜色查找表（LUT） */
    SlideColorManagement::LUT _LUT;

    /**
     * @brief   执行IO任务
     * @details 处理瓦片数据加载任务，从多分辨率图像中提取瓦片数据
     *
     * @param   job IO任务对象指针
     * @return  true表示任务执行成功，false表示失败
     * @note    该函数会发出tileLoaded信号通知主线程
     * @see     executeRenderJob, tileLoaded
     */
    bool executeIOJob(IOJob* job);

    /**
     * @brief   执行渲染任务
     * @details 处理瓦片渲染任务，将前景瓦片与背景瓦片进行合成
     *
     * @param   job 渲染任务对象指针
     * @return  true表示任务执行成功，false表示失败
     * @note    该函数会发出foregroundTileRendered信号通知主线程
     * @see     executeIOJob, foregroundTileRendered
     */
    bool executeRenderJob(RenderJob* job);

    /**
     * @brief   渲染背景图像瓦片
     * @details 将多分辨率图像中的瓦片数据渲染为QPixmap格式
     *
     * @tparam  T 图像数据类型（如unsigned char, unsigned short等）
     * @param   local_bck_img 背景图像对象的共享指针
     * @param   currentJob 当前IO任务对象指针
     * @param   colorType 颜色类型，决定渲染方式
     * @return  渲染完成的瓦片像素图指针
     * @note    该函数是模板函数，支持不同的图像数据类型
     * @see     getForegroundTile, renderForegroundImage
     */
    template <typename T>
    QPixmap* renderBackgroundImage(std::shared_ptr<MultiResolutionImage> local_bck_img, const IOJob* currentJob, SlideColorManagement::ColorType colorType);

    /**
     * @brief   获取前景瓦片数据
     * @details 从前景多分辨率图像中提取指定位置的瓦片数据
     *
     * @tparam  T 图像数据类型（如unsigned char, unsigned short等）
     * @param   local_for_img 前景图像对象的共享指针
     * @param   currentJob 当前IO任务对象指针
     * @return  前景瓦片数据补丁对象指针
     * @note    该函数是模板函数，支持不同的图像数据类型
     * @see     renderBackgroundImage, renderForegroundImage
     */
    template<typename T>
    Patch<T>* getForegroundTile(std::shared_ptr<MultiResolutionImage> local_for_img, const IOJob* currentJob);

    /**
     * @brief   渲染前景图像瓦片
     * @details 将前景瓦片数据渲染为QPixmap格式
     *
     * @tparam  T 图像数据类型（如unsigned char, unsigned short等）
     * @param   foregroundTile 前景瓦片数据补丁对象指针
     * @param   backgroundTileSize 背景瓦片大小，用于缩放前景瓦片
     * @return  渲染完成的前景瓦片像素图指针
     * @note    该函数是模板函数，支持不同的图像数据类型
     * @see     renderBackgroundImage, getForegroundTile
     */
    template<typename T>
    QPixmap* renderForegroundImage(Patch<T>* foregroundTile, unsigned int backgroundTileSize);
};