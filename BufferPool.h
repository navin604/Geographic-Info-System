//
// Created by Navin on 2022-11-30.
//
#ifndef CODE_BUFFERPOOL_H
#define CODE_BUFFERPOOL_H
#include <iostream>
#include <vector>
#include "GISRecord.h"


class BufferPool
{
private:
    struct poolItem {
        std::string offset;
        std::string record;
    };
    int MAX_SIZE = 15;
    std::vector<poolItem> pool;

public:
    BufferPool();
    int getMaxSize();
    void addElement(std::string line, std::string offset);
    std::string print();
    std::string search(std::string,char* database);



};

// BufferPool constructor
BufferPool::BufferPool() = default;

int BufferPool::getMaxSize() {
    return MAX_SIZE;

}

void BufferPool::addElement(std::string line, std::string offset) {
    if (pool.size() == getMaxSize()) {
        pool.erase(pool.begin());

    }
    poolItem x;
    x.record = line;
    x.offset = offset;
    pool.push_back(x);
}


std::string BufferPool::print() {
    std::ostringstream os;
    os << "MRU\n";
    for (int i = pool.size()-1; i >= 0; i--) {
        os <<"    "<<pool[i].offset << ": "<<pool[i].record << "\n";

    }
    os << "LRU\n";
    os << std::endl;
    return os.str();

}



std::string BufferPool::search(std::string offset,char* database) {
    for (int i = 0; i < pool.size(); i++) {
        for (int i = pool.size()-1; i >= 0; i--) {
            if (pool[i].offset == offset) {
                poolItem temp = pool[i];
                pool.erase(pool.begin()+i);
                addElement(temp.record,temp.offset);
                return temp.record;
            }
        }
    }
    int x = stoi(offset);
    std::string line;
    std::ifstream myfile(database);
    for (int i = 0; getline (myfile,line) && i < (x+1); i++)
        if (i == x){
            addElement(line,offset);
            return line;
        }




}

#endif //CODE_BUFFERPOOL_H
