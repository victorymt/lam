# lam — Lambda 演算解释器

## 技术栈

| 层 | 语言 / 工具 |
|---|------------|
| **主实现** | C99 (Bison 3.x + Flex) |
| **参考实现 A** | Python 3.14+ |
| **参考实现 B** | Chez Scheme 10.3 |
| **构建** | GNU Make |
| **解析器生成** | GNU Bison (`-vdty`)，Flex |
| **版本控制** | Git |

### 文件结构

```
.
├── src/            # C 实现（主程序）
│   ├── lam.y       #   Bison 语法定义（lambda 演算文法）
│   ├── lam.l       #   Flex 词法定义
│   ├── ast.h / .c  #   抽象语法树 + 解释器
│   ├── constructor.h / .c  # AST 构造器
│   ├── z.c         #   主程序入口
│   └── test.c      #   C 测试框架
├── ref/            # 参考实现
│   ├── z.py        #   Python
│   └── r.ss        #   Chez Scheme
├── Makefile        # 构建系统
├── SUMMARY.md      # 本文件
├── TODO            # 待办
└── .gitignore      # 忽略构建产物
```

### 运行方式

```bash
# C 主程序
make lam && echo '(+ 1 2)' | ./lam

# C 测试
make test && ./test

# Python
python ref/z.py

# Chez Scheme
chez --script ref/r.ss
```

---

## Git 提交规范

### 远程

- 协议：**SSH** (`git@github.com:victorymt/lam.git`)
- 分支：`main`

### 提交风格

单行标题 + 空行 + 按文件分点列出变更：

```
<简短标题>

<file>: <做了什么>
<file>: <做了什么>
...
```

**示例：**

```
修复内存管理 + Env 链表化 + pretty_print + 测试框架

ast.h/ast.c: RESULT 改为值类型，Env 改为函数式链表，消除内存泄漏
z.c: 适配新接口
test.c: 新增 11 项自动化测试
Makefile: 分离编译 + test 目标 + CFLAGS
z.py: 除号整数截断、lookup 迭代化、未定义变量抛异常、测试
r.ss: 适配 Chez Scheme，修复 lookup/extend-lst-env 逻辑错误
```

---

## 主要变更记录 (a9a86f8)

### 核心问题

来自 `TODO`：

1. **内存泄漏** — 代码中 `malloc` / `strdup` 遍地板但从不 `free`
2. **指针所有权混乱** — 函数参数和结构体中的指针混为一团，不知道何时该用指针、何时该传值
3. **Env 固定大小** — `Pair p[1024]` 硬上限
4. **缺少测试框架** — 三个实现都没有自动化测试
5. **缺少 pretty_print** — AST 不可读

### C 实现 (lam)

| 文件 | 变更 |
|------|------|
| `ast.h` | `RESULT` 改为值类型；`Env` 改为函数式链表 (`EnvNode`)；去掉 `MAXSIZE` / `Pair[1024]` / 死声明 |
| `ast.c` | 重写环境操作 (链表)、解释器返回值类型、S-表达式 pretty_print、去掉死代码 |
| `z.c` | 适配新接口，支持 INT / CLOSURE / error 三种结果 |
| `test.c` | **新增** — 11 项自动化测试，覆盖数值/运算/lambda/闭包 |
| `Makefile` | 分离编译、`test` 目标、`CFLAGS`、正确依赖顺序 |

**关键设计决策**：`RESULT` 从堆分配指针改为栈值类型，消除所有权歧义。`Env` 从固定数组改为单向链表，对应 TODO 中 "Env 不可变" 的函数式语义（与 Racket 参考一致）。

### Python 实现 (ref/z.py)

| 问题 | 修复 |
|------|------|
| `/` 返回 float 与 C 不一致 | `//` 整数截断 |
| `lookup` 递归 → 深层闭包栈溢出 | 改为 `for` 迭代 |
| 未定义变量返回 `None` 无提示 | `raise NameError` |
| 无 pretty_print | 新增 S-表达式打印 |
| 无测试 | 13 项自动化测试 |

### Chez Scheme 实现 (ref/r.ss)

| 问题 | 修复 |
|------|------|
| `#lang racket` 头部 (Chez 不识别) | 删除 |
| `empty?` (Chez 无此函数) | `null?` |
| `lookup` 空环境崩溃 | 先 `(null? env)` 检查 + `error` |
| `extend-lst-env` 多绑定逻辑错误 | 正确递归 `(cdr lst)` |
| `/` 返回小数 | `(quotient v1 v2)` |
| `match` 在 Chez 中不可用 | 全部改为 `cond` 手工模式匹配 |
| 无 pretty-print | 新增 S-表达式打印 |
| 无测试 | 16 项自动化测试（含多绑定 let） |
