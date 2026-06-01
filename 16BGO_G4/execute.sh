#!/bin/bash

# 执行 make 命令
make -j

# 执行 ./exampleB1 run1.mac 命令
./exampleB1 run1.mac

root -l -b -q "convert.cpp"


read -p "请输入数字 : " num
# 移动 eff.root 到 br{num}.root
mv eff.root "br${num}.root"
# 接收终端输入的数字

# 移动 out.root 到 out{num}.root
mv out.root "out${num}.root"



