# Git 迁移全过程复盘（从 git_C_learnt 到 learn 根仓库）

## 1. 背景与目标

### 1.1 初始背景
- 旧本地仓库位置：`C:\Users\28445\OneDrive\learn\git_C_learnt`
- 旧仓库中已有多次本地提交（约 10 个提交）
- 用户希望在 `C:\Users\28445\OneDrive\learn` 重新初始化 Git，并迁移历史

### 1.2 最终明确的目标
用户最终明确要求：
- **不是仅仅保留旧历史**，而是要进行**历史改写**
- 改写后效果：旧提交中的文件路径要变成以 `learn` 为根时的子路径，即统一带 `git_C_learnt/` 前缀

示例（改写后）：
- `git_C_learnt/main.c`
- `git_C_learnt/git_learn/git_learn.md`

---

## 2. 执行环境与约束

- 操作系统：Windows
- 终端：VS Code Bash（MINGW64）
- 工作目录：`C:/Users/28445/OneDrive/learn`
- 用户诉求重点：
  - 命令必须尽量稳妥
  - 如有歧义要讲清
  - 尽量避免失误导致重来

---

## 3. 方案选择过程

### 3.1 先讨论了两种方案
1. **历史挂接方案（不改写）**
   - 用 merge 把旧历史接进新仓库
   - 风险低，但旧提交路径不会变

2. **历史改写方案（最终采用）**
   - 使用 `git filter-repo --to-subdirectory-filter git_C_learnt`
   - 把旧历史每个提交中的路径整体加前缀
   - 满足“以 learn 为根路径”目标

### 3.2 用户决策
用户明确选择第 2 种：**历史改写**。

---

## 4. 实际执行时间线（按顺序）

> 说明：以下按实际对话中的执行顺序整理，包含关键命令与结果。

### 阶段 A：前置检查与准备

#### A1. 检查 `git-filter-repo` 是否可用
执行：
```bash
git filter-repo --help >/dev/null 2>&1; echo $?
```
结果：
- 返回 `0`
- 说明 `git-filter-repo` 已安装可用

#### A2. 旧仓库分支确认
用户反馈：
- 旧主分支为 `master`

#### A3. 工作区状态确认
用户反馈：
- “没有多余内容”（即旧仓库可继续迁移）

---

### 阶段 B：创建镜像并改写旧历史

#### B1. 在 learn 根目录创建镜像仓库
执行：
```bash
cd /c/Users/28445/OneDrive/learn
rm -rf _legacy_rewrite_mirror.git
git clone --mirror ./git_C_learnt ./_legacy_rewrite_mirror.git
cd ./_legacy_rewrite_mirror.git
```
结果：
- bare 镜像仓库创建成功

#### B2. 执行历史改写
执行：
```bash
git filter-repo --force --to-subdirectory-filter git_C_learnt
```
关键输出：
- `Parsed 10 commits`
- `New history written ...`
- `Completely finished ...`

说明：
- 改写成功
- 旧历史中的每个提交树均被迁移到 `git_C_learnt/` 子目录

#### B3. 验证改写是否生效
执行：
```bash
git log --oneline -n 5
git show --name-only --pretty=format: HEAD | head -n 20
```
验证结果：
- 日志正常
- 文件路径出现：
  - `git_C_learnt/git_learn/git_commit_vscode.png`
  - `git_C_learnt/git_learn/git_learn.md`
  - `git_C_learnt/git_learn/gitignore_vscode.png`
- 说明路径前缀改写成功

---

### 阶段 C：将改写后的历史导入 learn 根仓库

#### C1. 初始化并尝试切换到改写历史
执行关键命令：
```bash
git checkout -B main rewritten/master
```

#### C2. 遇到关键错误（第一次主要阻塞）
报错原文核心：
- `The following untracked working tree files would be overwritten by checkout`
- 列出大量 `git_C_learnt/...` 文件

#### C3. 错误原因分析
原因：
- `learn` 根目录中存在未跟踪目录（`git_C_learnt/`、`markdown_learn/` 等）
- `checkout rewritten/master` 会把同路径文件写入工作区
- Git 为防止覆盖未跟踪文件，主动中止

这是 **Git 的保护机制**，属于预期行为，不是仓库损坏。

#### C4. 解决方案（低风险）
策略：
- 先把冲突的未跟踪目录临时挪走，再 checkout

执行：
```bash
cd /c/Users/28445/OneDrive/learn
mkdir -p ../learn_migration_hold
mv ./git_C_learnt ../learn_migration_hold/git_C_learnt_local_backup
mv ./markdown_learn ../learn_migration_hold/markdown_learn_local_backup
mv ./git_C_learnt.zip ../learn_migration_hold/ 2>/dev/null || true
```

然后再次执行：
```bash
git checkout -B main rewritten/master
```
结果：
- 成功
- 输出：`branch 'main' set up to track 'rewritten/master'.`
- 输出：`Reset branch 'main'`

#### C5. 恢复根目录额外内容并提交
执行：
```bash
mv ../learn_migration_hold/markdown_learn_local_backup ./markdown_learn
mv ../learn_migration_hold/git_C_learnt.zip ./git_C_learnt.zip 2>/dev/null || true

git status --short
git add markdown_learn git_C_learnt.zip
git commit -m "chore: add root-level files after legacy rewrite"
```
结果：
- 成功提交 1 个新提交：`031c5ca`
- 新增文件包括：
  - `git_C_learnt.zip`
  - `markdown_learn/Markdown-书写规范.md`
  - `markdown_learn/example_anchor.png`
  - `markdown_learn/jump_to_non_title.png`

---

### 阶段 D：最终验收

执行：
```bash
git log --graph --oneline --decorate --all -n 30
git show --name-only --pretty=format: HEAD~1 | head -n 30
```

结果要点：
- `main` 当前头：`031c5ca chore: add root-level files after legacy rewrite`
- 其父提交：`adeb4b5`（改写后的旧历史顶点）
- 旧历史链条完整可见（共 10 个旧提交）
- `HEAD~1` 文件路径带 `git_C_learnt/` 前缀

结论：
- 目标已达成：**旧提交历史已改写为以 learn 为根路径的历史结构**

---

## 5. 本次遇到的错误与处理清单

### 错误 1：checkout 时未跟踪文件将被覆盖

错误现象：
- `error: The following untracked working tree files would be overwritten by checkout`

触发条件：
- 工作区已有同名未跟踪文件/目录
- 目标分支 checkout 会写入同路径内容

解决动作：
1. 暂存搬迁未跟踪目录到 `../learn_migration_hold`
2. 重新执行 `git checkout -B main rewritten/master`
3. 切换成功后再将需要保留的根目录内容搬回并提交

复盘结论：
- 这是迁移中最常见阻塞点之一
- 处理正确且无数据损失

---

## 6. 关键产物与当前状态

### 6.1 关键产物
- 改写镜像仓库：`_legacy_rewrite_mirror.git`（迁移临时产物）
- 新仓库当前分支：`main`
- 根目录补充提交：`031c5ca`

### 6.2 历史结构状态
- 旧提交已被改写并挂到新 `main` 主线
- 旧提交路径统一为 `git_C_learnt/...`
- 根目录附加内容（`markdown_learn`、zip）已以新提交纳入

### 6.3 临时目录状态
- `../learn_migration_hold` 曾用于防覆盖搬迁
- 其备份目录可按需保留或清理

---

## 7. 风险控制与为什么这次没出事故

本次流程安全点：
1. 使用临时镜像仓库改写，不直接在原仓库动刀
2. checkout 报错后没有强行覆盖，而是先搬迁冲突目录
3. 每个关键阶段都做了可视验证（log/show/status）
4. 最终提交把额外根目录内容单独记录，边界清晰

---

## 8. 下次复现的最小步骤索引（快速版）

1. 确认旧仓库干净、确认分支名
2. `git clone --mirror` 建镜像
3. 在镜像中 `git filter-repo --to-subdirectory-filter git_C_learnt`
4. 在新根仓库 `git fetch` 改写历史
5. 若 checkout 被未跟踪文件阻塞：先搬迁冲突目录，再 checkout
6. 搬回需要的根目录额外文件并提交
7. `git log --graph` + `git show --name-only` 验收路径前缀

---

## 9. 本次最终结论

本次迁移已经完成，且满足初始高优先级要求：
- 在 `learn` 重新建立了仓库语义
- 旧提交历史被完整改写为以 `learn` 为根的路径结构
- 过程中出现的关键错误已被正确处理且无覆盖丢失
- 当前仓库状态可继续用于后续远端推送与协作
