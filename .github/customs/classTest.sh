#!/bin/bash
# 定义一个目录变量,从第一个参数接收目录
dir=$1
# 定义一个标志变量，初始值为1，表示异常
flag=1
find $dir -type f -name "*.cpp" | xargs -I {} grep -q -w -E "class" {} && flag=0
# 根据标志变量的值输出结果
if [ $flag -eq 0 ]; then
  echo "[INFO] 检测1通过，已匹配到class的使用"
else
  echo "[ERROR] 检测失败"
fi
find $dir -type f -name "file_name"
exit $flag