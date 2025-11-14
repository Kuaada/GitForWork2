/**
 * @file FileWidget.cpp
 * @brief 文件窗口部件实现文件
 * @details 实现数字病理切片查看器的文件管理界面，包括文件树视图、最近文件列表等功能
 * @author [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0
 */

#include "FileWidget.h"
#include <QDebug>
//#pragma execution_character_set("utf-8")
/**
 * @brief 文件窗口部件构造函数
 * @param parent 父窗口指针
 * @details 初始化文件窗口部件，设置基本属性、界面布局和样式
 */
FileWidget::FileWidget(QWidget *parent = nullptr)
	: QWidget(parent)
{
    setFixedWidth(500);     // 设置固定宽度
    setMinimumSize(500, 786);  // 设置最小尺寸
    move(-500, 0);          // 初始位置在屏幕外
    initUI();               // 初始化用户界面
    setupStyle();           // 设置样式
}

/**
 * @brief 设置窗口样式
 * @details 配置文件窗口的视觉样式，包括字体大小、背景色等
 */
void FileWidget::setupStyle()
{
    // 设置样式表
    QString style = QString(   
        "QLabel {"
        "font-size: 35px; /* 设置 QLabel 字体大小 */"
        "padding: 0; "
        "margin: 0; "
  
        "}"
        "QTabWidget{"
        "font-size: 25px;"
        " background-color: white;"
        "}"
        "QListView#listView::item{"
        "font-size:40pt;"
        "height: 60px;"
        "}"

    );
    setStyleSheet(style);
}

/**
 * @brief 文件窗口部件析构函数
 * @details 清理资源，释放内存
 */
FileWidget::~FileWidget()
{
    // 析构函数实现
}

/**
 * @brief 初始化用户界面
 * @details 创建和配置文件窗口的所有UI组件，包括文件树、标签页、列表视图等
 */
void FileWidget::initUI()
{
    // 创建垂直布局
    QVBoxLayout* FileTreeVerticalLayout = new QVBoxLayout(this);
    FileTreeVerticalLayout->setContentsMargins(10, 10, 10, 10);
    
    // 创建文件树标签
    QLabel* FileTreeLabel = new QLabel(QStringLiteral("文件树"), this);
    FileTreeLabel->setObjectName("FileTreeLabel");
    FileTreeVerticalLayout->addWidget(FileTreeLabel, 0);

    // 创建水平布局
    QHBoxLayout* FileTreeHorizontalLayout = new QHBoxLayout();
    
    // 创建标签页控件
    QTabWidget* tabWidget = new QTabWidget(this);
    tabWidget->setTabPosition(QTabWidget::West);  // 标签页位置在左侧
    tabWidget->setStyle(/*new TabStyle(style())*/style());

    // 创建树形视图
    QTreeView* treeView = new QTreeView(this);
    treeView->setObjectName(QStringLiteral("treeView"));
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 禁止编辑

    // 创建文件系统模型
    QFileSystemModel* treeModel = new QFileSystemModel(this);
    treeModel->setObjectName(QStringLiteral("treeModel"));
    treeModel->setRootPath(QDir::currentPath());  // 设置根路径为当前目录
    
    // 设置文件过滤器，只显示支持的图像格式
    QStringList sNameFilter;
    sNameFilter << "*.tif"<<"*.svs"<<"*.czi"<<"*.ndpi"<<"*.dcm"<<"*.tiff"<<"*.scn"<<"*.zvi"<<"*.bif";
    treeModel->setNameFilterDisables(false);
    treeModel->setNameFilters(sNameFilter);
    
    treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    treeModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot); // 显示文件夹和文件
    
    treeView->setModel(treeModel);
    // 隐藏不需要的列
    treeView->setColumnHidden(1, true);  // 隐藏大小列
    treeView->setColumnHidden(2, true);  // 隐藏类型列
    treeView->setColumnHidden(3, true);  // 隐藏修改日期列
    
    // 连接树形视图点击信号
    connect(treeView, &QTreeView::clicked, this, &FileWidget::onTreeViewItemClicked);

    // 创建列表视图（最近文件）
    QListView* listView = new QListView(this);
    listView->setObjectName(QStringLiteral("listView"));
    QFont font = listView->font();
    font.setPointSize(25);
    listView->setFont(font);
    
    // 创建标准项模型
    QStandardItemModel* listmodel = new QStandardItemModel(this);
    listmodel->setObjectName(QStringLiteral("listmodel"));
    listView->setModel(listmodel);
    
    // 连接信号槽
    connect(this, &FileWidget::fileSelected, this, &FileWidget::onFileSelected);
    connect(listView, &QListView::clicked, this, &FileWidget::onListViewItemClicked); // 连接 QListView 点击信号

    // 添加标签页
    tabWidget->addTab(treeView, QStringLiteral("文件"));
    tabWidget->addTab(listView, QStringLiteral("最近"));
    FileTreeHorizontalLayout->addWidget(tabWidget);
    FileTreeVerticalLayout->addLayout(FileTreeHorizontalLayout, 9);
}

/**
 * @brief 树形视图项目点击事件处理
 * @param index 被点击的项目索引
 * @details 当用户在文件树中点击文件时，检查文件格式并发出选择信号
 */
void FileWidget::onTreeViewItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;
        
    QTreeView* treeView = this->findChild<QTreeView*>("treeView");
    QFileSystemModel* model = qobject_cast<QFileSystemModel*>(treeView->model());
    if (!model)
        return;

    QString filePath = model->filePath(index); // 获取文件路径
    QFileInfo fileInfo(filePath);

    // 检查是否为支持的文件格式
    if (fileInfo.isFile() && fileInfo.suffix().toLower() == "tif"   || 
        fileInfo.isFile() && fileInfo.suffix().toLower() == "tiff"  ||
        fileInfo.isFile() && fileInfo.suffix().toLower()=="svs"     || 
        fileInfo.isFile() && fileInfo.suffix().toLower() == "ndpi"  || 
        fileInfo.isFile() && fileInfo.suffix().toLower() == "czi"   ||
        fileInfo.isFile() && fileInfo.suffix().toLower() == "bif"   ||
        fileInfo.isFile() && fileInfo.suffix().toLower() == "zvi"   ||
        fileInfo.isFile() && fileInfo.suffix().toLower() == "scn"   ||
        fileInfo.isFile() && fileInfo.suffix().toLower() == "dcm") // 检查是否为支持的图像文件
    {
        emit fileSelected(filePath); // 发出自定义信号
    }
}

/**
 * @brief 文件选择事件处理
 * @param filePath 被选择的文件路径
 * @details 当文件被选择时，将其添加到最近文件列表中并发出路径传输信号
 */
void FileWidget::onFileSelected(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName(); // 获取文件名

    // 检查是否已经存在相同路径
    QListView* listView = this->findChild<QListView*>("listView");
    QStandardItemModel* listmodel = qobject_cast<QStandardItemModel*>(listView->model());
    if (!listmodel)
        return;

    // 检查是否已存在相同路径的项目
    for (int i = 0; i < listmodel->rowCount(); ++i)
    {
        QStandardItem* existingItem = listmodel->item(i);
        if (existingItem && existingItem->data().toString() == filePath)
        {
            emit filePathTrans(filePath);
            return; // 如果已经存在相同路径，直接返回
        }
    }

    // 如果没有找到相同路径，创建新项目
    QStandardItem* item = new QStandardItem(fileName);
    item->setData(filePath); // 将文件路径作为用户数据
    item->setEditable(false);

    listmodel->appendRow(item); // 将项目添加到模型中
    
    emit filePathTrans(filePath);
}

/**
 * @brief 列表视图项目点击事件处理
 * @param index 被点击的项目索引
 * @details 当用户在最近文件列表中点击文件时，发出路径传输信号
 */
void FileWidget::onListViewItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;
        
    QListView* listView = this->findChild<QListView*>("listView");
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(listView->model());
    if (!model)
        return;

    QStandardItem* item = model->itemFromIndex(index);
    if (item)
    {
        QString filePath = item->data().toString(); // 获取用户数据（文件路径）
        emit filePathTrans(filePath);
    }
}