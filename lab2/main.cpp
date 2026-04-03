// #include <iostream>
// #include <string>
// #include <algorithm>
// #include "patricia.hpp"
// #include <vector>

// std::string ToLower(const std::string& s) {
//     std::string res = s;
//     std::transform(res.begin(), res.end(), res.begin(), ::tolower);
//     return res;
// }

// int main() {
//     Patricia dict;
//     std::string line;

//     while (std::getline(std::cin, line)) {
//         try {
//             if (line.empty()) continue;

//             if (line[0] == '+') {
//                 // + word 34
//                 size_t pos = line.find(' ', 2);
//                 std::string word = ToLower(line.substr(2, pos - 2));
//                 uint64_t value = std::stoull(line.substr(pos + 1));

//                 try {
//                     dict.Add(word, value);
//                     std::cout << "OK\n";
//                 } catch (...) {
//                     std::cout << "Exist\n";
//                 }
//             }
//             else if (line[0] == '-') {
//                 // - word
//                 std::string word = ToLower(line.substr(2));

//                 try {
//                     dict.Erase(word);
//                     std::cout << "OK\n";
//                 } catch (...) {
//                     std::cout << "NoSuchWord\n";
//                 }
//             }
//             else {
//                 // search
//                 std::string word = ToLower(line);

//                 try {
//                     uint64_t val = dict.At(word);
//                     std::cout << "OK: " << val << "\n";
//                 } catch (...) {
//                     std::cout << "NoSuchWord\n";
//                 }
//             }
//         }
//         catch (const std::exception& e) {
//             std::cout << "ERROR: " << e.what() << "\n";
//         }
//     }

//     return 0;
// }

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "patricia.hpp"

static void CheckAt(Patricia& t, const std::string& k, bool shouldExist, int expected = 0) {
    try {
        int v = t.At(k);
        if(!shouldExist) {
            std::cout << "[FAIL] expected NOT FOUND, but found: " << k << " -> " << v << "\n";
        } else if(v != expected) {
            std::cout << "[FAIL] wrong value: " << k << " -> " << v << ", expected " << expected << "\n";
        } else {
            std::cout << "[ OK ] " << k << " -> " << v << "\n";
        }
    } catch(...) {
        if(shouldExist) std::cout << "[FAIL] expected FOUND, but NOT FOUND: " << k << "\n";
        else           std::cout << "[ OK ] " << k << " -> NOT FOUND\n";
    }
}

static void MustThrowAdd(Patricia& t, const std::string& k, int v) {
    try {
        t.Add(k, v);
        std::cout << "[FAIL] expected Add to throw for duplicate key: " << k << "\n";
    } catch(...) {
        std::cout << "[ OK ] Add duplicate throws for: " << k << "\n";
    }
}

static void MustThrowErase(Patricia& t, const std::string& k) {
    try {
        t.Erase(k);
        std::cout << "[FAIL] expected Erase to throw (missing): " << k << "\n";
    } catch(...) {
        std::cout << "[ OK ] Erase missing throws for: " << k << "\n";
    }
}

int main() {
    // =========================
    // TEST 1: твой сценарий + проверка что "9" реально удалён
    // =========================
    {
        std::cout << "\n========== TEST 1: erase existing key ==========\n";
        Patricia t;
        t.Add("1", 1);
        t.Add("9", 2);
        t.Add("8", 3);
        t.Add("12", 4);
        t.Add("13", 5);
        t.Add("3", 6);

        std::cout << "Before erase:\n";
        t.PrintTree();

        t.Erase("9");

        std::cout << "After erase:\n";
        t.PrintTree();

        CheckAt(t, "1", true, 1);
        CheckAt(t, "8", true, 3);
        CheckAt(t, "12", true, 4);
        CheckAt(t, "13", true, 5);
        CheckAt(t, "3", true, 6);
        CheckAt(t, "9", false);
    }

    // =========================
    // TEST 2: удалить все по одному + дерево должно стать пустым
    // =========================
    {
        std::cout << "\n========== TEST 2: erase all keys ==========\n";
        Patricia t;
        std::vector<std::pair<std::string,int>> items = {
            {"a",1},{"b",2},{"c",3},{"d",4},{"e",5}
        };
        for(auto& [k,v] : items) t.Add(k,v);

        for(auto& [k,v] : items) CheckAt(t, k, true, v);

        for(auto& [k,v] : items) {
            std::cout << "Erase: " << k << "\n";
            t.Erase(k);
            CheckAt(t, k, false);
        }

        // дерево пустое -> Search/At должны вести себя нормально
        MustThrowErase(t, "nope");
        CheckAt(t, "a", false);
        std::cout << "[INFO] printing tree (should be empty):\n";
        t.PrintTree();
    }

    // =========================
    // TEST 3: удаление несуществующего
    // =========================
    {
        std::cout << "\n========== TEST 3: erase missing ==========\n";
        Patricia t;
        t.Add("x", 10);
        MustThrowErase(t, "y");
        CheckAt(t, "x", true, 10);
    }

    // =========================
    // TEST 4: дубликаты Add должны кидать исключение
    // =========================
    {
        std::cout << "\n========== TEST 4: add duplicate ==========\n";
        Patricia t;
        t.Add("dup", 1);
        MustThrowAdd(t, "dup", 2);
        CheckAt(t, "dup", true, 1);
    }

    // =========================
    // TEST 5: ключи-префиксы (часто ломают tries/patricia)
    // =========================
    {
        std::cout << "\n========== TEST 5: prefix keys ==========\n";
        Patricia t;
        t.Add("a", 1);
        t.Add("aa", 2);
        t.Add("aaa", 3);
        t.Add("aaaa", 4);

        CheckAt(t, "a", true, 1);
        CheckAt(t, "aa", true, 2);
        CheckAt(t, "aaa", true, 3);
        CheckAt(t, "aaaa", true, 4);

        t.Erase("aa");
        CheckAt(t, "a", true, 1);
        CheckAt(t, "aa", false);
        CheckAt(t, "aaa", true, 3);
        CheckAt(t, "aaaa", true, 4);

        t.Erase("a");
        CheckAt(t, "a", false);
        CheckAt(t, "aaa", true, 3);
        CheckAt(t, "aaaa", true, 4);
    }

    // =========================
    // TEST 6: “сложные байты” (не ASCII), чтобы проверить GetBit/unsigned char
    // =========================
    {
        std::cout << "\n========== TEST 6: non-ascii bytes ==========\n";
        Patricia t;
        t.Add("привет", 1);
        t.Add("пока", 2);
        t.Add("прив", 3);

        CheckAt(t, "привет", true, 1);
        CheckAt(t, "пока", true, 2);
        CheckAt(t, "прив", true, 3);

        t.Erase("пока");
        CheckAt(t, "пока", false);
        CheckAt(t, "привет", true, 1);
        CheckAt(t, "прив", true, 3);
    }

    // =========================
    // TEST 7: перемешанные операции (мини “стресс” руками)
    // =========================
    {
        std::cout << "\n========== TEST 7: mixed operations ==========\n";
        Patricia t;
        t.Add("10", 10);
        t.Add("11", 11);
        t.Add("12", 12);
        t.Add("13", 13);

        t.Erase("12");
        CheckAt(t, "12", false);

        t.Add("12", 120);          // добавить снова после удаления
        CheckAt(t, "12", true, 120);

        t.Erase("10");
        CheckAt(t, "10", false);

        CheckAt(t, "11", true, 11);
        CheckAt(t, "13", true, 13);

        std::cout << "[INFO] final tree:\n";
        t.PrintTree();
    }
}
