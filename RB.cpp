#include "BST.h"
#include <algorithm>
#include <iostream>

BinarySearchTree::Node::Node(Key key, Value value, Node *parent, Node *left, Node *right, Color color) : keyValuePair{key, value}, parent(parent), left(left), right(right), color(color) {}

BinarySearchTree::Node::Node(const Node& other): keyValuePair(other.keyValuePair), parent(nullptr), left(nullptr), right(nullptr), color(other.color) {}

bool BinarySearchTree::Node::operator==(const Node& other) const {
    return keyValuePair == other.keyValuePair;
}

void BinarySearchTree::Node::output_node_tree() const {
    if (left) left->output_node_tree();
    std::cout << "[" << keyValuePair.first << ": " << keyValuePair.second << "] ";
    if (right) right->output_node_tree();
}

bool BinarySearchTree::isRed(Node* node) const {
    return node && node->color == RED;
}

BinarySearchTree::Node* BinarySearchTree::rotate_left(Node *h) {
    Node *x = h->right;
    h->right = x->left;
    if (x->left) x->left->parent = h;
    x->left = h;
    x->color = h->color;
    h->color = RED;
    x->parent = h->parent;
    h->parent = x;
    return x;
}

BinarySearchTree::Node* BinarySearchTree::rotate_right(Node *h) {
    Node *x = h->left;
    h->left = x->right;
    if (x->right) x->right->parent = h;
    x->right = h;
    x->color = h->color;
    h->color = RED;
    x->parent = h->parent;
    h->parent = x;
    return x;
}

void BinarySearchTree::flip_colors(Node *h) {
    h->color = RED;
    if (h->left) h->left->color = BLACK;
    if (h->right) h->right->color = BLACK;
}

BinarySearchTree::Node* BinarySearchTree::move_red_left(Node *h) {
    flip_colors(h);
    if (isRed(h->right->left)) {
        h->right = rotate_right(h->right);
        h = rotate_left(h);
        flip_colors(h);
    }
    return h;
}

BinarySearchTree::Node* BinarySearchTree::move_red_right(Node *h) {
    flip_colors(h);
    if (isRed(h->left->left)) {
        h = rotate_right(h);
        flip_colors(h);
    }
    return h;
}

BinarySearchTree::Node* BinarySearchTree::fix_up(Node *h) {
    if (isRed(h->right)) h = rotate_left(h);
    if (isRed(h->left) && isRed(h->left->left)) h = rotate_right(h);
    if (isRed(h->left) && isRed(h->right)) flip_colors(h);
    return h;
}

BinarySearchTree::Node* BinarySearchTree::min_node(Node *h) {
    while (h->left) h = h->left;
    return h;
}

void BinarySearchTree::insert(const Key& key, const Value& value) {
    _root = insert_rb(_root, key, value, nullptr);
    _root->color = BLACK;
    _size++;
}

BinarySearchTree::Node* BinarySearchTree::insert_rb(Node* h, const Key& key, const Value& value, Node *parent) {
    if (!h) {
        Node *new_node = new Node(key, value, parent);
        return new_node;
    }

    if (key < h->keyValuePair.first) {
        h->left = insert_rb(h->left, key, value, h);
    } else if (key > h->keyValuePair.first) {
        h->right = insert_rb(h->right, key, value, h);
    } else {
        h->keyValuePair.second = value;
    }
    if (isRed(h->right) && !isRed(h->left)) h = rotate_left(h);
    if (isRed(h->left) && isRed(h->left->left)) h = rotate_right(h);
    if (isRed(h->left) && isRed(h->right)) flip_colors(h);
    return h;
}

void BinarySearchTree::erase(const Key& key) {
    if (!_root) return;
    _root = erase_rb(_root, key);
    if (_root) _root->color = BLACK;
    _size--;
}

BinarySearchTree::Node* BinarySearchTree::erase_rb(Node *h, const Key &key) {
        if (key < h->keyValuePair.first) {
            if (!isRed(h->left) && !isRed(h->left ? h->left->left : nullptr))
                h = move_red_left(h);
            h->left = erase_rb(h->left, key);
        } else {
            if (isRed(h->left))
                h = rotate_right(h);
            if (key == h->keyValuePair.first && !h->right) {
                delete h;
                return nullptr;
            }
            if (!isRed(h->right) && !isRed(h->right ? h->right->left : nullptr))
                h = move_red_right(h);
            if (key == h->keyValuePair.first) {
                Node *min = min_node(h->right);
                h->keyValuePair = min->keyValuePair;
                h->right = erase_rb(h->right, min->keyValuePair.first);
            } else {
                h->right = erase_rb(h->right, key);
            }
        }
        return fix_up(h);
    }

BinarySearchTree::BinarySearchTree(const BinarySearchTree& other) {
    if (other._root) {
        _root = new Node(*other._root);
        _size = other._size;
    }
}

BinarySearchTree& BinarySearchTree::operator=(const BinarySearchTree& other) {
    if (this != &other) {
        BinarySearchTree temp(other);
        std::swap(_root, temp._root);
        std::swap(_size, temp._size);
    }
    return *this;
}

BinarySearchTree::BinarySearchTree(BinarySearchTree&& other) noexcept : _root(other._root), _size(other._size) {
    other._root = nullptr;
    other._size = 0;
}

BinarySearchTree& BinarySearchTree::operator=(BinarySearchTree&& other) noexcept {
    if (this != &other) {
        delete _root;
        _root = other._root;
        _size = other._size;
        other._root = nullptr;
        other._size = 0;
    }
    return *this;
}

void BinarySearchTree::delete_subtree(Node* node) {
    if (!node) return;
    delete_subtree(node->left);
    delete_subtree(node->right);
    delete node;
}

BinarySearchTree::~BinarySearchTree() {
    delete_subtree(_root);
}

BinarySearchTree::Iterator::Iterator(Node* node) : _node(node) {}

std::pair<Key, Value>& BinarySearchTree::Iterator::operator*() {
    return _node->keyValuePair;
}

const std::pair<Key, Value>& BinarySearchTree::Iterator::operator*() const {
    return _node->keyValuePair;
}

std::pair<Key, Value>* BinarySearchTree::Iterator::operator->() {
    return &_node->keyValuePair;
}

const std::pair<Key, Value>* BinarySearchTree::Iterator::operator->() const {
    return &_node->keyValuePair;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++() {
    if (_node->right) {
        _node = _node->right;
        while (_node->left) _node = _node->left;
    } else {
        Node* parent = _node->parent;
        while (parent && _node == parent->right) {
            _node = parent;
            parent = parent->parent;
        }
        _node = parent;
    }
    return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--() {
    if (_node->left) {
        _node = _node->left;
        while (_node->right) _node = _node->right;
    } else {
        Node* parent = _node->parent;
        while (parent && _node == parent->left) {
            _node = parent;
            parent = parent->parent;
        }
        _node = parent;
    }
    return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

bool BinarySearchTree::Iterator::operator==(const Iterator& other) const {
    return _node == other._node;
}

bool BinarySearchTree::Iterator::operator!=(const Iterator& other) const {
    return _node != other._node;
}

BinarySearchTree::ConstIterator::ConstIterator(const Node* node) : _node(node) {}

const std::pair<Key, Value>& BinarySearchTree::ConstIterator::operator*() const {
    return _node->keyValuePair;
}

const std::pair<Key, Value>* BinarySearchTree::ConstIterator::operator->() const {
    return &_node->keyValuePair;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++() {
    if (_node->right) {
        _node = _node->right;
        while (_node->left) _node = _node->left;
    } else {
        const Node* parent = _node->parent;
        while (parent && _node == parent->right) {
            _node = parent;
            parent = parent->parent;
        }
        _node = parent;
    }
    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++(int) {
    ConstIterator temp = *this;
    ++(*this);
    return temp;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--() {
    if (_node->left) {
        _node = _node->left;
        while (_node->right) _node = _node->right;
    } else {
        const Node* parent = _node->parent;
        while (parent && _node == parent->left) {
            _node = parent;
            parent = parent->parent;
        }
        _node = parent;
    }
    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--(int) {
    ConstIterator temp = *this;
    --(*this);
    return temp;
}

bool BinarySearchTree::ConstIterator::operator==(const ConstIterator& other) const {
    return _node == other._node;
}

bool BinarySearchTree::ConstIterator::operator!=(const ConstIterator& other) const {
    return _node != other._node;
}

// LLRB Tree implementation !!!!!!!!!
void BinarySearchTree::insert_iterator(const Key& key, const Value& value) {
    if (!_root) {
        _root = new Node(key, value);
        _size = 1;
        return;
    }

    Node* current = _root;
    Node* parent = nullptr;
    
    while (current) {
        parent = current;
        if (key < current->keyValuePair.first) {
            current = current->left;
        } else if (key > current->keyValuePair.first) {
            current = current->right;
        } else {
            // Key already exists, update value
            current->keyValuePair.second = value;
            return;
        }
    }
    
    if (key < parent->keyValuePair.first) {
        parent->left = new Node(key, value, parent);
    } else {
        parent->right = new Node(key, value, parent);
    }
    _size++;
}

void BinarySearchTree::erase_iterator(const Key& key) {
    Node* node = _root;
    Node* parent = nullptr;
    
    // Find the node to delete
    while (node && node->keyValuePair.first != key) {
        parent = node;
        if (key < node->keyValuePair.first) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    
    if (!node) return; // Key not found
    
    _size--;
    
    // Case 1: Node has two children
    if (node->left && node->right) {
        // Find inorder successor (leftmost in right subtree)
        Node* successor = node->right;
        while (successor->left) {
            successor = successor->left;
        }
        
        // Copy successor's data to node
        node->keyValuePair = successor->keyValuePair;
        
        // Now delete the successor (which has at most one child)
        node = successor;
        parent = node->parent;
    }
    
    // Case 2: Node has one child or no children
    Node* child = node->left ? node->left : node->right;
    
    if (child) {
        child->parent = parent;
    }
    
    if (!parent) {
        _root = child;
    } else if (node == parent->left) {
        parent->left = child;
    } else {
        parent->right = child;
    }
    
    delete node;
}

BinarySearchTree::ConstIterator BinarySearchTree::find(const Key& key) const {
    Node* current = _root;
    while (current) {
        if (key < current->keyValuePair.first) {
            current = current->left;
        } else if (key > current->keyValuePair.first) {
            current = current->right;
        } else {
            return ConstIterator(current);
        }
    }
    return cend();
}

BinarySearchTree::Iterator BinarySearchTree::find(const Key& key) {
    Node* current = _root;
    while (current) {
        if (key < current->keyValuePair.first) {
            current = current->left;
        } else if (key > current->keyValuePair.first) {
            current = current->right;
        } else {
            return Iterator(current);
        }
    }
    return end();
}

std::pair<BinarySearchTree::Iterator, BinarySearchTree::Iterator> BinarySearchTree::equalRange(const Key& key) {
    Iterator first = find(key);
    if (first == end()) {
        return std::make_pair(first, first);
    }
    
    Iterator last = first;
    ++last;
    while (last != end() && last->first == key) {
        ++last;
    }
    
    return std::make_pair(first, last);
}

std::pair<BinarySearchTree::ConstIterator, BinarySearchTree::ConstIterator> BinarySearchTree::equalRange(const Key& key) const {
    ConstIterator first = find(key);
    if (first == cend()) {
        return std::make_pair(first, first);
    }
    
    ConstIterator last = first;
    ++last;
    while (last != cend() && last->first == key) {
        ++last;
    }
    
    return std::make_pair(first, last);
}

BinarySearchTree::ConstIterator BinarySearchTree::min() const {
    if (!_root) return cend();
    
    const Node* current = _root;
    while (current->left) {
        current = current->left;
    }
    return ConstIterator(current);
}

BinarySearchTree::ConstIterator BinarySearchTree::max() const {
    if (!_root) return cend();
    
    const Node* current = _root;
    while (current->right) {
        current = current->right;
    }
    return ConstIterator(current);
}

BinarySearchTree::ConstIterator BinarySearchTree::min(const Key& key) const {
    ConstIterator it = find(key);
    if (it == cend()) return cend();
    
    ConstIterator result = it;
    while (it != cend() && it->first == key) {
        if (it->second < result->second) {
            result = it;
        }
        ++it;
    }
    return result;
}

BinarySearchTree::ConstIterator BinarySearchTree::max(const Key& key) const {
    ConstIterator it = find(key);
    if (it == cend()) return cend();
    
    ConstIterator result = it;
    while (it != cend() && it->first == key) {
        if (it->second > result->second) {
            result = it;
        }
        ++it;
    }
    return result;
}

BinarySearchTree::Iterator BinarySearchTree::begin() {
    if (!_root) return end();
    
    Node* current = _root;
    while (current->left) {
        current = current->left;
    }
    return Iterator(current);
}

BinarySearchTree::Iterator BinarySearchTree::end() {
    return Iterator(nullptr);
}

BinarySearchTree::ConstIterator BinarySearchTree::cbegin() const {
    if (!_root) return cend();
    
    const Node* current = _root;
    while (current->left) {
        current = current->left;
    }
    return ConstIterator(current);
}

BinarySearchTree::ConstIterator BinarySearchTree::cend() const {
    return ConstIterator(nullptr);
}

size_t BinarySearchTree::size() const {
    return _size;
}

void BinarySearchTree::output_tree() {
    if (_root) {
        _root->output_node_tree();
    }
    std::cout << std::endl;
}
