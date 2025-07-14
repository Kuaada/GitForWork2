# Git 完整操作流程指南

## 基础操作

### 初始化仓库
```bash
git init                    # 初始化Git仓库
git status                  # 查看仓库状态
```

### 配置用户信息（首次使用）
```bash
git config --global user.name "你的用户名"
git config --global user.email "你的邮箱"
```

---

## 提交和保存流程

### 1. 查看当前状态
```bash
git status
```

### 2. 添加文件到暂存区
```bash
git add .                   # 添加所有文件
git add 文件名.cpp          # 添加特定文件
```

### 3. 提交更改
```bash
git commit -m "描述你的更改"
```

### 4. 快速提交（跳过暂存区）
```bash
git commit -am "快速提交"
```

---

## 查看历史

### 查看提交历史
```bash
git log --oneline           # 简洁历史
git log --oneline -5        # 最近5次提交
git log --graph --oneline   # 图形化历史
```

### 查看文件差异
```bash
git diff                    # 工作区与暂存区差异
git diff --cached           # 暂存区与提交差异
```

---

## 回退操作

### 1. 查看可回退的节点
```bash
git log --oneline
```
输出示例：
```
e735d8a 本地保存：提交所有当前更改
9068a5b docs: 添加.gitignore和README文档
0ac9a78 首次提交
```

### 2. 回退操作
```bash
git reset --soft 节点ID     # 软回退（保留更改在暂存区）
git reset 节点ID            # 混合回退（保留更改在工作区）
git reset --hard 节点ID     # 硬回退（彻底删除更改）
```

### 3. 撤销操作
```bash
git checkout -- 文件名      # 撤销工作区更改
git reset HEAD 文件名       # 撤销暂存区更改
git reset --soft HEAD~1     # 撤销最后一次提交（保留更改）
git reset --hard HEAD~1     # 撤销最后一次提交（删除更改）
```

---

## 分支操作

### 查看和创建分支
```bash
git branch                  # 查看本地分支
git branch -a               # 查看所有分支
git checkout -b 新分支名    # 创建并切换到新分支
```

### 切换和合并分支
```bash
git checkout 分支名         # 切换到分支
git merge 分支名            # 合并分支
git branch -d 分支名        # 删除已合并分支
```

---

## 远程仓库操作

### 添加和推送
```bash
git remote add origin 仓库地址    # 添加远程仓库
git push -u origin master        # 首次推送
git push origin master           # 后续推送
```

### 拉取和克隆
```bash
git pull origin master           # 拉取并合并
git clone 仓库地址               # 克隆仓库
```

---

## 标签管理

```bash
git tag v1.0.0                    # 创建标签
git tag -a v1.0.0 -m "版本说明"   # 创建带注释标签
git push origin v1.0.0            # 推送标签
```

---

## 完整工作流程示例

### 日常开发流程
```bash
git status                        # 1. 查看状态
git add .                         # 2. 添加更改
git commit -m "feat: 新功能"      # 3. 提交更改
git push origin master            # 4. 推送到远程
```

### 修复bug流程
```bash
git checkout -b bugfix/fix-issue  # 1. 创建修复分支
# ... 修改代码 ...
git add .                         # 2. 添加修复
git commit -m "fix: 修复bug"      # 3. 提交修复
git checkout master               # 4. 切换主分支
git merge bugfix/fix-issue        # 5. 合并修复
git branch -d bugfix/fix-issue    # 6. 删除临时分支
```

---

## 常用命令速查

| 操作 | 命令 |
|------|------|
| 查看状态 | `git status` |
| 添加文件 | `git add .` |
| 提交更改 | `git commit -m "消息"` |
| 查看历史 | `git log --oneline` |
| 回退 | `git reset --hard 节点ID` |
| 创建分支 | `git checkout -b 分支名` |
| 切换分支 | `git checkout 分支名` |
| 合并分支 | `git merge 分支名` |
| 推送到远程 | `git push origin master` |
| 拉取更新 | `git pull origin master` |

---

## 注意事项

1. **重要操作前先备份**：特别是使用`--hard`参数时
2. **提交信息要清晰**：便于后续查看和维护
3. **定期推送**：防止本地数据丢失
4. **谨慎使用force**：`git push --force`会覆盖远程历史
5. **及时解决冲突**：避免代码混乱

---

## 获取帮助

```bash
git help 命令名              # 查看命令帮助
git 命令名 --help            # 查看详细说明
``` 