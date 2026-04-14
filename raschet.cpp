#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>

double f_top(double x) {
    return -0.2*x+150.0; //возвращает значение y
}
double f_bottom(double x) {
    return 0; //возвращает значение y
}
double f_right(double y) {
    return 100; //возвращает значение x
}
double f_left(double y) {
    return 0; //возвращает значение x
}

class CPoint {
private:

public:
    std::pair<double,double> y_x; 
    double h;
    std::pair<double,double> v; //будем считать, что v.first = v_y, а v.second = v_x;
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
double dx = 0.75;
double dy = 0.75;
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
        pole[i][j].y_x = {y[i], x[j]};
        //определяем показатель algpa для каждой точки
        if ((pole[i][j].y_x.second > f_left(pole[i][j].y_x.first)) && (pole[i][j].y_x.second < f_right(pole[i][j].y_x.first))
        && (pole[i][j].y_x.first > f_bottom(pole[i][j].y_x.second)) && (pole[i][j].y_x.first < f_top(pole[i][j].y_x.second))) {
            pole[i][j].alpha = 1;
        }
        else {pole[i][j].alpha = 4;}
    }
}

//определим тип границы для каждоый точки 
int sum = 0;
for(int i = 1; i<M-1; i++) {
    for(int j = 1; j<N-1; j++) {
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


//граничные условия на x=0 и x=l

for (int i = 0; i<M; i++) {
    pole[i][0].h = h1;
    pole[i][N-1].h = h2;
}

double e = 0.00001;
double e0 = 10.0;
double h_old = 0.0;

int max_iter = 10000;

for (int iter = 0; iter<max_iter; iter++) {
    e0 = 0;

    //пересчет дна 
    for (int j = 1; j<N-1; j++) {
        h_old = pole[0][j].h;
        pole[0][j].h = ((pole[0][j-1].h+pole[0][j+1].h)*dy/dx + 2*pole[1][j].h)/(2*(dy/dx) + 2);
        e0 = std::max(e0, std::abs(h_old - pole[0][j].h));
    }

    //пересчет основных точек
    for(int i = 1; i<M-1; i++) {
        for(int j = N-2; j>=1; j--) {
            h_old = pole[i][j].h;
            pole[i][j].h = ((pole[i][j-1].h+pole[i][j+1].h)*dy/dx + pole[i-1][j].h + pole[i+1][j].h)/(2*(dy/dx) + 2);
            e0 = std::max(e0, std::abs(h_old - pole[i][j].h));
        }
    }

    //пересчет крышки
    for (int j = 1; j<N-1; j++) {
        h_old = pole[M-1][j].h;
        pole[M-1][j].h = ((pole[M-1][j-1].h+pole[M-1][j+1].h)*dy/dx + 2*pole[M-2][j].h)/(2*(dy/dx) + 2);
        e0 = std::max(e0, std::abs(h_old - pole[M-1][j].h));
    }

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
        //std::cout << pole[i][j].board << " ";
        file << pole[i][j].board << " ";
    }
    //std::cout << std::endl;
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