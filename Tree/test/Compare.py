import matplotlib.pyplot as plt

with open('../test/compare.txt', 'r') as file:
    lines = file.readlines()

x = []
y1 = []
y2 = []

for line in lines:
    data = line.strip().split()
    x.append(float(data[0]))
    y1.append(float(data[1]))
    y2.append(float(data[2]))

plt.figure(figsize=(8, 6))
plt.plot(x, y1, label='std::set')
plt.plot(x, y2, label='RBTree')
plt.xlabel('size')
plt.ylabel('Time, msec')
plt.legend()
plt.title('Compare plots for std::set and RBTree')
plt.grid(True)
plt.savefig("../res/compare.png")
