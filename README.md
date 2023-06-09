# Mini-Lisp 解释器

北京大学《软件设计实践》课程的大作业：用 C++ 实现的简易 Mini-Lisp 语言解释器。

[Mini-Lisp 编程语言规范](https://pku-software.github.io/mini-lisp-spec/)。

项目仍在开发中。

## 启动程序

### REPL 模式

命令行参数：不传入任何命令行参数。

解释器以“读取-求值-打印”循环的方式解释执行输入的命令。

### 文件模式

命令行参数：Mini-Lisp 源代码文件名。

解释器解释执行源文件。

### 交互模式

命令行参数：选项 `-i`，Mini-Lisp 源代码文件名。`-i` 应位于文件名之前。

解释器先解释执行源文件，之后进入 REPL 模式。

## 高级特性

### 多行输入

可以将一个表达式拆成几行输入。当上一行的表达式输入还未结束时，解释器的提示符会变成 `... `。 

### 自动缩进

当你多行输入表达式时，解释器会解析已输入的表达式，提供自动缩进功能。

自动缩进按照“美观形式”的规则，即自动缩进到与当前表达式的首个非过程参数对齐的位置。

示例：

```scheme
>>> (define (triple x) (+ x    ; 此处换行
...                       x    ; 自动缩进
...                       x)
...         )
>>> double
```

### 更多的语法

#### 断言和测试

`(assert expr)`

`(assert expr msg)`

`msg` 是一个字符串。若 `expr` 求值结果**非虚值**，则返回 `#t`，否则抛出 Test Failure，信息为 `msg`。


`(assert-true expr)`

`(assert-true expr msg)`

`msg` 是一个字符串。`expr` 求值结果应当是布尔值。若 `expr` 求值结果**为 `#t`**，则返回 `#t`，否则抛出 Test Failure，信息为 `msg`。


`(check-error expr)`

`(check-error expr msg)`

`msg` 是一个字符串。先执行 `expr`，若 `expr` 执行过程中抛出 error，则返回 `#t`，否则抛出 error，error 信息为 `msg`。


`(define-test name expr...)`

定义名为 `name` 的测试用例，`expr` 是过程体，在运行测试时被顺序执行。


`(run-test tst)`

执行测试用例 `tst`。返回空表。


`(run-all-tests)`

加载并运行所有已定义的测试用例。返回空表。

#### 列表

`(list-ref ls x)`

返回值：列表 `ls` 索引为 `x` 的元素（索引自 0 开始）


`(list-tail ls x)`

返回值：列表 `ls` 的以索引为 `x` 的元素起始的子列表


`(for-each proc ls)`

将 `proc` 作用于 `ls` 的每一个元素，返回空表


`(reverse ls)`

返回将 `ls` 倒转后得到的新列表


`(member e ls)`

返回 `ls` 的最长子列表 `l`，`l` 使 `(equal? e (car l))` 为真，若找不到这样的列表，返回 `#f`


#### 字符串

`(make-string n)`

`(make-string n c)`

返回由 `n` 个 `c` 字符（默认为空格 ` `）填充的字符串（“字符”即长度为 1 的字符串）


`(string-ref str n)`

返回 `str` 索引为 `n` 的字符（长度为 1 的字符串）


`(string-length str n)`

返回 `str` 长度

    
`(substring str pos n)`

`(substring str pos)`

返回 `str` 从 `pos` 索引起的 `n` 个字符构成的子字符串，不提供 `n` 或 `n` 右越界时返回从 `pos` 索引起的整个字符串


`(string-append str0 str1)`

返回在 `str0` 后拼接 `str1` 得到的新字符串


`(string-copy str)`

返回一个和 `str` 相同的新字符串


`(number->string x)`

`(string->number str)`

`string` 与 `number` 之间的类型转换。


#### 其他

```scheme
#|
comments
|#
```

多行注释以 `#|` 开头，以 `|#` 结束，规则与 C++ 相同。多行注释不能嵌套。


`(max x ...)`

返回值：参数中的最大值。


`(min x ...)`

返回值：参数中的最小值。


`(load file)`

`file` 是 mini-Lisp 源文件的路径，解释器执行此文件中的所有代码，返回值：空表


`(read-line)`

从标准输入读取一行字符串，返回所得的字符串


`(read)`

从标准输入读取一个表达式，返回该表达式


`(read-eval)`

从标准输入读取一个表达式，返回该表达式的值
