import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('results.csv')

if 'CPU Utilization (%)' in df.columns:
    df['CPU Utilization'] = df['CPU Utilization (%)']

metrics = [
    'Throughput (processes/unit time)',
    'CPU Utilization (%)',
    'Average Turnaround Time',
    'Average Response Time',
    'Average Waiting Time'
]

algorithms = df['Algorithm']
plt.figure(figsize=(14, 10))

for i, metric in enumerate(metrics, 1):
    plt.subplot(3, 2, i)
    bars = plt.bar(algorithms, df[metric])

    title = metric
    plt.title(title)
    plt.xticks(rotation=15)

    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width() / 2., height,
                 f'{height:.2f}',
                 ha='center', va='bottom')

plt.tight_layout()

plt.savefig('scheduling_comparison.png', dpi=300, bbox_inches='tight')
print(f"Plot has been saved as 'scheduling_comparison.png'")

plt.show()
