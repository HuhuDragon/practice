import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams


plt.rcParams['font.family'] = ['DejaVu Sans'] #中文 SimHei
plt.rcParams['axes.unicode_minus'] = False

def plot_data_from_txt(filename, output_path):


    x = []  
    y = [] 
    
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

                    
                    x.append(x_val)
                    y.append(y_val)

                        
                except ValueError:
                    print(f"警告: 第{line_num}行包含非数值数据: {line}")
                    
        if not x:
            print("错误: 文件中没有有效数据")
            return
            
        # 绘制折线图
        plt.figure(figsize=(10, 6))
        plt.plot(x, y, color='red', linestyle='-', linewidth=2, alpha=0.7)

        plt.xlim(10, 60)
        plt.yscale('log') 
        plt.xlabel('Angle(degrees)')
        plt.ylabel('Corss Section (mb/sr)')
        plt.title('90Zr+12C')
        plt.grid(True, linestyle='--', alpha=0.5)

        
        # 保存为PDF
        plt.savefig(output_path, format='pdf', bbox_inches='tight')
        plt.show()
            
    except FileNotFoundError:
        print(f"错误: 文件 {filename} 未找到")
    except Exception as e:
        print(f"发生错误: {str(e)}")


if __name__ == "__main__":
    filename = 'data.txt' 
    output_path = 'histogram.pdf'  
    plot_data_from_txt(filename, output_path)