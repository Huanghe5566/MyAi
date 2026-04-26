# 科学计算器

这是一个使用 Python 编写的桌面科学计算器项目，图形界面基于标准库 `tkinter` 实现，作者显示为 `JOEL`。

项目已经支持打包为 Windows 可执行程序，最终可运行版本位于：

```text
dist\ScientificCalculator\ScientificCalculator.exe
```

注意：该程序采用 PyInstaller 的 `onedir` 方式打包，运行或分发时需要保留整个 `dist\ScientificCalculator` 文件夹，不能只单独复制 `ScientificCalculator.exe`。

## 主要功能

- 基本四则运算：加、减、乘、除
- 科学计算函数：`sin`、`cos`、`tan`、`asin`、`acos`、`atan`
- 对数和指数：`log`、`ln`、`exp`
- 开方、绝对值、幂运算、阶乘
- 常量：`pi`、`e`
- 支持 `Ans` 引用上一次计算结果
- 支持角度模式 `DEG` 和弧度模式 `RAD`
- 支持中文错误提示
- 支持常见数学写法自动修正，例如：

```text
2pi        -> 2*pi
3(4+5)     -> 3*(4+5)
sin(30     -> sin(30)
5!         -> factorial(5)
(3+2)!     -> factorial((3+2))
30tan(...) -> 30*tan(...)
```

## 运行源码

需要本机已安装 Python，并且 Python 带有 `tkinter`。

在项目根目录运行：

```powershell
python scientific_calculator.py
```

## 打包为 exe

项目使用 PyInstaller 打包。推荐使用已有的 spec 文件：

```powershell
pyinstaller -y --clean ScientificCalculator.spec
```

打包完成后，程序会生成在：

```text
dist\ScientificCalculator\ScientificCalculator.exe
```

## 主要文件说明

```text
scientific_calculator.py
```

主程序文件，包含计算器界面、按钮逻辑、表达式解析、中文错误提示和计算逻辑。

```text
ScientificCalculator.spec
```

PyInstaller 打包配置文件。该文件显式加入了 `tkinter`、Tcl/Tk DLL 和资源目录，用来避免打包后出现 `No module named 'tkinter'` 的问题。

```text
runtime_tkinter.py
```

PyInstaller 运行时辅助脚本，用来在打包后的程序启动时定位 Tcl/Tk 资源目录。

```text
hooks\pre_find_module_path\hook-tkinter.py
```

本地 PyInstaller hook，用来阻止当前环境中 PyInstaller 误判并排除 `tkinter`。

```text
dist\ScientificCalculator
```

最终可运行程序目录。分发给别人时应复制整个目录。

## 常见问题

### 1. 提示 `No module named 'tkinter'`

通常是运行了旧的单文件 exe，或者打包时没有正确包含 Tcl/Tk。

请运行：

```text
dist\ScientificCalculator\ScientificCalculator.exe
```

并确认旁边保留了 `_internal` 文件夹。

### 2. 只复制 exe 后无法运行

当前项目使用 `onedir` 打包方式，exe 依赖同目录下的 `_internal` 文件夹。请复制整个 `dist\ScientificCalculator` 文件夹。

### 3. 输入不完整括号是否可以计算

可以。程序会自动补齐缺失的右括号，例如：

```text
sin(30
```

会按：

```text
sin(30)
```

计算。

### 4. 角度和弧度有什么区别

选择 `角度 DEG` 时，`sin(30)` 表示 30 度。

选择 `弧度 RAD` 时，`sin(30)` 表示 30 弧度。

## 当前状态

- 作者名：`JOEL`
- 界面语言：中文为主
- 打包方式：PyInstaller `onedir`
- 当前推荐运行入口：`dist\ScientificCalculator\ScientificCalculator.exe`
