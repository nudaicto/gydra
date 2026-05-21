import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# ========== Читаем CSV файл ==========
def read_from_csv_single(filename):
    """Читает CSV с заголовками y,x,h,alpha,board,type,vy,vx,pressure"""
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
    P = df['pressure'].values.reshape(M, N)  # давление из CSV
    
    return X, Y, h, vx, vy, alpha, board, P, x, y

# ========== ЗАГРУЗКА ==========
X, Y, h, vx, vy, alpha, board, P, x, y = read_from_csv_single('result.csv')

# ========== ПОДГОТОВКА ДАННЫХ ==========
hide_mask = (board == "no") & (alpha == 4)
speed = np.sqrt(vx**2 + vy**2)

h_masked = np.ma.masked_where(hide_mask, h)
speed_masked = np.ma.masked_where(hide_mask, speed)
P_masked = np.ma.masked_where(hide_mask, P)

mask_for_arrows = ~hide_mask & ((alpha == 1) | (alpha == 2) | (alpha == 3))

# Разрежение для стрелок
step = 6
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
speed_quiv = np.sqrt(vx_quiv**2 + vy_quiv**2)

# ========== ПОСТРОЕНИЕ ТРЁХ ГРАФИКОВ ==========
fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(20, 7))

# ----- Левый график: Напор -----
contour1 = ax1.contourf(X, Y, h_masked, levels=100, cmap='coolwarm', alpha=0.85)
cbar1 = plt.colorbar(contour1, ax=ax1)
cbar1.set_label('Напор h, м', fontsize=10)

contour_lines1 = ax1.contour(X, Y, h_masked, levels=15, colors='black', linewidths=0.5, alpha=0.6)
ax1.clabel(contour_lines1, inline=True, fontsize=8, fmt='%.1f')

if len(X_quiv) > 0:
    ax1.quiver(X_quiv, Y_quiv, vx_quiv, vy_quiv, speed_quiv,
               cmap='plasma', scale=3, width=0.003,
               headwidth=4, headlength=5, headaxislength=4, alpha=0.7)

if np.any(hide_mask):
    ax1.contourf(X, Y, hide_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.5, hatches=['//'])
dry_mask = (alpha == 0) & ~hide_mask
if np.any(dry_mask):
    ax1.contourf(X, Y, dry_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.3)

ax1.set_xlabel('X, м')
ax1.set_ylabel('Y, м')
ax1.set_title('Поле напора', fontsize=12, fontweight='bold')
ax1.grid(True, alpha=0.3, linestyle='--')
ax1.set_aspect('equal')
ax1.set_xlim(min(x), max(x))
ax1.set_ylim(min(y), max(y))

# ----- Центральный график: Модуль скорости -----
contour2 = ax2.contourf(X, Y, speed_masked, levels=100, cmap='plasma', alpha=0.85)
cbar2 = plt.colorbar(contour2, ax=ax2)
cbar2.set_label('Модуль скорости, м/с', fontsize=10)

contour_lines2 = ax2.contour(X, Y, speed_masked, levels=15, colors='black', linewidths=0.5, alpha=0.6)
ax2.clabel(contour_lines2, inline=True, fontsize=8, fmt='%.4f')

if np.any(hide_mask):
    ax2.contourf(X, Y, hide_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.5, hatches=['//'])
if np.any(dry_mask):
    ax2.contourf(X, Y, dry_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.3)

ax2.set_xlabel('X, м')
ax2.set_ylabel('Y, м')
ax2.set_title('Модуль скорости', fontsize=12, fontweight='bold')
ax2.grid(True, alpha=0.3, linestyle='--')
ax2.set_aspect('equal')
ax2.set_xlim(min(x), max(x))
ax2.set_ylim(min(y), max(y))

# ----- Правый график: Давление -----
contour3 = ax3.contourf(X, Y, P_masked, levels=100, cmap='coolwarm', alpha=0.85)
cbar3 = plt.colorbar(contour3, ax=ax3)
cbar3.set_label('Давление P, Па', fontsize=10)

contour_lines3 = ax3.contour(X, Y, P_masked, levels=15, colors='black', linewidths=0.5, alpha=0.6)
ax3.clabel(contour_lines3, inline=True, fontsize=8, fmt='%.0f')

if len(X_quiv) > 0:
    ax3.quiver(X_quiv, Y_quiv, vx_quiv, vy_quiv, speed_quiv,
               cmap='plasma', scale=3, width=0.003,
               headwidth=4, headlength=5, headaxislength=4, alpha=0.7)

if np.any(hide_mask):
    ax3.contourf(X, Y, hide_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.5, hatches=['//'])
if np.any(dry_mask):
    ax3.contourf(X, Y, dry_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.3)

ax3.set_xlabel('X, м')
ax3.set_ylabel('Y, м')
ax3.set_title('Поле давления', fontsize=12, fontweight='bold')
ax3.grid(True, alpha=0.3, linestyle='--')
ax3.set_aspect('equal')
ax3.set_xlim(min(x), max(x))
ax3.set_ylim(min(y), max(y))

plt.tight_layout()
plt.savefig('graph_comparison.png', dpi=150, bbox_inches='tight')