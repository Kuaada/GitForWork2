/**
 * @file IOWorker.cpp
 * @brief IO工作线程实现文件
 * @details 该文件实现了多线程IO工作线程功能，包括：
 *          - 瓦片数据的读取和渲染
 *          - 背景和前景图像的处理
 *          - 多种数据类型的支持
 *          - 颜色查找表应用
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "IOWorker.h"
#include "ImageSource.h"
#include "MultiResolutionImage.h"
#include "IOThread.h"
#include "UtilityFunctions.h"
#include "SlideColorManagement.h"

/**
 * @brief 构造函数：初始化IO工作线程
 * @param thread 父IO线程对象
 * @details 初始化工作线程的成员变量，包括通道设置、图像引用等
 */
IOWorker::IOWorker(IOThread* thread) :
    QThread(thread),
    _abort(false),
    _backgroundChannel(0),
    _foregroundChannel(0),
    _foregroundImageScale(1.),
    _LUT()
{
}

/**
 * @brief 析构函数：等待线程结束
 * @details 等待工作线程完成当前任务后结束
 */
IOWorker::~IOWorker()
{
    wait();
}

/**
 * @brief 中止工作线程
 * @details 设置中止标志，线程将在下次检查时退出
 */
void IOWorker::abort() {
    _abort = true;
}

/**
 * @brief 设置背景通道
 * @param channel 通道索引
 * @details 线程安全地设置背景图像的显示通道
 */
void IOWorker::setBackgroundChannel(int channel) {
    mutex.lock();
    _backgroundChannel = channel;
    mutex.unlock();
}

/**
 * @brief 设置前景通道
 * @param channel 通道索引
 * @details 线程安全地设置前景图像的显示通道
 */
void IOWorker::setForegroundChannel(int channel) {
    mutex.lock();
    _foregroundChannel = channel;
    mutex.unlock();
}

/**
 * @brief 设置颜色查找表
 * @param LUT 颜色查找表
 * @details 线程安全地设置用于前景图像渲染的颜色查找表
 */
void IOWorker::setLUT(const SlideColorManagement::LUT& LUT) {
    mutex.lock();
    _LUT = LUT;
    mutex.unlock();
}

/**
 * @brief 设置背景图像
 * @param bck_img 背景图像弱指针
 * @details 线程安全地设置背景图像引用
 */
void IOWorker::setBackgroundImage(std::weak_ptr<MultiResolutionImage> bck_img) {
    mutex.lock();
    _bck_img = bck_img;
    mutex.unlock();
}

/**
 * @brief 设置前景图像
 * @param for_img 前景图像弱指针
 * @param scale 缩放比例
 * @details 线程安全地设置前景图像引用和缩放比例
 */
void IOWorker::setForegroundImage(std::weak_ptr<MultiResolutionImage> for_img, float scale) {
    mutex.lock();
    _for_img = for_img;
    _foregroundImageScale = scale;
    mutex.unlock();
}

/**
 * @brief 工作线程主循环
 * @details 持续从任务队列获取任务并执行，支持IOJob和RenderJob两种任务类型
 */
void IOWorker::run()
{
    forever{
      ThreadJob * newJob = dynamic_cast<IOThread*>(parent())->getJob();
      if (_abort) {
        return;
      }

      mutex.lock();
      if (IOJob* job = dynamic_cast<IOJob*>(newJob)) {
        executeIOJob(job);
      }
      else if (RenderJob* job = dynamic_cast<RenderJob*>(newJob)) {
        executeRenderJob(job);
      }
      delete newJob;
      mutex.unlock();
    }
}

bool IOWorker::executeIOJob(IOJob* job) {
    std::shared_ptr<MultiResolutionImage> local_bck_img = _bck_img.lock();
    float levelDownsample = local_bck_img->getLevelDownsample(job->_level);
    ImageSource* foregroundTile = NULL;
    QPixmap* foregroundPixmap = NULL;
    if (std::shared_ptr<MultiResolutionImage> local_for_img = _for_img.lock()) {
        if (local_for_img->getDataType() == SlideColorManagement::DataType::UChar) {
            foregroundTile = getForegroundTile<unsigned char>(local_for_img, job);
            foregroundPixmap = renderForegroundImage<unsigned char>(dynamic_cast<Patch<unsigned char>*>(foregroundTile), job->_tileSize);
        }
        else if (local_for_img->getDataType() == SlideColorManagement::DataType::UInt16) {
            foregroundTile = getForegroundTile<unsigned short>(local_for_img, job);
            foregroundPixmap = renderForegroundImage<unsigned short>(dynamic_cast<Patch<unsigned short>*>(foregroundTile), job->_tileSize);
        }
        else if (local_for_img->getDataType() == SlideColorManagement::DataType::UInt32) {
            foregroundTile = getForegroundTile<unsigned int>(local_for_img, job);
            foregroundPixmap = renderForegroundImage<unsigned int>(dynamic_cast<Patch<unsigned int>*>(foregroundTile), job->_tileSize);
        }
        else if (local_for_img->getDataType() == SlideColorManagement::DataType::Float) {
            foregroundTile = getForegroundTile<float>(local_for_img, job);
            foregroundPixmap = renderForegroundImage<float>(dynamic_cast<Patch<float>*>(foregroundTile), job->_tileSize);
        }
    }

    if (local_bck_img) {
        QPixmap* backgroundTile = NULL;
        SlideColorManagement::ColorType cType = local_bck_img->getColorType();
        if (local_bck_img->getDataType() == SlideColorManagement::DataType::UChar) {
            backgroundTile = renderBackgroundImage<unsigned char>(local_bck_img, job, cType);
        }
        else if (local_bck_img->getDataType() == SlideColorManagement::DataType::Float) {
            backgroundTile = renderBackgroundImage<float>(local_bck_img, job, cType);
        }
        else if (local_bck_img->getDataType() == SlideColorManagement::DataType::UInt16) {
            backgroundTile = renderBackgroundImage<unsigned short>(local_bck_img, job, cType);
        }
        else if (local_bck_img->getDataType() == SlideColorManagement::DataType::UInt32) {
            backgroundTile = renderBackgroundImage<unsigned int>(local_bck_img, job, cType);
        }
        emit tileLoaded(backgroundTile, job->_imgPosX, job->_imgPosY, job->_tileSize, job->_tileSize * job->_tileSize * local_bck_img->getSamplesPerPixel(), job->_level, foregroundTile, foregroundPixmap);
        return true;
    }
    return false;
}

bool IOWorker::executeRenderJob(RenderJob* job) {
    QPixmap* foregroundPixmap = NULL;
    if (job->_foregroundTile->getDataType() == SlideColorManagement::DataType::UChar) {
        foregroundPixmap = renderForegroundImage<unsigned char>(dynamic_cast<Patch<unsigned char>*>(job->_foregroundTile), job->_tileSize);
    }
    else if (job->_foregroundTile->getDataType() == SlideColorManagement::DataType::UInt16) {
        foregroundPixmap = renderForegroundImage<unsigned short>(dynamic_cast<Patch<unsigned short>*>(job->_foregroundTile), job->_tileSize);
    }
    else if (job->_foregroundTile->getDataType() == SlideColorManagement::DataType::UInt32) {
        foregroundPixmap = renderForegroundImage<unsigned int>(dynamic_cast<Patch<unsigned int>*>(job->_foregroundTile), job->_tileSize);
    }
    else if (job->_foregroundTile->getDataType() == SlideColorManagement::DataType::Float) {
        foregroundPixmap = renderForegroundImage<float>(dynamic_cast<Patch<float>*>(job->_foregroundTile), job->_tileSize);
    }
    if (foregroundPixmap) {
        emit foregroundTileRendered(foregroundPixmap, job->_imgPosX, job->_imgPosY, job->_level);
        return true;
    }
    else {
        return false;
    }
}

template<typename T>
QPixmap* IOWorker::renderBackgroundImage(std::shared_ptr<MultiResolutionImage> local_bck_img, const IOJob* job, SlideColorManagement::ColorType colorType) {
    float levelDownsample = local_bck_img->getLevelDownsample(job->_level);
    unsigned int samplesPerPixel = local_bck_img->getSamplesPerPixel();
    T* imgBuf = new T[job->_tileSize * job->_tileSize * samplesPerPixel];
    local_bck_img->getRawRegion(job->_imgPosX * levelDownsample * job->_tileSize, job->_imgPosY * levelDownsample * job->_tileSize, job->_tileSize, job->_tileSize, job->_level, imgBuf);
    QImage renderedImg;
    if (colorType == SlideColorManagement::ColorType::RGB) {
        renderedImg = QImage(reinterpret_cast<unsigned char*>(imgBuf), (job->_tileSize), (job->_tileSize), (job->_tileSize) * 3, QImage::Format_RGB888);
    }
    else if (colorType == SlideColorManagement::ColorType::RGBA) {
        renderedImg = QImage(reinterpret_cast<unsigned char*>(imgBuf), (job->_tileSize), (job->_tileSize), (job->_tileSize) * 4, QImage::Format_RGBA8888);
    }
    else {
        renderedImg = convertMonochromeToRGB(imgBuf, job->_tileSize, job->_tileSize, _backgroundChannel, samplesPerPixel, local_bck_img->getMinValue(_backgroundChannel), local_bck_img->getMaxValue(_backgroundChannel), SlideColorManagement::DefaultColorLUT["Background"]);
    }
    QPixmap* renderedPixmap = new QPixmap(QPixmap::fromImage(renderedImg));
    delete[] imgBuf;
    return renderedPixmap;
}

template<typename T>
Patch<T>* IOWorker::getForegroundTile(std::shared_ptr<MultiResolutionImage> local_for_img, const IOJob* job) {
    std::shared_ptr<MultiResolutionImage> loc_bck_img = _bck_img.lock();
    int levelDifference = loc_bck_img->getBestLevelForDownSample(_foregroundImageScale);
    int fgImageLevel = job->_level - levelDifference;

    // If we request a level which is outside the range of the foreground image (e.g. level 8 when it only has 7 levels), get level 7 and scale up.
    float foregroundExtraScaling = 1.;
    if (fgImageLevel > local_for_img->getNumberOfLevels()) {
        fgImageLevel = local_for_img->getNumberOfLevels() - 1;
    }
    else if (fgImageLevel < 0) {
        fgImageLevel = 0;
    }
    foregroundExtraScaling = local_for_img->getLevelDimensions(fgImageLevel)[0] / static_cast<float>(loc_bck_img->getLevelDimensions(job->_level)[0]);
    SlideColorManagement::DataType foregroundDataType = local_for_img->getDataType();

    int correctedTileSize = job->_tileSize * foregroundExtraScaling;
    unsigned int samplesPerPixel = local_for_img->getSamplesPerPixel();
    float fgLevelDownsample = local_for_img->getLevelDownsample(fgImageLevel);
    T* imgBuf = new T[correctedTileSize * correctedTileSize * samplesPerPixel];
    local_for_img->getRawRegion(job->_imgPosX * fgLevelDownsample * foregroundExtraScaling * job->_tileSize, job->_imgPosY * fgLevelDownsample * foregroundExtraScaling * job->_tileSize, correctedTileSize, correctedTileSize, fgImageLevel, imgBuf);
    std::vector<double> minValues, maxValues;
    for (unsigned int i = 0; i < local_for_img->getSamplesPerPixel(); ++i) {
        minValues.push_back(local_for_img->getMinValue(i));
        maxValues.push_back(local_for_img->getMaxValue(i));
    }
    Patch<T>* foregroundTile = new Patch<T>({ static_cast<unsigned long long>(correctedTileSize), static_cast<unsigned long long>(correctedTileSize), 1 }, local_for_img->getColorType(), imgBuf, true, minValues, maxValues);
    return foregroundTile;
}

template<typename T>
QPixmap* IOWorker::renderForegroundImage(Patch<T>* foregroundTile, unsigned int backgroundTileSize) {
    std::vector<unsigned long long> dims = foregroundTile->getDimensions();
    QImage renderedImage = convertMonochromeToRGB(foregroundTile->getPointer(), dims[0], dims[0], _foregroundChannel, foregroundTile->getSamplesPerPixel(), foregroundTile->getMinValue(_foregroundChannel), foregroundTile->getMaxValue(_foregroundChannel), _LUT);

    if (!renderedImage.isNull()) {
        if (backgroundTileSize != dims[0]) {
            renderedImage = renderedImage.scaled(backgroundTileSize, backgroundTileSize);
        }
        QPixmap* renderedPixmap = new QPixmap(QPixmap::fromImage(renderedImage));
        return renderedPixmap;
    }
    else {
        return new QPixmap();
    }
}