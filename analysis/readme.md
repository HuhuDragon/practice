How to reproduce:
1. 原始数据放在data/raw
2. 转数据程序src/convert/，先进decoderV2.1 make生成可执行文件（可以先make clean），执行按提示输入文件名，runnumber，max，min。再进Data_Ana同理先make再执行，最终刻度后数据在data/cal/，convert/的两个txt文件是刻度系数，自己设置。自动转数据的脚本在scripts/，.x auto_convert.sh运行后按示例输入即可。
3. 建议按照本次实验结果重新刻度，刻度方法：选一块探测器（一般取第一块，也就是raw[0]，当然也可以自己设置）取两个峰线性拟合得到参数cali_factor.txt，其他探测器对这个块探测器归一得到各自的归一系数放在alig_factor.txt。第一步自己画个图算都行，第二步的刻度程序在src/calibrate，自行修改程序设置拟合归一的峰位。
4. 画图程序src/drawspectrum/，寻峰程序src/findpeaks/，拟合峰位程序src/fitpeaks/（自动拟合扣除线性本底，如有其他需求自行需改）。使用时注意修改runname和实验数据命名一致。