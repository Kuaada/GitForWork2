/**
 * @file    PathologyViewer.h
 * @brief   病理图像查看器主类，整合图像显示、交互和标注功能
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details 该文件实现了DSV项目的核心查看器功能，包括：
 *          - 多分辨率图像的加载和显示
 *          - 瓦片管理和异步加载
 *          - 用户交互和视图控制（缩放、平移、旋转）
 *          - 测量和标注功能
 *          - 前景图像叠加和LUT应用
 *          - 性能监控和FPS显示
 *          - 与UI组件的集成和通信
 *          该类是DSV项目的核心组件，整合了所有功能模块，
 *          为用户提供完整的病理图像浏览和标注体验。
 *
 * @note    该类继承自QGraphicsView，是DSV项目的主要视图组件
 * @see     TileManager, QImageGraphicScene, RenderElement, IOThread
 */

#pragma once

#include <QGraphicsView>
#include <vector>
#include <memory>
#include <QPainterPath>
#include <QContextMenuEvent>
#include <QTimer>
#include <QDebug>

 // 前向声明
class MultiResolutionImage;
class IOThread;
class PrefetchThread;
class MiniMap;
class WSITileGraphicsItemCache;
class TileManager;
class ScaleBar;
class DrawTool;
class QGraphicsItem;
class QGraphicsLineItem;

class InputDialog;
class LabelWin;
class DetailDialog;
class QListWidget;
class QListWidgetItem;
class ItemDialog;
class QImageGraphicScene;
class QMenu;

/**
 * @enum   MouseType
 * @brief  鼠标操作类型枚举，定义不同的交互模式
 * @details 该枚举定义了用户在图像上可以进行的各种操作类型，
 *          包括绘制工具、选择工具等。
 */
enum MouseType {
    Nothing = -1,           // 无操作状态
    Line = 0,               // 直线绘制工具
    Rect = 1,               // 矩形绘制工具
    Ellipse = 2,            // 椭圆绘制工具
    Text = 3,               // 文本标注工具
    Contour = 4,            // 轮廓绘制工具
    RectangleSelection = 12 // 矩形选择工具
};

namespace SlideColorManagement {
    struct LUT; // 前向声明LUT结构体
}

/**
 * @class  PathologyViewer
 * @brief  病理图像查看器主类，整合图像显示、交互和标注功能
 * @details 该类是DSV项目的核心查看器类，负责：
 *          - 多分辨率图像的加载、显示和管理
 *          - 瓦片系统的协调和异步加载
 *          - 用户交互处理（鼠标、键盘、滚轮事件）
 *          - 视图变换（缩放、平移、旋转）
 *          - 前景图像叠加和颜色管理
 *          - 测量和标注工具
 *          - 性能监控和优化
 *          - 与UI组件的通信和集成
 *
 *          主要功能包括：
 *          - 图像显示：支持多分辨率图像的流畅浏览
 *          - 交互控制：鼠标拖拽、滚轮缩放、键盘导航
 *          - 标注工具：直线、矩形、椭圆、文本、轮廓等绘制工具
 *          - 前景叠加：支持前景图像的透明叠加和LUT应用
 *          - 性能优化：瓦片缓存、异步加载、FPS监控
 *          - 辅助功能：小地图、比例尺、覆盖度显示
 *
 * @note   该类继承自QGraphicsView，是DSV项目的主要视图组件
 * @example
 *          // 使用示例
 *          PathologyViewer* viewer = new PathologyViewer(parent);
 *
 *          // 初始化图像
 *          viewer->initialize(multiResImage);
 *
 *          // 设置交互参数
 *          viewer->setZoomSensitivity(1.2f);
 *          viewer->setPanSensitivity(1.0f);
 *
 *          // 设置前景图像
 *          viewer->setForegroundOpacity(0.7f);
 *          viewer->setForegroundChannel(1);
 *
 *          // 设置绘制工具
 *          viewer->setMouseType(MouseType::Rect);
 * @see     TileManager, QImageGraphicScene, RenderElement, IOThread
 */
class PathologyViewer : public QGraphicsView
{
    Q_OBJECT

public:
    /** @brief 缩放到场景坐标位置 */
    QPointF _zoomToScenePos;

    /** @brief 缩放到视图坐标位置 */
    QPointF _zoomToViewPos;

    /**
     * @brief   构造函数
     * @details 创建病理图像查看器对象
     *
     * @param   parent 父窗口指针，默认为0
     * @note    构造函数会初始化基本的UI组件和状态
     * @see     ~PathologyViewer, initialize
     */
    PathologyViewer(QWidget* parent = 0);

    /**
     * @brief   析构函数
     * @details 清理病理图像查看器资源
     * @note    析构函数会停止所有线程并清理资源
     * @see     close
     */
    ~PathologyViewer();

    /**
     * @brief   初始化查看器
     * @details 使用多分辨率图像初始化查看器
     *
     * @param   img 多分辨率图像对象的共享指针
     * @note    该函数会设置图像、初始化瓦片管理器等组件
     * @see     close, reset
     */
    void initialize(std::shared_ptr<MultiResolutionImage> img);

    /**
     * @brief   关闭查看器
     * @details 关闭当前图像并清理相关资源
     * @note    该函数会停止所有线程并清理图像资源
     * @see     initialize
     */
    void close();

    /**
     * @brief   获取缩放灵敏度
     * @details 返回当前的缩放灵敏度设置
     *
     * @return  缩放灵敏度值
     * @note    缩放灵敏度影响滚轮缩放的响应速度
     * @see     setZoomSensitivity
     */
    float getZoomSensitivity() const;

    /**
     * @brief   设置缩放灵敏度
     * @details 设置缩放操作的灵敏度
     *
     * @param   zoomSensitivity 新的缩放灵敏度值
     * @note    值越大，缩放响应越敏感
     * @see     getZoomSensitivity
     */
    void setZoomSensitivity(float zoomSensitivity);

    /**
     * @brief   设置平移灵敏度
     * @details 设置平移操作的灵敏度
     *
     * @param   panSensitivity 新的平移灵敏度值
     * @note    值越大，平移响应越敏感
     * @see     getPanSensitivity
     */
    void setPanSensitivity(float panSensitivity);

    /**
     * @brief   获取平移灵敏度
     * @details 返回当前的平移灵敏度设置
     *
     * @return  平移灵敏度值
     * @note    平移灵敏度影响鼠标拖拽的响应速度
     * @see     setPanSensitivity
     */
    float getPanSensitivity() const;

    /**
     * @brief   获取前景透明度
     * @details 返回前景图像的当前透明度
     *
     * @return  透明度值（0.0-1.0）
     * @note    透明度影响前景图像的显示效果
     * @see     setForegroundOpacity
     */
    float getForegroundOpacity() const;

    /**
     * @brief   设置前景透明度
     * @details 设置前景图像的透明度
     *
     * @param   opacity 新的透明度值（0.0-1.0）
     * @note    透明度设置会影响前景图像的显示效果
     * @see     getForegroundOpacity
     */
    void setForegroundOpacity(const float& opacity);

    /**
     * @brief   设置前景LUT
     * @details 设置前景图像的颜色查找表
     *
     * @param   LUT 颜色查找表对象
     * @note    LUT设置会影响前景图像的颜色显示
     * @see     setForegroundChannel
     */
    void setForegroundLUT(const SlideColorManagement::LUT& LUT);

    /**
     * @brief   设置前景通道
     * @details 设置前景图像的显示通道
     *
     * @param   channel 通道索引
     * @note    通道设置会影响前景图像的显示内容
     * @see     setForegroundLUT
     */
    void setForegroundChannel(unsigned int channel);

    /**
     * @brief   设置前景渲染开关
     * @details 启用或禁用前景图像的渲染
     *
     * @param   enableForegroundRendering 是否启用前景渲染
     * @note    该设置会影响前景图像的显示状态
     */
    void setEnableForegroundRendering(bool enableForegroundRendering);

    /**
     * @brief   切换平移模式
     * @details 启用或禁用平移模式
     *
     * @param   pan 是否启用平移模式
     * @param   startPos 平移起始位置，默认为空
     * @note    平移模式允许用户拖拽图像
     * @see     pan, isPanning
     */
    void togglePan(bool pan, const QPoint& startPos = QPoint());

    /**
     * @brief   执行平移操作
     * @details 将视图平移到指定位置
     *
     * @param   panTo 目标平移位置
     * @note    该函数会立即执行平移操作
     * @see     togglePan, isPanning
     */
    void pan(const QPoint& panTo);

    /**
     * @brief   检查是否正在平移
     * @details 返回当前是否处于平移模式
     *
     * @return  true表示正在平移，false表示未平移
     * @see     togglePan, pan
     */
    bool isPanning();

    /**
     * @brief   执行缩放操作
     * @details 根据步数执行缩放操作
     *
     * @param   numSteps 缩放步数，正数放大，负数缩小
     * @note    缩放操作会改变视图的显示比例
     */
    void zoom(float numSteps);

    /**
     * @brief   获取场景缩放比例
     * @details 返回当前场景的缩放比例
     *
     * @return  场景缩放比例
     * @note    缩放比例表示当前显示相对于原始图像的大小
     */
    float getSceneScale() { return _sceneScale; }

    /**
     * @brief   获取缓存大小
     * @details 返回当前瓦片缓存的大小
     *
     * @return  缓存大小（字节）
     * @note    缓存大小影响内存使用和性能
     * @see     setCacheSize
     */
    unsigned long long getCacheSize();

    /**
     * @brief   设置缓存大小
     * @details 设置瓦片缓存的最大大小
     *
     * @param   maxCacheSize 最大缓存大小（字节）
     * @note    缓存大小设置会影响内存使用和性能
     * @see     getCacheSize
     */
    void setCacheSize(unsigned long long& maxCacheSize);

    /**
     * @brief   更新当前视场
     * @details 更新当前显示的视场信息
     * @note    该函数会重新计算和更新视场数据
     */
    void updateCurrentFieldOfView();

    /**
     * @brief   重置视图
     * @details 将视图重置到初始状态
     * @note    该函数会重置缩放、平移等视图变换
     * @see     resetTime, resetFinished
     */
    void reset();

    /**
     * @brief   重置动画时间
     * @details 重置视图的动画时间参数
     *
     * @param   x 时间参数
     * @note    该函数用于重置动画的进度
     * @see     reset, resetFinished
     */
    void resetTime(qreal x);

    /**
     * @brief   重置完成
     * @details 重置操作完成后的处理
     * @note    该函数在重置动画完成后调用
     * @see     reset, resetTime
     */
    void resetFinished();

    /**
     * @brief   缩放到固定倍率
     * @details 将视图缩放到指定的固定倍率
     *
     * @param   targetMagnification 目标倍率
     * @note    该函数会执行平滑的缩放动画
     * @see     zoomToFixedScaleTime, zoomToFixedScaleFinished
     */
    void zoomToFixedMagnification(float targetMagnification);

    /**
     * @brief   固定缩放动画时间
     * @details 固定缩放动画的时间参数
     *
     * @param   x 时间参数
     * @note    该函数用于控制缩放动画的进度
     * @see     zoomToFixedMagnification, zoomToFixedScaleFinished
     */
    void zoomToFixedScaleTime(qreal x);

    /**
     * @brief   固定缩放完成
     * @details 固定缩放操作完成后的处理
     * @note    该函数在固定缩放动画完成后调用
     * @see     zoomToFixedMagnification, zoomToFixedScaleTime
     */
    void zoomToFixedScaleFinished();

    /**
     * @brief   设置绘制状态
     * @details 启用或禁用绘制模式
     *
     * @param   state 绘制状态，true启用，false禁用
     * @note    绘制状态影响用户是否可以绘制标注
     */
    void setPaintState(bool state);

    /**
     * @brief   设置鼠标类型
     * @details 设置当前的鼠标操作类型
     *
     * @param   type 鼠标操作类型枚举值
     * @note    鼠标类型决定了用户的交互模式
     * @see     MouseType
     */
    void setMouseType(MouseType type);

    /**
     * @brief   设置图形场景
     * @details 设置查看器使用的图形场景
     *
     * @param   pScene 图形场景指针
     * @note    图形场景负责管理渲染元素
     * @see     QImageGraphicScene
     */
    void setGraphicsScene(QImageGraphicScene* pScene);

    /**
     * @brief   获取当前FPS
     * @details 返回当前的帧率
     *
     * @return  当前FPS值
     * @note    FPS用于监控查看器的性能
     */
    int fps() const { return m_currentFPS; }

signals:
    /**
     * @brief   视场改变信号
     * @details 当视场发生变化时发出此信号
     *
     * @param   FOV 新的视场矩形
     * @param   level 当前层级
     * @note    该信号用于通知其他组件视场变化
     */
    void fieldOfViewChanged(const QRectF& FOV, const unsigned int level);

    /**
     * @brief   更新边界框信号
     * @details 当需要更新边界框时发出此信号
     *
     * @param   FOV 视场矩形
     * @note    该信号用于更新UI中的边界框显示
     */
    void updateBBox(const QRectF& FOV);

    /**
     * @brief   背景通道改变信号
     * @details 当背景通道改变时发出此信号
     *
     * @param   channelNr 新的通道编号
     * @note    该信号用于通知其他组件通道变化
     */
    void backgroundChannelChanged(int channelNr);

    /**
     * @brief   右键点击信号
     * @details 当用户右键点击时发出此信号
     * @note    该信号用于触发右键菜单
     */
    void rightClicked();

    /**
     * @brief   视图显示信号
     * @details 当需要显示视图时发出此信号
     *
     * @param   pix 要显示的像素图
     * @note    该信号用于更新UI中的视图显示
     */
    void viewShow(const QPixmap& pix);

    /**
     * @brief   因子变换信号
     * @details 当缩放因子改变时发出此信号
     *
     * @param   factor 缩放因子
     * @note    该信号用于通知其他组件缩放变化
     */
    void factorTrans(float factor);

    /**
     * @brief   初始化完成信号
     * @details 当初始化完成时发出此信号
     * @note    该信号用于通知其他组件初始化状态
     */
    void initOver();

    /**
     * @brief   微米每像素变换信号
     * @details 当微米每像素值改变时发出此信号
     *
     * @param   num 微米每像素值
     * @note    该信号用于通知其他组件比例变化
     */
    void mppTrans(float num);

    /**
     * @brief   FPS更新信号
     * @details 当FPS更新时发出此信号
     *
     * @param   fps 当前FPS值
     * @note    该信号用于更新UI中的FPS显示
     */
    void fpsUpdated(int fps);

    /**
    * @brief   返回绘制项长度值\周长等
    * @details 当绘制项更新发送值
    * @param   perimeter 当前\长度值周长值
    * @note    该信号用于更新UI中的绘制项的周长\长度
    */
    void perimeterUpdated(float perimeter);
    /**
    * @brief   返回绘制项面积
    * @details 当直线更新绘制的时候发送值
    * @param   Area 当前面积值
    * @note    该信号用于更新UI中的绘制项的面积
    */
    void areaAndPerimeterUpdated(float perimeter, float Area);


public slots:
    /**
     * @brief   移动到指定位置
     * @details 将视图中心移动到指定位置
     *
     * @param   pos 目标位置
     * @note    该函数会平滑地移动视图到指定位置
     */
    void moveTo(const QPointF& pos);

    /**
     * @brief   视场改变槽函数
     * @details 当视场改变时调用此槽函数
     *
     * @param   FOV 新的视场矩形
     * @param   level 当前层级
     * @note    该槽函数用于处理视场变化
     */
    void onFieldOfViewChanged(const QRectF& FOV, const unsigned int level);

    /**
     * @brief   前景图像改变槽函数
     * @details 当前景图像改变时调用此槽函数
     *
     * @param   for_img 前景图像的弱引用
     * @param   scale 前景图像的缩放因子
     * @note    该槽函数用于更新前景图像
     */
    void onForegroundImageChanged(std::weak_ptr<MultiResolutionImage> for_img, float scale);

    /**
     * @brief   设置文件窗口状态
     * @details 设置文件窗口的显示状态
     *
     * @param   state 显示状态，true显示，false隐藏
     * @note    该函数用于控制文件窗口的可见性
     */
    void setFileWidgetState(bool state);

    /**
     * @brief   设置小地图可见性
     * @details 设置小地图的显示状态
     *
     * @param   state 显示状态，true显示，false隐藏
     * @note    该函数用于控制小地图的可见性
     */
    void setMiniMapVisible(bool state);

    /**
     * @brief   设置覆盖度区域显示
     * @details 设置覆盖度区域的显示状态
     *
     * @param   state 显示状态，true显示，false隐藏
     * @note    该函数用于控制覆盖度区域的可见性
     */
    void setCoverageArea(bool state);

    /**
     * @brief   设置比例尺显示
     * @details 设置比例尺的显示状态
     *
     * @param   state 显示状态，true显示，false隐藏
     * @note    该函数用于控制比例尺的可见性
     */
    void setScaleBar(bool state);

    /**
     * @brief   更新快照
     * @details 更新当前的快照显示
     * @note    该函数用于刷新快照功能
     */
    void updateSnap();

    /**
     * @brief   设置标签地图可见性
     * @details 设置标签地图的显示状态
     * @note    该函数用于控制标签地图的可见性
     */
    void setLabelMapVisible();

    /**
     * @brief   设置详情窗口可见性
     * @details 设置详情窗口的显示状态
     * @note    该函数用于控制详情窗口的可见性
     */
    void setDetailVisible();

    /**
     * @brief   设置微米每像素值
     * @details 设置当前的微米每像素值
     *
     * @param   mpp 微米每像素值
     * @note    该函数用于更新比例信息
     */
    void setChangedMpp(float mpp);
    /**
     * @brief   删除选择槽函数
     * @details 处理删除选择的操作
     * @note    该槽函数用于删除当前选中的元素
     */
    void onActionDeleteSelection();

private:
    /** @brief 是否首次右键点击，用于关闭文件窗口 */
    bool _isFirstRightClick;

    // 平移相关函数
    /**
     * @brief   鼠标移动事件处理
     * @details 处理鼠标移动事件，实现平移和绘制功能
     *
     * @param   event 鼠标事件对象
     * @note    该函数处理鼠标拖拽和实时绘制
     */
    virtual void mouseMoveEvent(QMouseEvent* event);

    /**
     * @brief   鼠标按下事件处理
     * @details 处理鼠标按下事件，开始平移或绘制
     *
     * @param   event 鼠标事件对象
     * @note    该函数处理鼠标按下时的交互
     */
    virtual void mousePressEvent(QMouseEvent* event);

    /**
     * @brief   鼠标释放事件处理
     * @details 处理鼠标释放事件，结束平移或绘制
     *
     * @param   event 鼠标事件对象
     * @note    该函数处理鼠标释放时的交互
     */
    virtual void mouseReleaseEvent(QMouseEvent* event);

    /**
     * @brief   窗口大小改变事件处理
     * @details 处理窗口大小改变事件
     *
     * @param   event 大小改变事件对象
     * @note    该函数会重新调整视图布局
     */
    virtual void resizeEvent(QResizeEvent* event);

    /**
     * @brief   键盘按下事件处理
     * @details 处理键盘按下事件，忽略方向键
     *
     * @param   event 键盘事件对象
     * @note    该函数会忽略方向键事件，其他按键按默认方式处理
     */
    void keyPressEvent(QKeyEvent* event) override {
        // 检查按键是否为方向键
        if (event->key() == Qt::Key_Left ||
            event->key() == Qt::Key_Right ||
            event->key() == Qt::Key_Up ||
            event->key() == Qt::Key_Down) {
            // 忽略这些事件，不进行默认处理
            event->ignore();
            return;
        }
        // 其他按键按默认方式处理
        QGraphicsView::keyPressEvent(event);
    }

    /**
     * @brief   右键菜单事件处理
     * @details 处理右键菜单事件，显示上下文菜单
     *
     * @param   event 右键菜单事件对象
     * @note    该函数会显示自定义的右键菜单
     */
    void contextMenuEvent(QContextMenuEvent* event) override;

    /**
     * @brief   滚轮事件处理
     * @details 处理滚轮事件，实现缩放功能
     *
     * @param   event 滚轮事件对象
     * @note    该函数处理鼠标滚轮的缩放操作
     */
    void wheelEvent(QWheelEvent* event);

    // 小地图相关函数
    /**
     * @brief   初始化GUI组件
     * @details 根据层级初始化GUI组件
     *
     * @param   level 层级索引
     * @note    该函数会初始化小地图等GUI组件
     */
    void initializeGUIComponents(unsigned int level);

    /**
     * @brief   初始化图像
     * @details 初始化图像显示和瓦片管理
     *
     * @param   scn 图形场景指针
     * @param   tileSize 瓦片大小
     * @param   lastLevel 最后层级
     * @note    该函数会设置图像和瓦片管理器
     */
    void initializeImage(QGraphicsScene* scn, unsigned int tileSize, unsigned int lastLevel);

    /** @brief 场景缩放比例 */
    float _sceneScale;

    /**
     * @brief 多分辨率图像接口
     * @details 请注意，PathologyViewer不应该修改_img
     *          （它是在RenderThread中由于调用readRegion而修改的），
     *          否则可能发生竞态条件。
     */
    std::shared_ptr<MultiResolutionImage> _img;

    /** @brief 前景图像的弱引用 */
    std::weak_ptr<MultiResolutionImage> _for_img;

    /** @brief 小地图组件指针 */
    MiniMap* _map;

    /** @brief 比例尺组件指针 */
    ScaleBar* _scaleBar;

    // 平移和缩放跟踪成员
    /** @brief 缩放灵敏度 */
    float _zoomSensitivity;

    /** @brief 计划缩放次数 */
    float _numScheduledScalings;

    /** @brief 平移灵敏度 */
    float _panSensitivity;

    /** @brief 是否正在平移 */
    bool _pan;

    /** @brief 前一次平移位置 */
    QPoint _prevPan;

    // 渲染相关成员
    /** @brief IO线程指针 */
    IOThread* _ioThread;

    /** @brief 背景通道索引 */
    int _backgroundChannel;

    /** @brief 前景通道索引 */
    int _foregroundChannel;

    /** @brief 透明度值 */
    float _opacity;

    /** @brief LUT名称 */
    std::string _LUTname;

    /** @brief 前景图像缩放因子 */
    float _foregroundImageScale;

    /** @brief 是否渲染前景 */
    bool _renderForeground;

    /** @brief 预取线程指针 */
    PrefetchThread* _prefetchthread;

    /** @brief 瓦片管理器指针 */
    TileManager* _manager;

    /** @brief 缓存大小 */
    unsigned long long _cacheSize;

    /** @brief 瓦片图形项缓存指针 */
    WSITileGraphicsItemCache* _cache;

    // 初始状态
    /** @brief 初始变换矩阵 */
    QTransform _initialTransform;

    /** @brief 初始中心位置 */
    QPointF _initialCenter;

    // 固定倍率缩放
    /** @brief 固定缩放初始中心位置 */
    QPointF _initialCenterFixedScale;

    /** @brief 目标变换矩阵 */
    QTransform _targetTransform;

    // 对话框组件
    /** @brief 输入对话框指针 */
    InputDialog* _InputDialog;

    /** @brief 标签窗口指针 */
    LabelWin* _labelWin;

    /** @brief 详情对话框指针 */
    DetailDialog* _detailDialog;

    /** @brief 绘制状态 */
    bool _PaintingState;

    // 绘制相关成员
    /** @brief 多边形数据 */
    QPolygonF m_polygon;

    /** @brief 当前鼠标类型 */
    MouseType m_mouseType;

    /** @brief 临时图形项指针 */
    QGraphicsItem* m_pTempItem;

    /** @brief 右键菜单指针 */
    QMenu* m_contextMenu;

    /** @brief 删除选择动作指针 */
    QAction* m_actionDeleteSelection;

    /** @brief 图形场景指针 */
    QImageGraphicScene* m_pGraphicsScene;

    /** @brief 实时绘制画笔 */
    QPen m_penRealTime;

    /** @brief 原始位置 */
    QPoint m_ptOri;

    /** @brief 移动位置 */
    QPoint m_ptMove;

    /** @brief 左键是否按下 */
    bool m_bLButtonDown = false;

    /**
     * @brief   绘制事件处理
     * @details 处理绘制事件，实现自定义绘制
     *
     * @param   event 绘制事件对象
     * @note    该函数用于实现自定义的绘制功能
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief   创建右键菜单
     * @details 创建和初始化右键菜单
     * @note    该函数会创建包含各种操作选项的右键菜单
     */
    void createContextMenu();

    /** @brief FPS定时器 */
    QTimer m_fpsTimer{ this };

    /** @brief 帧计数器 */
    int m_frameCount;

    /** @brief 当前FPS值 */
    int m_currentFPS = 0;

private slots:
    /**
     * @brief   缩放动画时间槽函数
     * @details 处理缩放动画的时间参数
     *
     * @param   x 时间参数
     * @note    该槽函数用于控制缩放动画的进度
     */
    void scalingTime(qreal x);

    /**
     * @brief   缩放完成槽函数
     * @details 缩放操作完成后的处理
     * @note    该槽函数在缩放动画完成后调用
     */
    void zoomFinished();

    /**
     * @brief   点击选中操作
     * @details 细分选中Item的内容是什么
     * @note    该函数在点击事件中调用
     */
    void handleItemSelection(QGraphicsItem* item);

    /**
     * @brief   更新FPS槽函数
     * @details 更新当前的FPS值并发出信号
     * @note    该槽函数由FPS定时器定期调用
     */
    void updateFPS() {
        m_currentFPS = m_frameCount;
        m_frameCount = 0; // 重置计数器
        emit fpsUpdated(m_currentFPS); // 发出信号更新UI
        qDebug() << "FPS:" << m_currentFPS;
    }
    /**
    * @brief   直线长度中转
    * @details 更新当前的FPS值并发出信号
    * @note    该槽函数由FPS定时器定期调用
    */

};