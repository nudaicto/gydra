#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>

double f_top(double x) {
    return -0.3*x+150; //возвращает значение y
}
double f_bottom(double x) {
    return 0.3*x; //возвращает значение y
}
double f_right(double y) {
    return 100; //возвращает значение x
}
double f_left(double y) {
    return 0; //возвращает значение x
}

double f_k(double y, double x) {
    if (y<0.5*x) {return 2;}
    if (((x-40)*(x-40)+(y-80)*(y-80))<=160) {return 2;}
    return 1;
}

class CPoint {
private:

public:
    std::pair<double,double> y_x; 
    double h;
    std::pair<double,double> v; //будем считать, что v.first = v_y, а v.second = v_x;
    int alpha;
    int k;
    std::string board;
    std::string type;

    CPoint(std::pair<double,double> _y_x = {0,0}, 
        double _h = 0, 
        std::pair<double,double> _v = {0, 0}, 
        int _alpha = 1,
        int _k = 1, 
        std::string _board = "no",
        std::string _type = "fixed") 
        : y_x(_y_x), h(_h), v(_v), alpha(_alpha), k(_k), board(_board), type(_type) {}

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
        //определяем показатель alpha для каждой точки
        if ((pole[i][j].y_x.second > f_left(pole[i][j].y_x.first)) && (pole[i][j].y_x.second < f_right(pole[i][j].y_x.first))
        && (pole[i][j].y_x.first > f_bottom(pole[i][j].y_x.second)) && (pole[i][j].y_x.first < f_top(pole[i][j].y_x.second))) {
            pole[i][j].alpha = 1;
        }
        else {pole[i][j].alpha = 4;}
        pole[i][j].k = f_k(pole[i][j].y_x.first, pole[i][j].y_x.second);
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

for (int i = 0; i<M; i++) {
    for (int j = 1; j<N-1; j++) {
        if (pole[i][j].board!="no") {
            pole[i][j].type = "zero_gradient";
        } 
    }
}
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
    for (int i = 0; i<M; i++) {
        for (int j = 1; j<N-1; j++) {
            if (pole[i][j].alpha == 4 && pole[i][j].board == "no") {continue;}
            h_old = pole[i][j].h;
            if (pole[i][j].alpha == 1 && pole[i][j].board == "no") {
                double dx2 = dx * dx;
                double dy2 = dy * dy;

                double k_ij = pole[i][j].k;

                double k_left   = k_ij + pole[i][j-1].k;
                double k_right  = k_ij + pole[i][j+1].k;
                double k_down   = k_ij + pole[i-1][j].k;
                double k_up     = k_ij + pole[i+1][j].k;

                double numerator = (k_left   / dx2) * pole[i][j-1].h +
                   (k_right  / dx2) * pole[i][j+1].h +
                   (k_down   / dy2) * pole[i-1][j].h +
                   (k_up     / dy2) * pole[i+1][j].h;

                double denominator = k_left / dx2 + k_right / dx2 + k_down / dy2 + k_up / dy2;

                pole[i][j].h = numerator / denominator;
                
            }

            if (pole[i][j].board == "h_bottom") {pole[i][j].h = pole[i+1][j].h;}

            if (pole[i][j].board == "h_top") {pole[i][j].h = pole[i-1][j].h;}

            if (pole[i][j].board == "v_right") {pole[i][j].h = pole[i][j-1].h;}

            if (pole[i][j].board == "v_left") {pole[i][j].h = pole[i][j+1].h;}

            if (pole[i][j].board == "ugol_1") {pole[i][j].h = (pole[i-1][j].h + pole[i][j+1].h)/2;}
            if (pole[i][j].board == "ugol_2") {pole[i][j].h = (pole[i-1][j].h + pole[i][j-1].h)/2;}
            if (pole[i][j].board == "ugol_3") {pole[i][j].h = (pole[i+1][j].h + pole[i][j-1].h)/2;}
            if (pole[i][j].board == "ugol_4") {pole[i][j].h = (pole[i+1][j].h + pole[i][j+1].h)/2;}

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
        file1 << pole[i][j].h << " ";
    }
    file1 << std::endl;  
}

double k = 1.0;

//теперь нам нужно посчитать поле скоростей. 

for (int i = 0; i<M; i++) {
    for(int j = 0; j<N; j++) {
        if (pole[i][j].board == "v_right" || pole[i][j].board == "ugol_2" || pole[i][j].board == "ugol_3") {
            if (pole[i][j].type == "zero_gradient") {pole[i][j].v.second = 0;}
            if (pole[i][j].type == "fixed") {pole[i][j].v.second = (pole[i][j].h - pole[i][j-1].h)/dx;}
        }
        
        else if (pole[i][j].board == "v_left" || pole[i][j].board == "ugol_1" || pole[i][j].board == "ugol_4") {
            if (pole[i][j].type == "zero_gradient") {pole[i][j].v.second = 0;}
            if (pole[i][j].type == "fixed") {pole[i][j].v.second = (pole[i][j+1].h - pole[i][j].h)/dx;}
        }
        else if (!(pole[i][j].alpha == 4 && pole[i][j].board == "no")) {
            pole[i][j].v.second = (pole[i][j+1].h - pole[i][j].h)/dx;
        }
        
        if (pole[i][j].board == "h_bottom" || pole[i][j].board == "ugol_3" || pole[i][j].board == "ugol_4") {
            if (pole[i][j].type == "zero_gradient") {pole[i][j].v.first = 0;}
            if (pole[i][j].type == "fixed") {pole[i][j].v.first = (pole[i+1][j].h - pole[i][j].h)/dy;}
        }
        
        else if (pole[i][j].board == "h_top" || pole[i][j].board == "ugol_1" || pole[i][j].board == "ugol_2") {
            if (pole[i][j].type == "zero_gradient") {pole[i][j].v.first = 0;}
            if (pole[i][j].type == "fixed") {pole[i][j].v.first = (pole[i][j].h - pole[i-1][j].h)/dy;}
        }
        
        else if (!(pole[i][j].alpha == 4 && pole[i][j].board == "no")) {
            pole[i][j].v.first = (pole[i+1][j].h - pole[i][j].h)/dy;
        }
    }
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file1 << -pole[i][j].k*pole[i][j].v.first << " ";
    }
    file1 << std::endl;  
}

for(int i = 0; i<M; i++ ) {
    for (int j = 0; j<N; j++) {
        file1 << -pole[i][j].k*pole[i][j].v.second << " ";
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
              << -k * pole[i][j].v.first << "," 
              << -k * pole[i][j].v.second << std::endl;
    } 
}

file.close();

//считаем расход через вертикальную стенку
int x_j = 5;
double Q = 0;

for (int i = 0; i<M; i++) {
    Q+=-pole[i][x_j].k*pole[i][x_j].v.second*dy;
}

std::cout << "Расход воды через вертикальную стенку x = " << pole[0][x_j].y_x.second << " Q = " << Q << " " << "m^3/s" << std::endl;

return 0;


}