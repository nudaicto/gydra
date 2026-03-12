import numpy as np
import matplotlib.pyplot as plt

f = open('data.txt')

h = []
x = []
y = []
i = 0

s = f.readline().split()
for q in s: x.append(float(q))

s = f.readline().split()
for q in s: y.append(float(q))


for line in f:
    s = line.split()
    h.append([])
    for q in s: h[i].append(float(q))
    i+=1
N = len(h[0])
M = len(h)


X,Y = np.meshgrid(x,y)

levels = np.linspace(90, 101, 256)  # 256 оттенков между 2 и 10
plt.contourf(X,Y,h,levels)
plt.colorbar()
plt.savefig('graph.png')
