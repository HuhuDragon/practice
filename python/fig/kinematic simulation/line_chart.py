import subprocess
import matplotlib.pyplot as plt
from scipy import interpolate
import numpy as np
from matplotlib import rcParams

# 设置中文字体
rcParams['font.sans-serif'] = ['SimHei']
rcParams['axes.unicode_minus'] = False  # 解决负号显示问题

def plot_data_from_txt(filename, output_path):

    x = []  # 角度
    y = []  
    z = []  
    p = []  
    q = []
    e = []  
    d = []  
    f = [] 

    try:
        with open(filename, 'r') as file:
            for line_num, line in enumerate(file, 1):
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                
                # 替换所有分隔符为空格然后分割
                parts = line.replace(',', ' ').replace('\t', ' ').split()
                
                if len(parts) < 2:
                    print(f"警告: 第{line_num}行数据不足(至少需要2列): {line}")
                    continue
                
                try:
                    x_val = float(parts[0])
                    y_val = float(parts[1])
                    z_val = float(parts[2]) if len(parts) > 2 else None
                    p_val = float(parts[3]) if len(parts) > 3 else None
                    q_val = float(parts[4]) if len(parts) > 4 else None
                    e_val = float(parts[5]) if len(parts) > 5 else None
                    d_val = float(parts[6]) if len(parts) > 6 else None
                    f_val = float(parts[7]) if len(parts) > 7 else None
                    
                    
                    x.append(x_val)
                    y.append(y_val)
                    z.append(z_val)
                    p.append(p_val)
                    q.append(q_val)
                    e.append(e_val)
                    d.append(d_val)
                    f.append(f_val)
                        
                except ValueError:
                    print(f"警告: 第{line_num}行包含非数值数据: {line}")
                    
        if not x:
            print("错误: 文件中没有有效数据")
            return
            
        # 绘制折线图
        plt.figure(figsize=(10, 6))
        plt.plot(x, y, color='red', linestyle='-', linewidth=2, alpha=0.7, label='16O 9.59')
        plt.plot(x, z, color='blue', linestyle='-', linewidth=2, alpha=0.7, label='18O 9.67')
        plt.plot(x, p, color='green', linestyle='-', linewidth=2, alpha=0.7, label='14N 9.51')
        plt.plot(x, q, color='orange', linestyle='-', linewidth=2, alpha=0.7, label='12C 9.64')
        plt.plot(x, e, color='purple', linestyle='-', linewidth=2, alpha=0.7, label='13C 9.5')
        # plt.plot(x, d, color='red', linestyle='--', linewidth=2, alpha=0.7)
        # plt.plot(x, f, color='red', linestyle='--', linewidth=2, alpha=0.7)
        
        plt.ylim(-300, 300)
        plt.xlabel(r"$\theta_{lab}$ (degree)")
        plt.ylabel('Position (mm)')
        plt.title('运动学模拟')
        plt.grid(True, linestyle='--', alpha=0.5)
        plt.legend()
        
        # 保存为PDF
        plt.savefig(output_path, format='pdf', bbox_inches='tight')
        plt.show()
            
    except FileNotFoundError:
        print(f"错误: 文件 {filename} 未找到")
    except Exception as e:
        print(f"发生错误: {str(e)}")

# 使用示例
if __name__ == "__main__":
    filename = 'data.txt' 
    output_path = 'histogram.pdf'  
    plot_data_from_txt(filename, output_path)