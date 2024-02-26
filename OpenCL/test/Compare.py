import matplotlib.pyplot as plt
import os

comparePath = os.path.join(os.path.dirname(__file__), '../res/compare.txt')
plotPath = os.path.join(os.path.dirname(__file__), '../res/compare.png')

with open(comparePath, 'r') as file:
    lines = file.readlines()

size = []
cpuSortCpu = []
gpuSortCpu = []
gpuSortGpu = []

for line in lines:
    data = line.strip().split()
    size.append(float(data[0]))
    cpuSortCpu.append(float(data[1]))
    gpuSortCpu.append(float(data[2]))
    gpuSortGpu.append(float(data[3]))

grouped_data = {}

uniqueSizeCount = []
uniqueSize = []
uniqueCpuSortCpu = []
uniqueGpuSortCpu = []
uniqueGpuSortGpu = []

for i in range(len(size)):
    if size[i] not in uniqueSize:
        uniqueSizeCount.append(1)
        uniqueSize.append(size[i])
        uniqueCpuSortCpu.append(cpuSortCpu[i])
        uniqueGpuSortCpu.append(gpuSortCpu[i])
        uniqueGpuSortGpu.append(gpuSortGpu[i])
    else:
        j = uniqueSize.index(size[i])
        uniqueSizeCount[j] += 1
        uniqueCpuSortCpu[j] += cpuSortCpu[i]
        uniqueGpuSortCpu[j] += gpuSortCpu[i]
        uniqueGpuSortGpu[j] += gpuSortGpu[i]

for i in range(len(uniqueSizeCount)):
    uniqueCpuSortCpu[i] /= uniqueSizeCount[i]
    uniqueGpuSortCpu[i] /= uniqueSizeCount[i]
    uniqueGpuSortGpu[i] /= uniqueSizeCount[i]

uniqueSize.sort()
uniqueCpuSortCpu.sort()
uniqueGpuSortCpu.sort()
uniqueGpuSortGpu.sort()

plt.figure(figsize=(8, 6))
plt.plot(uniqueSize, uniqueCpuSortCpu, label='Cpu sort')
plt.plot(uniqueSize, uniqueGpuSortCpu, label='Gpu sort in cpu')
plt.plot(uniqueSize, uniqueGpuSortGpu, label='Gpu sort in gpu')
plt.xlabel('size')
plt.ylabel('Time, microsec')
plt.legend()
plt.title('Compare plots for bitonic sort')
plt.grid(True)
plt.savefig(plotPath)
