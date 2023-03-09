# 基于跳表的KV存储引擎
参考 https://github.com/youngyangyang04/Skiplist-CPP 实现的KV存储引擎。

基于跳表和面向对象思想实现。使用C++对跳表类实现并封装，实现插入、删除、查找、更新、显示、导出等功能。

完善内存释放。

使用更严格的互斥锁，允许并发操作跳表。

支持自定义函数对象比较器。


# 文件结构
- `main.c` KV存储引擎的简单使用
- `SkipList.h` KV存储引擎实现
- `test_pressure.cpp` 压力测试
- `makefile` 编译脚本
- `readme.md` 介绍

# 接口
- `insert` 插入键值对
- `erase` 删除键值对
- `search` 查询键值对
- `update` 更新键值对
- `display` 显示所有键值对
- `size` 返回键值对数量
- `dump_file` 导出数据

# 运行
简单使用
```shell
make
./main
```
压力测试
```shell
g++ test_pressure.cpp -o test --std=c++11 -pthread
./test
```

# 测试结果
- 数据插入

|数据规模|时间(s)|
|---|---|
|50000|0.732688|
|100000|1.58284|
|500000|9.83535|

QPS = 50837
- 查询数据

|数据规模|时间(s)|
|---|---|
|50000|0.658459|
|100000|1.48961|
|500000|9.57469|

QPS = 52221
