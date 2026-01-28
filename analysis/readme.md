How to reproduce:
1. 原始数据放在data/raw
2. 转数据程序src/convert/，先进decoderV2.1 make生成可执行文件（可以先make clean），执行按提示输入文件名，runnumber，max，min。再进Data_Ana同理先make再执行，最终刻度后数据在data/cal/，convert/的两个txt文件是刻度系数，可以自己设置。
3. 