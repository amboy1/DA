#include <iostream>
#include <vector>
#include <string>
#include <numeric>

struct Date {
    int day, month, year;
    Date(const std::string& s) {
        size_t p1 = s.find('.'), p2 = s.find('.', p1 + 1);
        day = std::stoi(s.substr(0, p1));
        month = std::stoi(s.substr(p1 + 1, p2 - p1 - 1));
        year = std::stoi(s.substr(p2 + 1));
    }
};

struct Pair { Date key; std::string key_str, value; };

std::vector<Pair> pairs; 

auto get_digit = [](const Date& k, int pass) {
    if (pass == 0) return k.day % 10;
    if (pass == 1) return (k.day / 10) % 10;
    if (pass == 2) return k.month % 10;
    if (pass == 3) return (k.month / 10) % 10;
    if (pass == 4) return k.year % 10;
    if (pass == 5) return (k.year / 10) % 10;
    if (pass == 6) return (k.year / 100) % 10;
    return (k.year / 1000) % 10;
};

void radix_sort(std::vector<size_t>& indices) { //теперь сортируем индексы
    std::vector<std::vector<size_t>> buckets(10);

    for (int pass = 0; pass < 8; ++pass) {
        for (auto& b : buckets) b.clear();

        for (size_t i : indices) {
            int digit = get_digit(pairs[i].key, pass);
            buckets[digit].push_back(i);
        }

        size_t idx = 0;
        for (auto& b : buckets) {
            for (size_t pos : b) indices[idx++] = pos;
        }
    }
}

int main() {
    std::string k, v;
    while (std::cin >> k >> v) { 
        pairs.push_back({ Date(k), k, v });
    }

    std::vector<size_t> indices(pairs.size());
    std::iota(indices.begin(), indices.end(), 0);
    radix_sort(indices);

    for (size_t i : indices) {
        std::cout << pairs[i].key_str << "\t" << pairs[i].value << "\n";  
    }
}
