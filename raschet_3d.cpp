#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <string>
#include "configuration_3d.h"

class CPoint3D {
public:
    double x, y, z;
    double h;
    double vx, vy, vz;
    int alpha;
    double k;
    std::string board;
    std::string type;
    double value;
    double pressure;

    CPoint3D() : x(0), y(0), z(0), h(0), vx(0), vy(0), vz(0), alpha(1), k(1.0), 
                 board("no"), type("no"), value(0), pressure(0) {}
};

// === ФУНКЦИЯ f_k ДЛЯ ОПРЕДЕЛЕНИЯ ПРОНИЦАЕМОСТИ ===
double f_k(double x, double y, double z, 
           const std::vector<std::variant<CSphere3D, CBox3D>>& primitives, 
           double k_default) {
    for (const auto& prim : primitives) {
        std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr (std::is_same_v<T, CSphere3D>) {
                double dist = std::sqrt((x-obj.x0)*(x-obj.x0) + 
                                       (y-obj.y0)*(y-obj.y0) + 
                                       (z-obj.z0)*(z-obj.z0));
                if (dist <= obj.R) {
                    k_default = obj.k_value;
                }
            }
            else if constexpr (std::is_same_v<T, CBox3D>) {
                if (x >= obj.x1 && x <= obj.x2 && 
                    y >= obj.y1 && y <= obj.y2 && 
                    z >= obj.z1 && z <= obj.z2) {
                    k_default = obj.k_value;
                }
            }
        }, prim);
    }
    return k_default;
}

int main() {
    // === ЧТЕНИЕ КОНФИГУРАЦИИ ИЗ ФАЙЛА ===
    Config3D config;
    
    if (!read_configuration_3d("configuration_3d.txt", config)) {
        std::cerr << "Используются параметры по умолчанию" << std::endl;
    }

    double x_max = config.x_max;
    double y_max = config.y_max;
    double z_max = config.z_max;
    double dx = config.dx;
    double dy = config.dy;
    double dz = config.dz;

    std::vector<double> x, y, z;
    for(double val = 0; val <= x_max + 1e-9; val += dx) x.push_back(val);
    for(double val = 0; val <= y_max + 1e-9; val += dy) y.push_back(val);
    for(double val = 0; val <= z_max + 1e-9; val += dz) z.push_back(val);

    int Nx = x.size();
    int Ny = y.size();
    int Nz = z.size();

    std::cout << "Размер сетки: " << Nx << " x " << Ny << " x " << Nz << std::endl;

    // Инициализация поля
    std::vector<std::vector<std::vector<CPoint3D>>> pole(Nz, 
        std::vector<std::vector<CPoint3D>>(Ny, 
        std::vector<CPoint3D>(Nx)));

    for(int i = 0; i < Nz; i++) {
        for(int j = 0; j < Ny; j++) {
            for(int k = 0; k < Nx; k++) {
                pole[i][j][k].x = x[k];
                pole[i][j][k].y = y[j];
                pole[i][j][k].z = z[i];
                
                bool is_boundary = (i == 0 || i == Nz-1 || j == 0 || j == Ny-1 || k == 0 || k == Nx-1);
                
                if (is_boundary) {
                    pole[i][j][k].alpha = 4;
                    
                    // === ГРАНИЧНЫЕ УСЛОВИЯ ИЗ КОНФИГУРАЦИИ ===
                    if (k == 0) {
                        pole[i][j][k].board = "x_left";
                        pole[i][j][k].type = config.bc_x_left.type;
                        pole[i][j][k].value = config.bc_x_left.value;
                    }
                    else if (k == Nx-1) {
                        pole[i][j][k].board = "x_right";
                        pole[i][j][k].type = config.bc_x_right.type;
                        pole[i][j][k].value = config.bc_x_right.value;
                    }
                    else if (j == 0) {
                        pole[i][j][k].board = "y_bottom";
                        pole[i][j][k].type = config.bc_y_bottom.type;
                        pole[i][j][k].value = config.bc_y_bottom.value;
                    }
                    else if (j == Ny-1) {
                        pole[i][j][k].board = "y_top";
                        pole[i][j][k].type = config.bc_y_top.type;
                        pole[i][j][k].value = config.bc_y_top.value;
                    }
                    else if (i == 0) {
                        pole[i][j][k].board = "z_back";
                        pole[i][j][k].type = config.bc_z_back.type;
                        pole[i][j][k].value = config.bc_z_back.value;
                    }
                    else if (i == Nz-1) {
                        pole[i][j][k].board = "z_front";
                        pole[i][j][k].type = config.bc_z_front.type;
                        pole[i][j][k].value = config.bc_z_front.value;
                    }
                } else {
                    pole[i][j][k].alpha = 1;
                    
                    // === ОПРЕДЕЛЕНИЕ ПРОНИЦАЕМОСТИ k ===
                    pole[i][j][k].k = f_k(x[k], y[j], z[i], config.k_primitives, 1.0);
                }
            }
        }
    }

    // Итерационный решатель
    double e = 1e-5;
    double e0 = 10.0;
    int max_iter = 15000;

    for (int iter = 0; iter < max_iter; iter++) {
        e0 = 0;
        double h_old;

        // ЭТАП 1: Применяем граничные условия
        for (int i = 0; i < Nz; i++) {
            for (int j = 0; j < Ny; j++) {
                for (int k = 0; k < Nx; k++) {
                    h_old = pole[i][j][k].h;
                    
                    if (pole[i][j][k].type == "fixed") {
                        pole[i][j][k].h = pole[i][j][k].value;
                    }
                    else if (pole[i][j][k].type == "zero_gradient") {
                        bool on_x_left   = (k == 0);
                        bool on_x_right  = (k == Nx-1);
                        bool on_y_bottom = (j == 0);
                        bool on_y_top    = (j == Ny-1);
                        bool on_z_back   = (i == 0);
                        bool on_z_front  = (i == Nz-1);
                        
                        int boundary_count = 0;
                        if (on_x_left || on_x_right) boundary_count++;
                        if (on_y_bottom || on_y_top) boundary_count++;
                        if (on_z_back || on_z_front) boundary_count++;
                        
                        if (boundary_count == 3) {
                            double sum = 0.0;
                            int count = 0;
                            if (on_x_left)     { sum += pole[i][j][k+1].h; count++; }
                            if (on_x_right)    { sum += pole[i][j][k-1].h; count++; }
                            if (on_y_bottom)   { sum += pole[i][j+1][k].h; count++; }
                            if (on_y_top)      { sum += pole[i][j-1][k].h; count++; }
                            if (on_z_back)     { sum += pole[i+1][j][k].h; count++; }
                            if (on_z_front)    { sum += pole[i-1][j][k].h; count++; }
                            pole[i][j][k].h = sum / count;
                        }
                        else if (boundary_count == 2) {
                            double sum = 0.0;
                            int count = 0;
                            if (on_x_left)     { sum += pole[i][j][k+1].h; count++; }
                            if (on_x_right)    { sum += pole[i][j][k-1].h; count++; }
                            if (on_y_bottom)   { sum += pole[i][j+1][k].h; count++; }
                            if (on_y_top)      { sum += pole[i][j-1][k].h; count++; }
                            if (on_z_back)     { sum += pole[i+1][j][k].h; count++; }
                            if (on_z_front)    { sum += pole[i-1][j][k].h; count++; }
                            pole[i][j][k].h = sum / count;
                        }
                        else if (boundary_count == 1) {
                            if (on_x_left)      pole[i][j][k].h = pole[i][j][k+1].h;
                            else if (on_x_right)  pole[i][j][k].h = pole[i][j][k-1].h;
                            else if (on_y_bottom) pole[i][j][k].h = pole[i][j+1][k].h;
                            else if (on_y_top)    pole[i][j][k].h = pole[i][j-1][k].h;
                            else if (on_z_back)   pole[i][j][k].h = pole[i+1][j][k].h;
                            else if (on_z_front)  pole[i][j][k].h = pole[i-1][j][k].h;
                        }
                    }
                    
                    e0 = std::max(e0, std::abs(h_old - pole[i][j][k].h));
                }
            }
        }

        // ЭТАП 2: Считаем внутренние точки
        for (int i = 1; i < Nz-1; i++) {
            for (int j = 1; j < Ny-1; j++) {
                for (int k = 1; k < Nx-1; k++) {
                    if (pole[i][j][k].alpha == 1) {
                        h_old = pole[i][j][k].h;
                        
                        double dx2 = dx * dx;
                        double dy2 = dy * dy;
                        double dz2 = dz * dz;
                        double k_ijk = pole[i][j][k].k;

                        double k_left  = 2.0 * k_ijk * pole[i][j][k-1].k / (k_ijk + pole[i][j][k-1].k + 1e-15);
                        double k_right = 2.0 * k_ijk * pole[i][j][k+1].k / (k_ijk + pole[i][j][k+1].k + 1e-15);
                        double k_down  = 2.0 * k_ijk * pole[i][j-1][k].k / (k_ijk + pole[i][j-1][k].k + 1e-15);
                        double k_up    = 2.0 * k_ijk * pole[i][j+1][k].k / (k_ijk + pole[i][j+1][k].k + 1e-15);
                        double k_back  = 2.0 * k_ijk * pole[i-1][j][k].k / (k_ijk + pole[i-1][j][k].k + 1e-15);
                        double k_front = 2.0 * k_ijk * pole[i+1][j][k].k / (k_ijk + pole[i+1][j][k].k + 1e-15);

                        double numerator = (k_left / dx2) * pole[i][j][k-1].h +
                                           (k_right / dx2) * pole[i][j][k+1].h +
                                           (k_down / dy2) * pole[i][j-1][k].h +
                                           (k_up / dy2) * pole[i][j+1][k].h +
                                           (k_back / dz2) * pole[i-1][j][k].h +
                                           (k_front / dz2) * pole[i+1][j][k].h;

                        double denominator = k_left / dx2 + k_right / dx2 + 
                                             k_down / dy2 + k_up / dy2 + 
                                             k_back / dz2 + k_front / dz2;

                        if (denominator > 1e-15) {
                            pole[i][j][k].h = numerator / denominator;
                            e0 = std::max(e0, std::abs(h_old - pole[i][j][k].h));
                        }
                    }
                }
            }
        }

        if (e0 < e) {
            std::cout << "Сошлось за " << iter + 1 << " итераций" << std::endl;
            break;
        }
    }

    // Расчет скоростей
    for (int i = 0; i < Nz; i++) {
        for (int j = 0; j < Ny; j++) {
            for (int k = 0; k < Nx; k++) {
                if (pole[i][j][k].alpha == 4) {
                    pole[i][j][k].vx = 0;
                    pole[i][j][k].vy = 0;
                    pole[i][j][k].vz = 0;
                    continue;
                }

                if (k == 0 || k == Nx-1) {
                    if (pole[i][j][k].type == "zero_gradient") pole[i][j][k].vx = 0;
                    else if (k == 0) pole[i][j][k].vx = -pole[i][j][k].k * (pole[i][j][k+1].h - pole[i][j][k].h) / dx;
                    else pole[i][j][k].vx = -pole[i][j][k].k * (pole[i][j][k].h - pole[i][j][k-1].h) / dx;
                } else {
                    pole[i][j][k].vx = -pole[i][j][k].k * (pole[i][j][k+1].h - pole[i][j][k].h) / dx;
                }

                if (j == 0 || j == Ny-1) {
                    if (pole[i][j][k].type == "zero_gradient") pole[i][j][k].vy = 0;
                    else if (j == 0) pole[i][j][k].vy = -pole[i][j][k].k * (pole[i][j+1][k].h - pole[i][j][k].h) / dy;
                    else pole[i][j][k].vy = -pole[i][j][k].k * (pole[i][j][k].h - pole[i][j-1][k].h) / dy;
                } else {
                    pole[i][j][k].vy = -pole[i][j][k].k * (pole[i][j+1][k].h - pole[i][j][k].h) / dy;
                }

                if (i == 0 || i == Nz-1) {
                    if (pole[i][j][k].type == "zero_gradient") pole[i][j][k].vz = 0;
                    else if (i == 0) pole[i][j][k].vz = -pole[i][j][k].k * (pole[i+1][j][k].h - pole[i][j][k].h) / dz;
                    else pole[i][j][k].vz = -pole[i][j][k].k * (pole[i][j][k].h - pole[i-1][j][k].h) / dz;
                } else {
                    pole[i][j][k].vz = -pole[i][j][k].k * (pole[i+1][j][k].h - pole[i][j][k].h) / dz;
                }
            }
        }
    }

    // Расчет давления
    double rho = 1000.0;
    double g = 9.81;
    for (int i = 0; i < Nz; i++) {
        for (int j = 0; j < Ny; j++) {
            for (int k = 0; k < Nx; k++) {
                pole[i][j][k].pressure = rho * g * (pole[i][j][k].h - pole[i][j][k].z);
            }
        }
    }

    // Вывод результатов
    std::ofstream file("result_3d.csv");
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return 1;
    }
    file << "z,y,x,h,alpha,board,type,vz,vy,vx,pressure,k\n";
    file << std::fixed << std::setprecision(5);

    for (int i = 0; i < Nz; i++) {
        for (int j = 0; j < Ny; j++) {
            for (int k = 0; k < Nx; k++) {
                file << pole[i][j][k].z << ","
                     << pole[i][j][k].y << ","
                     << pole[i][j][k].x << ","
                     << pole[i][j][k].h << ","
                     << pole[i][j][k].alpha << ","
                     << pole[i][j][k].board << ","
                     << pole[i][j][k].type << ","
                     << pole[i][j][k].vz << ","
                     << pole[i][j][k].vy << ","
                     << pole[i][j][k].vx << ","
                     << pole[i][j][k].pressure << ","
                     << pole[i][j][k].k << "\n";
            }
        }
    }
    file.close();
    std::cout << "Результаты сохранены в result_3d.csv" << std::endl;

    // Расход через центральное сечение
    int k_sec = Nx / 2;
    double Q = 0;
    for (int i = 0; i < Nz; i++) {
        for (int j = 0; j < Ny; j++) {
            if (pole[i][j][k_sec].alpha != 4) {
                Q += pole[i][j][k_sec].vx * dy * dz;
            }
        }
    }
    std::cout << "Расход Q через сечение x=" << pole[0][0][k_sec].x 
              << " составляет " << Q << " м³/с\n";

    return 0;
}