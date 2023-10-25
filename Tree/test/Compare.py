import matplotlib.pyplot as plt
import os

comparePath = os.path.join(os.path.dirname(__file__), '../tests/compare.txt')
plotPath = os.path.join(os.path.dirname(__file__), '../res/compare.png')

with open(comparePath, 'r') as file:
    lines = file.readlines()

x = []
y1 = []
y2 = []

for line in lines:
    data = line.strip().split()
    x.append(float(data[0]))
    y1.append(float(data[2]))
    y2.append(float(data[4]))

plt.figure(figsize=(8, 6))
plt.plot(x, y1, label='std::set')
plt.plot(x, y2, label='RBTree')
plt.xlabel('size')
plt.ylabel('Time, msec')
plt.legend()
plt.title('Compare plots for std::set and RBTree')
plt.grid(True)
plt.savefig(plotPath)
