import numpy as np
import matplotlib.pyplot as plt

f = open('data.txt')

h = []
v_x = []
v_y = []
x = []
y = []
i = 0

s = f.readline().split()
for q in s: x.append(float(q))

s = f.readline().split()
for q in s: y.append(float(q))

N = len(x)
M = len(y)
for i in range(M):
    s = f.readline().split()
    h.append([])
    for q in s: h[i].append(float(q))
for i in range(M):
    s = f.readline().split()
    v_y.append([])
    for q in s: v_y[i].append(float(q))
for i in range(M):
    s = f.readline().split()
    v_x.append([])
    for q in s: v_x[i].append(float(q))

X,Y = np.meshgrid(x,y)

H = np.array(h)      # поле напора
Vx = np.array(v_x)   # поле скорости по X
Vy = np.array(v_y)   # поле скорости по Y

'''
levels = np.linspace(90, 101, 256)  # 256 оттенков между 2 и 10
plt.contourf(X,Y,h,levels)
plt.colorbar()
Q = plt.quiver(x, y, v_x, v_y, scale=5, width = 0.002)
qk = plt.quiverkey(Q, X=0.5, Y=0.9, U=1,
                  label='Скорость фильтрации, м/с',  # Подпись
                  labelpos='N',                 # Положение текста (E=восток, справа)
                  coordinates='figure')         # Координаты X,Y относительные
'''
fig, ax = plt.subplots(figsize=(10, 8))

# 1. Заливка поля напора с красивой цветовой картой
# 'RdYlBu_r' - красный (высокий напор) -> синий (низкий напор)
# Можно также 'viridis', 'plasma', 'coolwarm'
contour = ax.contourf(X, Y, h, levels=100, cmap='RdYlBu_r', alpha=0.85)
cbar = plt.colorbar(contour, ax=ax)
cbar.set_label('Напор h, м', fontsize=12, fontweight='bold')

# 2. Добавляем линии равного напора (изолинии) поверх заливки
# Это делает картинку более "научной"
contour_lines = ax.contour(X, Y, h, levels=10, colors='black', linewidths=1, alpha=1)
ax.clabel(contour_lines, inline=True, fontsize=12, fmt='%.1f')

# 3. Векторное поле (разреженное, чтобы не перегружать)
# Уменьшаем количество стрелок для читаемости
step = 4  # Берем каждую 3-ю точку (экспериментируй: 2, 3, 4)
X_sparse = X[::step, ::step]
Y_sparse = Y[::step, ::step]
vx_sparse = Vx[::step, ::step]
vy_sparse = Vy[::step, ::step]

# Цвет стрелок в зависимости от скорости (длины вектора)
speed = np.sqrt(vx_sparse**2 + vy_sparse**2)

# Стрелки с цветовой картой
quiver = ax.quiver(X_sparse, Y_sparse, vx_sparse, vy_sparse, speed,
                   cmap='plasma', scale=2, width=0.005, 
                   headwidth=4, headlength=5, headaxislength=3)

# Добавляем цветовую шкалу для стрелок
cbar_quiver = plt.colorbar(quiver, ax=ax)
cbar_quiver.set_label('Скорость фильтрации, м/с', fontsize=10)

# 4. Настройки осей
ax.set_xlabel('X, м', fontsize=12)
ax.set_ylabel('Y, м', fontsize=12)
ax.set_title('Фильтрация грунтовых вод\nПоле напора и скорости', fontsize=14, fontweight='bold')

# 5. Добавляем контуры плотины/границ (опционально)
# Например, если есть непроницаемая стенка или форма области
# ax.axvline(x=0, color='blue', linewidth=2, linestyle='--')
# ax.axvline(x=max(x), color='red', linewidth=2, linestyle='--')
# ax.axhline(y=0, color='black', linewidth=2)
# ax.axhline(y=max(y), color='black', linewidth=2)

# 6. Сетка и соотношение сторон
ax.grid(True, alpha=0.3, linestyle='--')
ax.set_aspect('equal')  # Чтобы масштаб по X и Y был одинаковым

# 7. Настройка границ графика
ax.set_xlim(min(x), max(x))
ax.set_ylim(min(y), max(y))

plt.tight_layout()
plt.savefig('graph.png')
