#!/bin/bash

# ======================
# 参数检查
# ======================
if [ $# -ne 4 ]; then
    echo "用法: $0 <FILENAME> <ROUND> <MAX> <MIN>"
    echo "示例: $0 raw000001.dat 1 4096 0"
    exit 1
fi

FILENAME=$1
ROUND=$2
MAX=$3
MIN=$4

echo ">>> 输入参数确认"
echo "    文件名: $FILENAME"
echo "    轮次  : $ROUND"
echo "    MAX   : $MAX"
echo "    MIN   : $MIN"
echo

# ======================
# 第一次转换
# ======================
echo ">>> Step 1: Data_convert"

cd .. || exit 1
cd src/convert/decoderV1.2/ || exit 1

./Data_convert << EOF
${FILENAME}
${ROUND}
${MAX}
${MIN}
EOF

if [ $? -ne 0 ]; then
    echo " Data_convert 运行失败，脚本终止"
    exit 1
fi

echo ">>> Data_convert 完成"
echo

# ======================
# 第二次转换
# ======================
echo ">>> Step 2: DataAna"

cd ../Data_Ana/ || exit 1

./DataAna << EOF
${FILENAME}
${ROUND}
${MAX}
${MIN}
EOF

if [ $? -ne 0 ]; then
    echo "❌ DataAna 运行失败"
    exit 1
fi

echo
echo "✅ 所有转换流程结束"
