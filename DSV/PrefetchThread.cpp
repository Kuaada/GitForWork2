/**
 * @file PrefetchThread.cpp
 * @brief 预取线程实现文件
 * @details 该文件实现了多分辨率图像的预取功能，包括：
 *          - 基于当前视野范围的智能预取
 *          - 多层级数据的预加载
 *          - 8连通邻域的瓦片预取
 *          - 线程安全的预取管理
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "PrefetchThread.h"

#include <QPixmap>
#include <QElapsedTimer>
#include <QDebug>

#include "MultiResolutionImage.h"

/**
 * @brief 构造函数：初始化预取线程
 * @param parent 父对象
 * @details 初始化预取线程的成员变量，包括控制标志、视野范围等
 */
PrefetchThread::PrefetchThread(QObject* parent) :
    QThread(parent),
    _restart(false),
    _abort(false),
    _FOV(QRectF()),
    _level(0),
    _img(NULL)
{
}

/**
 * @brief 析构函数：清理预取线程资源
 * @details 设置中止标志，唤醒等待的线程，并等待线程结束
 */
PrefetchThread::~PrefetchThread()
{
    _mutex.lock();
    _abort = true;
    _mutex.unlock();
    while (isRunning()) {
        _condition.wakeOne();
    }

    wait();
}

/**
 * @brief 视野范围改变回调
 * @param img 多分辨率图像对象
 * @param FOV 新的视野范围
 * @param level 图像层级
 * @details 更新预取参数并启动或重启预取线程
 */
void PrefetchThread::FOVChanged(MultiResolutionImage* img, const QRectF& FOV, const unsigned int level)
{
    QMutexLocker locker(&_mutex);

    _img = img;
    _level = level;
    _FOV = FOV;

    if (!isRunning()) {
        start(HighPriority);
    }
    else {
        _restart = true;
        _condition.wakeOne();
    }
}

/**
 * @brief 预取线程主循环
 * @details 基于当前视野范围和层级进行智能预取：
 *          - 缓存当前层级上下各一级的数据
 *          - 在8连通邻域内预取相同大小的瓦片
 *          - 考虑降采样比例进行坐标转换
 */
void PrefetchThread::run()
{
    forever{
      if (_img) {
        _mutex.lock();
        QRectF FOV = _FOV;
        int level = _level;
        MultiResolutionImage* img = _img;
        std::vector<unsigned long long> L0Dims = img->getDimensions();
        QRectF maxFOV(0, 0, L0Dims[0], L0Dims[1]);
        int levelDownsample = img->getLevelDownsample(level);
        _mutex.unlock();

        if (img) {
          if (_abort) {
            return;
          }
          // 缓存下一级（更高分辨率）
          if (level - 1 >= 0 && !_restart) {
            unsigned int width = FOV.width() / img->getLevelDownsample(level - 1);
            unsigned int height = FOV.height() / img->getLevelDownsample(level - 1);
            unsigned char* data = NULL;
            data = new unsigned char[width * height * img->getSamplesPerPixel()];
            img->getRawRegion(FOV.left(), FOV.top(), width, height, level - 1, data);
            delete[] data;
          }
          // 在当前视野范围周围进行8连通预取
          for (int x = -1; x < 2; ++x) {
            if (_restart) {
              break;
            }
            for (int y = -1; y < 2; ++y) {
              if (_restart) {
                break;
              }
              if (x == 0 && y == 0) {
                continue; // 跳过中心位置（当前视野范围）
              }
              QRectF cur(FOV.left() + x * FOV.width(), FOV.top() + y * FOV.height(), FOV.width(), FOV.height());
              cur = cur.intersected(maxFOV); // 确保不超出图像边界
              unsigned int width = cur.width() / levelDownsample;
              unsigned int height = cur.height() / levelDownsample;
              unsigned char* data = new unsigned char[width * height * img->getSamplesPerPixel()];
              img->getRawRegion(cur.left(), cur.top(), width, height, level, data);
              delete[] data;
            }
          }
        }

        _mutex.lock();
        if (!_restart) {
          qDebug() << "Prefetching finished!";
          _condition.wait(&_mutex); // 等待新的视野范围变化
        }
        _restart = false;
        _mutex.unlock();
        if (_abort) {
          return;
        }
      }
    }
}