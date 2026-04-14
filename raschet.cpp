#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>

double y_top(double x) {
    return 150.0; //возвращает значение y
}
double y_bottom(double x) {
    return 0; //возвращает значение y
}
double x_right(double y) {
    return 100; //возвращает значение x
}
double x_left(double y) {
    return 0; //возвращает значение x
}

class CPoint {
private:

public:
    std::pair<double,double> y_x;
    double h;
    std::pair<double,double> v;
    int alpha;
    std::string board; 

    CPoint(std::pair<double,double> _y_x = {0,0}, 
        double _h = 0, 
        std::pair<double,double> _v = {0, 0}, 
        int _alpha = 1, 
        std::string _board = "no") 
        : y_x(_y_x), h(_h), v(_v), alpha(_alpha), board(_board) {}
};

int main() {
int x_max = 100;
int y_max = 150;
double dx = 2.5;
double dy = 2.5;
int h1 = 100;
int h2 =90;
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

for(int i = 0; i<M; i++) {
    for(int j = 0; j<N; j++) {
        pole[i][j].h = (h1+h2)/2;
    }
}

std::vector<std::vector<double>> h(M, std::vector<double>(N, (h1+h2)/2));
std::vector<std::vector<std::pair<double, double>>> v(M, std::vector<std::pair<double, double>>(N, std::pair<double,double>(0,0)));
//будем считать, что v[][].first = v_y, а v[][].second = v_x;

/*for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        std::cout << v[i][j].first << " ";
    }
    std::cout << std::endl;  
}*/

//граничные условия на x=0 и x=l

for (int i = 0; i<M; i++) {
    pole[i][0].h = h1;
    pole[i][N-1].h = h2;
}

//обращаться надо h[y][x]

double e = 0.00001;
double e0 = 10.0;
double h_old = 0.0;

int max_iter = 10000;

for (int iter = 0; iter<max_iter; iter++) {
    e0 = 0;

    //std:: cout << "1" << std::endl;
    //пересчет дна 
    for (int j = 1; j<N-1; j++) {
        h_old = pole[0][j].h;
        pole[0][j].h = ((pole[0][j-1].h+pole[0][j+1].h)*dy/dx + 2*pole[1][j].h)/(2*(dy/dx) + 2);
        e0 = std::max(e0, std::abs(h_old - pole[0][j].h));
    }
    //std:: cout << "2" << std::endl;
    //пересчет основных точек
    for(int i = 1; i<M-1; i++) {
        for(int j = N-2; j>=1; j--) {
            h_old = pole[i][j].h;
            pole[i][j].h = ((pole[i][j-1].h+pole[i][j+1].h)*dy/dx + pole[i-1][j].h + pole[i+1][j].h)/(2*(dy/dx) + 2);
            e0 = std::max(e0, std::abs(h_old - pole[i][j].h));
        }
    }
    //std:: cout << "3" << std::endl;

    //пересчет крышки
    for (int j = 1; j<N-1; j++) {
        h_old = pole[M-1][j].h;
        pole[M-1][j].h = ((pole[M-1][j-1].h+pole[M-1][j+1].h)*dy/dx + 2*pole[M-2][j].h)/(2*(dy/dx) + 2);
        e0 = std::max(e0, std::abs(h_old - pole[M-1][j].h));
    }
    //std:: cout << "4" << std::endl;
    //проверка сходимости

    /*if (iter == 0 || iter == 9 || iter == 99 || iter == 999) {
        std::cout << std::fixed << std::setprecision(5);
        std::cout << "Iter " << iter+1 << ": " << h[M/2][N/2] << std::endl;
    }*/

    if (e0 < e) {
        std:: cout << "Сошлось за " << iter + 1 << " итераций" << std::endl;
        break;
    }
}

std::ofstream file("data.txt");

if (!file.is_open()) {
    std::cerr << "Ошибка открытия файла!" << std::endl;
    return 1;
}

file << std::fixed << std::setprecision(5);

for (int i =0; i<N; i++) {
    file << x[i] << " ";
}

file << std::endl; 

for (int i =0; i<M; i++) {
    file << y[i] << " ";
}

file << std::endl; 

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file << pole[i][j].h << " ";
    }
    file << std::endl;  
}

double k = 1.0;

//теперь нам нужно посчитать поле скоростей. 

//найдем значения для v_x
//для внутр точек + левая грань
for(int i = 0; i<M; i++) {
    for (int j = 0; j<N-1; j++) { 
        pole[i][j].v.second = (pole[i][j+1].h - pole[i][j].h)/dx;
    }
}
//для правой границы
for (int i = 0; i<M; i++) { 
        pole[i][N-1].v.second = (pole[i][N-1].h - pole[i][N-2].h)/dx;
}

//найдем значения для v_y
//для внутр точек
for(int i = 1; i<M-1; i++) {
    for (int j = 0; j<N; j++) { 
        pole[i][j].v.first = (pole[i+1][j].h - pole[i][j].h)/dy;
    }
}
//для нижней и верхней граней
for (int j = 0; j<N; j++) { 
        pole[0][j].v.first = 0;
        pole[M-1][j].v.first = 0;
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file << -k*pole[i][j].v.first << " ";
    }
    file << std::endl;  
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file << -k*pole[i][j].v.second << " ";
    }
    file << std::endl;  
}

file.close();

return 0;

}