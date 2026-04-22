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
class CSquare {
public:
    double x1;
    double y1;
    double x2;
    double y2;
    std::string state;
    double value;
};
class CTriangle {
public:
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;
    std::string state;
    double value;
};

template<typename VariantType, typename ClassType, typename FieldType>
bool setField(const std::vector<std::string>& result, int& i, int n,
              VariantType& last, FieldType ClassType::*field) {
    if (i >= n) return false;
    bool final = std::visit([&](auto& obj) {
        using T = std::decay_t<decltype(obj)>;
        if constexpr(std::is_same_v<T, ClassType>) {
            if constexpr(std::is_same_v<FieldType, double>) {
                obj.*field = std::stod(result[i]);
                return true;
            } 
            else if constexpr(std::is_same_v<FieldType, std::string>) {
                obj.*field = result[i];
                return true;
            }
            else {return false;}
        }
        else {return false;}
    }, last);
    i++;
    if (final) {return true;}
    else {return false;}
}

template<typename VariantType, typename FieldsArrayType>
void prohod(const std::vector<std::string>& result, int& i, int n,
                        VariantType& last,
                        const std::vector<std::string>& nameLine,
                        const FieldsArrayType& fieldsLine) {
    while (i < n) {
        int j = 0;
        while (j < nameLine.size()) {
            if (result[i] == nameLine[j]) {
                i++;
                if (i >= n) return;
                if (!setField(result, i, n, last, fieldsLine[j])) {
                    std::cout << "неправильный синтаксис" << std::endl;
                    return;
                }
                break;
            }
            if (result[i] == "alpha") {
                i++; 
                if (i >= n) return;
                std::visit([&](auto& obj) {
                    using T = std::decay_t<decltype(obj)>;
                    obj.value = std::stod(result[i]);
                }, last);
                i++; 
                if (i >= n) return;
                break;
            }
            j++;
        }
    }
}

void read(const std::string& line, std::vector<std::variant<CLine, CCircle, CSquare, CTriangle>>& vec, double &allalpha) {
    char delimeter = ' ';
    std::vector<std::string> result; 
    std::stringstream ss(line);
    std::string word;
    while (std::getline(ss, word, delimeter)) {result.push_back(word);}
    int n = result.size();
    int i = 0;

    std::vector<double CLine::*> fieldsLine = {&CLine::b, &CLine::k, &CLine::start, &CLine::finish};
    std::vector<std::string> nameLine = {"b", "k", "start", "finish", "x0", "y0", "R"};
    std::vector<double CCircle::*> fieldsCircle = {&CCircle::x0, &CCircle::y0, &CCircle::R};
    std::vector<std::string> nameCircle = {"x0", "y0", "R"};
    std::vector<double CSquare::*> fieldsSquare = {&CSquare::x1, &CSquare::y1, &CSquare::x2, &CSquare::y2};
    std::vector<std::string> nameSquare = {"x1", "y1", "x2", "y2"};

    while (i<n) {
        if (result[i]== "form") {
        i++; if (i>=n) {break;}
        if (result[i] == "type") {
            i++; if (i>=n) {break;}
            if (result[i] == "line") {vec.push_back(CLine{}); i++; if (i>=n) {break;}}
            else if (result[i] == "circle") {vec.push_back(CCircle{}); i++; if (i>=n) {break;}}
            else if (result[i] == "square") {vec.push_back(CSquare{}); i++; if (i>=n) {break;}}
            else if (result[i] == "triangle") {vec.push_back(CTriangle{}); i++; if (i>=n) {break;}}
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

            std::visit([&](auto& obj) {
                using T = std::decay_t<decltype(obj)>;
                if constexpr(std::is_same_v<T, CLine>) {
                    prohod(result, i, n, last, nameLine, fieldsLine);
                }
            }, last);
            std::visit([&](auto& obj) {
                using T = std::decay_t<decltype(obj)>;
                if constexpr(std::is_same_v<T, CCircle>) {
                    prohod(result, i, n, last, nameCircle, fieldsCircle);
                }
            }, last);
            std::visit([&](auto& obj) {
                using T = std::decay_t<decltype(obj)>;
                if constexpr(std::is_same_v<T, CSquare>) {
                    prohod(result, i, n, last, nameSquare, fieldsSquare);
                }
            }, last);
        }
        else {break;}
        }
        if (i>=n) {break;}
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