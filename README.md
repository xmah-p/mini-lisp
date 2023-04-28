# Mini-Lisp 解释器

这是北京大学《软件设计实践》课程的大作业：用 C++ 实现的简易 Mini-Lisp 语言解释器。

关于 Mini-Lisp 语言，请参考[Mini-Lisp 编程语言规范](https://pku-software.github.io/mini-lisp-spec/)。

项目仍在开发中。

## REPL 模式

命令行参数：不传入任何命令行参数。

解释器以“读取-求值-打印”循环的方式解释执行输入的命令。

## 文件模式

命令行参数：Mini-Lisp 源代码文件名。

解释器解释执行源文件。

## 交互模式

命令行参数：选项 `-i`，Mini-Lisp 源代码文件名。`-i` 应位于文件名之前。

解释器先解释执行源文件，之后进入 REPL 模式。


