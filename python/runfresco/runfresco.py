import subprocess
import matplotlib.pyplot as plt
import os
from scipy import interpolate
import numpy as np
from scipy.optimize import curve_fit

# 启用 LaTeX 支持
plt.rcParams["text.usetex"] = True  # 使用 LaTeX 渲染文本
plt.rcParams["font.family"] = "serif"  # 设置字体为衬线字体（LaTeX 默认字体）

# 1. 运行fresco
main = "frescoR.exe"
if os.path.exists(main):
    os.system(main + ' < 26mg8.324.in > test26mg.out') # 指定输入输出文件
# 2. 读取生成的文本文件
file_name1 = "fort.16" 
file_name2 = "expdata.txt" 

x1 = []  
y1 = []  

x2 = []
# ex2 = []
y2 = []
ey2 = []

# a = 0.074   #谱因子

with open(file_name1, "r") as file:
    for line in file:
        if line.startswith("#"):
            continue
        if line.startswith("@"):
            continue
        # 假设数据以空格或制表符分隔
        columns = line.strip().split()  # 去除首尾空白并分割
        if len(columns) >= 2:  # 确保至少有两列数据
            x1.append(float(columns[0]))
            y1.append(float(columns[1]))
file.close()

with open(file_name2, "r") as file:
    for line in file:
        if line.startswith("#"):
            continue
        if line.startswith("@"):
            continue
        # 假设数据以空格或制表符分隔
        columns = line.strip().split()  # 去除首尾空白并分割
        if len(columns) >= 3:  # 确保至少有三列数据
            x2.append(float(columns[0]))
            # ex2.append(float(columns[1]))
            y2.append(float(columns[1]))
            ey2.append(float(columns[2]))

file.close()

# 3.最小卡方拟合
cubic_interp = interpolate.interp1d(x1, y1, kind='cubic') #三次样条插值

def spectroscopic_factor(x,a):
    return cubic_interp(x)*a

popt, pcov = curve_fit(spectroscopic_factor, x2, y2, sigma=ey2, absolute_sigma=True) #最小二乘拟合

a= popt    # 获取拟合参数
print(f"谱因子: a = {a}")

y_dwba = spectroscopic_factor(x2, a)  # 计算理论值

chi_square = np.sum(((y2 - y_dwba) / ey2) ** 2)
print(f"最小卡方值: {chi_square/(len(x2) - 1)}")

chi_square_error = np.sqrt(2 * len(x2))  # 卡方值的统计误差
print(f"卡方值的误差: {chi_square_error}")

# 4. 绘制折线图
plt.figure(figsize=(10, 6))
plt.plot(x1, y1*a, linestyle='-', color='b', label="DWBA")
plt.errorbar(x2, y2, yerr=ey2, fmt='o', color='b', ecolor='r', capsize=5, label="experimental data")
plt.yscale('log') 
plt.title('8.324 state',fontsize=18)
plt.xlabel(r"$\theta_{lab}$ (degree)",fontsize=16)
plt.ylabel('cross section (mb/sr)',fontsize=16)
plt.grid(True)
plt.show()