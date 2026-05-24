#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <variant>
#include "configuration.h"

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
    double pressure;

    CPoint(std::pair<double,double> _y_x = {0,0}, 
        double _h = 0, 
        std::pair<double,double> _v = {0, 0}, 
        int _alpha = 1,
        int _k = 1, 
        std::string _board = "no",
        std::string _type = "not_speciefed",
        double _value = 0,
        double pressure = 0) 
        : y_x(_y_x), h(_h), v(_v), alpha(_alpha), k(_k), board(_board), type(_type), value(_value) {}

};

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
std::string f_top_type(CPoint& point, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec) {
    std::string sms = std::string("error");
    double x = point.y_x.second;
    double y = point.y_x.first;
    for (auto& item: vec) {
        std::string result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (x>=obj.start && x<=obj.finish) {
                    if (obj.state == "fixed") {point.value = obj.value;}
                    return obj.state;
                }
                else {return sms;}
            }
            else {return sms;}
        }, item);
        if (result!=sms) {return result;}
    }
    return sms;
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

std::string f_bottom_type (CPoint& point, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec) {
    std::string sms = std::string("error");
    double x = point.y_x.second;
    double y = point.y_x.first;    
    for (auto& item: vec) {
        std::string result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (x>=obj.start && x<=obj.finish) {
                    if (obj.state == "fixed") {point.value = obj.value;}
                    return obj.state;
                }
                else {return sms;}
            }
            else {return sms;}
        }, item);
        if (result!=sms) {return result;}
    }
    return sms;
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

std::string f_right_type(CPoint& point, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec) {
    std::string sms = std::string("error");
    double x = point.y_x.second;
    double y = point.y_x.first;    
    for (auto& item: vec) {
        std::string result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (x>=obj.start && x<=obj.finish) {
                    if (obj.state == "fixed") {point.value = obj.value;}
                    return obj.state;
                }
                else {return sms;}
            }
            else {return sms;}
        }, item);
        if (result!=sms) {return result;}
    }
    return sms;
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

std::string f_left_type(CPoint& point, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec) {
    std::string sms = std::string("error");
    double x = point.y_x.second;
    double y = point.y_x.first;    
    for (auto& item: vec) {
        std::string result = std::visit([&](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                if (x>=obj.start && x<=obj.finish) {
                    if (obj.state == "fixed") {point.value = obj.value;}
                    return obj.state;
                }
                else {return sms;}
            }
            else {return sms;}
        }, item);
        if (result!=sms) {return result;}
    }
    return sms;
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

int main() {
double x_max;
double y_max;
double dx;
double dy;

//получаем информацию о границах из файла
std::ifstream out;
std::string filename = "configuration.txt";

std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> top;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> bottom;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> right;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> left;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> vec_f_k;
std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>> vec_f_inlet;

std::ifstream file2(filename);
if (!file2.is_open()) {
    std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: не удалось открыть файл '" << filename << "'" << std::endl;
    return 1;
}

x_max = 0; y_max = 0; dx = 0; dy = 0;
double allalpha = 1.0; // значение по умолчанию

std::string line;
int line_number = 0;

while (std::getline(file2, line)) {
    line_number++;
    
    // Пропуск пустых строк и комментариев
    if (line.empty() || line[0] == '#') continue;
    
    // Токенизация
    std::vector<std::string> result; 
    std::stringstream ss(line);
    std::string word;
    while (std::getline(ss, word, ' ')) {
        if (!word.empty()) result.push_back(word);
    }
    
    if (result.empty()) continue;
    
    try {
        // === Параметры сетки ===
        if (result[0] == "x_max") {
            if (result.size() < 2) throw std::runtime_error("x_max: отсутствует значение");
            x_max = std::stod(result[1]);
            if (x_max <= 0) throw std::runtime_error("x_max должен быть > 0");
        }
        else if (result[0] == "y_max") {
            if (result.size() < 2) throw std::runtime_error("y_max: отсутствует значение");
            y_max = std::stod(result[1]);
            if (y_max <= 0) throw std::runtime_error("y_max должен быть > 0");
        }
        else if (result[0] == "dx") {
            if (result.size() < 2) throw std::runtime_error("dx: отсутствует значение");
            dx = std::stod(result[1]);
            if (dx <= 0) throw std::runtime_error("dx должен быть > 0");
        }
        else if (result[0] == "dy") {
            if (result.size() < 2) throw std::runtime_error("dy: отсутствует значение");
            dy = std::stod(result[1]);
            if (dy <= 0) throw std::runtime_error("dy должен быть > 0");
        }
        // === Секции границ ===
        else if (result[0] == "f_top" || result[0] == "f_bottom" || 
                 result[0] == "f_right" || result[0] == "f_left" ||
                 result[0] == "f_k" || result[0] == "f_inlet") {
            
            std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>* target_vec = nullptr;
            if (result[0] == "f_top") target_vec = &top;
            else if (result[0] == "f_bottom") target_vec = &bottom;
            else if (result[0] == "f_right") target_vec = &right;
            else if (result[0] == "f_left") target_vec = &left;
            else if (result[0] == "f_k") target_vec = &vec_f_k;
            else if (result[0] == "f_inlet") target_vec = &vec_f_inlet;
            
            // Читаем строки секции до пустой строки
            while (std::getline(file2, line) && !line.empty()) {
                line_number++;
                if (line.empty() || line[0] == '#') continue;
                read(line, *target_vec, allalpha);
                // read() теперь сама обрабатывает ошибки и возвращает управление
            }
        }
        // === Неизвестный ключ ===
        else {
            throw std::runtime_error("Неизвестный параметр: '" + result[0] + "'");
        }
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "ОШИБКА ПАРСИНГА (строка " << line_number << "): некорректное числовое значение в \"" 
                  << line << "\"" << std::endl;
        return 1;
    }
    catch (const std::out_of_range& e) {
        std::cerr << "ОШИБКА ПАРСИНГА (строка " << line_number << "): значение вне диапазона в \"" 
                  << line << "\"" << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "ОШИБКА КОНФИГУРАЦИИ (строка " << line_number << "): " << e.what() 
                  << "\nСтрока: \"" << line << "\"" << std::endl;
        return 1;
    }
}

file2.close();

// === Финальная валидация обязательных параметров ===
if (x_max <= 0 || y_max <= 0 || dx <= 0 || dy <= 0) {
    std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: не заданы или некорректны параметры сетки (x_max, y_max, dx, dy)" << std::endl;
    return 1;
}

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

// === Проверка адекватности размеров сетки ===
int N = x.size();
int M = y.size();

if (N > 10000 || M > 10000) {
    std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: Слишком мелкая сетка или большая область. "
              << "N=" << N << ", M=" << M << ". Программа требует >" 
              << (N * M * sizeof(CPoint) / (1024*1024)) << " МБ памяти." << std::endl;
    return 1;
}

if (N < 3 || M < 3) {
    std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: Сетка слишком мала для расчёта (требуется минимум 3x3)." << std::endl;
    return 1;
}

std::vector<std::vector<CPoint>> pole(M, std::vector<CPoint>(N, CPoint()));

for(int i = 0; i<M; i++) {
    for(int j = 0; j<N; j++) {
        pole[i][j].h = 0;
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
                if (s[1] == 1) {pole[i][j].board = "h_bottom"; pole[i][j].type = f_bottom_type(pole[i][j], bottom);}
                else {pole[i][j].board = "h_top"; pole[i][j].type = f_top_type(pole[i][j], top);}
            }
            if (s[1] == s[3] && s[1] == 4) {
                if (s[0] == 1) {pole[i][j].board = "v_right"; pole[i][j].type = f_right_type(pole[i][j], right);}
                else {pole[i][j].board = "v_left"; pole[i][j].type = f_left_type(pole[i][j], left);}
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
    if (s1[0] == s1[2] && s1[0] == 4) {pole[i][0].board = "v_left"; pole[i][0].type = f_left_type(pole[i][0], left);}
    if (s1[0] == s1[1] && s1[0] == 4) {pole[i][0].board = "ugol_4";}
    if (s1[1] == s1[2] && s1[1] == 4) {pole[i][0].board = "ugol_1";}
    }

    s1[0] = pole[i+1][N-1].alpha;
    s1[1] = pole[i][N-2].alpha;
    s1[2] = pole[i-1][N-1].alpha;
    sum = s1[0] + s1[1] + s1[2];
    if (sum!=12) {
    if (s1[0] == s1[2] && s1[0] == 4) {pole[i][N-1].board = "v_right"; pole[i][N-1].type = f_right_type(pole[i][N-1], right);}
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
    if (s1[0] == s1[2] && s1[0] == 4) {pole[0][j].board = "h_bottom"; pole[0][j].type = f_bottom_type(pole[0][j], bottom);}
    if (s1[0] == s1[1] && s1[0] == 4) {pole[0][j].board = "ugol_3";}
    if (s1[1] == s1[2] && s1[1] == 4) {pole[0][j].board = "ugol_4";}
    }

    s1[0] = pole[M-1][j-1].alpha;
    s1[1] = pole[M-2][j].alpha;
    s1[2] = pole[M-1][j-1].alpha;
    sum = s1[0] + s1[1] + s1[2];
    if (sum!=12) {
    if (s1[0] == s1[2] && s1[0] == 4) {pole[M-1][j].board = "h_top"; pole[M-1][j].type = f_top_type(pole[M-1][j], top);}
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

    // ЭТАП 1: Применяем все граничные условия (без пересчёта по формуле)
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

    // ЭТАП 2: Считаем только внутренние точки (alpha==1, board=="no")
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
                    
                    // Защита от численного взрыва или потери точности
                    if (std::isnan(pole[i][j].h) || std::isinf(pole[i][j].h)) {
                        std::cerr << "ОШИБКА РАСЧЁТА: Получено NaN или Inf в ячейке [" << i << "][" << j << "]. "
                                << "Возможные причины: некорректные граничные условия или нулевая проницаемость k." << std::endl;
                        return 1;
                    }
                    e0 = std::max(e0, std::abs(h_old - pole[i][j].h));
                } else {
                    // Если знаменатель близок к нулю, ячейка изолирована. Оставляем h без изменений, но логируем предупреждение.
                    // (Это не фатально, но полезно знать)
                }
            }
        }
    }

    if (std::isnan(e0) || std::isinf(e0)) {
        std::cerr << "ОШИБКА СХОДИМОСТИ: Ошибка e0 стала нечисловой. Прерывание." << std::endl;
        return 1;
    }
    if (e0 < e) {
        std::cout << "Сошлось за " << iter + 1 << " итераций" << std::endl;
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
        file1 << pole[i][j].h << " ";
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

if (!file1.good()) {
    std::cerr << "ОШИБКА ДИСКА: Не удалось корректно записать data.txt." << std::endl;
    return 1;
}
file1.close();

file << "y,x,h,alpha,board,type,vy,vx,pressure\n";

double rho = 1000.0;  // кг/м³
double g = 9.81;      // м/с²

for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
        double P = rho * g * (pole[i][j].h - y[i]);
        pole[i][j].pressure = P;  // если добавить поле pressure в класс
    }
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
         file << pole[i][j].y_x.first << ","    
              << pole[i][j].y_x.second << ","    
              << pole[i][j].h << ","            
              << pole[i][j].alpha << ","         
              << pole[i][j].board << ","     
              << pole[i][j].type << ","         
              << pole[i][j].v.first << "," 
              << pole[i][j].v.second << ","
              << pole[i][j].pressure << std::endl;
    } 
}

if (!file.good()) {
    std::cerr << "ОШИБКА ДИСКА: Не удалось корректно записать result.csv (возможно, закончилось место или ошибка файловой системы)." << std::endl;
    return 1;
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
auto calculate_Q = [&](int x_j) {
    if (x_j < 0 || x_j >= N) {
        std::cout << "ВНИМАНИЕ: Створ x_j=" << x_j << " за пределами сетки (N=" << N << "). Расход не считается." << std::endl;
        return 0.0;
    }
    double Q_local = 0;
    for (int i = 0; i < M; i++) {
        if (pole[i][x_j].alpha == 4) continue;
        Q_local += pole[i][x_j].v.second * dy;
    }
    std::cout << "Q(x=" << pole[0][x_j].y_x.second << ") = " << Q_local << " m³/s\n";
    return Q_local;
};

calculate_Q(50);
calculate_Q(65);
calculate_Q(125);

}