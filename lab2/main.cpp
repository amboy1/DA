#include <iostream>
#include <string>
#include <algorithm>
#include "patricia.hpp"

std::string ToLower(const std::string& s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

int main() {
    Patricia dict;
    std::string line;

    while (std::getline(std::cin, line)) {
        try {
            if (line.empty()) continue;

            if (line[0] == '+') {
                // + word 34
                size_t pos = line.find(' ', 2);
                std::string word = ToLower(line.substr(2, pos - 2));
                uint64_t value = std::stoull(line.substr(pos + 1));

                try {
                    dict.Add(word, value);
                    std::cout << "OK\n";
                } catch (...) {
                    std::cout << "Exist\n";
                }
            }
            else if (line[0] == '-') {
                // - word
                std::string word = ToLower(line.substr(2));

                try {
                    dict.Erase(word);
                    std::cout << "OK\n";
                } catch (...) {
                    std::cout << "NoSuchWord\n";
                }
            }
            else {
                // search
                std::string word = ToLower(line);

                try {
                    uint64_t val = dict.At(word);
                    std::cout << "OK: " << val << "\n";
                } catch (...) {
                    std::cout << "NoSuchWord\n";
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }

    return 0;
}