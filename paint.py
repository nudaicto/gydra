import numpy as np
import matplotlib.pyplot as plt


l = 100
h0 = 150
dx = 2.5
dy = 2.5
h1 = 90
h2 = 90
x = [0]
y = [0]
i = 0
while i<l:
    i+=dx
    x.append(i)
i = 0
while i<h0:
    i+=dy
    y.append(i)
N = len(x)
M = len(y)
print(N, M)

h = [[(h1+h2)/2]*N for i in range (M)]
for i in range(M):
    h[i][0] = h1
    h[i][N-1] = h2

#обращаться надо h[y][x]

h_1 = [(h1+h2)/2]*N #фиктивные узлы на -1
h_M = [(h1+h2)/2]*N #фиктивные узлы на M+1

e = 0.00001
e0 = 10
h_old = 0
max_iter = 10000
for iter in range(max_iter):
    e0=0
    #пересчет фиктивных точек снизу
    for i in range(N):
        h_1[i] = h[1][i]
    #пересчет дна
    for j in range(1,N-1):
        h_old = h[0][j]
        h[0][j] = ((h[0][j-1]+h[0][j+1])*dy/dx + 2*h[1][j])/(2*(dy/dx) + 2)
        e0 = max(e0, abs(h_old - h[0][j]))
    #пересчет основных точек
    for i in range (1,M-1):
        for j in range(1,N-1):
            h_old = h[i][j]
            h[i][j] = ((h[i][j-1]+h[i][j+1])*dy/dx + h[i-1][j] + h[i+1][j])/(2*(dy/dx) + 2)
            e0 = max(e0, abs(h_old - h[i][j]))
    #пересчет крышки
    for j in range(1,N-1):
        h_old = h[-1][j]
        h[-1][j] = ((h[-1][j-1]+h[-1][j+1])*dy/dx + 2*h[-2][j])/(2*(dy/dx) + 2)
        e0 = max(e0, abs(h_old - h[-1][j]))
    #пересчет фиктивных точек сверху
    for i in range(N):
        h_M[i] = h[-2][i]
        # Проверка сходимости

    if (iter == 0 or iter == 9 or iter == 99 or iter == 999):
        print(f"Iter {iter + 1} : {h[M // 2][N // 2]}")
    if e0 < e:
        print(f"Сошлось за {iter + 1} итераций")
        break
for i in range(M):
    print(h[i])
X,Y = np.meshgrid(x,y)

levels = np.linspace(90, 101, 256)  # 256 оттенков между 2 и 10
plt.contourf(X,Y,h,levels)
plt.colorbar()
plt.savefig('graph.png')
