import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams
from scipy.interpolate import make_interp_spline

plt.rcParams['font.family'] = ['DejaVu Sans']  # 中文 SimHei
plt.rcParams['axes.unicode_minus'] = False

def plot_data_from_txt(filename, output_path, smooth_curve=False, k=3):
    """
    从文本文件读取数据并绘制图表，可选择添加平滑曲线
    
    参数:
        filename: 输入数据文件名
        output_path: 输出PDF文件路径
        smooth_curve: 是否生成平滑曲线 (默认False)
        k: 样条曲线阶数 (默认3，即三次样条)
    """
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
            
        # 转换为numpy数组
        x = np.array(x)
        y = np.array(y)
        
        # 绘制折线图
        plt.figure(figsize=(10, 6))
        # plt.plot(x, y, color='red', linestyle='-', linewidth=2, alpha=0.7, label='原始数据')

        # 如果需要生成平滑曲线
        if smooth_curve:
            try:
                # 对数据进行排序，因为样条插值需要x值单调递增
                sort_idx = np.argsort(x)
                x_sorted = x[sort_idx]
                y_sorted = y[sort_idx]
                
                # 创建样条插值器
                spl = make_interp_spline(x_sorted, y_sorted, k=k)
                
                # 生成平滑曲线的x值
                x_smooth = np.linspace(min(x), max(x), 500)
                y_smooth = spl(x_smooth)
                
                # 绘制平滑曲线
                plt.plot(x_smooth, y_smooth, 'b-', linewidth=2, 
                         label=f'{k}阶样条插值', alpha=0.7)
                
            except Exception as e:
                print(f"\n生成平滑曲线时出错: {str(e)}")

        plt.xlim(10, 60)
        plt.yscale('log') 
        plt.xlabel('Angle(degrees)')
        plt.ylabel('Cross Section (mb/sr)')
        plt.title('90Zr(12C,13N)89Y')
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
    plot_data_from_txt(filename, output_path, smooth_curve=True, k=3)  # 使用三次样条插值