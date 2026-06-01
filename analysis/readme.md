    git clone git@github.com:HuhuDragon/practice.git
# How to reproduce:
1. 原始数据放在data/raw
2. 转数据程序src/convert/，先进decoderV2.1 make生成可执行文件（可以先make clean），执行按提示输入文件名，runnumber，max，min。再进Data_Ana同理先make再执行，最终刻度后数据在data/cal/，convert/的两个txt文件是刻度系数，自己设置。自动转数据的脚本在scripts/，运行方式：

        .x auto_convert.sh
3. 建议按照本次实验结果重新刻度。刻度程序在src/calibrate/CalibrateDetectors.C，会对48个探测器分别拟合多个已知峰，并输出每个探测器自己的刻度系数到src/convert/cali_factor.txt。输出格式为每行一个探测器：det module channel p0 p1 p2 nfit，其中E_cal = p0 + p1 * raw + p2 * raw^2。使用时根据当前源数据自行修改knownEnergies、fitMins和fitMaxs。旧版cali_factor.txt + alig_factor.txt格式仍可被Data_Ana兼容读取。
4. 画图程序src/drawspectrum/，寻峰程序src/findpeaks/，拟合峰位程序src/fitpeaks/（自动拟合扣除线性本底，如有其他需求自行需改）。使用时注意修改runname和实验数据命名一致。
