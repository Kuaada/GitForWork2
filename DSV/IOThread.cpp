/**
 * @file IOThread.cpp
 * @brief IO线程管理器实现文件
 * @details 该文件实现了多线程IO任务管理功能，包括：
 *          - 多线程工作池管理
 *          - 任务队列管理
 *          - 背景和前景图像设置
 *          - 颜色查找表管理
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "IOThread.h"
#include "MultiResolutionImage.h"
#include "IOWorker.h"

/**
 * @brief 构造函数：初始化IO线程管理器
 * @param parent 父对象
 * @param nrThreads 工作线程数量
 * @details 创建指定数量的IOWorker线程，并设置为高优先级
 */
IOThread::IOThread(QObject *parent, unsigned int nrThreads)
	: QObject(parent),
	_abort(false),
	_threadsWaiting(0)
{
	for (int i = 0; i < nrThreads; ++i) {
		IOWorker* worker = new IOWorker(this);
		worker->start(QThread::HighPriority);
		_workers.push_back(worker);
	}
}

/**
 * @brief 获取当前任务数量
 * @return 任务队列中的任务数量
 * @details 线程安全地获取任务列表的大小
 */
unsigned int IOThread::numberOfJobs()
{
	_jobListMutex.lock();
	unsigned int nrJobs = _jobList.size();
	_jobListMutex.unlock();
	return nrJobs;
}

/**
 * @brief 关闭IO线程管理器
 * @details 设置中止标志，等待所有工作线程完成并清理资源
 */
void IOThread::shutdown() {
	_abort = true;
	for (std::vector<IOWorker*>::iterator it = _workers.begin(); it != _workers.end(); ++it) {
		(*it)->abort();
		while ((*it)->isRunning()) {
			_condition.wakeOne();
		}
		delete (*it);
	}
	_workers.clear();
}

/**
 * @brief 获取所有工作线程
 * @return 工作线程向量
 * @details 返回所有IOWorker线程的指针
 */
std::vector<IOWorker*> IOThread::getWorkers()
{
	return _workers;
}

/**
 * @brief 获取等待中的线程数量
 * @return 等待任务的线程数量
 * @details 返回当前等待在条件变量上的线程数量
 */
unsigned int IOThread::getWaitingThreads() {
	return _threadsWaiting;
}

/**
 * @brief 析构函数：清理IO线程管理器
 * @details 调用shutdown函数清理所有资源
 */
IOThread::~IOThread()
{
	shutdown();
}

/**
 * @brief 添加任务到队列
 * @param tileSize 瓦片大小
 * @param imgPosX 图像X位置
 * @param imgPosY 图像Y位置
 * @param level 图像层级
 * @param foregroundTile 前景瓦片（可选）
 * @details 根据是否提供前景瓦片创建IOJob或RenderJob，并添加到任务队列前端
 */
void IOThread::addJob(const unsigned int tileSize, const long long imgPosX, const long long imgPosY, const unsigned int level, ImageSource* foregroundTile)
{
	ThreadJob* job = NULL;
	if (foregroundTile) {
		job = new RenderJob(tileSize, imgPosX, imgPosY, level, foregroundTile);
	}
	else {
		job = new IOJob(tileSize, imgPosX, imgPosY, level);
	}
	QMutexLocker locker(&_jobListMutex);
	_jobList.push_front(job);
	_condition.wakeOne();
}

/**
 * @brief 设置背景图像
 * @param bck_img 背景图像弱指针
 * @details 为所有工作线程设置背景图像引用
 */
void IOThread::setBackgroundImage(std::weak_ptr<MultiResolutionImage> bck_img)
{
	QMutexLocker locker(&_jobListMutex);
	_bck_img = bck_img;
	for (unsigned int i = 0; i < _workers.size(); ++i) {
		_workers[i]->setBackgroundImage(_bck_img);
	}
}

/**
 * @brief 设置前景图像
 * @param for_img 前景图像弱指针
 * @param scale 缩放比例
 * @details 为所有工作线程设置前景图像引用和缩放比例
 */
void IOThread::setForegroundImage(std::weak_ptr<MultiResolutionImage> for_img, float scale)
{
	QMutexLocker locker(&_jobListMutex);
	_for_img = for_img;
	for (unsigned int i = 0; i < _workers.size(); ++i) {
		_workers[i]->setForegroundImage(for_img, scale);
	}
}

/**
 * @brief 获取任务
 * @return 任务指针，如果没有任务或已中止则返回NULL
 * @details 从任务队列前端获取任务，如果队列为空则等待
 */
ThreadJob* IOThread::getJob()
{
	_jobListMutex.lock();
	while (_jobList.empty() && !_abort) {
		_threadsWaiting++;
		_condition.wait(&_jobListMutex);
		_threadsWaiting--;
	}
	if (_abort) {
		_jobListMutex.unlock();
		return NULL;
	}
	ThreadJob* job = _jobList.front();
	_jobList.pop_front();
	_jobListMutex.unlock();
	return job;
}

/**
 * @brief 清空任务队列
 * @details 清空所有待处理任务，并为每个任务发送取消信号
 */
void IOThread::clearJobs()
{
	QMutexLocker locker(&_jobListMutex);
	for (auto job : _jobList) {
		if (_workers.size() > 0) {
			if (dynamic_cast<IOJob*>(job)) {
				emit _workers[0]->tileLoaded(nullptr, job->_imgPosX, job->_imgPosY, job->_tileSize, 0, job->_level, nullptr, nullptr);
			}
			else {
				emit _workers[0]->foregroundTileRendered(nullptr, job->_imgPosX, job->_imgPosY, job->_level);
			}
		}
		delete job;
	}
	_jobList.clear();
}

/**
 * @brief 背景通道改变回调
 * @param channel 新的通道索引
 * @details 为所有工作线程设置新的背景通道
 */
void IOThread::onBackgroundChannelChanged(int channel) {
	_jobListMutex.lock();
	for (unsigned int i = 0; i < _workers.size(); ++i) {
		_workers[i]->setBackgroundChannel(channel);
	}
	_jobListMutex.unlock();
}

/**
 * @brief 前景通道改变回调
 * @param channel 新的通道索引
 * @details 为所有工作线程设置新的前景通道
 */
void IOThread::onForegroundChannelChanged(int channel) {
	_jobListMutex.lock();
	for (unsigned int i = 0; i < _workers.size(); ++i) {
		_workers[i]->setForegroundChannel(channel);
	}
	_jobListMutex.unlock();
}

/**
 * @brief 颜色查找表改变回调
 * @param LUT 新的颜色查找表
 * @details 为所有工作线程设置新的颜色查找表
 */
void IOThread::onLUTChanged(const SlideColorManagement::LUT& LUT) {
	_jobListMutex.lock();
	for (unsigned int i = 0; i < _workers.size(); ++i) {
		_workers[i]->setLUT(LUT);
	}
	_jobListMutex.unlock();
}