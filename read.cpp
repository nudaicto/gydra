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


    std::string line;
    std::ifstream file(filename);

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line=="f_top") {
                while (std::getline(file, line) && !line.empty()) {
                    read(line, top);
                }
            }
            if (line=="f_bottom") {
                while (std::getline(file, line) && !line.empty()) {
                    read(line, bottom);
                }
            }
            if (line=="f_right") {
                while (std::getline(file, line) && !line.empty()) {
                    read(line, right);
                }
            }
            if (line=="f_left") {
                while (std::getline(file, line) && !line.empty()) {
                    read(line, left);
                }
            }
        }
    }

    for (auto& item: top) {
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
    for (auto& item: bottom) {
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