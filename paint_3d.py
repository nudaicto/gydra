import numpy as np
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# ========== ЧТЕНИЕ CSV ==========
def read_3d_csv(filename):
    """Читает CSV с заголовками z,y,x,h,alpha,board,type,vz,vy,vx,pressure,k"""
    df = pd.read_csv(filename)
    
    x = np.sort(df['x'].unique())
    y = np.sort(df['y'].unique())
    z = np.sort(df['z'].unique())
    
    Nx, Ny, Nz = len(x), len(y), len(z)
    
    h = df['h'].values.reshape(Nz, Ny, Nx)
    vx = df['vx'].values.reshape(Nz, Ny, Nx)
    vy = df['vy'].values.reshape(Nz, Ny, Nx)
    vz = df['vz'].values.reshape(Nz, Ny, Nx)
    alpha = df['alpha'].values.reshape(Nz, Ny, Nx)
    P = df['pressure'].values.reshape(Nz, Ny, Nx)
    k = df['k'].values.reshape(Nz, Ny, Nx)
    
    speed = np.sqrt(vx**2 + vy**2 + vz**2)
    
    return x, y, z, h, speed, P, alpha, vx, vy, vz, k

# ========== ЗАГРУЗКА ДАННЫХ ==========
x, y, z, h, speed, P, alpha, vx, vy, vz, k = read_3d_csv('result_3d.csv')

X, Y, Z = np.meshgrid(x, y, z, indexing='xy')

h_t = np.transpose(h, (1, 2, 0))
speed_t = np.transpose(speed, (1, 2, 0))
P_t = np.transpose(P, (1, 2, 0))
alpha_t = np.transpose(alpha, (1, 2, 0))
k_t = np.transpose(k, (1, 2, 0))
vx_t = np.transpose(vx, (1, 2, 0))
vy_t = np.transpose(vy, (1, 2, 0))
vz_t = np.transpose(vz, (1, 2, 0))

obstacle_mask = (alpha_t == 4)
h_masked = np.where(obstacle_mask, np.nan, h_t)
speed_masked = np.where(obstacle_mask, np.nan, speed_t)
P_masked = np.where(obstacle_mask, np.nan, P_t)
k_masked = np.where(obstacle_mask, np.nan, k_t)

# ========== ПОДГОТОВКА СТРЕЛОК ==========
step = 3

arrow_mask = (alpha_t == 1)

X_sparse = X[::step, ::step, ::step]
Y_sparse = Y[::step, ::step, ::step]
Z_sparse = Z[::step, ::step, ::step]
vx_sparse = vx_t[::step, ::step, ::step]
vy_sparse = vy_t[::step, ::step, ::step]
vz_sparse = vz_t[::step, ::step, ::step]
speed_sparse = speed_t[::step, ::step, ::step]
mask_sparse = arrow_mask[::step, ::step, ::step]

i_sparse, j_sparse, l_sparse = np.where(mask_sparse)

X_quiv = X_sparse[i_sparse, j_sparse, l_sparse]
Y_quiv = Y_sparse[i_sparse, j_sparse, l_sparse]
Z_quiv = Z_sparse[i_sparse, j_sparse, l_sparse]
vx_quiv = vx_sparse[i_sparse, j_sparse, l_sparse]
vy_quiv = vy_sparse[i_sparse, j_sparse, l_sparse]
vz_quiv = vz_sparse[i_sparse, j_sparse, l_sparse]
speed_quiv = speed_sparse[i_sparse, j_sparse, l_sparse]

# ========== ПОСТРОЕНИЕ ГРАФИКОВ ==========
fig = make_subplots(
    rows=1, cols=4,
    specs=[[{'is_3d': True}, {'is_3d': True}, {'is_3d': True}, {'is_3d': True}]],
    subplot_titles=('Поле напора h', 'Модуль скорости |v|', 'Давление P', 'Проницаемость k')
)

# --- График 1: Напор + стрелки скорости ---
fig.add_trace(
    go.Isosurface(
        x=X.flatten(), y=Y.flatten(), z=Z.flatten(),
        value=h_masked.flatten(),
        isomin=np.nanmin(h_masked),
        isomax=np.nanmax(h_masked),
        surface_count=10,
        colorscale='balance',
        opacity=0.4,
        name='h',
        colorbar=dict(
            title='h, м',
            x=0.125,  # центр первого subplot
            y=0.05,   # внизу
            len=0.15,
            orientation='h',  # горизонтальная
            thickness=15
        )
    ),
    row=1, col=1
)

# Добавляем стрелки
if len(X_quiv) > 0:
    fig.add_trace(
        go.Cone(
            x=X_quiv.flatten(),
            y=Y_quiv.flatten(),
            z=Z_quiv.flatten(),
            u=vx_quiv.flatten(),
            v=vy_quiv.flatten(),
            w=vz_quiv.flatten(),
            colorscale='Reds',
            sizemode='absolute',
            sizeref=2.0,
            anchor='tail',
            showscale=True,
            cmin=0,
            cmax=np.max(speed_quiv) if len(speed_quiv) > 0 else 1.0,
            colorbar=dict(
                title='|v|, м/с',
                x=0.125,
                y=0.0,  # чуть ниже первого colorbar
                len=0.15,
                orientation='h',
                thickness=15
            ),
            opacity=1.0,
            name='v',
            hovertemplate='x: %{x:.2f}<br>y: %{y:.2f}<br>z: %{z:.2f}<br>|v|: %{norm:.4f} м/с<extra></extra>'
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
        colorscale='plasma',
        opacity=0.6,
        name='|v|',
        colorbar=dict(
            title='|v|, м/с',
            x=0.375,  # центр второго subplot
            y=0.05,
            len=0.15,
            orientation='h',
            thickness=15
        )
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
        colorscale='viridis',
        opacity=0.6,
        name='P',
        colorbar=dict(
            title='P, Па',
            x=0.625,  # центр третьего subplot
            y=0.05,
            len=0.15,
            orientation='h',
            thickness=15
        )
    ),
    row=1, col=3
)

# --- График 4: Проницаемость k ---
fig.add_trace(
    go.Isosurface(
        x=X.flatten(), y=Y.flatten(), z=Z.flatten(),
        value=k_masked.flatten(),
        isomin=np.nanmin(k_masked),
        isomax=np.nanmax(k_masked),
        surface_count=10,
        colorscale='hot',
        opacity=0.7,
        name='k',
        colorbar=dict(
            title='k',
            x=0.875,  # центр четвертого subplot
            y=0.05,
            len=0.15,
            orientation='h',
            thickness=15
        )
    ),
    row=1, col=4
)

fig.update_layout(
    title='3D визуализация результатов моделирования',
    width=2400,
    height=800,  # увеличил высоту для colorbar внизу
    scene=dict(
        xaxis_title='X',
        yaxis_title='Y',
        zaxis_title='Z',
        aspectmode='data',
        bgcolor='white',
        domain=dict(x=[0, 0.25], y=[0.15, 1])  # оставляем место внизу
    ),
    scene2=dict(
        xaxis_title='X',
        yaxis_title='Y',
        zaxis_title='Z',
        aspectmode='data',
        domain=dict(x=[0.25, 0.5], y=[0.15, 1])
    ),
    scene3=dict(
        xaxis_title='X',
        yaxis_title='Y',
        zaxis_title='Z',
        aspectmode='data',
        domain=dict(x=[0.5, 0.75], y=[0.15, 1])
    ),
    scene4=dict(
        xaxis_title='X',
        yaxis_title='Y',
        zaxis_title='Z',
        aspectmode='data',
        domain=dict(x=[0.75, 1], y=[0.15, 1])
    )
)

fig.write_html('visualization_3d.html')
print("Интерактивная визуализация сохранена в visualization_3d.html")
print(f"Добавлено стрелок: {len(X_quiv)}")