#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <variant>
#include <stdexcept>

// === Макросы для безопасного парсинга ===
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

#define VALIDATE_GEOM(cond, msg) \
    do { \
        if (!(cond)) { throw std::runtime_error(std::string("Ошибка геометрии: ") + (msg)); } \
    } while(0)

// === 3D ПРИМИТИВЫ ДЛЯ ПЕРЕМЕННОЙ k ===
class CSphere3D {
public:
    double x0, y0, z0;
    double R;
    double k_value;
    
    CSphere3D() : x0(0), y0(0), z0(0), R(0), k_value(1.0) {}
};

class CBox3D {
public:
    double x1, y1, z1;
    double x2, y2, z2;
    double k_value;
    
    CBox3D() : x1(0), y1(0), z1(0), x2(0), y2(0), z2(0), k_value(1.0) {}
};

// === СТРУКТУРА ГРАНИЧНОГО УСЛОВИЯ ===
struct BoundaryCondition3D {
    std::string type;
    double value;
    bool is_set;
    
    BoundaryCondition3D() : type("zero_gradient"), value(0.0), is_set(false) {}
};

// === ЧТЕНИЕ 3D ПРИМИТИВОВ ДЛЯ k ===
void read_k_primitive(const std::string& line, 
                      std::vector<std::variant<CSphere3D, CBox3D>>& vec) {
    std::vector<std::string> result; 
    std::stringstream ss(line);
    std::string word;
    while (std::getline(ss, word, ' ')) {
        if (!word.empty()) result.push_back(word);
    }
    
    int n = static_cast<int>(result.size());
    int i = 0;

    while (i < n) {
        SAFE_IDX(result, i);
        
        if (result[i] == "form") {
            i++; SAFE_IDX(result, i);
            if (result[i] != "type") throw std::runtime_error("Синтаксическая ошибка: после 'form' ожидается 'type'");
            i++; SAFE_IDX(result, i);
            
            if (result[i] == "sphere") {
                vec.push_back(CSphere3D{}); i++;
            }
            else if (result[i] == "box") {
                vec.push_back(CBox3D{}); i++;
            }
            else {
                throw std::runtime_error("Неизвестный тип 3D фигуры: '" + result[i] + "'");
            }

            auto& last = vec.back();

            std::visit([&](auto& obj) {
                using T = std::decay_t<decltype(obj)>;
                
                if constexpr(std::is_same_v<T, CSphere3D>) {
                    while (i < n) {
                        SAFE_IDX(result, i);
                        bool found = false;
                        
                        if (result[i] == "x0") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.x0);
                            i++; found = true;
                        }
                        else if (result[i] == "y0") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.y0);
                            i++; found = true;
                        }
                        else if (result[i] == "z0") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.z0);
                            i++; found = true;
                        }
                        else if (result[i] == "R") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.R);
                            i++; found = true;
                        }
                        else if (result[i] == "k") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.k_value);
                            i++; found = true;
                        }
                        
                        if (!found) break;
                    }
                    VALIDATE_GEOM(obj.R > 0, "для сферы: радиус R должен быть > 0");
                }
                else if constexpr(std::is_same_v<T, CBox3D>) {
                    while (i < n) {
                        SAFE_IDX(result, i);
                        bool found = false;
                        
                        if (result[i] == "x1") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.x1);
                            i++; found = true;
                        }
                        else if (result[i] == "y1") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.y1);
                            i++; found = true;
                        }
                        else if (result[i] == "z1") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.z1);
                            i++; found = true;
                        }
                        else if (result[i] == "x2") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.x2);
                            i++; found = true;
                        }
                        else if (result[i] == "y2") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.y2);
                            i++; found = true;
                        }
                        else if (result[i] == "z2") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.z2);
                            i++; found = true;
                        }
                        else if (result[i] == "k") {
                            i++; SAFE_IDX(result, i);
                            SAFE_STOD(result[i], obj.k_value);
                            i++; found = true;
                        }
                        
                        if (!found) break;
                    }
                    VALIDATE_GEOM(obj.x1 < obj.x2 && obj.y1 < obj.y2 && obj.z1 < obj.z2, 
                                 "для бокса: x1 < x2, y1 < y2, z1 < z2");
                }
            }, last);
        }
        else {
            i++;
        }
    }
}

// === ЧТЕНИЕ ГРАНИЧНОГО УСЛОВИЯ ===
void read_boundary_condition(const std::string& line, BoundaryCondition3D& bc) {
    std::vector<std::string> result; 
    std::stringstream ss(line);
    std::string word;
    while (std::getline(ss, word, ' ')) {
        if (!word.empty()) result.push_back(word);
    }
    
    int n = static_cast<int>(result.size());
    int i = 0;

    while (i < n) {
        SAFE_IDX(result, i);
        
        if (result[i] == "u") {
            i++; SAFE_IDX(result, i);
            if (result[i] != "type") throw std::runtime_error("Синтаксическая ошибка: после 'u' ожидается 'type'");
            i++; SAFE_IDX(result, i);
            
            if (result[i] == "zero_gradient") {
                bc.type = "zero_gradient";
                bc.is_set = true;
                i++;
            }
            else if (result[i] == "fixed") {
                bc.type = "fixed";
                bc.is_set = true;
                i++; SAFE_IDX(result, i);
                if (result[i] == "value") {
                    i++; SAFE_IDX(result, i);
                    SAFE_STOD(result[i], bc.value);
                    i++;
                }
            }
            else {
                throw std::runtime_error("Неизвестный тип ГУ: '" + result[i] + "'");
            }
        }
        else {
            i++;
        }
    }
}

// === СТРУКТУРА КОНФИГУРАЦИИ ===
struct Config3D {
    double x_max, y_max, z_max;
    double dx, dy, dz;
    
    BoundaryCondition3D bc_x_left;
    BoundaryCondition3D bc_x_right;
    BoundaryCondition3D bc_y_bottom;
    BoundaryCondition3D bc_y_top;
    BoundaryCondition3D bc_z_back;
    BoundaryCondition3D bc_z_front;
    
    std::vector<std::variant<CSphere3D, CBox3D>> k_primitives;
    
    Config3D() : x_max(10), y_max(10), z_max(10), 
                 dx(1), dy(1), dz(1) {}
};

bool read_configuration_3d(const std::string& filename, Config3D& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ВНИМАНИЕ: не удалось открыть файл '" << filename 
                  << "', используются значения по умолчанию" << std::endl;
        return false;
    }

    std::string current_section = "";
    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
        line_number++;
        
        if (line.empty() || line[0] == '#') continue;
        
        std::vector<std::string> result; 
        std::stringstream ss(line);
        std::string word;
        while (std::getline(ss, word, ' ')) {
            if (!word.empty()) result.push_back(word);
        }
        
        if (result.empty()) continue;
        
        try {
            if (result[0] == "x_left" || result[0] == "x_right" ||
                result[0] == "y_bottom" || result[0] == "y_top" ||
                result[0] == "z_back" || result[0] == "z_front") {
                current_section = result[0];
                continue;
            }
            else if (result[0] == "f_k") {
                current_section = "f_k";
                continue;
            }
            
            if (result[0] == "x_max") {
                if (result.size() < 2) throw std::runtime_error("x_max: отсутствует значение");
                SAFE_STOD(result[1], config.x_max);
                current_section = "";
            }
            else if (result[0] == "y_max") {
                if (result.size() < 2) throw std::runtime_error("y_max: отсутствует значение");
                SAFE_STOD(result[1], config.y_max);
                current_section = "";
            }
            else if (result[0] == "z_max") {
                if (result.size() < 2) throw std::runtime_error("z_max: отсутствует значение");
                SAFE_STOD(result[1], config.z_max);
                current_section = "";
            }
            else if (result[0] == "dx") {
                if (result.size() < 2) throw std::runtime_error("dx: отсутствует значение");
                SAFE_STOD(result[1], config.dx);
                current_section = "";
            }
            else if (result[0] == "dy") {
                if (result.size() < 2) throw std::runtime_error("dy: отсутствует значение");
                SAFE_STOD(result[1], config.dy);
                current_section = "";
            }
            else if (result[0] == "dz") {
                if (result.size() < 2) throw std::runtime_error("dz: отсутствует значение");
                SAFE_STOD(result[1], config.dz);
                current_section = "";
            }
            else if (current_section == "x_left") {
                read_boundary_condition(line, config.bc_x_left);
            }
            else if (current_section == "x_right") {
                read_boundary_condition(line, config.bc_x_right);
            }
            else if (current_section == "y_bottom") {
                read_boundary_condition(line, config.bc_y_bottom);
            }
            else if (current_section == "y_top") {
                read_boundary_condition(line, config.bc_y_top);
            }
            else if (current_section == "z_back") {
                read_boundary_condition(line, config.bc_z_back);
            }
            else if (current_section == "z_front") {
                read_boundary_condition(line, config.bc_z_front);
            }
            else if (current_section == "f_k") {
                read_k_primitive(line, config.k_primitives);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "ОШИБКА в строке " << line_number << ": " << e.what() 
                      << "\nСтрока: \"" << line << "\"" << std::endl;
            return false;
        }
    }

    file.close();
    
    if (config.x_max <= 0 || config.y_max <= 0 || config.z_max <= 0 ||
        config.dx <= 0 || config.dy <= 0 || config.dz <= 0) {
        std::cerr << "ОШИБКА: некорректные параметры сетки" << std::endl;
        return false;
    }
    
    return true;
}