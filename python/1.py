import re

# --- 在这里填入你的 Message-ID ---
message_id = "<CA+h_W8g7q=2X5s9r-Q3nZ4@mail.gmail.com>" 
# --------------------------------

# 1. 找出里面的所有数字
digits = re.findall(r'\d', message_id)

# 2. 检查数字够不够
if len(digits) < 2:
    print("错误：这个 Message-ID 里数字不够两个，无法计算。")
else:
    # 3. 取出前两个数字
    num1 = int(digits[0])
    num2 = int(digits[1])

    # 4. 计算余数
    result1 = num1 % 3
    result2 = num2 % 3

    print(f"原始ID: {message_id}")
    print("-" * 30)
    print(f"Journal 序号是: {num1} -> 除以3余: 【{result1}】")
    print(f"报告人序号是: {num2} -> 除以3余: 【{result2}】")