import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams

# 设置中文字体
rcParams['font.sans-serif'] = ['SimHei']
rcParams['axes.unicode_minus'] = False  # 解决负号显示问题

def plot_data_from_txt(filename, output_path):
    """
    从txt文件中读取数据并绘制多条曲线
    
    参数:
        filename (str): 数据文件路径
        output_path (str): 输出PDF文件路径
    """
    x = []  # 深度 (nm)
    y = []  # 27Al计数
    z = []  # W计数
    p = []  # SRIM模拟的x数据
    q = []  # SRIM模拟的y数据
    
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
                    
                    x.append(x_val)
                    y.append(y_val)
                    z.append(z_val)
                    if p_val is not None and q_val is not None:
                        p.append(p_val)
                        q.append(q_val)
                        
                except ValueError:
                    print(f"警告: 第{line_num}行包含非数值数据: {line}")
                    
        if not x:
            print("错误: 文件中没有有效数据")
            return
            
        # 绘制折线图
        plt.figure(figsize=(10, 6))
        plt.plot(x, y, color='red', linestyle='-', linewidth=2, alpha=0.7, label='27Al')
        plt.plot(x, z, color='blue', linestyle='-', linewidth=2, alpha=0.7, label='W')
        
        if p and q:
            plt.plot(p, q, color='green', linestyle='--', linewidth=2, alpha=0.7, label='SRIM模拟')
        
        plt.xlabel('Depth (nm)')
        plt.ylabel('Counts')
        plt.title('27Al和W的深度分布')
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