import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.patheffects as path_effects

# ========== Читаем CSV файл ==========
def read_from_csv_single(filename):
    """Читает CSV с заголовками y,x,h,alpha,board,type,vy,vx"""
    df = pd.read_csv(filename)
    
    x = np.sort(df['x'].unique())
    y = np.sort(df['y'].unique())
    
    N = len(x)
    M = len(y)
    
    X, Y = np.meshgrid(x, y)
    
    h = df['h'].values.reshape(M, N)
    vx = df['vx'].values.reshape(M, N)
    vy = df['vy'].values.reshape(M, N)
    alpha = df['alpha'].values.reshape(M, N)
    board = df['board'].values.reshape(M, N)
    
    return X, Y, h, vx, vy, alpha, board, x, y

# ========== ЗАГРУЗКА ==========
X, Y, h, vx, vy, alpha, board, x, y = read_from_csv_single('result.csv')

# ========== ПОСТРОЕНИЕ ГРАФИКА (фон и линии — скорость) ==========
fig, ax = plt.subplots(figsize=(12, 10))

# Маска для скрытой области
hide_mask = (board == "no") & (alpha == 4)

# Вычисляем модуль скорости
speed = np.sqrt(vx**2 + vy**2)

# Маскируем скорость в скрытой области
speed_masked = np.ma.masked_where(hide_mask, speed)

# 1. Заливка — поле СКОРОСТИ
contour = ax.contourf(X, Y, speed_masked, levels=100, cmap='plasma', alpha=0.85)
cbar = plt.colorbar(contour, ax=ax)
cbar.set_label('Модуль скорости, м/с', fontsize=12, fontweight='bold')

# 2. Линии равного МОДУЛЯ СКОРОСТИ (а не напора)
contour_lines = ax.contour(X, Y, speed_masked, levels=15, colors='black', linewidths=0.6, alpha=0.7)
labels = ax.clabel(contour_lines, inline=True, fontsize=10, fmt='%.3f')

# 3. Маска для стрелок
mask_for_arrows = ~hide_mask & ((alpha == 1) | (alpha == 2) | (alpha == 3))

# 4. Векторное поле (разреженное)
step = 4
X_sparse = X[::step, ::step]
Y_sparse = Y[::step, ::step]
vx_sparse = vx[::step, ::step]
vy_sparse = vy[::step, ::step]
mask_sparse = mask_for_arrows[::step, ::step]

i_sparse, j_sparse = np.where(mask_sparse)
X_quiv = X_sparse[i_sparse, j_sparse]
Y_quiv = Y_sparse[i_sparse, j_sparse]
vx_quiv = vx_sparse[i_sparse, j_sparse]
vy_quiv = vy_sparse[i_sparse, j_sparse]

# Цвет стрелок — по модулю скорости
speed_quiv = np.sqrt(vx_quiv**2 + vy_quiv**2)

if len(X_quiv) > 0:
    quiver = ax.quiver(X_quiv, Y_quiv, vx_quiv, vy_quiv, speed_quiv,
                       cmap='plasma', scale=3, width=0.003,
                       headwidth=4, headlength=5, headaxislength=4)

# 5. Скрытая область
if np.any(hide_mask):
    ax.contourf(X, Y, hide_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.5, hatches=['//'])

# 6. Сухая зона
dry_mask = (alpha == 0) & ~hide_mask
if np.any(dry_mask):
    ax.contourf(X, Y, dry_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.3)

# 7. Оформление
ax.set_xlabel('X, м', fontsize=12)
ax.set_ylabel('Y, м', fontsize=12)
ax.set_title('Фильтрация грунтовых вод\nМодуль скорости (фон и изолинии)', fontsize=14, fontweight='bold')

ax.grid(True, alpha=0.3, linestyle='--')
ax.set_aspect('equal')
ax.set_xlim(min(x), max(x))
ax.set_ylim(min(y), max(y))

plt.tight_layout()
plt.savefig('graph_speed.png', dpi=150, bbox_inches='tight')