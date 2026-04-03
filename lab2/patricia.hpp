#ifndef PATRICIA_HPP
#define PATRICIA_HPP

#include <string>
#include <tuple>
#include <stdexcept>

class Patricia {
public:
    struct Node {
        std::string key;
        int value;
        size_t index;
        Node *left, *right;
        
        Node(const std::string& k, int v, size_t idx) 
            : key(k), value(v), index(idx), left(nullptr), right(nullptr) {}
    };

private:
    Node* root;
    
    struct SearchResult {
        Node* currentNode;
        Node* prevNode;
        Node* prevPrevNode;
    };
    
    SearchResult SearchInternal(const std::string& findKey, bool needPrevPrev = false) const;
    void Insert(const std::string& key, int value, size_t index);  
    void ClearRecursive(Node* node, Node* parent);

public:
    Patricia() : root(nullptr) {}
    ~Patricia() { Clear(); } 
    
    void Clear();

    Node* Search(const std::string& findKey) const;
    std::tuple<Node*, Node*, Node*> SearchE(const std::string& findKey) const;
    void Add(const std::string& key, int value);
    int& At(const std::string& findKey) const;
    void Erase(const std::string& key);
    void PrintTree() const;
};

#endif