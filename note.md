# TODO


- [ ] 舍弃 NumericValue 的外部表示中尾部 0。
- [x] EvalEnv::eval 中令 define 抛出操作数过多异常
- [x] exit() 函数是特判
- [x] 非ascii输入 （使用gcc kit）
- [x] (1 . 2 . 3)
- [ ] Builtins
    - sqrt
    - 


# learn

- [ ] `dynamic_cast` `dynamic_pointer_cast`
- [ ] `unique_ptr` 不支持复制
- [ ] `stringstream`
- [ ] `deque` 的使用
- [ ] `static_cast<NUMERICLITERALTOKEN&>(*token)`
- [ ] `std::optional`
- [ ] `namespace string_literals`
- [ ] `unique_ptr` 和 `shared_ptr` 的管理细节 引用计数块
- [ ] error
- [ ] `std::pmr::vector`
- [ ] `std::enable_shared_from_this`  CRTP, review lv.6.2
- [ ] `std::fmod, std::modf`
- [ ] decomposition declare: `auto[name, func]`
- [ ] `std::numeric_limits<std::size_t>::max()`
- [ ] [[noreturn]]
- [ ] `std::shared_ptr<EvalEnv>(new EvalEnv(*this))` is ok, but `std::shared_ptr<EvalEnv>(EvalEnv(*this))` is not
  

# 控制台

## 代码高亮

1. 左右括号：红
2. 落单的双引号：红
3. 字面量，包括 BOOLEAN, NUMERIC, STRING：绿
4. 符号：蓝
5. 注释：黄
6. 句点：默认黑色
7. Error：红

需要提供深浅两套配色方案

## 换行自动缩进

连续换行缩进位置不变。

```scheme
>>> (+ 1 2  ; 换行
... ###     ; 缩进到下一行第一个参数前的位置     
       ^ 
```

每一层表达式都有一个“参数位”，它就是表达式过程的第一个参数之前。

自动缩进会将光标定位到当前表达式的参数位上。

## 括号定位

输入右括号时，光标定位到最近的左括号之**前**。多余右括号不定位


## 读取器

设计读取器 reader 替代 getline。

基本作用：每次读取一整个表达式字符串（包含换行符），输出去除换行符（替代成空格）后的结果。

高级作用：结合 Token 类，实现代码高亮、自动缩进和括号定位




```lisp
; test.scm
(define x 42)
(define (square x) (* x x))
(define (add-one x) (+ x 1))
; 42 * 42 + 1 = 1765

kkk ; this should error
(displayln (add-one (square x)))
```