import pandas as pd
import matplotlib.pyplot as plt

# 读取CSV文件
df = pd.read_csv('results.csv')

# 确保CPU利用率列的格式正确
if 'CPU Utilization (%)' in df.columns:
    df['CPU Utilization'] = df['CPU Utilization (%)']

# 定义要展示的指标（使用与CSV文件匹配的列名）
metrics = [
    'Throughput (processes/unit time)',
    'CPU Utilization (%)',
    'Average Turnaround Time',
    'Average Response Time',
    'Average Waiting Time'
]

algorithms = df['Algorithm']

# 创建图表
plt.figure(figsize=(14, 10))

# 绘制每个指标的柱状图
for i, metric in enumerate(metrics, 1):
    plt.subplot(3, 2, i)
    bars = plt.bar(algorithms, df[metric])

    # 设置标题和标签
    title = metric
    plt.title(title)
    plt.xticks(rotation=15)

    # 添加数值标签
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width() / 2., height,
                 f'{height:.2f}',
                 ha='center', va='bottom')

# 调整布局
plt.tight_layout()

# 保存图表
plt.savefig('scheduling_comparison.png', dpi=300, bbox_inches='tight')
print(f"Plot has been saved as 'scheduling_comparison.png'")

# 显示图表
plt.show()