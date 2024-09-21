#pragma once

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace sonic {

struct Option {
    std::string name, type;
    std::string default_value, current_value;
    int min, max;
    std::function<void(void)> button;

    Option() {}
    Option(std::string name_, std::string type_) : name(name_), type(type_) {}
    Option(std::string name_, std::string type_, std::function<void(void)> button_) : name(name_), type(type_), button(button_) {}
    Option(std::string name_, std::string type_, std::string default_val) : name(name_), type(type_), default_value(default_val), current_value(default_val) {}
    Option(std::string name_, std::string type_, std::string default_val, int min_, int max_) : name(name_), type(type_), default_value(default_val), current_value(default_val), min(min_), max(max_) {}

    operator int() const { return stoi(current_value); }
    operator double() const { return stof(current_value); }
    operator std::string() const { return current_value; }

    std::string to_string() const {
        std::string result = std::string("option name ") + name + " type " + type;
        if(type != "button") {
            result += std::string(" default ") + default_value;
            if(type == "spin") {
                result += std::string(" min ") + std::to_string(min) + " max " + std::to_string(max);
            }
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& out, const Option& o) {
        return out << o.to_string();
    }
};

class OptionsMap {
public:
    void add_option(std::string name, std::string type) { options[name] = Option(name, type); }
    void add_option(std::string name, std::string type, std::function<void(void)> button) { options[name] = Option(name, type, button); }
    void add_option(std::string name, std::string type, std::string default_value) { options[name] = Option(name, type, default_value); }
    void add_option(std::string name, std::string type, std::string default_value, int min, int max) { options[name] = Option(name, type, default_value, min, max); }

    void button(const std::string& name) {
        if(options.find(name) == options.end()) {
            std::cout << "Unknown command: \"" << name << "\".\n";
            return;
        }
        Option& option = options[name];
        option.button();
    }

    void set(const std::string& name, const std::string& value) {
        if(options.find(name) == options.end()) {
            std::cout << "Unknown command: \"" << name << "\".\n";
            return;
        }
        Option& option = options[name];
        if(option.type == "spin") {
            int value_int = stoi(value);
            if(option.min <= value_int && value_int <= option.max) {
                option.current_value = value;
            }
            return;
        }
        options[name].current_value = value;
    }

    std::string to_string() const {
        std::string result;
        for(auto p : options) {
            result += p.second.to_string();
            result += "\n";
        }
        result.pop_back();
        return result;
    }

    friend std::ostream& operator<<(std::ostream& out, const OptionsMap& o) {
        return out << o.to_string();
    }

    const Option& operator[](const std::string& name) {
        return options[name];
    }

private:
    std::unordered_map<std::string, Option> options;
};
    
} // namespace sonic