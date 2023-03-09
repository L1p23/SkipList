#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <mutex>
#include <fstream>
#include <type_traits>

template<typename K, typename V>
struct slNode {
    slNode() {}
    slNode(K k, V v, int l);
    ~slNode();

    K key;
    V value;
    int level;
    slNode<K, V> **forward;
};

template<typename K, typename V>
slNode<K, V>::slNode(K k, V v, int l) {
    key = k;
    value = v;
    level = l;
    forward = new slNode<K, V>* [level];
    memset(forward, 0, sizeof(slNode<K, V>*) * level);
}

template<typename K, typename V>
slNode<K, V>::~slNode() {
    delete []forward;
}

template<typename K, typename V, typename Compare = std::less<K>>
class skiplist {
public:
    skiplist(int maxLevel = 32, float sl_P = 0.25);
    ~skiplist();
    bool insert(K k, V v);
    bool erase(K k);
    bool search(K k, V &v);
    bool update(K k, V v);
    void display();
    unsigned int size();
    void dump_file(std::string filename);

private:
    int _randomLevel();
    slNode<K, V>* _createNode(K k, V v, int level);
    slNode<K, V>* _search(K k);
    bool _getKeyValueFromString(std::string s, std::string &key, std::string &value);

private:
    Compare _comp;
    std::mutex _mtx;
    int _maxLevel;
    float _sl_p;
    slNode<K, V> *_head;
    unsigned int _length;
    int _level;
    std::ofstream _dumpFile;
};

// Generate a random level for node
template<typename K, typename V, typename Compare>
int skiplist<K, V, Compare>::_randomLevel() {
    static const int threshold = this->_sl_p * RAND_MAX;
    int level = 1;
    while (rand() < threshold)
        level++;
    return level < this->_maxLevel ? level : this->_maxLevel;
}

// Create a new node
template<typename K, typename V, typename Compare>
slNode<K, V>* skiplist<K, V, Compare>::_createNode(K k, V v, int level) {
    slNode<K, V>* node = new slNode<K, V>(k, v, level);
    return node;
}

// Search key in skip list
// return pointer of the node if key exists, otherwise return nullptr
template<typename K, typename V, typename Compare>
slNode<K, V>* skiplist<K, V, Compare>::_search(K k) {
    this->_mtx.lock();
    slNode<K, V>* cur = this->_head;
    for (int i = this->_level - 1; i >= 0; i--) {
        while (cur->forward[i] != nullptr && this->_comp(cur->forward[i]->key, k))
            cur = cur->forward[i];
    }
    cur = cur->forward[0];
    if (cur != nullptr && cur->key == k)
        return cur;
    this->_mtx.unlock();
    return nullptr;
}

template<typename K, typename V, typename Compare>
skiplist<K, V, Compare>::skiplist(int maxLevel, float sl_P) {
    this->_maxLevel = maxLevel;
    this->_sl_p = sl_P;
    K k;
    V v;
    this->_head = new slNode<K, V>(k, v, this->_maxLevel);
    this->_length = 0;
    this->_level = 1;
}

template<typename K, typename V, typename Compare>
skiplist<K, V, Compare>::~skiplist() {
    slNode<K, V>* del = nullptr;
    for (slNode<K, V>* cur = this->_head; cur != nullptr; cur = del) {
        del = cur->forward[0];
        delete cur;
    }
    if (this->_dumpFile.is_open())
        this->_dumpFile.close();
}

// Insert key-value into skip list
// return true if insert successfully
// return false if key exists
template<typename K, typename V, typename Compare>
bool skiplist<K, V, Compare>::insert(K k, V v) {
    this->_mtx.lock();
    slNode<K, V>* cur = this->_head;
    slNode<K, V>* update[this->_maxLevel];
    memset(update, 0, sizeof(slNode<K, V>*) * this->_maxLevel);
    for (int i = this->_level - 1; i >= 0; i--) {
        while (cur->forward[i] != nullptr && this->_comp(cur->forward[i]->key, k))
            cur = cur->forward[i];
        update[i] = cur;
    }
    cur = cur->forward[0];
    if (cur != nullptr && cur->key == k) {
        this->_mtx.unlock();
        return false;
    }
    int random_level = this->_randomLevel();
    if (random_level > this->_level) {
        for (int i = random_level - 1; i >= this->_level; i--)
            update[i] = this->_head;
        this->_level = random_level;
    }
    slNode<K, V>* newNode = new slNode<K, V>(k, v, random_level);
    for (int i = 0; i < random_level; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
    this->_length++;
    this->_mtx.unlock();
    return true;
}

// Delete key-value from skip list
// return true if delete successfully
// return false if key does not exist
template<typename K, typename V, typename Compare>
bool skiplist<K, V, Compare>::erase(K k) {
    this->_mtx.lock();
    slNode<K, V>* cur = this->_head;
    slNode<K, V>* del = nullptr;
    for (int i = this->_level - 1; i >= 0; i--) {
        while (cur->forward[i] != nullptr && this->_comp(cur->forward[i]->key, k))
            cur = cur->forward[i];
        if (cur->forward[i] != nullptr && cur->forward[i]->key == k) {
            del = cur->forward[i];
            cur->forward[i] = del->forward[i];
        }
    }
    if (del == nullptr)
        return false;
    this->_length--;
    delete del;
    this->_mtx.unlock();
    return true;
}

// Search key in skip list
// return true if key exists and save value in v
// return false if key does not exist
template<typename K, typename V, typename Compare>
bool skiplist<K, V, Compare>::search(K k, V &v) {
    slNode<K, V>* node = this->_search(k);
    if (node == nullptr)
        return false;
    v = node->value;
    this->_mtx.unlock();
    return true;
}

// Update value of given key
// return true if update successfully
// return false if key does not exists
template<typename K, typename V, typename Compare>
bool skiplist<K, V, Compare>::update(K k, V v) {
    slNode<K, V>* node = this->_search(k);
    if (node == nullptr)
        return false;
    node->value = v;
    this->_mtx.unlock();
    return true;
}

// Display skip list
template<typename K, typename V, typename Compare>
void skiplist<K, V, Compare>::display() {
    this->_mtx.lock();
    for (int i = this->_level - 1; i >= 0; i--) {
        std::cout<<"level "<<i<<":\t";
        for (slNode<K, V>* cur = this->_head->forward[i]; cur != nullptr; cur = cur->forward[i])
            std::cout<<cur->key<<": "<<cur->value<<";\t";
        std::cout<<std::endl;
    }
    this->_mtx.unlock();
}

// Return length of skip list (level 0)
template<typename K, typename V, typename Compare>
unsigned int skiplist<K, V, Compare>::size() {
    return this->_length;
}

// Get key and value from string
// return true if string is valid, otherwise return false
template<typename K, typename V, typename Compare>
bool skiplist<K, V, Compare>::_getKeyValueFromString(std::string s, std::string &key, std::string &value) {
    if (s.empty())
        return false;
    int idx = s.find(':');
    if (idx <= 0 || idx == s.length() - 1)
        return false;
    key = s.substr(0, idx);
    value = s.substr(idx + 1, s.length() - idx);
    return true;
}

// Dump data to file
template<typename K, typename V, typename Compare>
void skiplist<K, V, Compare>::dump_file(std::string filename) {
    this->_mtx.lock();
    this->_dumpFile.open(filename);
    for (slNode<K, V>* cur = this->_head->forward[0]; cur != nullptr; cur = cur->forward[0])
        this->_dumpFile<<cur->key<<':'<<cur->value<<'\n';
    this->_dumpFile.flush();
    this->_dumpFile.close();
    this->_mtx.unlock();
}

#endif