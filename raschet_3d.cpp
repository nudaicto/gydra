#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <string>

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

int main() {
    // Параметры сетки
    double x_max = 10.0, y_max = 10.0, z_max = 10.0;
    double dx = 1.0, dy = 1.0, dz = 1.0;

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
                    
                    // === НАСТРОЙКА ГРАНИЧНЫХ УСЛОВИЙ ===
                    // z=0 и z=z_max — непроницаемые (zero_gradient)
                    // Остальные — фиксированный напор (fixed)
                    
                    if (i == 0) {
                        pole[i][j][k].board = "z_back";
                        pole[i][j][k].type = "zero_gradient";  // НЕПРОНИЦАЕМАЯ
                    }
                    else if (i == Nz-1) {
                        pole[i][j][k].board = "z_front";
                        pole[i][j][k].type = "zero_gradient";  // НЕПРОНИЦАЕМАЯ
                    }
                    else if (k == 0) {
                        pole[i][j][k].board = "x_left";
                        pole[i][j][k].type = "fixed";
                        pole[i][j][k].value = 10.0;
                    }
                    else if (k == Nx-1) {
                        pole[i][j][k].board = "x_right";
                        pole[i][j][k].type = "fixed";
                        pole[i][j][k].value = 0.0;
                    }
                    else if (j == 0) {
                        pole[i][j][k].board = "y_bottom";
                        pole[i][j][k].type = "zero_gradient";  // НЕПРОНИЦАЕМАЯ
                    }
                    else if (j == Ny-1) {
                        pole[i][j][k].board = "y_top";
                        pole[i][j][k].type = "zero_gradient";  // НЕПРОНИЦАЕМАЯ
                    }
                } else {
                    pole[i][j][k].alpha = 1;
                    pole[i][j][k].k = 1.0;
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
                        // Определяем, сколько границ пересекается
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
                        
                        // Вершина (3 границы) - усредняем 3 соседа
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
                        // Ребро (2 границы) - усредняем 2 соседа
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
                        // Грань (1 граница) - копируем из соседней точки
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
    file << "z,y,x,h,alpha,board,type,vz,vy,vx,pressure\n";
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
                     << pole[i][j][k].pressure << "\n";
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