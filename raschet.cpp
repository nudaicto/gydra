#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>

int main() {
int l = 100;
int h0 = 150;
double dx = 2.5;
double dy = 2.5;
int h1 = 100;
int h2 =90;
std::vector<double> x(1);
std::vector<double> y(1);

double i = 0;

while (i<l) {
    i=i+dx;
    x.push_back(i);
}

i = 0;
while (i<h0) {
    i=i+dy;
    y.push_back(i);
}

int N = x.size();
int M = y.size();

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
    h[i][0] = h1;
    h[i][N-1] = h2;
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
        h_old = h[0][j];
        h[0][j] = ((h[0][j-1]+h[0][j+1])*dy/dx + 2*h[1][j])/(2*(dy/dx) + 2);
        e0 = std::max(e0, std::abs(h_old - h[0][j]));
    }
    //std:: cout << "2" << std::endl;
    //пересчет основных точек
    for(int i = 1; i<M-1; i++) {
        for(int j = N-2; j>=1; j--) {
            h_old = h[i][j];
            h[i][j] = ((h[i][j-1]+h[i][j+1])*dy/dx + h[i-1][j] + h[i+1][j])/(2*(dy/dx) + 2);
            e0 = std::max(e0, std::abs(h_old - h[i][j]));
        }
    }
    //std:: cout << "3" << std::endl;

    //пересчет крышки
    for (int j = 1; j<N-1; j++) {
        h_old = h[M-1][j];
        h[M-1][j] = ((h[M-1][j-1]+h[M-1][j+1])*dy/dx + 2*h[M-2][j])/(2*(dy/dx) + 2);
        e0 = std::max(e0, std::abs(h_old - h[M-1][j]));
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
        file << h[i][j] << " ";
    }
    file << std::endl;  
}

double k = 1.0;

//теперь нам нужно посчитать поле скоростей. 

//найдем значения для v_x
//для внутр точек + левая грань
for(int i = 0; i<M; i++) {
    for (int j = 0; j<N-1; j++) { 
        v[i][j].second = (h[i][j+1] - h[i][j])/dx;
    }
}
//для правой границы
for (int i = 0; i<M; i++) { 
        v[i][N-1].second = (h[i][N-1] - h[i][N-2])/dx;
}

//найдем значения для v_y
//для внутр точек
for(int i = 1; i<M-1; i++) {
    for (int j = 0; j<N; j++) { 
        v[i][j].first = (h[i+1][j] - h[i][j])/dy;
    }
}
//для нижней и верхней граней
for (int j = 0; j<N; j++) { 
        v[0][j].first = 0;
        v[M-1][j].first = 0;
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file << -k*v[i][j].first << " ";
    }
    file << std::endl;  
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file << -k*v[i][j].second << " ";
    }
    file << std::endl;  
}

file.close();

return 0;

}