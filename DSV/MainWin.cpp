/**
 * @file MainWin.cpp
 * @brief 主窗口实现文件
 * @details 实现数字病理切片查看器的主窗口界面，包括工具栏、文件管理、状态栏等核心UI组件
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "MainWin.h"
#include <QDebug>
#include "MultiResolutionImage.h"
#include "ScaleBar.h"

/**
 * @brief 主窗口构造函数
 * @param parent 父窗口指针
 * @details 初始化主窗口界面，设置工具栏、文件管理组件等
 */
MainWin::MainWin(QWidget *parent)
	: QMainWindow(parent)
{
	initUi();           // 初始化用户界面
	initConnect();      // 初始化信号槽连接
	installEventFilter(this);  // 安装事件过滤器
	setupStyle();       // 设置样式
}

/**
 * @brief 主窗口析构函数
 * @details 清理资源，释放内存
 */
MainWin::~MainWin()
{
	// 析构函数实现
}

/**
 * @brief 窗口大小改变事件处理
 * @param event 大小改变事件对象
 * @details 当窗口大小改变时，重新调整各个UI组件的位置和大小
 */
void MainWin::resizeEvent(QResizeEvent* event)
{
	// 调整文件树按钮位置
	QPushButton* button = this->findChild<QPushButton*>("FileTreeButton");
	button->move(this->size().width() * 0.05, this->size().height() * 0.07);
	
	// 调整工具栏位置，使其居中显示
	int windowWidth = width();
	int toolBarWidth = m_ToolBar->width();
	int newX = (windowWidth - toolBarWidth) / 2;
	m_ToolBar->move(newX, 0);

	// 调整文件窗口高度
	int windowHeight = height();
	m_FileWidget->setFixedHeight(windowHeight);

	// 根据窗口高度调整文件树标签显示
	QLabel* FileTreeLabel = this->findChild<QLabel*>("FileTreeLabel");
	if (FileTreeLabel) {
		// 设定窗口高度阈值
		const int heightThreshold = 786;
		if (windowHeight > heightThreshold) {
			FileTreeLabel->setText(QStringLiteral("文件树"));
		}
		else {
			FileTreeLabel->setText("");
		}
	}

	// 调整绘图工具栏位置
	QList<QToolButton*> toolButtons = m_ToolBar->findChildren<QToolButton*>();
	PlotBar* plotBar = this->findChild<PlotBar*>("PlotBarWidget");
	for (QToolButton* button : toolButtons) {
		if (button->text() == "tagAction") {
			// 获取按钮在工具栏中的位置
			QPoint posInToolBar = button->pos();
			// 将位置转换到主窗口坐标系
			QPoint posInThis = m_ToolBar->mapTo(this, posInToolBar);
			if (plotBar)
			{
				int newPosX = posInThis.x() - plotBar->width()/2 + 40;//35button+5margin
				int newPosY = m_ToolBar->height();
				plotBar->move(newPosX, newPosY+5);
			}
			break;
		}
	}

	// 调用父类的resizeEvent处理
	QWidget::resizeEvent(event);
}


/**
 * @brief 初始化用户界面
 * @details 创建和配置主窗口的所有UI组件，包括病理查看器、工具栏、文件管理等
 */
void MainWin::initUi()
{
	// 设置窗口基本属性
	if (this->objectName().isEmpty()) {
		this->setObjectName(QStringLiteral("DSV"));
	}
	this->setMinimumSize(1037, 786);//最小窗口尺寸
	this->setTabPosition(Qt::DockWidgetArea::LeftDockWidgetArea, QTabWidget::East);
	this->setTabPosition(Qt::DockWidgetArea::RightDockWidgetArea, QTabWidget::West);
	
	// 设置窗口大小策略
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
	this->setSizePolicy(sizePolicy);
	this->setWindowIcon(QIcon(":/resources/DSV.png"));
	
	// 创建中央窗口部件
	centralWidget = new QWidget(this);
	pathologyView = new PathologyViewer(centralWidget);
	pathologyView->setObjectName("pathologyView");

	// 创建网格布局
	QGridLayout* layout = new QGridLayout(centralWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	// 将病理查看器添加到布局中
	layout->addWidget(pathologyView);

	this->setCentralWidget(centralWidget);

	// 设置工具栏
	m_ToolBar = new CenteredToolBar(this);
	m_ToolBar->setObjectName(QStringLiteral("ToolBar"));
	m_ToolBar->show();

	// 创建文件按钮
	m_FileButton = new QPushButton(this);
	m_FileButton->setObjectName(QStringLiteral("FileTreeButton"));
	m_FileButton->setFixedSize(50, 50);
	m_FileButton->setIcon(QIcon(":/resources/openData.png"));
	m_FileButton->setStyleSheet(
		{
			"QPushButton {"
		"   border-radius: 25px;"   // 圆角半径（宽度的一半）
		"   border: 2px solid #999;"
		"   background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);"
		"}"
		"QPushButton:hover {"
		"   background-color: white;" // 悬停时的背景颜色
		"}"
		"QPushButton:pressed {"
		"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);"
		"}"
		}
	);

	// 创建文件窗口部件
	m_FileWidget = new FileWidget(this);
	m_FileWidget->setObjectName(QStringLiteral("FileWidget"));
	m_FileWidget->setStyleSheet(
		"#FileWidget{"
		"   background-color: white;"
		"}"
	);
	m_FileWidget->show();

	// 创建状态栏
	statusBar = new QStatusBar(this);
	statusBar->setObjectName(QStringLiteral("statusBar"));
	this->setStatusBar(statusBar);//设置状态栏
	statusBar->show();
	
	// 创建截图对话框
	m_SnapDialog = new SnapDialog(this);
	m_SnapDialog->hide();

	// 创建绘图工具栏
	m_PlotBar = new PlotBar(this);
	m_PlotBar->hide();
	
	// 创建图像过滤器
	m_ImageFilter = new ImageFilter(this);
	m_ImageFilter->hide();
}

/**
 * @brief 设置样式
 * @details 配置窗口的视觉样式
 */
void MainWin::setupStyle()
{
	// 样式设置实现
}

/**
 * @brief 初始化信号槽连接
 * @details 建立各个UI组件之间的信号槽连接，实现交互功能
 */
void MainWin::initConnect()
{
	// 文件按钮点击事件
	connect(m_FileButton, &QPushButton::clicked, this, &MainWin::setFilePathTree);
	
	// 文件窗口路径传输信号
	connect(m_FileWidget, &FileWidget::filePathTrans, this, &MainWin::onOpenFile);
	
	// 病理查看器右键点击信号
	connect(pathologyView, &PathologyViewer::rightClicked, this, &MainWin::setFilePathTree);
	
	// 文件窗口状态信号
	connect(this, &MainWin::fileWidgetSate, pathologyView, &PathologyViewer::setFileWidgetState);
	
	// 病理查看器MPP传输信号
	PathologyViewer* view = this->findChild<PathologyViewer*>("pathologyView");

	connect(view, &PathologyViewer::mppTrans, m_ToolBar, &CenteredToolBar::setMpp);
	
	// 视图显示信号
	connect(view, &PathologyViewer::viewShow, m_SnapDialog, &SnapDialog::drawPic);
	
	// 初始化完成信号
	connect(view, &PathologyViewer::initOver, this, &MainWin::setEnableToolBar);
	
	// 缩放因子传输信号
	connect(view, &PathologyViewer::factorTrans, m_ToolBar, &CenteredToolBar::setFactor);
	
	// FPS更新信号
	connect(view, &PathologyViewer::fpsUpdated, [this](int fps) {qDebug() << "Fps:" << fps;});
	
	// MPP改变信号
	connect(m_ToolBar, &CenteredToolBar::TransChangedMpp, view, &PathologyViewer::setChangedMpp);
	connect(m_ToolBar, &CenteredToolBar::TransChangedMpp, [view](float data) {
		view->setChangedMpp(data);
		});
	
	// 截图窗口更新信号
	SnapWidget* widgetShow = m_SnapDialog->findChild<SnapWidget*>("SnapWidget");
	connect(widgetShow, &SnapWidget::widgetUpdate, view, &PathologyViewer::updateSnap);
	
	// 标签信息复选框
	QCheckBox* label = m_SnapDialog->findChild<QCheckBox*>(QStringLiteral("labelInfo"));
	connect(label, &QCheckBox::toggled, view, &PathologyViewer::setLabelMapVisible);
	
	// 比例尺复选框
	QCheckBox* scalebar = m_SnapDialog->findChild<QCheckBox*>(QStringLiteral("scaleBar"));
	connect(scalebar, &QCheckBox::toggled, view, &PathologyViewer::setScaleBar);
	
	// 小地图复选框
	QCheckBox* minimap = m_SnapDialog->findChild<QCheckBox*>(QStringLiteral("miniMap"));
	connect(minimap, &QCheckBox::toggled, view, &PathologyViewer::setMiniMapVisible);
	
	//位置按钮复选框
	QPushButton* changeBtn = m_SnapDialog->findChild<QPushButton*>(QStringLiteral("changePos"));
	connect(changeBtn, &QPushButton::clicked, view, &PathologyViewer::changeViewPos);

	// 绘图工具栏按钮连接
	QToolButton* cancel = m_PlotBar->findChild<QToolButton*>("Cancel");
	connect(cancel, &QToolButton::clicked, [view]() {view->setMouseType(MouseType::Nothing);});
	
	QToolButton* lineTool = m_PlotBar->findChild<QToolButton*>("LineTool");
	connect(lineTool, &QToolButton::clicked, [view]() {view->setMouseType(MouseType::Line);});
	
	QToolButton* rectTool = m_PlotBar->findChild<QToolButton*>("RectTool");
	connect(rectTool, &QToolButton::clicked, [view]() {view->setMouseType(MouseType::Rect);});
	
	QToolButton* circleTool = m_PlotBar->findChild<QToolButton*>("CircleTool");
	connect(circleTool, &QToolButton::clicked, [view]() {view->setMouseType(MouseType::Ellipse);});
	
	QToolButton* pathTool = m_PlotBar->findChild<QToolButton*>("PathTool");
	connect(pathTool, &QToolButton::clicked, [view]() {view->setMouseType(MouseType::Contour);});
	
	QToolButton* textTool = m_PlotBar->findChild<QToolButton*>("TextTool");
	connect(textTool, &QToolButton::clicked, [view]() {view->setMouseType(MouseType::Text);});
	
	QToolButton* deleteTool = m_PlotBar->findChild<QToolButton*>("DeleteTool");
	connect(deleteTool, &QToolButton::clicked, [view]() {view->onActionDeleteSelection();}); 

	connect(view, &PathologyViewer::perimeterUpdated, m_PlotBar, &PlotBar::setOneLabel);
	connect(view, &PathologyViewer::areaAndPerimeterUpdated, m_PlotBar, &PlotBar::setTwoLabel);
	// 比例尺和工具栏连接
	ScaleBar* scaleBar = view->findChild<ScaleBar*>("ScaleBar");
	CenteredToolBar* toolBar = this->findChild<CenteredToolBar*>("ToolBar");
	QList<QAction*> actions = toolBar->actions();
	for (QAction* action : actions) {
	    if (action->objectName() == "miniAction") {
			action->setChecked(true);
			connect(action, &QAction::triggered, [minimap]() {minimap->setChecked(!minimap->isChecked());});
	    }
		else if (action->objectName() == "detailAction")
		{
			connect(action, &QAction::triggered, view, &PathologyViewer::setDetailVisible);
		}
		else if (action->objectName() == "dealAction")
		{
			connect(action, &QAction::triggered, [this]() {
				if (m_ImageFilter->isHidden())
				{
					m_ImageFilter->show();
				}
				else
				{
					m_ImageFilter ->hide();
				}
				});

		}
		else if (action->objectName()=="labelAction")
		{
			connect(action, &QAction::triggered, [label]()
				{
					label->setChecked(!label->isChecked());
				});

		}
		else if(action->objectName() == "resetAction")
		{
			connect(action, &QAction::triggered, view, &PathologyViewer::reset);
		}
		else if(action->objectName() == "X2Action")
		{
			connect(action, &QAction::triggered, [view]() {
				view->zoomToFixedMagnification(2.);
				});
		}
		else if (action->objectName() == "X5Action")
		{
			connect(action, &QAction::triggered, [view]() {
				view->zoomToFixedMagnification(5.);
				});
		}
		else if (action->objectName() == "X10Action")
		{
			connect(action, &QAction::triggered, [view]() {
				view->zoomToFixedMagnification(10.);
				});
		}
		else if (action->objectName() == "X20Action")
		{
			connect(action, &QAction::triggered, [view]() {
				view->zoomToFixedMagnification(20.);
				});
		}
		else if (action->objectName() == "X40Action")
		{
			connect(action, &QAction::triggered, [view]() {
				view->zoomToFixedMagnification(40.);
				});
		}
		else if (action->objectName() == "R1Action")
		{
			connect(action, &QAction::triggered, [view]() {
				view->zoomToFixedMagnification(100.);
				});
		}
		else if (action->objectName() == "snapAction")
		{
			connect(action, &QAction::triggered, [this]() {
				if (m_SnapDialog->isHidden()){
					m_SnapDialog->show();
				}
				else
				{
					m_SnapDialog->fadeOutAndHide();
				}
				});
		}
		else if (action->objectName() == "tagAction")
		{
			connect(action, &QAction::triggered, [this,view]() {
				if (m_PlotBar->isHidden()) {
					m_PlotBar->show();
					view->setPaintState(true);
				}
				else
				{
					m_PlotBar->hide();
					view->setPaintState(false);
				}
				});
		}
		/*else if (action->objectName() == "scaleAction")
		{
			connect(action, &QAction::triggered, [view]() {
				view->setDrawingType(5);
				});
		}*/
	}
}
void MainWin::setFilePathTree()
{
	QPropertyAnimation* animation = new QPropertyAnimation(m_FileWidget, "pos");
	animation->setDuration(500); // 动画持续时间为 500 毫秒

	// 获取小部件的当前几何
	QRect widgetGeometry = m_FileWidget->geometry();
	int startX = widgetGeometry.x();
	int endX;

	// 判断小部件是否处于静止状态
	if (startX < 0) {
		// 小部件处于静止状态，强制使用
		emit fileWidgetSate(false);
		endX = 0;
	}
	else {
		// 小部件处于可移动状态，强制使用
		emit fileWidgetSate(true);
		endX = -widgetGeometry.width(); // 计算移动后的位置
	}

	// 设置动画的起始值和结束值
	animation->setStartValue(QPoint(startX, widgetGeometry.y()));
	animation->setEndValue(QPoint(endX, widgetGeometry.y()));

	// 开始动画
	animation->start();

}
void MainWin::onOpenFile(const QString& fileName)
{
	const QString factoryName = "default";
	statusBar->clearMessage();
	if (!fileName.isEmpty()) {
		if (_img) {
		
		}
		qDebug() << fileName << factoryName;
		std::string fn = fileName.toStdString();
		this->setWindowTitle(QString("DSV - ") + QFileInfo(fileName).fileName());
		MultiResolutionImageReader imgReader;
		_img.reset(imgReader.open(fn, factoryName.toStdString()));
		if (_img) {
			if (_img->valid()) {
				std::vector<unsigned long long> dimensions = _img->getLevelDimensions(_img->getNumberOfLevels() - 1);
				qDebug() << dimensions;
				PathologyViewer* view = this->findChild<PathologyViewer*>("pathologyView");
				view->initialize(_img);
			}
			else {
				statusBar->showMessage(QStringLiteral("Unsupport Format"));
			}
		}
		else {
			statusBar->showMessage(QStringLiteral("InValid File"));
		}
	}



}
QList<QString> MainWin::getFileNameAndFactory() {
	QString filterList;
	return QList<QString>();
}
void MainWin::setEnableToolBar()
{
	m_ToolBar->setEnabled(true);
}
