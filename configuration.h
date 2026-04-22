#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <variant>

class CLine {
public:
    double k;
    double b;
    double start;
    double finish;
    std::string state;
    double value;
};
class CCircle {
public:
    double x0;
    double y0;
    double R;
    std::string state;
    double value;
};

void read(const std::string& line, std::vector<std::variant<CLine, CCircle>>& vec, double &allalpha) {
    char delimeter = ' ';
    std::vector<std::string> result; 
    std::stringstream ss(line);
    std::string word;
    while (std::getline(ss, word, delimeter)) {result.push_back(word);}
    int n = result.size();
    int i = 0;

    while (i<n) {
        if (result[i]== "form") {
        i++; if (i>=n) {break;}
        if (result[i] == "type") {
            i++; if (i>=n) {break;}
            if (result[i] == "line") {vec.push_back(CLine{}); i++; if (i>=n) {break;}}
            else if (result[i] == "circle") {vec.push_back(CCircle{}); i++; if (i>=n) {break;}}
            else if (result[i] == "all") {
                i++; if (i>=n) {break;}
                if (result[i] == "alpha") {
                    i++; if (i>=n) {break;}
                    allalpha = std::stod(result[i]);
                    i++; if (i>=n) {break;}
                }
            }
            else {std::cout << "Ошибка в синтаксисе файла" << std::endl; break;}

            auto& last = vec.back();
            if (result[i] == "k") {
                i++; if (i>=n) {break;}
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    if constexpr(std::is_same_v<T, CLine>) {obj.k = std::stod(result[i]);}
                }, last);
                i++; if (i>=n) {break;}
            }
            if (result[i] == "b") {
                i++; if (i>=n) {break;}
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    if constexpr(std::is_same_v<T, CLine>) {obj.b = std::stod(result[i]);}
                }, last);
                i++; if (i>=n) {break;}
            }
            if (result[i] == "start") {
                i++; if (i>=n) {break;}
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    if constexpr(std::is_same_v<T, CLine>) {obj.start = std::stod(result[i]);}
                }, last);
                i++; if (i>=n) {break;}
            }
            if (result[i] == "finish") {
                i++; if (i>=n) {break;}
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    if constexpr(std::is_same_v<T, CLine>) {obj.finish = std::stod(result[i]);}
                }, last);
                i++; if (i>=n) {break;}
            }

            if (result[i] == "x0") {
                i++; if (i>=n) {break;}
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    if constexpr(std::is_same_v<T, CCircle>) {obj.x0 = std::stod(result[i]);}
                }, last);
                i++; if (i>=n) {break;}
            }
            if (result[i] == "y0") {
                i++; if (i>=n) {break;}
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    if constexpr(std::is_same_v<T, CCircle>) {obj.y0 = std::stod(result[i]);}
                }, last);
                i++; if (i>=n) {break;}
            }
            if (result[i] == "R") {
                i++; if (i>=n) {break;}
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    if constexpr(std::is_same_v<T, CCircle>) {obj.R = std::stod(result[i]);}
                }, last);
                i++; if (i>=n) {break;}
            }
            if (result[i]=="alpha") {
                i++; if (i>=n) {break;}
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    obj.value = std::stod(result[i]);
                }, last);
                i++; if (i>=n) {break;}
            }
        }
        else {break;}
        }
        if (result[i]== "u") {
        i++; if (i>=n) {break;}
        if (result[i] == "type") {
            i++; if (i>=n) {break;}
            auto& last = vec.back();
            if (result[i] == "zero_gradient") {
                std::visit([&](auto& obj) {obj.state = result[i];}, last);
                i++; if (i>=n) {break;}
            }
            if (result[i] == "fixed") {
                std::visit([&](auto& obj) {obj.state = result[i];}, last);
                i++; if (i>=n) {break;}
                if (result[i] == "value") {
                    i++; if (i>=n) {break;}
                    std::visit([&](auto& obj) {obj.value = std::stoi(result[i]);}, last);
                    i++; if (i>=n) {break;}
                }
            }
        }
        else {break;}
        }
    }              
}