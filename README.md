# DSV - 数字切片查看器

## 项目简介

DSV (Digital Slide Viewer) 是一个基于Qt和OpenSlide开发的数字病理切片查看器。该软件支持查看和分析各种格式的数字病理切片，包括WSI (Whole Slide Image) 格式。

## 功能特性

- 支持多种数字切片格式 (WSI, TIFF等)
- 多分辨率图像浏览
- 图像标注和测量工具
- 缩放和平移功能
- 缩略图导航
- 图像过滤和处理
- 标注保存和导出

## 系统要求

- Windows 10/11
- Visual Studio 2019/2022
- Qt 5.15 或更高版本
- OpenSlide 库

## 安装和构建

### 1. 克隆仓库

```bash
git clone <repository-url>
cd DSV-1
```

### 2. 环境配置

确保已安装以下依赖：
- Visual Studio 2019/2022
- Qt 5.15+
- OpenSlide 库 (已包含在Env目录中)

### 3. 构建项目

1. 使用Visual Studio打开 `DSV.sln`
2. 配置项目属性，确保Qt路径正确
3. 选择Release或Debug配置
4. 构建解决方案 (Ctrl+Shift+B)

### 4. 运行程序

构建完成后，可执行文件位于：
- Debug版本: `x64/Debug/DSV.exe`
- Release版本: `x64/Release/DSV.exe`

## 使用方法

1. 启动DSV.exe
2. 点击"打开数据"按钮选择数字切片文件
3. 使用鼠标进行缩放和平移操作
4. 使用工具栏进行标注和测量
5. 保存标注结果

## Git 操作指南

### 初始化仓库（如果还没有）

```bash
git init
```

### 查看状态

```bash
git status
```

### 添加文件到暂存区

```bash
# 添加所有文件
git add .

# 添加特定文件
git add DSV/main.cpp
git add DSV/MainWin.h
```

### 提交更改

```bash
# 提交所有暂存的更改
git commit -m "描述你的更改"

# 提交特定文件
git commit -m "更新主窗口功能" DSV/MainWin.cpp
```

### 查看提交历史

```bash
# 查看所有提交
git log

# 查看简洁的提交历史
git log --oneline

# 查看特定文件的提交历史
git log --follow DSV/main.cpp
```

### 分支操作

```bash
# 创建新分支
git branch feature-name

# 切换到分支
git checkout feature-name

# 创建并切换到新分支
git checkout -b feature-name

# 查看所有分支
git branch -a
```

### 合并分支

```bash
# 切换到主分支
git checkout master

# 合并功能分支
git merge feature-name
```

### 远程仓库操作

```bash
# 添加远程仓库
git remote add origin <repository-url>

# 推送到远程仓库
git push origin master

# 从远程仓库拉取更新
git pull origin master

# 克隆远程仓库
git clone <repository-url>
```

### 撤销操作

```bash
# 撤销工作区的更改
git checkout -- <file-name>

# 撤销暂存区的更改
git reset HEAD <file-name>

# 撤销最后一次提交
git reset --soft HEAD~1

# 完全撤销最后一次提交
git reset --hard HEAD~1
```

### 标签管理

```bash
# 创建标签
git tag v1.0.0

# 创建带注释的标签
git tag -a v1.0.0 -m "版本1.0.0发布"

# 查看所有标签
git tag

# 推送标签到远程
git push origin v1.0.0
```

## 项目结构

```
DSV-1/
├── DSV/                    # 主要源代码目录
│   ├── main.cpp           # 程序入口
│   ├── MainWin.cpp        # 主窗口
│   ├── PathologyViewer.cpp # 病理查看器
│   ├── Item/              # 渲染元素
│   ├── resources/         # 资源文件
│   └── x64/              # 构建输出
├── Env/                   # 环境依赖
│   ├── bin/              # OpenSlide动态库
│   ├── include/          # 头文件
│   └── lib/              # 静态库
├── .gitignore            # Git忽略文件
└── README.md             # 项目说明
```

## 开发指南

### 代码规范

- 使用C++11/14标准
- 遵循Qt编码规范
- 类名使用PascalCase
- 函数名使用camelCase
- 变量名使用camelCase

### 提交信息规范

提交信息应包含：
- 类型：feat(新功能), fix(修复), docs(文档), style(格式), refactor(重构), test(测试), chore(构建)
- 简短描述
- 详细说明（可选）

示例：
```
feat: 添加图像缩放功能

- 实现鼠标滚轮缩放
- 添加缩放比例显示
- 优化缩放性能
```

## 常见问题

### Q: 编译时找不到Qt头文件
A: 检查项目属性中的Qt路径配置，确保指向正确的Qt安装目录。

### Q: 运行时缺少OpenSlide库
A: 确保 `libopenslide-1.dll` 在可执行文件同目录下或系统PATH中。

### Q: 图像加载失败
A: 检查图像文件格式是否支持，确保文件路径正确。

## 许可证

本项目采用 [许可证名称] 许可证。

## 贡献

欢迎提交Issue和Pull Request来改进项目。

## 联系方式

如有问题，请通过以下方式联系：
- 邮箱：[your-email@example.com]
- 项目地址：[repository-url] 