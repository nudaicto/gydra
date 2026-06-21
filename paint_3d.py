import numpy as np
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# ========== ЧТЕНИЕ CSV ==========
def read_3d_csv(filename):
    """Читает CSV с заголовками z,y,x,h,alpha,board,type,vz,vy,vx,pressure"""
    df = pd.read_csv(filename)
    
    x = np.sort(df['x'].unique())
    y = np.sort(df['y'].unique())
    z = np.sort(df['z'].unique())
    
    Nx, Ny, Nz = len(x), len(y), len(z)
    
    # Преобразуем плоские массивы в 3D сетки
    # Порядок индексов: [z, y, x] как в C++ коде
    h = df['h'].values.reshape(Nz, Ny, Nx)
    vx = df['vx'].values.reshape(Nz, Ny, Nx)
    vy = df['vy'].values.reshape(Nz, Ny, Nx)
    vz = df['vz'].values.reshape(Nz, Ny, Nx)
    alpha = df['alpha'].values.reshape(Nz, Ny, Nx)
    P = df['pressure'].values.reshape(Nz, Ny, Nx)
    
    speed = np.sqrt(vx**2 + vy**2 + vz**2)
    
    return x, y, z, h, speed, P, alpha, vx, vy, vz

# ========== ЗАГРУЗКА ДАННЫХ ==========
x, y, z, h, speed, P, alpha, vx, vy, vz = read_3d_csv('result_3d.csv')

# Создаем 3D meshgrid для координат
X, Y, Z = np.meshgrid(x, y, z, indexing='ij')
# Но нам нужен порядок [z, y, x], поэтому переставим
X, Y, Z = np.meshgrid(x, y, z, indexing='xy')  # теперь shape: (Ny, Nx, Nz)
# Нужно транспонировать данные под этот порядок
h_t = np.transpose(h, (1, 2, 0))  # (Ny, Nx, Nz)
speed_t = np.transpose(speed, (1, 2, 0))
P_t = np.transpose(P, (1, 2, 0))
alpha_t = np.transpose(alpha, (1, 2, 0))

# Маскируем препятствия (alpha == 4)
obstacle_mask = (alpha_t == 4)
h_masked = np.where(obstacle_mask, np.nan, h_t)
speed_masked = np.where(obstacle_mask, np.nan, speed_t)
P_masked = np.where(obstacle_mask, np.nan, P_t)

# ========== ПОСТРОЕНИЕ ГРАФИКОВ ==========
# Создаем фигуру с тремя подграфиками
fig = make_subplots(
    rows=1, cols=3,
    specs=[[{'is_3d': True}, {'is_3d': True}, {'is_3d': True}]],
    subplot_titles=('Поле напора h', 'Модуль скорости |v|', 'Давление P')
)

# --- График 1: Напор ---
fig.add_trace(
    go.Isosurface(
        x=X.flatten(), y=Y.flatten(), z=Z.flatten(),
        value=h_masked.flatten(),
        isomin=np.nanmin(h_masked),
        isomax=np.nanmax(h_masked),
        surface_count=10,
        colorscale='balance',  # ✅ ИСПРАВЛЕНО (было 'Coolwarm')
        opacity=0.6,
        name='h'
    ),
    row=1, col=1
)

# --- График 2: Скорость ---
fig.add_trace(
    go.Isosurface(
        x=X.flatten(), y=Y.flatten(), z=Z.flatten(),
        value=speed_masked.flatten(),
        isomin=np.nanmin(speed_masked),
        isomax=np.nanmax(speed_masked),
        surface_count=10,
        colorscale='plasma',  # ✅ В нижнем регистре
        opacity=0.6,
        name='|v|'
    ),
    row=1, col=2
)

# --- График 3: Давление ---
fig.add_trace(
    go.Isosurface(
        x=X.flatten(), y=Y.flatten(), z=Z.flatten(),
        value=P_masked.flatten(),
        isomin=np.nanmin(P_masked),
        isomax=np.nanmax(P_masked),
        surface_count=10,
        colorscale='viridis',  # ✅ В нижнем регистре
        opacity=0.6,
        name='P'
    ),
    row=1, col=3
)
# Настройка осей и отображения
fig.update_layout(
    title='3D визуализация результатов моделирования',
    width=1800,
    height=600,
    scene=dict(
        xaxis_title='X',
        yaxis_title='Y',
        zaxis_title='Z',
        aspectmode='data'
    ),
    scene2=dict(
        xaxis_title='X',
        yaxis_title='Y',
        zaxis_title='Z',
        aspectmode='data'
    ),
    scene3=dict(
        xaxis_title='X',
        yaxis_title='Y',
        zaxis_title='Z',
        aspectmode='data'
    )
)

# Сохраняем в HTML
fig.write_html('visualization_3d.html')
print("Интерактивная визуализация сохранена в visualization_3d.html")