#!/bin/bash
# 定义一个目录变量
dir=${{ github.workspace }}  # github.workspace是GitHub Actions内置的环境变量，表示工作目录
# 定义一个标志变量，初始值为1，表示异常
flag=1
# 使用find命令递归遍历目录下的所有文件，-type f选项表示只匹配文件，-name "*.cpp"选项表示只匹配.cpp文件
# 使用xargs命令将文件名传递给grep命令，-I {}选项表示用{}代替文件名，-r选项表示递归搜索
# 使用grep命令搜索class和public关键字，-q选项表示静默模式，-w选项表示匹配整个单词，-E选项表示使用扩展正则表达式
# 使用&&逻辑运算符表示两个条件都要满足，如果满足则将标志变量赋值为0，表示正常，然后跳出循环
find $dir -type f -name "*.cpp" | xargs -I {} grep -q -w -E "class" {} && flag=0
# 根据标志变量的值输出结果
if [ $flag -eq 0 ]; then
  echo "[INFO] 检测1通过，已匹配到class的使用"
else
  echo "[ERROR] 检测失败"
fi
find $dir -type f -name "file_name"
exit $flag