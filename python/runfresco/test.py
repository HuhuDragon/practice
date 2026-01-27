import subprocess


# 定义文件路径
input_file = r"C:\Users\77421\Desktop\python\runfresco\26mg8.324.in"  # 输入文件
output_file = r"C:\Users\77421\Desktop\python\runfresco\test26mg.out"  # 输出文件
program_name = r"C:\Users\77421\Desktop\python\runfresco\frescoR.exe"      # 要运行的程序

# 使用 subprocess 运行程序并重定向输入输出
try:
    with open(input_file, "r") as infile, open(output_file, "w") as outfile:
        # 运行程序
        result = subprocess.run(
            [program_name],  # 要运行的程序
            stdin=infile,    # 输入重定向
            stdout=outfile,  # 输出重定向
            stderr=subprocess.PIPE,  # 捕获错误输出
            text=True        # 以文本模式处理输入输出
        )

        # 检查程序是否成功运行
        if result.returncode == 0:
            print(f"程序 {program_name} 运行成功，输出已保存到 {output_file}")
        else:
            print(f"程序 {program_name} 运行失败，错误信息：")
            print(result.stderr)
except FileNotFoundError:
    print(f"错误：未找到程序 {program_name} 或输入文件 {input_file}")
except Exception as e:
    print(f"发生未知错误：{e}")