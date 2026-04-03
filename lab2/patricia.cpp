#include "patricia.hpp"
#include <tuple>
#include <iostream>
#include <functional>
#include <queue>
#include <unordered_set>
#include <stdexcept>

inline bool GetBit(const std::string& s, size_t bitIndex) {
    size_t byte = bitIndex / 8;
    if (byte >= s.size()) return 0;

    unsigned char c = static_cast<unsigned char>(s[byte]);
    return (c >> (7 - (bitIndex % 8))) & 1;
}

Patricia::SearchResult Patricia::SearchInternal(const std::string& findKey, bool needPrevPrev) const {
    if(!root) return {nullptr, nullptr, nullptr};

    Node *currentNode = root->left, *prevNode = root, *prevPrevNode = root;

    size_t steps = 0;
    const size_t maxSteps = 1024;

    while(currentNode->index > prevNode->index){
        if(++steps > maxSteps)
            throw std::runtime_error("Patricia invariant broken: search loop");

        bool currentBit = GetBit(findKey, currentNode->index);

        if(needPrevPrev) prevPrevNode = prevNode;
        prevNode = currentNode;
        currentNode = currentBit ? currentNode->right : currentNode->left;
    }

    return {currentNode, prevNode, prevPrevNode};
}

Patricia::Node* Patricia::Search(const std::string& findKey) const {
    if(!root) return nullptr;

    auto result = SearchInternal(findKey, false);

    if(result.currentNode && result.currentNode->key == findKey)
        return result.currentNode;

    return nullptr;
}

std::tuple<Patricia::Node*, Patricia::Node*, Patricia::Node*>
Patricia::SearchE(const std::string& findKey) const {
    if(!root) return {nullptr, nullptr, nullptr};
    auto result = SearchInternal(findKey, true);
    return std::make_tuple(result.currentNode, result.prevNode, result.prevPrevNode);
}

static std::pair<Patricia::Node*, Patricia::Node*>
SearchInsertPlace(Patricia::Node* root, const std::string& key, size_t newIndex) {
    Patricia::Node* parent = root;
    Patricia::Node* current = root->left;

    while(current->index > parent->index && current->index < newIndex) {
        parent = current;
        bool b = GetBit(key, current->index);
        current = b ? current->right : current->left;
    }
    return {current, parent}; 
}

void Patricia::Add(const std::string& key, int value){
    if(!root){
        root = new Node(key, value, 0);
        root->left = root;
        root->right = root;
        return;
    }

    Node *foundNode = Search(key);
    if(foundNode) {
        throw std::runtime_error("\t\"" + key + "\" already exist!\n");
    }

    Node *nearest = SearchInternal(key, false).currentNode;

    size_t bitIndex = 0;
    while(true){
        bool foundBit = GetBit(nearest->key, bitIndex);
        bool newBit   = GetBit(key, bitIndex);

        if(foundBit != newBit){
            Insert(key, value, bitIndex);
            return;
        }
        bitIndex++;
    }
}

void Patricia::Insert(const std::string& key, int value, size_t index){
    auto [currentNode, prevNode] = SearchInsertPlace(root, key, index);

    Node* newNode = new Node(key, value, index);
    bool b = GetBit(key, index);

    if(prevNode->left == currentNode) prevNode->left = newNode;
    else                              prevNode->right = newNode;

    if(b) { newNode->right = newNode; newNode->left  = currentNode; }
    else  { newNode->left  = newNode; newNode->right = currentNode; }
}

int& Patricia::At(const std::string& findKey) const{
    Node* get = Search(findKey);
    if(!get)
        throw std::runtime_error("\t\"" + findKey + "\" hadn't found!\n");
    return get->value;
}

void Patricia::Erase(const std::string& key){
    if(!root)
        throw std::runtime_error("\t\"" + key + "\" hadn't found!\n");

    auto [deleteNode, ownerDeleteNode, parentOwnerDeleteNode] = SearchE(key);

    if(!deleteNode || deleteNode->key != key)
        throw std::runtime_error("\t\"" + key + "\" hadn't found!\n");

    if(deleteNode == root && root->left == root){
        delete root;
        root = nullptr;
        return;
    }

    // If delete node is leaf
    if(ownerDeleteNode == deleteNode){
        if(parentOwnerDeleteNode->right == deleteNode)
            parentOwnerDeleteNode->right = (deleteNode->right == deleteNode) ? deleteNode->left : deleteNode->right;
        else
            parentOwnerDeleteNode->left  = (deleteNode->right == deleteNode) ? deleteNode->left : deleteNode->right;

        delete deleteNode;
        return;
    }

    auto [__, ownerOwnerDelNode, ___] = SearchE(ownerDeleteNode->key);

    deleteNode->key = ownerDeleteNode->key;
    deleteNode->value = ownerDeleteNode->value;

    if(ownerOwnerDelNode == ownerDeleteNode){
        if(parentOwnerDeleteNode->right == ownerDeleteNode)
            parentOwnerDeleteNode->right = deleteNode;
        else
            parentOwnerDeleteNode->left = deleteNode;
    } else {
        if(parentOwnerDeleteNode->right == ownerDeleteNode)
            parentOwnerDeleteNode->right = (ownerDeleteNode->right == deleteNode) ? ownerDeleteNode->left : ownerDeleteNode->right;
        else
            parentOwnerDeleteNode->left  = (ownerDeleteNode->right == deleteNode) ? ownerDeleteNode->left : ownerDeleteNode->right;

        if(ownerOwnerDelNode->right == ownerDeleteNode)
            ownerOwnerDelNode->right = deleteNode;
        else
            ownerOwnerDelNode->left = deleteNode;
    }

    delete ownerDeleteNode;
}

void Patricia::Clear() {
    if(!root) return;

    std::unordered_set<Node*> visited;
    std::vector<Node*> stack;
    stack.push_back(root);

    while(!stack.empty()) {
        Node* n = stack.back();
        stack.pop_back();
        if(!n) continue;
        if(visited.insert(n).second == false) continue;

        // добавляем детей, но не боимся self/циклов — visited защитит
        if(n->left)  stack.push_back(n->left);
        if(n->right) stack.push_back(n->right);
    }

    // удаляем после обхода
    for(Node* n : visited) {
        delete n;
    }

    root = nullptr;
}

void Patricia::PrintTree() const {
    if(!root) {
        std::cout << "Empty tree" << std::endl;
        return;
    }

    std::cout << "\n=== Tree Structure ===" << std::endl;

    std::queue<Node*> q;
    std::unordered_set<Node*> visited;

    q.push(root);
    visited.insert(root);

    while(!q.empty()) {
        Node* current = q.front();
        q.pop();

        std::cout << "Node: key=\"" << current->key << "\""
                  << ", value=" << current->value
                  << ", index=" << current->index;

        if(current->left == current) {
            std::cout << ", left=self";
        } else if(current->left) {
            std::cout << ", left=\"" << current->left->key << "\"";
        } else {
            std::cout << ", left=null";
        }

        if(current->right == current) {
            std::cout << ", right=self";
        } else if(current->right) {
            std::cout << ", right=\"" << current->right->key << "\"";
        } else {
            std::cout << ", right=null";
        }

        std::cout << std::endl;

        if(current->left && current->left != current && !visited.count(current->left)) {
            visited.insert(current->left);
            q.push(current->left);
        }

        if(current->right && current->right != current && !visited.count(current->right)) {
            visited.insert(current->right);
            q.push(current->right);
        }
    }

    std::cout << "===================\n" << std::endl;
}