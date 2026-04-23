#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <variant>
#include "configuration.h"

bool f_top(double y, double x, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec, double& allalpha) {
    for (auto& item: vec) {
        bool result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (x>=obj.start && x<=obj.finish) {
                    return y < obj.k*x+obj.b;
                }
                else {return false;}
            }
            else {return false;}
        }, item);
        if (result) {return true;}
    }
    return false;
}
bool f_bottom(double y, double x, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec, double& allalpha) {
    for (auto& item: vec) {
        bool result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (x>=obj.start && x<=obj.finish) {
                    return y > obj.k*x+obj.b;
                }
                else {return false;}
            }
            else {return false;}
        }, item);
        if (result) {return true;}
    }
    return false;
}
bool f_right(double y, double x, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec, double& allalpha) {
    for (auto& item: vec) {
        bool result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (y>=obj.start && y<=obj.finish) {
                    return x < obj.k*y+obj.b;
                }
                else {return false;}
            }
            else {return false;}
        }, item);
        if (result) {return true;}
    }
    return false;
}
bool f_left(double y, double x, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec, double& allalpha) {
    for (auto& item: vec) {
        bool result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (y>=obj.start && y<=obj.finish) {
                    return x > obj.k*y+obj.b;
                }
                else {return false;}
            }
            else {return false;}
        }, item);
        if (result) {return true;}
    }
    return false;
}

bool f_inlet(double y, double x, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec, double& allalpha) {
    for (auto& item: vec) {
        bool result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CSquare>) {
                if (x>obj.x1 && x<obj.x2 && y>obj.y1 && y<obj.y2) {
                    return true;
                }
                else {return false;}
            }
            else if constexpr(std::is_same_v<T,CCircle>) {
                if (((x-obj.x0)*(x-obj.x0)+(y-obj.y0)*(y-obj.y0))<=obj.R*obj.R) {return true;}
                else {return false;}
            }
            else {return false;}
        }, item);
        if (result) {return true;}
    }
    return false;
}

double f_k(double y, double x, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec, double& allalpha) {
    for (auto& item: vec) {
         double result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (x>=obj.start && x<=obj.finish) {
                    if (y<obj.k*x+obj.b) {return obj.value;}
                }
            }
            if constexpr(std::is_same_v<T,CCircle>) {
                if (((x-obj.x0)*(x-obj.x0)+(y-obj.y0)*(y-obj.y0))<=obj.R*obj.R) {return obj.value;}
            }
            return allalpha;
        }, item);
        if (result != allalpha) {return result;}
    }
    return allalpha;
}

class CPoint {
public:
    std::pair<double,double> y_x; 
    double h;
    std::pair<double,double> v; //будем считать, что v.first = v_y, а v.second = v_x;
    int alpha;
    int k;
    std::string board;
    std::string type;
    double value;

    CPoint(std::pair<double,double> _y_x = {0,0}, 
        double _h = 0, 
        std::pair<double,double> _v = {0, 0}, 
        int _alpha = 1,
        int _k = 1, 
        std::string _board = "no",
        std::string _type = "not_speciefed",
        double _value = 0) 
        : y_x(_y_x), h(_h), v(_v), alpha(_alpha), k(_k), board(_board), type(_type), value(_value) {}

};

int main() {
int x_max = 100;
int y_max = 150;
double dx = 0.75;
double dy = 0.75;
int h1 = 100;
int h2 = 90;
std::vector<double> x(1);
std::vector<double> y(1);

double i = 0;

while (i<x_max) {
    i=i+dx;
    x.push_back(i);
}

i = 0;
while (i<y_max) {
    i=i+dy;
    y.push_back(i);
}

int N = x.size();
int M = y.size();

std::vector<std::vector<CPoint>> pole(M, std::vector<CPoint>(N, CPoint()));

//получаем информацию о границах из файла
std::ifstream out;
std::string filename = "configuration.txt";

std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> top;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> bottom;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> right;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> left;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> vec_f_k;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> vec_f_inlet;

double allalpha;

std::string line;
std::ifstream file2(filename);
if (file2.is_open()) {
    while (std::getline(file2, line)) {
        if (line=="f_top") {
            while (std::getline(file2, line) && !line.empty()) {
                read(line, top, allalpha);
            }
        }
        if (line=="f_bottom") {
            while (std::getline(file2, line) && !line.empty()) {
                read(line, bottom, allalpha);
            }
        }
        if (line=="f_right") {
            while (std::getline(file2, line) && !line.empty()) {
                read(line, right, allalpha);
            }
        }
        if (line=="f_left") {
            while (std::getline(file2, line) && !line.empty()) {
                read(line, left, allalpha);
            }
        }
        if (line=="f_k") {
            while (std::getline(file2, line) && !line.empty()) {
                read(line, vec_f_k, allalpha);
            }
        }
        if (line=="f_inlet") {
            while (std::getline(file2, line) && !line.empty()) {
                read(line, vec_f_inlet, allalpha);
            }
        }
    }
}

for(int i = 0; i<M; i++) {
    for(int j = 0; j<N; j++) {
        pole[i][j].h = (h1+h2)/2;
        pole[i][j].y_x = {y[i], x[j]};

        //определяем показатель alpha для каждой точки
        if (f_left(pole[i][j].y_x.first, pole[i][j].y_x.second, left, allalpha) && f_right(pole[i][j].y_x.first, pole[i][j].y_x.second, right, allalpha)
        && f_bottom(pole[i][j].y_x.first, pole[i][j].y_x.second, bottom, allalpha) && f_top(pole[i][j].y_x.first, pole[i][j].y_x.second, top, allalpha)) {
            pole[i][j].alpha = 1;
        }
        else {pole[i][j].alpha = 4;}
        pole[i][j].k = f_k(pole[i][j].y_x.first, pole[i][j].y_x.second, vec_f_k, allalpha);
        if (f_inlet(pole[i][j].y_x.first, pole[i][j].y_x.second, vec_f_inlet, allalpha)) {
            pole[i][j].alpha = 4;            
        }
    }
}

for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
        
        // Проверка: точка внутри кубика И не на внешней границе
        bool is_boundary = (i == 0 || i == M-1 || j == 0 || j == N-1);
        
        if (!is_boundary && f_inlet(pole[i][j].y_x.first, pole[i][j].y_x.second, vec_f_inlet, allalpha)) {
            
            // Помечаем соседей, но только если они:
            // 1. В пределах массива
            // 2. Не являются препятствием
            // 3. Не являются внешней границей (board == "no")
            
            // Левый сосед
            if (j > 0 && pole[i][j-1].alpha != 4 && pole[i][j-1].board == "no") {
                pole[i][j-1].board = "obs_right";
            }
            // Правый сосед
            if (j + 1 < N && pole[i][j+1].alpha != 4 && pole[i][j+1].board == "no") {
                pole[i][j+1].board = "obs_left";
            }
            // Нижний сосед
            if (i > 0 && pole[i-1][j].alpha != 4 && pole[i-1][j].board == "no") {
                pole[i-1][j].board = "obs_bottom";
            }
            // Верхний сосед
            if (i + 1 < M && pole[i+1][j].alpha != 4 && pole[i+1][j].board == "no") {
                pole[i+1][j].board = "obs_top";
            }
        }
    }
}

file2.close();

//определим тип границы для каждоый точки 
int sum = 0;
for(int i = 1; i<M-1; i++) {
    for(int j = 1; j<N-1; j++) {
        if (f_inlet(pole[i][j].y_x.first, pole[i][j].y_x.second, vec_f_inlet, allalpha)) {continue;}
        if (pole[i][j].alpha==4) {
            int s[4];
            s[0] = pole[i][j-1].alpha;
            s[1] = pole[i+1][j].alpha;
            s[2] = pole[i][j+1].alpha;
            s[3] = pole[i-1][j].alpha;
            sum = s[0] + s[1] + s[2] + s[3];
            if (sum!=16) {
            if (s[0] == s[2] && s[0] == 4) {
                if (s[1] == 1) {pole[i][j].board = "h_bottom"; }
                else {pole[i][j].board = "h_top";}
            }
            if (s[1] == s[3] && s[1] == 4) {
                if (s[0] == 1) {pole[i][j].board = "v_right"; }
                else {pole[i][j].board = "h_left";}
            }
            if (s[0] == s[1] && s[0] == 4 && s[2]==s[3]) {pole[i][j].board = "ugol_3";}
            if (s[1] == s[2] && s[1] == 4 && s[3]==s[0]) {pole[i][j].board = "ugol_4";}
            if (s[2] == s[3] && s[2] == 4 && s[0]==s[1]) {pole[i][j].board = "ugol_1";}
            if (s[0] == s[3] && s[0] == 4 && s[2]==s[1]) {pole[i][j].board = "ugol_2";}
            }
            else {
                if (pole[i-1][j-1].alpha == 1) {pole[i][j].board = "ugol_2";}
                if (pole[i+1][j+1].alpha == 1) {pole[i][j].board = "ugol_4";}
                if (pole[i-1][j+1].alpha == 1) {pole[i][j].board = "ugol_1";}
                if (pole[i+1][j-1].alpha == 1) {pole[i][j].board = "ugol_3";}
            }
        }
    }
}
int s1[3];
for (int i = 1; i<M-1; i++) {
    s1[0] = pole[i+1][0].alpha;
    s1[1] = pole[i][1].alpha;
    s1[2] = pole[i-1][0].alpha;
    sum = s1[0] + s1[1] + s1[2];
    if (sum!=12) {
    if (s1[0] == s1[2] && s1[0] == 4) {pole[i][0].board = "v_left";}
    if (s1[0] == s1[1] && s1[0] == 4) {pole[i][0].board = "ugol_4";}
    if (s1[1] == s1[2] && s1[1] == 4) {pole[i][0].board = "ugol_1";}
    }

    s1[0] = pole[i+1][N-1].alpha;
    s1[1] = pole[i][N-2].alpha;
    s1[2] = pole[i-1][N-1].alpha;
    sum = s1[0] + s1[1] + s1[2];
    if (sum!=12) {
    if (s1[0] == s1[2] && s1[0] == 4) {pole[i][N-1].board = "v_right";}
    if (s1[0] == s1[1] && s1[0] == 4) {pole[i][N-1].board = "ugol_3";}
    if (s1[1] == s1[2] && s1[1] == 4) {pole[i][N-1].board = "ugol_2";}
    }
}

for (int j = 1; j<N-1; j++) {
    s1[0] = pole[0][j-1].alpha;
    s1[1] = pole[1][j].alpha;
    s1[2] = pole[0][j+1].alpha;
    sum = s1[0] + s1[1] + s1[2];
    if (sum!=12) {
    if (s1[0] == s1[2] && s1[0] == 4) {pole[0][j].board = "h_bottom";}
    if (s1[0] == s1[1] && s1[0] == 4) {pole[0][j].board = "ugol_3";}
    if (s1[1] == s1[2] && s1[1] == 4) {pole[0][j].board = "ugol_4";}
    }

    s1[0] = pole[M-1][j-1].alpha;
    s1[1] = pole[M-2][j].alpha;
    s1[2] = pole[M-1][j-1].alpha;
    sum = s1[0] + s1[1] + s1[2];
    if (sum!=12) {
    if (s1[0] == s1[2] && s1[0] == 4) {pole[M-1][j].board = "h_top";}
    if (s1[0] == s1[1] && s1[0] == 4) {pole[M-1][j].board = "ugol_2";}
    if (s1[1] == s1[2] && s1[1] == 4) {pole[M-1][j].board = "ugol_1";}
    }
}

if (pole[0][0].alpha == 4 && pole[1][1].alpha == 1) {pole[0][0].board = "ugol_4";}
if (pole[M-1][0].alpha == 4 && pole[M-2][1].alpha == 1) {pole[M-1][0].board = "ugol_1";}
if (pole[M-1][N-1].alpha == 4 && pole[M-2][N-2].alpha == 1) {pole[M-1][N-1].board = "ugol_2";}
if (pole[0][N-1].alpha == 4 && pole[1][N-2].alpha == 1) {pole[0][N-1].board = "ugol_3";}

//определяем типа граничного условия для каждой точки
for (int i = 0; i<M; i++) {
    for (int j = 1; j<N-1; j++) {
        if (pole[i][j].board!="no" && pole[i][j].board!="obs_right" && pole[i][j].board!="obs_left" && pole[i][j].board!="obs_top"&& pole[i][j].board!="obs_bottom") {
            pole[i][j].type = "zero_gradient";
        } 
    }
}

for (int i = 0; i<M; i++) {
    pole[i][0].type = "fixed";
    pole[i][0].value = h1;
    pole[i][N-1].type = "fixed";
    pole[i][N-1].value = h2;
}

double e = 0.00001;
double e0 = 10.0;
double h_old = 0.0;

int max_iter = 15000;

int count = 0;
for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
        if (pole[i][j].alpha == 1 && pole[i][j].board == "no") count++;
    }
}
std::cout << "Внутренних водных узлов: " << count << std::endl;

//сам расчет

for (int iter = 0; iter < max_iter; iter++) {
    e0 = 0;
    // ЭТАП 1: Считаем только внутренние точки (alpha==1, board=="no")
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            h_old = pole[i][j].h;
            
            if (pole[i][j].alpha) {
                
                double dx2 = dx * dx;
                double dy2 = dy * dy;
                double k_ij = pole[i][j].k;

                double k_left = 0, k_right = 0, k_down = 0, k_up = 0;
                double h_left = pole[i][j].h, h_right = pole[i][j].h, 
                    h_down = pole[i][j].h, h_up = pole[i][j].h;

                // Левая грань: если НЕТ препятствия слева (board != "obs_left")
                if (j > 0 && pole[i][j].board != "obs_left") {
                    k_left = 2.0 * k_ij * pole[i][j-1].k / (k_ij + pole[i][j-1].k + 1e-15);
                    h_left = pole[i][j-1].h;
                }

                // Правая грань: если НЕТ препятствия справа (board != "obs_right")
                if (j + 1 < N && pole[i][j].board != "obs_right") {
                    k_right = 2.0 * k_ij * pole[i][j+1].k / (k_ij + pole[i][j+1].k + 1e-15);
                    h_right = pole[i][j+1].h;
                }

                // Нижняя грань: если НЕТ препятствия снизу (board != "obs_bottom")
                if (i > 0 && pole[i][j].board != "obs_bottom") {
                    k_down = 2.0 * k_ij * pole[i-1][j].k / (k_ij + pole[i-1][j].k + 1e-15);
                    h_down = pole[i-1][j].h;
                }

                // Верхняя грань: если НЕТ препятствия сверху (board != "obs_top")
                if (i + 1 < M && pole[i][j].board != "obs_top") {
                    k_up = 2.0 * k_ij * pole[i+1][j].k / (k_ij + pole[i+1][j].k + 1e-15);
                    h_up = pole[i+1][j].h;
                }

                double numerator = (k_left / dx2) * h_left +
                                   (k_right / dx2) * h_right +
                                   (k_down / dy2) * h_down +
                                   (k_up / dy2) * h_up;

                double denominator = k_left / dx2 + k_right / dx2 + k_down / dy2 + k_up / dy2;

                if (denominator > 1e-15) {
                    pole[i][j].h = numerator / denominator;
                    e0 = std::max(e0, std::abs(h_old - pole[i][j].h));
                }
            }
        }
    }
    // ЭТАП 2: Применяем все граничные условия (без пересчёта по формуле)
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            h_old = pole[i][j].h;
            if (pole[i][j].type == "fixed") {
                pole[i][j].h = pole[i][j].value;
            }
            if (pole[i][j].type == "zero_gradient") {
                if (pole[i][j].board == "h_bottom") pole[i][j].h = pole[i+1][j].h;
                if (pole[i][j].board == "h_top")    pole[i][j].h = pole[i-1][j].h;
                if (pole[i][j].board == "v_right")  pole[i][j].h = pole[i][j-1].h;
                if (pole[i][j].board == "v_left")   pole[i][j].h = pole[i][j+1].h;
            }
            if (pole[i][j].board == "ugol_1") pole[i][j].h = (pole[i-1][j].h + pole[i][j+1].h)/2;
            if (pole[i][j].board == "ugol_2") pole[i][j].h = (pole[i-1][j].h + pole[i][j-1].h)/2;
            if (pole[i][j].board == "ugol_3") pole[i][j].h = (pole[i+1][j].h + pole[i][j-1].h)/2;
            if (pole[i][j].board == "ugol_4") pole[i][j].h = (pole[i+1][j].h + pole[i][j+1].h)/2;
            e0 = std::max(e0, std::abs(h_old - pole[i][j].h));
        }
        
    }

    if (e0 < e) {
        std:: cout << "Сошлось за " << iter + 1 << " итераций" << std::endl;
        break;
    }
}

std::ofstream file("result.csv");

if (!file.is_open()) {
    std::cerr << "Ошибка открытия файла!" << std::endl;
    return 1;
}

file << std::fixed << std::setprecision(5);

std::ofstream file1("data.txt");

if (!file1.is_open()) {
    std::cerr << "Ошибка открытия файла!" << std::endl;
    return 1;
}

file1 << std::fixed << std::setprecision(5);

for (int i =0; i<N; i++) {
    file1 << x[i] << " ";
}

file1 << std::endl; 

for (int i =0; i<M; i++) {
    file1 << y[i] << " ";
}

file1 << std::endl; 

for(int i = M-1; i>-1; i-- ) {
    for (int j = 0; j<N; j++) {
        file1 << pole[i][j].board << " ";
    }
    file1 << std::endl;  
}

//теперь нам нужно посчитать поле скоростей. 

for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
        
        // === Пропускаем само препятствие ===
        if (pole[i][j].alpha == 4) {
            pole[i][j].v.first = 0;
            pole[i][j].v.second = 0;
            continue;
        }

        // =======================
        // РАСЧЁТ V_x (v.second)
        // =======================
        
        // 1. Приоритет 1: Граница с препятствием (нулевой поток через грань)
        if (pole[i][j].board == "obs_right") {
            // Препятствие справа → нормальная скорость (вправо) должна быть 0
            pole[i][j].v.second = 0; 
        }
        else if (pole[i][j].board == "obs_left") {
            // Препятствие слева → нормальная скорость (влево) должна быть 0
            pole[i][j].v.second = 0;
        }
        // 2. Приоритет 2: Внешние границы домена
        else if (pole[i][j].board == "v_right" || pole[i][j].board == "ugol_2" || pole[i][j].board == "ugol_3") {
            if (pole[i][j].type == "zero_gradient") pole[i][j].v.second = 0;
            else if (pole[i][j].type == "fixed") pole[i][j].v.second = -pole[i][j].k * (pole[i][j].h - pole[i][j-1].h) / dx;
        }
        else if (pole[i][j].board == "v_left" || pole[i][j].board == "ugol_1" || pole[i][j].board == "ugol_4") {
            if (pole[i][j].type == "zero_gradient") pole[i][j].v.second = 0;
            else if (pole[i][j].type == "fixed") pole[i][j].v.second = -pole[i][j].k * (pole[i][j+1].h - pole[i][j].h) / dx;
        }
        // 3. Приоритет 3: Обычная внутренняя точка
        else {
            pole[i][j].v.second = -pole[i][j].k * (pole[i][j+1].h - pole[i][j].h) / dx;
        }


        // =======================
        // РАСЧЁТ V_y (v.first) — аналогично
        // =======================
        
        // 1. Приоритет 1: Граница с препятствием
        if (pole[i][j].board == "obs_top") {
            pole[i][j].v.first = 0;
        }
        else if (pole[i][j].board == "obs_bottom") {
            pole[i][j].v.first = 0;
        }
        // 2. Приоритет 2: Внешние границы
        else if (pole[i][j].board == "h_bottom" || pole[i][j].board == "ugol_3" || pole[i][j].board == "ugol_4") {
            if (pole[i][j].type == "zero_gradient") pole[i][j].v.first = 0;
            else if (pole[i][j].type == "fixed") pole[i][j].v.first = -pole[i][j].k * (pole[i+1][j].h - pole[i][j].h) / dy;
        }
        else if (pole[i][j].board == "h_top" || pole[i][j].board == "ugol_1" || pole[i][j].board == "ugol_2") {
            if (pole[i][j].type == "zero_gradient") pole[i][j].v.first = 0;
            else if (pole[i][j].type == "fixed") pole[i][j].v.first = -pole[i][j].k * (pole[i][j].h - pole[i-1][j].h) / dy;
        }
        // 3. Приоритет 3: Обычная точка
        else {
            pole[i][j].v.first = -pole[i][j].k * (pole[i+1][j].h - pole[i][j].h) / dy;
        }
    }
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file1 << pole[i][j].v.first << " ";
    }
    file1 << std::endl;  
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file1 << pole[i][j].v.second << " ";
    }
    file1 << std::endl;  
}

file1.close();

file << "y,x,h,alpha,board,type,vy,vx\n";

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
         file << pole[i][j].y_x.first << ","    
              << pole[i][j].y_x.second << ","    
              << pole[i][j].h << ","            
              << pole[i][j].alpha << ","         
              << pole[i][j].board << ","     
              << pole[i][j].type << ","         
              << pole[i][j].v.first << "," 
              << pole[i][j].v.second << std::endl;
    } 
}

file.close();

int count_obstacle = 0;
for (int i = 0; i < M; i++)
    for (int j = 0; j < N; j++)
        if (pole[i][j].alpha == 4) count_obstacle++;

std::cout << "Obstacle cells: " << count_obstacle << "\n";

// Проверка: у всех ячеек препятствия скорость должна быть ~0
double max_v_obstacle = 0;
for (int i = 0; i < M; i++)
    for (int j = 0; j < N; j++)
        if (pole[i][j].alpha == 4) {
            double v_mag = std::hypot(pole[i][j].v.first, pole[i][j].v.second);
            max_v_obstacle = std::max(max_v_obstacle, v_mag);
        }
std::cout << "Max |v| inside obstacle: " << max_v_obstacle << "\n";

//считаем расход через вертикальную стенку
int x_j = 50;
double Q = 0;
for (int i = 0; i < M; i++) {
    // Пропускаем ячейки внутри препятствия
    if (pole[i][x_j].alpha == 4) continue;
    
    // v.second уже = V_x (содержит k), просто интегрируем по y
    Q += pole[i][x_j].v.second * dy;
}
std::cout << "Q(x=" << pole[0][x_j].y_x.second << ") = " << Q << " m³/s\n";

x_j = 65;
Q = 0;
for (int i = 0; i < M; i++) {
    // Пропускаем ячейки внутри препятствия
    if (pole[i][x_j].alpha == 4) continue;
    
    // v.second уже = V_x (содержит k), просто интегрируем по y
    Q += pole[i][x_j].v.second * dy;
}
std::cout << "Q(x=" << pole[0][x_j].y_x.second << ") = " << Q << " m³/s\n";

x_j = 125;
Q = 0;
for (int i = 0; i < M; i++) {
    // Пропускаем ячейки внутри препятствия
    if (pole[i][x_j].alpha == 4) continue;
    
    // v.second уже = V_x (содержит k), просто интегрируем по y
    Q += pole[i][x_j].v.second * dy;
}
std::cout << "Q(x=" << pole[0][x_j].y_x.second << ") = " << Q << " m³/s\n";

}