# Linear Algebra Calculator Specification
## 1. 计算功能
### 1.1 输入约定
用户输入表达式时，所有token之间要用空格进行分隔。  
支持多行表达式输入，每个表达式输完之后再输入一行以`:`开头的字符表示确认输入完成，开始计算。  
通过赋值操作可以储存变量，可以在表达式中通过变量名调用。每次表达式计算完后的结果都会自动存入变量`Ans`中，特征值和特征向量的计算结果会存入`eigen_val_Ans`和`eigen_vec_Ans`中。
1. 交互式表达式运算
例：
```
>>> 1 +（ 2 * 3 ）
:
Ans = 
7
>>> [ 1 2 3 ; 4 5 6 ] * 2
:
Ans = 
2  4  6
8 10 12
```
2. 变量赋值与调用
例：
```
>>> A = [ 1 2 3 ; 
4 5 6 ]
:
Ans = 
1 2 3
4 5 6
>>> A * Ans '
:
Ans = 
14 32
32 77
```
3. 函数调用
```
>>> A = det ( [ 1 1 ; 4 5 ] )
:
Ans = 
1
>>> power ( inverse ( [ 1 0 ; 0 1 ] ) , 3 )
:
Ans = 
1 0
0 1
``` 
## 2. 命令执行
### 2.1 输入约定
输入命令时，该行需要以`:`开头，命令的格式为：
```
: <cmd> <arg1> <arg2> ...
```
### 2.2 命令列表
1. help  
打印帮助信息
```
: help
: help <cmd>
: help <operator>
```
2. memory  
查询存储的所有矩阵
```
: memory
```
3. lookup  
查询某个特定矩阵
```
: lookup <matrix_name>
```
4. rename  
给某一矩阵重命名
```
: rename <old_name> <new_name>
```
5. delete  
删除存储空间中的某一矩阵
```
: delete <matrix_name>
```
6. clear  
清除存储空间
```
: clear
```
7. exit
退出程序
```
: exit
```