//
// Created by Navin on 2022-11-30.
//
#ifndef CODE_BUFFERPOOL_H
#define CODE_BUFFERPOOL_H
#include <iostream>
#include <vector>
#include <algorithm>
#include "GISRecord.h"


class BufferPool
{
private:
    //Holds a record and its offset
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
    std::string filter_search(std::string,char* database,std::string filter);
};

// BufferPool constructor
BufferPool::BufferPool() = default;

int BufferPool::getMaxSize() {
    //Returns max amount of records the pool can hold
    return MAX_SIZE;

}

void BufferPool::addElement(std::string line, std::string offset) {
    if (pool.size() == getMaxSize()) {
        //If pool is full, remove LRU element(least recently used)
        pool.erase(pool.begin());

    }
    //Inserts record
    poolItem x;
    x.record = line;
    x.offset = offset;
    pool.push_back(x);
}

std::string BufferPool::print() {
    //Prints content of pool for debug command
    std::ostringstream os;
    if (pool.size() <=0) {
        os << "EMPTY POOL\n";
        os << std::endl;
        return os.str();
    }
    os << "MRU\n";
    for (int i = pool.size()-1; i >= 0; i--) {
        os <<"    "<<pool[i].offset << ": "<<pool[i].record << "\n";
    }
    os << "LRU\n";
    os << std::endl;
    return os.str();
}

std::string BufferPool::search(std::string offset,char* database) {
    //Checks if record exists in pool, if so, remove and reinsert as MRU
    for (int i = 0; i < pool.size(); i++) {
        if (pool[i].offset == offset) {
            pool.push_back(pool[i]);
            pool.erase(pool.begin()+i);
            return pool[pool.size()-1].record;
        }
    }
    //Record doesnt exist, retrieve from database file
    int x = stoi(offset);
    std::string line;
    std::ifstream myfile(database);
    for (int i = 0; getline (myfile,line) && i < (x+1); i++) {
        if (i == x) {
            addElement(line, offset);
            return line;
        }
    }
}

std::string BufferPool::filter_search(std::string offset,char* database, std::string filter) {
    //Searches for record based on filter
    for (int i = 0; i < pool.size(); i++) {
        if (pool[i].offset == offset) {
            std::vector<std::string> record = GISRecord::convert(pool[i].record);
            if (GISRecord::check_filter(record[2],filter)==1){
                pool.push_back(pool[i]);
                pool.erase(pool.begin()+i);
                return pool[pool.size()-1].record;
            }
            return "";
        }
    }
    //File not in pool, retrieves from database if filter matches
    int x = stoi(offset);
    std::string line;
    std::ifstream myfile(database);
    for (int i = 0; getline (myfile,line) && i < (x+1); i++) {
        if (i == x) {
            std::vector<std::string> record = GISRecord::convert(line);
            if (GISRecord::check_filter(record[2],filter)==1){
                addElement(line, offset);
                return line;
            }
            return "";
        }
    }
}

#endif //CODE_BUFFERPOOL_H