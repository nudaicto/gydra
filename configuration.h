#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <variant>
#include <string>
#include <stdexcept> // Нужно для std::runtime_error

// === Макросы для безопасного парсинга (теперь бросают исключения) ===
#define SAFE_IDX(vec, idx) \
    do { \
        if ((idx) >= static_cast<int>((vec).size())) { \
            throw std::runtime_error("Индекс " + std::to_string(idx) + " вне диапазона (размер: " + std::to_string((vec).size()) + ")"); \
        } \
    } while(0)

#define SAFE_STOD(str, out) \
    do { \
        try { (out) = std::stod(str); } \
        catch (const std::invalid_argument&) { throw std::runtime_error("'" + (str) + "' не является числом"); } \
        catch (const std::out_of_range&) { throw std::runtime_error("Значение '" + (str) + "' вне диапазона double"); } \
    } while(0)

#define SAFE_STOI(str, out) \
    do { \
        try { (out) = std::stoi(str); } \
        catch (const std::invalid_argument&) { throw std::runtime_error("'" + (str) + "' не является целым числом"); } \
        catch (const std::out_of_range&) { throw std::runtime_error("Значение '" + (str) + "' вне диапазона int"); } \
    } while(0)

#define VALIDATE_GEOM(cond, msg) \
    do { \
        if (!(cond)) { throw std::runtime_error(std::string("Ошибка геометрии: ") + (msg)); } \
    } while(0)

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
    std::vector<std::string> result; 
    std::stringstream ss(line);
    std::string word;
    while (std::getline(ss, word, ' ')) {
        if (!word.empty()) result.push_back(word);
    }
    
    int n = static_cast<int>(result.size());
    int i = 0;

    std::vector<double CLine::*> fieldsLine = {&CLine::b, &CLine::k, &CLine::start, &CLine::finish};
    std::vector<std::string> nameLine = {"b", "k", "start", "finish", "x0", "y0", "R"};
    std::vector<double CCircle::*> fieldsCircle = {&CCircle::x0, &CCircle::y0, &CCircle::R};
    std::vector<std::string> nameCircle = {"x0", "y0", "R"};
    std::vector<double CSquare::*> fieldsSquare = {&CSquare::x1, &CSquare::y1, &CSquare::x2, &CSquare::y2};
    std::vector<std::string> nameSquare = {"x1", "y1", "x2", "y2"};

    while (i < n) {
        SAFE_IDX(result, i);
        
        if (result[i] == "form") {
            i++; SAFE_IDX(result, i);
            if (result[i] != "type") throw std::runtime_error("Синтаксическая ошибка: после 'form' ожидается 'type'");
            i++; SAFE_IDX(result, i);
            
            if (result[i] == "line") {
                vec.push_back(CLine{}); i++;
            }
            else if (result[i] == "circle") {
                vec.push_back(CCircle{}); i++;
            }
            else if (result[i] == "square") {
                vec.push_back(CSquare{}); i++;
            }
            else if (result[i] == "triangle") {
                vec.push_back(CTriangle{}); i++;
            }
            else if (result[i] == "all") {
                i++; SAFE_IDX(result, i);
                if (result[i] == "alpha") {
                    i++; SAFE_IDX(result, i);
                    SAFE_STOD(result[i], allalpha);
                    i++;
                }
                continue;
            }
            else {
                throw std::runtime_error("Неизвестный тип фигуры: '" + result[i] + "'");
            }

            auto& last = vec.back();

            std::visit([&](auto& obj) {
                if constexpr(std::is_same_v<std::decay_t<decltype(obj)>, CLine>) {
                    while (i < n) {
                        SAFE_IDX(result, i);
                        bool found = false;
                        for (size_t fld = 0; fld < nameLine.size(); fld++) {
                            if (result[i] == nameLine[fld]) {
                                i++; SAFE_IDX(result, i);
                                SAFE_STOD(result[i], obj.*fieldsLine[fld]);
                                i++; found = true; break;
                            }
                        }
                        if (result[i] == "alpha") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.value);
                            i++; found = true;
                        }
                        if (!found) break;
                    }
                    VALIDATE_GEOM(obj.start < obj.finish, "для линии: start должен быть < finish");
                }
                else if constexpr(std::is_same_v<std::decay_t<decltype(obj)>, CCircle>) {
                    while (i < n) {
                        SAFE_IDX(result, i);
                        bool found = false;
                        for (size_t fld = 0; fld < nameCircle.size(); fld++) {
                            if (result[i] == nameCircle[fld]) {
                                i++; SAFE_IDX(result, i);
                                SAFE_STOD(result[i], obj.*fieldsCircle[fld]);
                                i++; found = true; break;
                            }
                        }
                        if (result[i] == "alpha") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.value);
                            i++; found = true;
                        }
                        if (!found) break;
                    }
                    VALIDATE_GEOM(obj.R > 0, "для круга: радиус R должен быть > 0");
                }
                else if constexpr(std::is_same_v<std::decay_t<decltype(obj)>, CSquare>) {
                    while (i < n) {
                        SAFE_IDX(result, i);
                        bool found = false;
                        for (size_t fld = 0; fld < nameSquare.size(); fld++) {
                            if (result[i] == nameSquare[fld]) {
                                i++; SAFE_IDX(result, i);
                                SAFE_STOD(result[i], obj.*fieldsSquare[fld]);
                                i++; found = true; break;
                            }
                        }
                        if (result[i] == "alpha") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.value);
                            i++; found = true;
                        }
                        if (!found) break;
                    }
                    VALIDATE_GEOM(obj.x1 < obj.x2 && obj.y1 < obj.y2, 
                                 "для квадрата: x1 < x2 и y1 < y2");
                }
            }, last);
        }
        else if (result[i] == "u") {
            i++; SAFE_IDX(result, i);
            if (result[i] != "type") throw std::runtime_error("Синтаксическая ошибка: после 'u' ожидается 'type'");
            i++; SAFE_IDX(result, i);
            
            if (vec.empty()) {
                throw std::runtime_error(
                    "Синтаксическая ошибка: параметр 'u type' должен идти ПОСЛЕ объявления фигуры 'form type'. "
                    "Сначала задайте форму объекта, затем укажите для него граничное условие."
                );
            }
            auto& last = vec.back();
            
            if (result[i] == "zero_gradient") {
                std::visit([&](auto& obj) { obj.state = result[i]; }, last);
                i++;
            }
            else if (result[i] == "fixed") {
                std::visit([&](auto& obj) { obj.state = result[i]; }, last);
                i++; SAFE_IDX(result, i);
                if (result[i] == "value") {
                    i++; SAFE_IDX(result, i);
                    std::visit([&](auto& obj) { 
                        try { obj.value = std::stod(result[i]); } 
                        catch (...) { throw std::runtime_error("Параметр 'value' должен быть числом"); } 
                    }, last);
                    i++;
                }
            }
            else {
                throw std::runtime_error("Неизвестный тип граничного условия: '" + result[i] + "'");
            }
        }
        else {
            // Неизвестный токен пропускаем (или можно заменить на throw)
            i++;
        }
    }
}