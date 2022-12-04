//
// Created by Navin on 2022-11-30.
//
#ifndef CODE_BUFFERPOOL_H
#define CODE_BUFFERPOOL_H
#include <iostream>
#include <vector>


class BufferPool
{
private:
    int MAX_SIZE = 15;
    std::vector<std::string> pool;

public:
    BufferPool();
    int getMaxSize();
    void addElement(std::string);
    std::string print();


};

// BufferPool constructor
BufferPool::BufferPool() = default;

int BufferPool::getMaxSize() {
    return MAX_SIZE;

};

void BufferPool::addElement(std::string x) {
    if (pool.size() == getMaxSize()) {
        pool.erase(pool.begin());

    }

    pool.push_back(x);
}


std::string BufferPool::print() {
    std::ostringstream os;
    os << "MRU\n";
    for (int i = pool.size()-1; i >= 0; i--) {
        os <<"    "<<pool[i] << "\n";

    }
    os << "LRU\n";
    os << std::endl;
    return os.str();

}

#endif //CODE_BUFFERPOOL_H
