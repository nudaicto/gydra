import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# ========== СПОСОБ 1: Читаем один CSV файл ==========
def read_from_csv_single(filename):
    """Читает CSV с заголовками y,x,h,alpha,board,type,vy,vx"""
    df = pd.read_csv(filename)
    
    # Получаем уникальные координаты
    x = np.sort(df['x'].unique())
    y = np.sort(df['y'].unique())
    
    N = len(x)
    M = len(y)
    
    # Преобразуем в сетки
    X, Y = np.meshgrid(x, y)
    
    # Перестраиваем данные в матрицы MxN
    h = df['h'].values.reshape(M, N)
    vx = df['vx'].values.reshape(M, N)
    vy = df['vy'].values.reshape(M, N)
    alpha = df['alpha'].values.reshape(M, N)
    
    return X, Y, h, vx, vy, alpha, x, y

# ========== ВЫБЕРИ СПОСОБ ==========
# Способ 1: один CSV файл
X, Y, h, vx, vy, alpha, x, y = read_from_csv_single('result.csv')

# ========== ПОСТРОЕНИЕ ГРАФИКА ==========
fig, ax = plt.subplots(figsize=(12, 10))

# 1. Заливка поля напора
contour = ax.contourf(X, Y, h, levels=100, cmap='RdYlBu_r', alpha=0.85)
cbar = plt.colorbar(contour, ax=ax)
cbar.set_label('Напор h, м', fontsize=12, fontweight='bold')

# 2. Линии равного напора (изолинии)
contour_lines = ax.contour(X, Y, h, levels=15, colors='black', linewidths=0.8, alpha=0.7)
ax.clabel(contour_lines, inline=True, fontsize=10, fmt='%.1f')

# 3. Маска области (где alpha=0 или alpha=4 — не рисуем стрелки)
# Создаем маску для стрелок (только там, где alpha == 1 или вода)
mask = (alpha == 1) | (alpha == 2) | (alpha == 3)  # вода, песок, глина и т.д.

# 4. Векторное поле (только в активной области)
step = 4  # Разрежение

# Создаем разреженные сетки
X_sparse = X[::step, ::step]
Y_sparse = Y[::step, ::step]
vx_sparse = vx[::step, ::step]
vy_sparse = vy[::step, ::step]
mask_sparse = mask[::step, ::step]

# Применяем маску (убираем стрелки в воздухе)
vx_sparse_masked = np.where(mask_sparse, vx_sparse, np.nan)
vy_sparse_masked = np.where(mask_sparse, vy_sparse, np.nan)

# Убираем NaN для quiver (заменяем на 0, но потом убираем стрелки)
# Лучше создать отдельные списки координат для стрелок
i_sparse, j_sparse = np.where(mask_sparse)
X_quiv = X_sparse[i_sparse, j_sparse]
Y_quiv = Y_sparse[i_sparse, j_sparse]
vx_quiv = vx_sparse[i_sparse, j_sparse]
vy_quiv = vy_sparse[i_sparse, j_sparse]

# Цвет стрелок в зависимости от скорости
speed = np.sqrt(vx_quiv**2 + vy_quiv**2)

# Стрелки
if len(X_quiv) > 0:
    quiver = ax.quiver(X_quiv, Y_quiv, vx_quiv, vy_quiv, speed,
                       cmap='plasma', scale=3, width=0.005,
                       headwidth=4, headlength=5, headaxislength=4)
    cbar_quiver = plt.colorbar(quiver, ax=ax)
    cbar_quiver.set_label('Скорость фильтрации, м/с', fontsize=10)

# 5. Закрашиваем область, где нет воды (alpha == 0)
# Это опционально — показывает "сухую" зону
dry_mask = (alpha == 0)
if np.any(dry_mask):
    ax.contourf(X, Y, dry_mask, levels=[0.5, 1.5], colors='lightgray', alpha=0.3)

# 6. Настройки осей
ax.set_xlabel('X, м', fontsize=12)
ax.set_ylabel('Y, м', fontsize=12)
ax.set_title('Фильтрация грунтовых вод\nПоле напора и скорости', fontsize=14, fontweight='bold')

# 7. Границы плотины (если нужно)
# ax.axvline(x=0, color='blue', linewidth=2, linestyle='--', label='Левая граница')
# ax.axvline(x=max(x), color='red', linewidth=2, linestyle='--', label='Правая граница')
# ax.axhline(y=0, color='black', linewidth=2, linestyle='-', label='Дно')

# 8. Сетка и соотношение сторон
ax.grid(True, alpha=0.3, linestyle='--')
ax.set_aspect('equal')

# 9. Границы графика
ax.set_xlim(min(x), max(x))
ax.set_ylim(min(y), max(y))

# 10. Легенда (опционально)
# ax.legend(loc='upper right')

plt.tight_layout()
plt.savefig('graph.png', dpi=150, bbox_inches='tight')
plt.show()

# Дополнительно: выводим информацию о расчете
print(f"Размер сетки: {len(x)} x {len(y)}")
print(f"Мин напор: {h.min():.3f}, Макс напор: {h.max():.3f}")
print(f"Мин скорость: {np.sqrt(vx**2 + vy**2).min():.6f}, Макс скорость: {np.sqrt(vx**2 + vy**2).max():.6f}")
print(f"Количество водных узлов: {np.sum(alpha == 1)}")
print(f"Количество воздушных узлов: {np.sum(alpha == 0)}")