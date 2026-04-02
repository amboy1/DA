#pragma once 
#include <string>
#include <tuple>

const int BIT_COUNT = 8;

class Patricia {
private:
    struct Node;
    Node *root = nullptr;
    Node* Search(const std::string& findKey) const;
    std::tuple<Patricia::Node*, Patricia::Node*, Patricia::Node*> SearchE(const std::string& findKey) const;
    void Insert(const std::string& key, const int& value, const size_t& index);
public:
    void Add(const std::string& key, int value);
    int& At(const std::string& findKey) const;
    void Erase(const std::string& key);
};