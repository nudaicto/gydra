#include "configuration.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <variant>

int main() {
    std::ifstream out;
    std::string filename = "configuration.txt";

    std::vector<std::variant<CLine, CCircle>> top;
    std::vector<std::variant<CLine, CCircle>> bottom;
    std::vector<std::variant<CLine, CCircle>> right;
    std::vector<std::variant<CLine, CCircle>> left;
    std::vector<std::variant<CLine, CCircle>> f_k;
    double allalpha;


    std::string line;
    std::ifstream file(filename);

    if (file.is_open()) {
        while (std::getline(file, line)) {
            char delimeter = ' ';
            std::vector<std::string> result; 
            std::stringstream ss(line);
            std::string word;
            while (std::getline(ss, word, delimeter)) {result.push_back(word);}

            if (result[0]=="x_max") {x_max=std::stod(result[1])}
            if (result[0]=="y_max") {x_max=std::stod(result[1])}
            if (result[0]=="dx") {x_max=std::stod(result[1])}
            if (result[0]=="dy") {x_max=std::stod(result[1])}

            if (result[0]=="f_k") {
                while (std::getline(file, line) && !line.empty()) {
                    std::cout << line << std::endl;
                    read(line, f_k, allalpha);
                }
            }
            if (result[0]=="f_top") {
                while (std::getline(file, line) && !line.empty()) {
                    read(line, bottom, allalpha);
                }
                //std::cout << 1 << std::endl;
            }
            if (result[0]=="f_bottom") {
                while (std::getline(file, line) && !line.empty()) {
                    read(line, bottom, allalpha);
                }
            }
            if (result[0]=="f_right") {
                while (std::getline(file, line) && !line.empty()) {
                    read(line, right, allalpha);
                }
            }
            if (result[0]=="f_left") {
                while (std::getline(file, line) && !line.empty()) {
                    read(line, left, allalpha);
                }
            }
        }
    }

    for (auto& item: f_k) {
        std::visit([](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                std::cout << "allalpha" << obj.k << " " << obj.b << " " << obj.start << " " << obj.finish << " " << obj.state << obj.value << std::endl;
            }
            if constexpr(std::is_same_v<T,CCircle>) {
                std::cout << "allalpha" << obj.x0 << " " << obj.y0 << " " << obj.R << " " << obj.state << " " << obj.value << std::endl;
            }
        }, item);
    }
    std::cout << "allalpha" << allalpha << std::endl;
    for (auto& item: bottom) {
        std::visit([](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                std::cout << obj.k << " " << obj.b << " " << obj.start << " " << obj.finish << " " << obj.state << obj.value << std::endl;
            }
            if constexpr(std::is_same_v<T,CCircle>) {
                std::cout << obj.x0 << " " << obj.y0 << " " << obj.R << " " << obj.state << " " << obj.value << std::endl;
            }

        }, item);
    }
    for (auto& item: left) {
        std::visit([](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                std::cout << obj.k << " " << obj.b << " " << obj.start << " " << obj.finish << " " << obj.state << std::endl;
            }
            if constexpr(std::is_same_v<T,CCircle>) {
                std::cout << obj.x0 << " " << obj.y0 << " " << obj.R << " " << obj.state << " " << obj.value << std::endl;
            }

        }, item);
    }
    for (auto& item: right) {
        std::visit([](auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            if constexpr(std::is_same_v<T,CLine>) {
                std::cout << obj.k << " " << obj.b << " " << obj.start << " " << obj.finish << " " << obj.state << std::endl;
            }
            if constexpr(std::is_same_v<T,CCircle>) {
                std::cout << obj.x0 << " " << obj.y0 << " " << obj.R << " " << obj.state << " " << obj.value << std::endl;
            }

        }, item);
    }

    file.close();

}