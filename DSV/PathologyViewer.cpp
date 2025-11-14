/**
 * @file PathologyViewer.cpp
 * @brief 病理查看器实现文件
 * @details 实现数字病理切片查看器的核心显示功能，包括图像显示、缩放、平移、标注等
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "PathologyViewer.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QResizeEvent>
//#include <QGLWidget>
#include <QTimeLine>
#include <QScrollBar>
#include <QHBoxLayout>
//#include <QSettings>
#include <QGuiApplication>
#include <QMainWindow>
#include <QStatusBar>
#include <QLineEdit>

#include "MiniMap.h"
#include "ScaleBar.h"
#include "IOThread.h"
#include "PrefetchThread.h"
#include "MultiResolutionImage.h"
#include "SlideColorManagement.h"
#include "WSITileGraphicsItem.h"
#include "WSITileGraphicsItemCache.h"
#include "TileManager.h"
#include "IOWorker.h"
//#include "CenteredToolBar.h"
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <InputDialog.h>
#include "LabelWin.h"
#include "DetailDialog.h"
#include <QListWidget>
#include <QListWidgetItem>
#include "ItemDialog.h"
#include "Item/QImageGraphicScene.h"
#include "Item/LineRenderElement.h"
#include "Item/RectRenderElement.h"
#include "Item/EllipseRenderElement.h"
#include "Item/TextRenderElement.h"
#include "Item/ContourRenderElement.h"
#include <QMenu>

using std::vector;

/**
 * @brief 病理查看器构造函数
 * @param parent 父窗口指针
 * @details 初始化病理查看器，设置视图属性、创建图形场景、建立信号槽连接
 */
PathologyViewer::PathologyViewer(QWidget* parent) :
    QGraphicsView(parent),
    _ioThread(NULL),
    _prefetchthread(NULL),
    _zoomSensitivity(0.5),
    _panSensitivity(0.5),
    _numScheduledScalings(0),
    _pan(false),
    _prevPan(0, 0),
    _map(NULL),
    _cache(NULL),
    _cacheSize(1000 * 512 * 512 * 3),
    _sceneScale(1.),
    _manager(NULL),
    _scaleBar(NULL),
    _renderForeground(true),
    _isFirstRightClick(false),
    _opacity(1.),
    m_frameCount(0)
{
    // 设置视图属性
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏水平滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);    // 隐藏垂直滚动条
    setResizeAnchor(QGraphicsView::ViewportAnchor::AnchorViewCenter);  // 设置调整大小锚点
    setDragMode(QGraphicsView::DragMode::NoDrag);          // 设置拖拽模式
    setContentsMargins(0, 0, 0, 0);                        // 设置内容边距
    setAutoFillBackground(true);                           // 自动填充背景
    setViewportUpdateMode(ViewportUpdateMode::FullViewportUpdate);  // 设置视口更新模式
    setInteractive(true);                                  // 启用交互

    // 创建图形场景
    m_pGraphicsScene = new QImageGraphicScene(this);
    m_pGraphicsScene->setStickyFocus(true);
    this->setScene(m_pGraphicsScene);
    this->setBackgroundBrush(QBrush(QColor(255, 255, 255)));  // 设置白色背景
    this->scene()->setBackgroundBrush(QBrush(QColor(255, 255, 255)));
    this->setContextMenuPolicy(Qt::CustomContextMenu);     // 设置自定义上下文菜单

    // 创建输入对话框
    _InputDialog = new InputDialog(this);
    _InputDialog->hide();

    // 初始化成员变量
    _labelWin = nullptr;
    _detailDialog = nullptr;
    _PaintingState = false;

    createContextMenu();  // 创建上下文菜单

    // 设置实时绘制画笔
    m_penRealTime.setColor(Qt::green);
    m_penRealTime.setWidth(5);
    m_pTempItem = nullptr;
    m_mouseType = Nothing;
    setMouseTracking(true);  // 启用鼠标跟踪
    
    // 初始化性能测量
    m_fpsHistory.reserve(m_fpsHistorySize);
    m_isFirstLoad = true;
    
    // 连接自定义上下文菜单信号
    connect(this, &QGraphicsView::customContextMenuRequested,
        [this](const QPoint& pos) {
            if (m_contextMenu&&_PaintingState) {
           
                // 将视图坐标转换为全局坐标
                QPoint globalPos = mapToGlobal(pos);
                // 在鼠标右键位置显示菜单
                m_contextMenu->exec(globalPos);
            }
        });

    // 设置FPS计时器
    m_fpsTimer.start(1000);
    connect(&m_fpsTimer, &QTimer::timeout, this, &PathologyViewer::updateFPS);
}

/**
 * @brief 设置标签图可见性
 * @details 切换标签窗口的显示状态
 */
void PathologyViewer::setLabelMapVisible()
{
    qDebug() << "setLabelMapVisible called";
    if (_labelWin->isHidden())
    {
        _labelWin->show(); 
    }
    else
    {
        _labelWin->hide();
    }
   
    //changeViewPosWithAnimation();
    emit viewShow(this->grab());
}

/**
 * @brief 设置详细信息可见性
 * @details 切换详细信息对话框的显示状态
 */
void PathologyViewer::setDetailVisible()
{
    if (_detailDialog->isHidden())
    {
        _detailDialog->show();
    }
    else
    {
        _detailDialog->hide();
    }
}

/**
 * @brief 病理查看器析构函数
 * @details 清理资源，关闭图像
 */
PathologyViewer::~PathologyViewer()
{
    close();
}

/**
 * @brief 获取缓存大小
 * @return 当前缓存大小（字节）
 * @details 返回瓦片缓存的最大大小
 */
unsigned long long PathologyViewer::getCacheSize() {
    if (_cache) {
        return _cache->maxCacheSize();
    }
    else {
        return 0;
    }
}

/**
 * @brief 设置缓存大小
 * @param maxCacheSize 新的缓存大小（字节）
 * @details 设置瓦片缓存的最大大小
 */
void PathologyViewer::setCacheSize(unsigned long long& maxCacheSize) {
    if (_cache) {
        _cache->setMaxCacheSize(maxCacheSize);
    }
}

/**
 * @brief 窗口大小改变事件处理
 * @param event 大小改变事件对象
 * @details 当窗口大小改变时，重新计算视场和边界框
 */
void PathologyViewer::resizeEvent(QResizeEvent* event) {
    QRect rect = QRect(QPoint(0, 0), event->size());
    QRectF FOV = this->mapToScene(rect).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / this->_sceneScale, FOV.top() / this->_sceneScale, 
                            FOV.width() / this->_sceneScale, FOV.height() / this->_sceneScale);
    float maxDownsample = 1. / this->_sceneScale;
    QGraphicsView::resizeEvent(event);
    if (_img) {
        emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample(maxDownsample / this->transform().m11()));
        emit updateBBox(FOV);
    }
}

/**
 * @brief 鼠标滚轮事件处理
 * @param event 滚轮事件对象
 * @details 处理鼠标滚轮缩放功能
 */
void PathologyViewer::wheelEvent(QWheelEvent* event) {
    m_zoomTimer.start();
    if (_PaintingState)
    {
        return;
    }

    int numDegrees = event->angleDelta().y() / 8;
    //int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;  
    _zoomToScenePos = this->mapToScene(event->position().toPoint());
    _zoomToViewPos = event->position();
    zoom(numSteps);
}

/**
 * @brief 缩放功能
 * @param numSteps 缩放步数
 * @details 执行缩放动画
 */
void PathologyViewer::zoom(float numSteps) {
    if (!_img) {
        return;
    }
    _numScheduledScalings += numSteps;
    if (_numScheduledScalings* numSteps < 0) {
        _numScheduledScalings = numSteps;
    }

    QTimeLine* anim = new QTimeLine(300, this);
    anim->setUpdateInterval(5);

    connect(anim, SIGNAL(valueChanged(qreal)), SLOT(scalingTime(qreal)));
    connect(anim, SIGNAL(finished()), SLOT(zoomFinished()));
    anim->start();
}

/**
 * @brief 缩放动画时间处理
 * @param x 动画进度值
 * @details 在缩放动画过程中更新变换矩阵
 */
void PathologyViewer::scalingTime(qreal x)
{
    qreal factor = 1.0 + qreal(_numScheduledScalings) * x / 300.;
    float maxDownsample = 1. / this->_sceneScale;
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / this->_sceneScale, FOV.top() / this->_sceneScale, FOV.width() / this->_sceneScale, FOV.height() / this->_sceneScale);
    float scaleX = static_cast<float>(_img->getDimensions()[0]) / FOVImage.width();
    float scaleY = static_cast<float>(_img->getDimensions()[1]) / FOVImage.height();
    float minScale = scaleX > scaleY ? scaleY : scaleX;
    float maxScale = scaleX > scaleY ? scaleX : scaleY;
    if ((factor < 1.0 && maxScale < 0.5) || (factor > 1.0 && minScale > 2 * maxDownsample)) {
        return;
    }
    scale(factor, factor);
    centerOn(_zoomToScenePos);
    QPointF delta_viewport_pos = _zoomToViewPos - QPointF(width() / 2.0, height() / 2.0);
    QPointF viewport_center = mapFromScene(_zoomToScenePos) - delta_viewport_pos;
    centerOn(mapToScene(viewport_center.toPoint()));
    float tm11 = this->transform().m11();
    emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample((1. / this->_sceneScale) / this->transform().m11()));
    emit updateBBox(FOV);
    emit factorTrans(float(transform().m11()));
}
void PathologyViewer::setChangedMpp(float mpp)
{
    _scaleBar->setResolution(mpp);

    double fac = mpp / _sceneScale;
    m_pGraphicsScene->setPixelSize(fac);
    //emit mppTrans(mpp);
}

void PathologyViewer::zoomFinished()
{
    if (_numScheduledScalings > 0)
        _numScheduledScalings--;
    else
        _numScheduledScalings++;
    emit factorTrans(float(transform().m11()));
    
    if (m_zoomTimer.isValid()) {
        qint64 zoomTime = m_zoomTimer.elapsed();
        qDebug() << "🔍 Zoom delay:" << zoomTime << "ms";
    }
    
    sender()->~QObject();
}
void PathologyViewer::handleItemSelection(QGraphicsItem* item)
{
    if (LineRenderElement* line = dynamic_cast<LineRenderElement*>(item)) {
        emit perimeterUpdated(line->getPerimeter());
    }
    else if (RectRenderElement* rect = dynamic_cast<RectRenderElement*>(item)) {
        emit areaAndPerimeterUpdated(rect->getPerimeter(), rect->getArea());
    }
    else if (EllipseRenderElement* ellipse = dynamic_cast<EllipseRenderElement*>(item)) {
        emit areaAndPerimeterUpdated(ellipse->getPerimeter(), ellipse->getArea());
    }
    else if (ContourRenderElement* contour = dynamic_cast<ContourRenderElement*>(item)) {
        emit areaAndPerimeterUpdated(contour->getPerimeter(), contour->getArea());
    }
    else if (TextRenderElement* text = dynamic_cast<TextRenderElement*>(item)) {
       // emit textSelected(text->text()); // 示例：发送文本内容
    }
}
void PathologyViewer::moveTo(const QPointF& pos) {
    this->centerOn(pos);
    float maxDownsample = 1. / this->_sceneScale;
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / this->_sceneScale, FOV.top() / this->_sceneScale, FOV.width() / this->_sceneScale, FOV.height() / this->_sceneScale);
    emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample(maxDownsample / this->transform().m11()));
    emit updateBBox(FOV);
}
void PathologyViewer::onFieldOfViewChanged(const QRectF& FOV, const unsigned int level) {
    if (_manager) {
        _manager->loadTilesForFieldOfView(FOV, level);
    }
}
void PathologyViewer::initialize(std::shared_ptr<MultiResolutionImage> img) {
    if (m_isFirstLoad) {
        m_loadTimer.start();
        qDebug() << "🚀 Image loading started...";
    }
    close();
    setEnabled(true);
    _img = img;
    unsigned int tileSize = 512;
    unsigned int lastLevel = _img->getNumberOfLevels() - 1;
    for (int i = lastLevel; i >= 0; --i) {
        std::vector<unsigned long long> lastLevelDimensions = _img->getLevelDimensions(i);
        if (lastLevelDimensions[0] > tileSize && lastLevelDimensions[1] > tileSize) {
            lastLevel = i;
            break;
        }
    }
    if (!_img->getLabel().isNull())
    {
        _labelWin = new LabelWin(this, _img->getLabel());
        _labelWin->hide();
    }

    _detailDialog = new DetailDialog(this, _img->getProperties());
    _detailDialog->hide();

    _cache = new WSITileGraphicsItemCache();
    _cache->setMaxCacheSize(_cacheSize);
    _ioThread = new IOThread(this);
    _ioThread->setBackgroundImage(img);
    _manager = new TileManager(_img, tileSize, lastLevel, _ioThread, _cache, scene());
    setMouseTracking(true);
    std::vector<IOWorker*> workers = _ioThread->getWorkers();
    for (int i = 0; i < workers.size(); ++i) {
        QObject::connect(workers[i], SIGNAL(tileLoaded(QPixmap*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, ImageSource*, QPixmap*)), _manager, SLOT(onTileLoaded(QPixmap*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, ImageSource*, QPixmap*)));
        QObject::connect(workers[i], SIGNAL(foregroundTileRendered(QPixmap*, unsigned int, unsigned int, unsigned int)), _manager, SLOT(onForegroundTileRendered(QPixmap*, unsigned int, unsigned int, unsigned int)));
    }
    initializeImage(scene(), tileSize, lastLevel);
    initializeGUIComponents(lastLevel);
    QObject::connect(this, SIGNAL(backgroundChannelChanged(int)), _ioThread, SLOT(onBackgroundChannelChanged(int)));
    QObject::connect(_cache, SIGNAL(itemEvicted(WSITileGraphicsItem*)), _manager, SLOT(onTileRemoved(WSITileGraphicsItem*)));
    QObject::connect(this, SIGNAL(fieldOfViewChanged(const QRectF, const unsigned int)), this, SLOT(onFieldOfViewChanged(const QRectF, const unsigned int)));
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / this->_sceneScale, FOV.top() / this->_sceneScale, FOV.width() / this->_sceneScale, FOV.height() / this->_sceneScale);
    emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample((1. / this->_sceneScale) / this->transform().m11()));
    emit factorTrans(transform().m11());
    // 记录初始变换
    _initialTransform = this->transform();
    // 记录初始图像中心
    _initialCenter = this->mapToScene(this->viewport()->rect().center());
    // 记录初始_sceneScale
    emit viewShow(this->grab());
}
void PathologyViewer::onForegroundImageChanged(std::weak_ptr<MultiResolutionImage> for_img, float scale) {
    _for_img = for_img;
    if (_ioThread) {
        _ioThread->setForegroundImage(_for_img, scale);
        _manager->refresh();
    }
}
void PathologyViewer::setForegroundLUT(const SlideColorManagement::LUT& LUT)
{
    if (_ioThread) {
        _ioThread->onLUTChanged(LUT);
        if (_for_img.lock()) {
            _manager->updateTileForegounds();
        }
    }
}
void PathologyViewer::setForegroundChannel(unsigned int channel) {
    if (_ioThread) {
        _ioThread->onForegroundChannelChanged(channel);
        if (_for_img.lock()) {
            _manager->updateTileForegounds();
        }
    }
}
void PathologyViewer::setEnableForegroundRendering(bool enableForegroundRendering)
{
    _renderForeground = enableForegroundRendering;
    _manager->onRenderForegroundChanged(enableForegroundRendering);
}
void PathologyViewer::initializeImage(QGraphicsScene* scn, unsigned int tileSize, unsigned int lastLevel) {
    unsigned int nrLevels = _img->getNumberOfLevels();
    std::vector<unsigned long long> lastLevelDimensions = _img->getLevelDimensions(lastLevel);
    float lastLevelWidth = ((lastLevelDimensions[0] / tileSize) + 1) * tileSize;
    float lastLevelHeight = ((lastLevelDimensions[1] / tileSize) + 1) * tileSize;
    float longest = lastLevelWidth > lastLevelHeight ? lastLevelWidth : lastLevelHeight;
    _sceneScale = 1. / _img->getLevelDownsample(lastLevel);
    QRectF n((lastLevelDimensions[0] / 2) - 1.5 * longest, (lastLevelDimensions[1] / 2) - 1.5 * longest, 3 * longest, 3 * longest);
    this->setSceneRect(n);
    this->fitInView(QRectF(0, 0, lastLevelDimensions[0], lastLevelDimensions[1]), Qt::AspectRatioMode::KeepAspectRatio);
   
    _manager->loadAllTilesForLevel(lastLevel);
    float maxDownsample = 1. / this->_sceneScale;
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / this->_sceneScale, FOV.top() / this->_sceneScale, FOV.width() / this->_sceneScale, FOV.height() / this->_sceneScale);
    emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample(maxDownsample / this->transform().m11()));
    emit factorTrans(transform().m11());
    emit mppTrans(float(_detailDialog->retMpp()));
    
    double fac = _detailDialog->retMpp()/_sceneScale;
    m_pGraphicsScene->setPixelSize(fac);
    while (_ioThread->numberOfJobs() > 0) {
    }
    
    if (m_isFirstLoad) {
        qint64 loadTime = m_loadTimer.elapsed();
        qDebug() << "⏱️ First To View:" << loadTime << "ms";
        m_isFirstLoad = false;
    }
}
void PathologyViewer::initializeGUIComponents(unsigned int level) {
    // Initialize the minimap
    std::vector<unsigned long long> overviewDimensions = _img->getLevelDimensions(level);
    unsigned int size = overviewDimensions[0] * overviewDimensions[1] * _img->getSamplesPerPixel();
    unsigned char* overview = new unsigned char[size];
    _img->getRawRegion<unsigned char>(0, 0, overviewDimensions[0], overviewDimensions[1], level, overview);
    QImage ovImg;
    if (_img->getColorType() == SlideColorManagement::ColorType::RGBA) {
        ovImg = QImage(overview, overviewDimensions[0], overviewDimensions[1], overviewDimensions[0] * 4, QImage::Format_RGBA8888).convertToFormat(QImage::Format_RGB888);
    }
    else if (_img->getColorType() == SlideColorManagement::ColorType::RGB) {
        ovImg = QImage(overview, overviewDimensions[0], overviewDimensions[1], overviewDimensions[0] * 3, QImage::Format_RGB888);
    }
    QPixmap ovPixMap = QPixmap(QPixmap::fromImage(ovImg));
    delete[] overview;
    if (_map) {
        _map->deleteLater();
        _map = NULL;
    }
    _map = new MiniMap(ovPixMap, this);
    QWidget* parent = this->parentWidget();
    if (_scaleBar) {
        _scaleBar->deleteLater();
        _scaleBar = NULL;
    }
    std::vector<double> spacing = _img->getSpacing();
    if (!spacing.empty()) {
        _scaleBar = new ScaleBar(spacing[0], this);
    }
    else {
        _scaleBar = new ScaleBar(-1, this);
    }
    _scaleBar->setObjectName("ScaleBar");
    if (this->layout()) {
        delete this->layout();
    }
    QHBoxLayout* Hlayout = new QHBoxLayout(this);
    QVBoxLayout* Vlayout = new QVBoxLayout();
    QVBoxLayout* Vlayout2 = new QVBoxLayout();
    Vlayout2->addStretch(4);
    Hlayout->addLayout(Vlayout2);
    Hlayout->addStretch(4);
    Hlayout->setContentsMargins(30, 30, 30, 30);
    Hlayout->addLayout(Vlayout, 1);
    Vlayout->addStretch(4);
    if (_map) {
        Vlayout->addWidget(_map, 1);
    }
    if (_scaleBar) {
        Vlayout2->addWidget(_scaleBar);
    }
    _map->setTileManager(_manager);
    _map->toggleCoverageMap(false);
    QObject::connect(this, SIGNAL(updateBBox(const QRectF&)), _map, SLOT(updateFieldOfView(const QRectF&)));
    QObject::connect(_manager, SIGNAL(coverageUpdated()), _map, SLOT(onCoverageUpdated()));
    QObject::connect(_map, SIGNAL(positionClicked(QPointF)), this, SLOT(moveTo(const QPointF&)));
    QObject::connect(this, SIGNAL(fieldOfViewChanged(const QRectF&, const unsigned int)), _scaleBar, SLOT(updateForFieldOfView(const QRectF&)));
    _map->show();

  
    emit initOver();
}


void PathologyViewer::close() {
    if (this->window()) {
    }
    if (_prefetchthread) {
        _prefetchthread->deleteLater();
        _prefetchthread = NULL;
    }
    scene()->clear();
    if (_manager) {
        _manager->clear();
        delete _manager;
        _manager = NULL;
    }
    if (_cache) {
        _cache->clear();
        delete _cache;
        _cache = NULL;
    }
    _img = NULL;
    if (_ioThread) {
        _ioThread->shutdown();
        _ioThread->deleteLater();
        _ioThread = NULL;
    }
    if (_map) {
        _map->setHidden(true);
        _map->deleteLater();
        _map = NULL;
    }
    if (_scaleBar) {
        _scaleBar->setHidden(true);
        _scaleBar->deleteLater();
        _scaleBar = NULL;
    }
    if (_labelWin)
    {
        _labelWin->setHidden(true);
        _labelWin->deleteLater();
        _labelWin = NULL;
    }

    if (_detailDialog)
    {
        _detailDialog->setHidden(true);
        _detailDialog->deleteLater();
        _detailDialog = NULL;
    }
    
    setEnabled(false);
}
void PathologyViewer::togglePan(bool pan, const QPoint& startPos) {
    if (pan) {
        if (_pan) {
            return;
        }
        m_panTimer.start();
        _pan = true;
        _prevPan = startPos;
        setCursor(Qt::ClosedHandCursor);
    }
    else {
        if (!_pan) {
            return;
        }
        _pan = false;
        _prevPan = QPoint(0, 0);
        setCursor(Qt::ArrowCursor);
        
        if (m_panTimer.isValid()) {
            qint64 panTime = m_panTimer.elapsed();
            qDebug() << "🖱️ Pan delay:" << panTime << "ms";
        }
    }
}
void PathologyViewer::pan(const QPoint& panTo) {
    QScrollBar* hBar = horizontalScrollBar();
    QScrollBar* vBar = verticalScrollBar();
    QPoint delta = panTo - _prevPan;
    _prevPan = panTo;
    hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
    vBar->setValue(vBar->value() - delta.y());
    float maxDownsample = 1. / this->_sceneScale;
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / this->_sceneScale, FOV.top() / this->_sceneScale, FOV.width() / this->_sceneScale, FOV.height() / this->_sceneScale);
    emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample(maxDownsample / this->transform().m11()));
    emit updateBBox(FOV);
    
}
void PathologyViewer::updateCurrentFieldOfView() {
    float maxDownsample = 1. / this->_sceneScale;
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / this->_sceneScale, FOV.top() / this->_sceneScale, FOV.width() / this->_sceneScale, FOV.height() / this->_sceneScale);
    emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample(maxDownsample / this->transform().m11()));
    emit updateBBox(FOV);


}

void PathologyViewer::setForegroundOpacity(const float& opacity) {
    _opacity = opacity;
    _manager->onForegroundOpacityChanged(opacity);
}
float PathologyViewer::getForegroundOpacity() const {
    return _opacity;
}
void PathologyViewer::setMiniMapVisible(bool state)
{
    _map->setVisible(state);
    emit viewShow(this->grab());
}
void PathologyViewer::setCoverageArea(bool state)
{
    _map->toggleCoverageMap(state);
}
void PathologyViewer::setScaleBar(bool state)
{
    _scaleBar->setVisible(state);
    emit viewShow(this->grab());
}
void PathologyViewer::updateSnap()
{
    emit viewShow(this->grab());

}
void PathologyViewer::setFileWidgetState(bool state)
{
    _isFirstRightClick = state;
}
bool PathologyViewer::isPanning() {
    return _pan;
}
void PathologyViewer::setPanSensitivity(float panSensitivity) {
    if (panSensitivity > 1) {
        _panSensitivity = 1;
    }
    else if (panSensitivity < 0.01) {
        _panSensitivity = 0.01;
    }
    else {
        _panSensitivity = panSensitivity;
    }
};
float PathologyViewer::getPanSensitivity() const {
    return _panSensitivity;
};
void PathologyViewer::setZoomSensitivity(float zoomSensitivity) {
    if (zoomSensitivity > 1) {
        _zoomSensitivity = 1;
    }
    else if (zoomSensitivity < 0.01) {
        _zoomSensitivity = 0.01;
    }
    else {
        _zoomSensitivity = zoomSensitivity;
    }
};
float PathologyViewer::getZoomSensitivity() const {
    return _zoomSensitivity;
};
void PathologyViewer::reset()
{
    QTimeLine* anim = new QTimeLine(400, this);
    anim->setUpdateInterval(10);

    connect(anim, &QTimeLine::valueChanged, this, &PathologyViewer::resetTime);
    connect(anim, &QTimeLine::finished, this, &PathologyViewer::resetFinished);
    anim->start();
}
void PathologyViewer::resetTime(qreal x)
{
    // 计算当前变换矩阵
    QTransform currentTransform = this->transform();
    QTransform targetTransform = _initialTransform;

    // 计算插值得到中间变换矩阵
    QTransform interpolatedTransform;
    const qreal m13 = 0; // 3D 变换元素，2D 变换为 0
    const qreal m23 = 0; // 3D 变换元素，2D 变换为 0
    const qreal m31 = 0;
    const qreal m32 = 0;
    const qreal m33 = 1; // 2D 变换下，m33 通常为 1
    interpolatedTransform.setMatrix(
        currentTransform.m11() + (targetTransform.m11() - currentTransform.m11()) * x,
        currentTransform.m12() + (targetTransform.m12() - currentTransform.m12()) * x,
        m13,
        currentTransform.m21() + (targetTransform.m21() - currentTransform.m21()) * x,
        currentTransform.m22() + (targetTransform.m22() - currentTransform.m22()) * x,
        m23,
        currentTransform.dx() + (targetTransform.dx() - currentTransform.dx()) * x,
        currentTransform.dy() + (targetTransform.dy() - currentTransform.dy()) * x,
        m33
    );

    this->setTransform(interpolatedTransform);

    // 计算插值得到中间图像中心位置
    QPointF currentCenter = this->mapToScene(this->viewport()->rect().center());
    QPointF targetCenter = _initialCenter;
    QPointF interpolatedCenter(
        currentCenter.x() + (targetCenter.x() - currentCenter.x()) * x,
        currentCenter.y() + (targetCenter.y() - currentCenter.y()) * x
    );
    this->centerOn(interpolatedCenter);
    // 计算边缘变化信号
    float maxDownsample = 1. / _sceneScale;
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / _sceneScale, FOV.top() / _sceneScale, FOV.width() / _sceneScale, FOV.height() / _sceneScale);
    emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample(maxDownsample / this->transform().m11()));
    emit updateBBox(FOV);
    emit factorTrans(float(transform().m11()));
}
void PathologyViewer::resetFinished()
{
    emit factorTrans(float(transform().m11()));
    sender()->deleteLater();
}
void PathologyViewer::zoomToFixedMagnification(float targetMagnification) {
    QTimeLine* anim = new QTimeLine(450, this);
    anim->setUpdateInterval(10);

    _initialCenterFixedScale = this->mapToScene(this->viewport()->rect().center());

    const qreal m11 = targetMagnification;
    const qreal m12 = this->transform().m12();
    const qreal m13 = 0; // 3D 变换元素，2D 变换为 0
    const qreal m21 = this->transform().m21();
    const qreal m22 = targetMagnification;
    const qreal m23 = 0; // 3D 变换元素，2D 变换为 0
    const qreal m31 = this->transform().dx();
    const qreal m32 = this->transform().dy();
    const qreal m33 = 1; // 2D 变换下，m33 通常为 1

    _targetTransform.setMatrix(
        m11,m12,m13,
        m21,m22,m23,
        m31,m32,m33
    );
    connect(anim, &QTimeLine::valueChanged, this, &PathologyViewer::zoomToFixedScaleTime);
    connect(anim, &QTimeLine::finished, this, &PathologyViewer::zoomToFixedScaleFinished);
    anim->start();
}
void PathologyViewer::zoomToFixedScaleTime(qreal x)
{
    // 计算当前变换矩阵
    QTransform currentTransform = this->transform();

    // 计算插值得到中间变换矩阵
    QTransform interpolatedTransform;
    const qreal m13 = 0; // 3D 变换元素，2D 变换为 0
    const qreal m23 = 0; // 3D 变换元素，2D 变换为 0
    const qreal m31 = 0;
    const qreal m32 = 0;
    const qreal m33 = 1; // 2D 变换下，m33 通常为 1
    interpolatedTransform.setMatrix(
        currentTransform.m11() + (_targetTransform.m11() - currentTransform.m11()) * x,
        currentTransform.m12() + (_targetTransform.m12() - currentTransform.m12()) * x,
        m13,
        currentTransform.m21() + (_targetTransform.m21() - currentTransform.m21()) * x,
        currentTransform.m22() + (_targetTransform.m22() - currentTransform.m22()) * x,
        m23,
        currentTransform.dx() + (_targetTransform.dx() - currentTransform.dx()) * x,
        currentTransform.dy() + (_targetTransform.dy() - currentTransform.dy()) * x,
        m33
    );

    this->setTransform(interpolatedTransform);

    // 计算插值得到中间图像中心位置
    //QPointF currentCenter = this->mapToScene(this->viewport()->rect().center());
    QPointF currentCenter = this->mapToScene(this->viewport()->rect().center());
    QPointF targetCenter = _initialCenterFixedScale;
    QPointF interpolatedCenter(
        currentCenter.x() + (targetCenter.x() - currentCenter.x()) * x,
        currentCenter.y() + (targetCenter.y() - currentCenter.y()) * x
    );
    this->centerOn(interpolatedCenter);
    // 计算边缘变化信号
    float maxDownsample = 1. / _sceneScale;
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left() / _sceneScale, FOV.top() / _sceneScale, FOV.width() / _sceneScale, FOV.height() / _sceneScale);
    emit fieldOfViewChanged(FOVImage, _img->getBestLevelForDownSample(maxDownsample / this->transform().m11()));
    emit updateBBox(FOV);
    emit factorTrans(float(transform().m11()));
}
void PathologyViewer::zoomToFixedScaleFinished()
{
    emit factorTrans(float(transform().m11()));
    sender()->deleteLater();
}

void PathologyViewer::setPaintState(bool state)
{
    _PaintingState = state;
}

void PathologyViewer::setMouseType(MouseType type)
{
    if (type == Nothing)
    {
       /* setCursor(QCursor(Qt::PointingHandCursor));
        setCursor(QCursor(Qt::));*/
    }
    else
    {
        setCursor(QCursor(Qt::ArrowCursor));
    }
    m_mouseType = type;
    if (m_mouseType == RectangleSelection)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
        setRubberBandSelectionMode(Qt::IntersectsItemShape);
    }
    else if(m_mouseType ==Nothing)
    {
        // 允许中键拖拽，但禁用左键拖拽
        setDragMode(QGraphicsView::NoDrag);
        // 注意：我们通过自定义的中键事件处理来实现拖拽
    }
    else
    {
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
    if (m_pTempItem)
    {
        delete m_pTempItem;
        m_pTempItem = nullptr;
    }
}

void PathologyViewer::setGraphicsScene(QImageGraphicScene* pScene)
{
    m_pGraphicsScene = pScene;
    m_pGraphicsScene->setStickyFocus(true);
    setScene(pScene);
}

void PathologyViewer::mousePressEvent(QMouseEvent* event)
{
    if (_img)
    {

        switch (m_mouseType)
        {
        case Nothing:
            if (event->button() == Qt::MiddleButton)
            {
                // qDebug() << "🖱️ Middle button pressed at:" << event->pos();
                togglePan(true, event->pos());
                event->accept();
                return;
            }
            else if (event->button() == Qt::RightButton) {
                if (!_isFirstRightClick)
                {
                    emit rightClicked(); // 右键点击信号，返回filePath
                    event->accept();
                    return;
                }          
            }
            else if (event->button() == Qt::LeftButton) {
                // 新增：处理左键选中图形项
                QGraphicsItem* clickedItem = itemAt(event->pos());
                if (clickedItem) {
                    // 清除之前的选择
                    scene()->clearSelection();
                    // 选中当前点击的项
                    clickedItem->setSelected(true);
                    // 发送选中信号
                    if (LineRenderElement* line = dynamic_cast<LineRenderElement*>(clickedItem)) {
                        emit perimeterUpdated(line->getPerimeter());
                    }
                    else if (RectRenderElement* rect = dynamic_cast<RectRenderElement*>(clickedItem)) {
                        emit areaAndPerimeterUpdated(rect->getPerimeter(), rect->getArea());
                    }
                    else if (EllipseRenderElement* ellipse = dynamic_cast<EllipseRenderElement*>(clickedItem)) {
                        emit areaAndPerimeterUpdated(ellipse->getPerimeter(), ellipse->getArea());
                    }
                    else if (ContourRenderElement* contour = dynamic_cast<ContourRenderElement*>(clickedItem)) {
                        emit areaAndPerimeterUpdated(contour->getPerimeter(), contour->getArea());
                    }
                    else if (TextRenderElement* text = dynamic_cast<TextRenderElement*>(clickedItem)) {
                        // emit textSelected(text->text()); // 示例：发送文本内容
                    }

                }
            }

            break;
        case Line:
            if (event->button() == Qt::LeftButton)
            {
                m_bLButtonDown = true;
                m_ptOri = event->pos();
                ///** @brief 临时图形项指针 */
                //QGraphicsItem* m_pTempItem;    LineRenderElemnt继承于QGraphicsItem,其次继承于另外一个类
                m_pTempItem = new LineRenderElement("LineRenderElement");
       
                m_pGraphicsScene->addItem(m_pTempItem);
                static_cast<LineRenderElement*>(m_pTempItem)->setPen(m_penRealTime);
                // 连接信号与槽 - 修正类型转换，确保正确
                  // 设置画笔样式
                connect(static_cast<LineRenderElement*>(m_pTempItem), &LineRenderElement::sendLength,
                    this, &PathologyViewer::perimeterUpdated);
            }
            break;
        case Rect:
            if (event->button() == Qt::LeftButton)
            {
                m_bLButtonDown = true;
                m_ptOri = event->pos();
                m_pTempItem = new RectRenderElement("RectangleRenderElement");
                m_pGraphicsScene->addItem(m_pTempItem);
                static_cast<RectRenderElement*>(m_pTempItem)->setPen(m_penRealTime);

                connect(static_cast<RectRenderElement*>(m_pTempItem), &RectRenderElement::sendPerimeterAndArea,
                    this, &PathologyViewer::areaAndPerimeterUpdated);
            }
            break;
        case Ellipse:
            if (event->button() == Qt::LeftButton)
            {
                m_bLButtonDown = true;
                m_ptOri = event->pos();
                m_pTempItem = new EllipseRenderElement("EllipseRenderElement");
                m_pGraphicsScene->addItem(m_pTempItem);
                static_cast<EllipseRenderElement*>(m_pTempItem)->setPen(m_penRealTime);
                connect(static_cast<EllipseRenderElement*>(m_pTempItem), &EllipseRenderElement::sendPerimeterAndArea,
                    this, &PathologyViewer::areaAndPerimeterUpdated);
            }
            break;
        case Text:
            if (event->button() == Qt::LeftButton)
            {
                m_bLButtonDown = true;
                m_ptOri = event->pos();
                m_pTempItem = new TextRenderElement("TextRenderElement");
                m_pGraphicsScene->addItem(m_pTempItem);
                m_pTempItem->setPos(mapToScene(m_ptOri));
                static_cast<TextRenderElement*>(m_pTempItem)->setDefaultTextColor(m_penRealTime.color());
            }
            break;
        case Contour:
            if (event->button() == Qt::LeftButton)
            {
                m_bLButtonDown = true;
                m_ptOri = event->pos();
                m_pTempItem = new ContourRenderElement("ContourRenderElement");
                m_pGraphicsScene->addItem(m_pTempItem);
                static_cast<ContourRenderElement*>(m_pTempItem)->setPen(m_penRealTime);
            }
            break;
        default:
            break;
        }

    }
    else
    {
        emit rightClicked();
    }
    QGraphicsView::mousePressEvent(event);
}

void PathologyViewer::mouseReleaseEvent(QMouseEvent* event)
{


    if (event->button() == Qt::LeftButton) {
        m_bLButtonDown = false;
    }
    switch (m_mouseType)
    {
    case Nothing:
        if (event->button() == Qt::MiddleButton)
        {
            // qDebug() << "🖱️ Middle button released";
            togglePan(false);
            event->accept();
            return;
        }
 
        event->accept();
        break;

    case Line:
        if (event->button() == Qt::RightButton)
        {
            return;
        }
        emit perimeterUpdated(static_cast<LineRenderElement*>(m_pTempItem)->getPerimeter());
        if (static_cast<QGraphicsItem*>(m_pTempItem)->boundingRect().width() < 3)
        delete m_pTempItem;
        m_pTempItem = nullptr;
        setMouseType(Nothing);
        break;
    case Rect:
        if (event->button() == Qt::RightButton)
        {
            return;
        }
        emit areaAndPerimeterUpdated(static_cast<LineRenderElement*>(m_pTempItem)->getPerimeter(), static_cast<LineRenderElement*>(m_pTempItem)->getArea());
        if (static_cast<QGraphicsItem*>(m_pTempItem)->boundingRect().width() < 3)
            delete m_pTempItem;
        m_pTempItem = nullptr;
        setMouseType(Nothing);
        break;
    case Ellipse:
        if (event->button() == Qt::RightButton)
        {
            return;
        }
        emit areaAndPerimeterUpdated(static_cast<LineRenderElement*>(m_pTempItem)->getPerimeter(), static_cast<LineRenderElement*>(m_pTempItem)->getArea());
        if (static_cast<QGraphicsItem*>(m_pTempItem)->boundingRect().width() < 3)
            delete m_pTempItem;
        m_pTempItem = nullptr;
        setMouseType(Nothing);
        break;
    case Text:
        if (event->button() == Qt::RightButton)
        {
            return;
        }
        m_pTempItem = nullptr;
        setMouseType(Nothing);
        break;
    case Contour:
        if (event->button() == Qt::RightButton)
        {
            return;
        }
        emit areaAndPerimeterUpdated(static_cast<LineRenderElement*>(m_pTempItem)->getPerimeter(), static_cast<LineRenderElement*>(m_pTempItem)->getArea());
        if (static_cast<QGraphicsItem*>(m_pTempItem)->boundingRect().width() < 3)
        delete m_pTempItem;
        m_pTempItem = nullptr;
        setMouseType(Nothing);
        m_polygon.clear();
        break;

    case RectangleSelection:
    {
        setMouseType(Nothing);
        return;
    }
    default:
        break;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void PathologyViewer::mouseMoveEvent(QMouseEvent* event)
{

    if (m_mouseType == Nothing)
    {

        QPointF imgLoc = this->mapToScene(event->pos()) / this->_sceneScale;
        qobject_cast<QMainWindow*>(this->parentWidget()->parentWidget())->statusBar()->showMessage(QStringLiteral("当前位置: (") + QString::number(imgLoc.x()) + QString(", ") + QString::number(imgLoc.y()) + QString(")"), 1000);
        if (this->_pan) {
            // qDebug() << "🖱️ Panning to:" << event->pos();
            pan(event->pos());
            event->accept();
            return;
        }
        QGraphicsView::mouseMoveEvent(event);
        return;

    }
    else if (m_mouseType == Line && m_pTempItem)
    {
        if (event->button() == Qt::RightButton)
        {
            return;
        }
        m_ptMove = event->pos();
        static_cast<LineRenderElement*>(m_pTempItem)->updateLine(mapToScene(m_ptOri), mapToScene(m_ptMove));
        emit perimeterUpdated(static_cast<LineRenderElement*>(m_pTempItem)->getPerimeter());
    }
    else if (m_mouseType == Rect && m_pTempItem)
   {
        if (event->button() == Qt::RightButton)
        {
            return;
        }
       m_ptMove = event->pos();
       QPoint endPoint = event->pos();
       qreal x = (std::min)(m_ptOri.x(), m_ptMove.x());
       qreal y = (std::min)(m_ptOri.y(), m_ptMove.y());
       QPointF ptTopLeft = mapToScene(x, y);
       qreal width = std::abs(m_ptOri.x() - m_ptMove.x());
       qreal height = std::abs(m_ptOri.y() - m_ptMove.y());
       QPointF ptBottomRight = mapToScene(x + width, y + height);
       static_cast<RectRenderElement*>(m_pTempItem)->updateRect(QRectF(ptTopLeft, ptBottomRight));
       emit areaAndPerimeterUpdated(static_cast<LineRenderElement*>(m_pTempItem)->getPerimeter(), static_cast<LineRenderElement*>(m_pTempItem)->getArea());

   }
    else if (m_mouseType == Ellipse && m_pTempItem)
        {
        if (event->button() == Qt::RightButton)
        {
            return;
        }
            m_ptMove = event->pos();
            QPoint endPoint = event->pos();
            qreal x = (std::min)(m_ptOri.x(), m_ptMove.x());
            qreal y = (std::min)(m_ptOri.y(), m_ptMove.y());
            QPointF ptTopLeft = mapToScene(x, y);
            qreal width = std::abs(m_ptOri.x() - m_ptMove.x());
            qreal height = std::abs(m_ptOri.y() - m_ptMove.y());
            QPointF ptBottomRight = mapToScene(x + width, y + height);
            static_cast<EllipseRenderElement*>(m_pTempItem)->updateRect(QRectF(ptTopLeft, ptBottomRight));
            emit areaAndPerimeterUpdated(static_cast<LineRenderElement*>(m_pTempItem)->getPerimeter(), static_cast<LineRenderElement*>(m_pTempItem)->getArea());
        }
    else if (m_mouseType == Contour && m_pTempItem)
        {
        if (event->button() == Qt::RightButton)
        {
            return;
        }
            m_ptMove = event->pos();
            m_polygon.push_back(mapToScene(m_ptMove));
            static_cast<ContourRenderElement*>(m_pTempItem)->updateContour(m_polygon);
            emit areaAndPerimeterUpdated(static_cast<LineRenderElement*>(m_pTempItem)->getPerimeter(), static_cast<LineRenderElement*>(m_pTempItem)->getArea());
        }

}

void PathologyViewer::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    m_actionDeleteSelection = new QAction(QStringLiteral("删除选择"), this);
    m_contextMenu->addAction(m_actionDeleteSelection);

    connect(m_actionDeleteSelection, &QAction::triggered, this, &PathologyViewer::onActionDeleteSelection);
}
void PathologyViewer::contextMenuEvent(QContextMenuEvent* event) {
    if (m_contextMenu) {
        m_contextMenu->exec(event->globalPos()); // 使用全屏菜单显示菜单
    }
    QGraphicsView::contextMenuEvent(event);
}

void PathologyViewer::onActionDeleteSelection()
{
    m_pGraphicsScene->deleteItems(m_pGraphicsScene->selectedItems());
}

#define DEBUGPOS 0

void PathologyViewer::changeViewPos()
{

    if (DEBUGPOS) return;



    // 定义四个角落的位置（考虑边距）
    /*QPoint firstPos(0,0);*/
    QPoint topRight(250,250);
    QPoint bottomRight(500,500);
    QPoint bottomLeft(750,750);

    QVector<QPoint> firstPos;
    firstPos.push_back(QPoint(0, 0));
    firstPos.push_back(QPoint(50, 50));
    firstPos.push_back(QPoint(50, 50));

    QVector<QPoint> secondPos;
    secondPos.push_back(QPoint(0, 980));
    secondPos.push_back(QPoint(50 ,1250));
    secondPos.push_back(QPoint(50, 1000));

    QVector<QPoint> thirdPos;
    thirdPos.push_back(QPoint(2250, 980));
    thirdPos.push_back(QPoint(2250, 1250));
    thirdPos.push_back(QPoint(2250, 1000));

    // 保存当前三个控件的位置
    static int rotationState = 0; // 0: 初始状态, 1: 顺时针旋转一次, 2: 顺时针旋转两次

    // 根据旋转状态设置新的位置
    switch (rotationState) {
    case 0: 
        _labelWin->setGeometry(QRect(firstPos[0], _labelWin->size()));
        _scaleBar->setGeometry(QRect(secondPos[1], _scaleBar->size()));
        //_map->setGeometry(QRect(thirdPos[2], _map->size()));
        break;
    case 1: 
        _labelWin->setGeometry(QRect(secondPos[0], _labelWin->size()));
        _scaleBar->setGeometry(QRect(secondPos[1], _scaleBar->size()));
        //_map->setGeometry(QRect(thirdPos[0], _map->size()));
        break;
    case 2: 
        _labelWin->setGeometry(QRect(thirdPos[0], _labelWin->size()));
        _scaleBar->setGeometry(QRect(firstPos[1], _scaleBar->size()));
        //_map->setGeometry(QRect(thirdPos[1], _map->size()));
        break;
    }

    // 更新旋转状态，准备下一次旋转
    rotationState = (rotationState + 1) % 3;


}

#include <QPropertyAnimation>

void PathologyViewer::changeViewPosWithAnimation(bool clockwise)
{
    // 获取父控件(QGraphicsView)的尺寸
    QRect parentRect = this->rect();

    // 获取三个控件的当前尺寸
    QSize labelWinSize = _labelWin->size();
    QSize scaleBarSize = _scaleBar->size();
    QSize mapSize = _map->size();

    // 定义四个角落的位置
    QPoint topLeft(0, 0);
    QPoint topRight(parentRect.width() - labelWinSize.width(), 0);
    QPoint bottomRight(parentRect.width() - mapSize.width(), parentRect.height() - mapSize.height());
    QPoint bottomLeft(0, parentRect.height() - scaleBarSize.height());

    // 保存当前三个控件的位置
    static int rotationState = 0;

    // 目标位置
    QRect labelWinTarget, scaleBarTarget, mapTarget;

    if (clockwise) {
        // 顺时针旋转
        switch (rotationState) {
        case 0:
            labelWinTarget = QRect(topLeft, labelWinSize);
            scaleBarTarget = QRect(topRight, scaleBarSize);
            mapTarget = QRect(bottomRight, mapSize);
            break;
        case 1:
            labelWinTarget = QRect(topRight, labelWinSize);
            scaleBarTarget = QRect(bottomRight, scaleBarSize);
            mapTarget = QRect(bottomLeft, mapSize);
            break;
        case 2:
            labelWinTarget = QRect(bottomRight, labelWinSize);
            scaleBarTarget = QRect(bottomLeft, scaleBarSize);
            mapTarget = QRect(topLeft, mapSize);
            break;
        }
    }
    else {
        // 逆时针旋转
        switch (rotationState) {
        case 0:
            labelWinTarget = QRect(topLeft, labelWinSize);
            scaleBarTarget = QRect(bottomLeft, scaleBarSize);
            mapTarget = QRect(bottomRight, mapSize);
            break;
        case 1:
            labelWinTarget = QRect(bottomLeft, labelWinSize);
            scaleBarTarget = QRect(bottomRight, scaleBarSize);
            mapTarget = QRect(topRight, mapSize);
            break;
        case 2:
            labelWinTarget = QRect(bottomRight, labelWinSize);
            scaleBarTarget = QRect(topRight, scaleBarSize);
            mapTarget = QRect(topLeft, mapSize);
            break;
        }
    }

    // 创建动画
    QPropertyAnimation* labelAnim = new QPropertyAnimation(_labelWin, "geometry");
    labelAnim->setDuration(300); // 300毫秒
    labelAnim->setStartValue(_labelWin->geometry());
    labelAnim->setEndValue(labelWinTarget);

    QPropertyAnimation* scaleBarAnim = new QPropertyAnimation(_scaleBar, "geometry");
    scaleBarAnim->setDuration(300);
    scaleBarAnim->setStartValue(_scaleBar->geometry());
    scaleBarAnim->setEndValue(scaleBarTarget);

    QPropertyAnimation* mapAnim = new QPropertyAnimation(_map, "geometry");
    mapAnim->setDuration(300);
    mapAnim->setStartValue(_map->geometry());
    mapAnim->setEndValue(mapTarget);

    // 启动动画
    labelAnim->start();
    scaleBarAnim->start();
    mapAnim->start();

    // 更新旋转状态
    rotationState = (rotationState + 1) % 3;
}

void PathologyViewer::paintEvent(QPaintEvent* event)
{
    QPainter painter(viewport());
    painter.fillRect(viewport()->rect(), Qt::black);
    QGraphicsView::paintEvent(event);
    m_frameCount++; // 每帧计数增加
}
