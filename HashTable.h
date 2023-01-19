//
// Created by navin on 2022-12-01.
//
#ifndef CODE_HASHTABLE_H
#define CODE_HASHTABLE_H
#include <vector>
#include <math.h>
#include <sstream>

#define MAX_LOAD 0.70


struct HashEntry {
    std::string key;
    std::vector<int> value;
    bool isActive;
    HashEntry() : key(""),value({}), isActive(false){}
};


class HashTable {
private:
    void expand() {
        rehash(static_cast<int>(array.size() * 2 ));
    }
    void rehash(int newSize) {
        std::vector<HashEntry> oldArray = array;
        // Create new double-sized, empty table
        array.resize(newSize);
        for (auto &entry: array)
            entry.isActive = false;

        // Copy table over
        currentSize = 0;
        for (auto &entry: oldArray)
            if (entry.isActive)
                re_insert(entry);

    }

    void re_insert(HashEntry entry){
        int i = 0;
        int key = elfhash(entry.key) % capacity();
        while (isActive(key)){
            i++;
            key = (key + (i*i + i) / 2) % capacity();
        }
        array[key] = entry;
        currentSize++;
    }

    bool isActive(int currentPos) const { return currentPos != -1 && array[currentPos].isActive; }
    std::vector<HashEntry> array;
    int currentSize;

public:
    explicit HashTable(int size = 1024) : array(size) {
        currentSize = 0;
        makeEmpty();
    }
    void makeEmpty() {
        currentSize = 0;
        for (auto &entry: array)
            //Sets each entry as false during initialization
            entry.isActive = false;
    }

    int capacity() const {
        //Returns current size of array
        return array.size();
    }

    std::string print(){
        std::ostringstream os;
        os << "Format of display is\n";
        os << "Slot number: data record\n";
        os << "Current table size is " << array.size() << "\n";
        os << "Number of elements in table is " << this->currentSize << "\n";
        os << "\n";
        for (int key = 0;key<capacity(); key++) {
            if (array[key].isActive){
                if (array[key].value.size() == 1){
                    os << key << ": [" << array[key].key<< "], [" << array[key].value[0] <<"]]\n";
                } else{
                    os << key << ": [" << array[key].key<< "], [";
                    for (int k = 0; k < array[key].value.size();k++){
                        os << array[key].value[k] << ", ";
                    }
                    os <<"]]\n";
                }
            }
        }
        os << std::endl;
        return os.str();
    }

    int elfhash(std::string &s) {
        //Elfhash
        const int addr = 0xF0000000;
        int hashVal = 0;
        unsigned int x = 0;
        unsigned int i = 0;
        int stringLength = s.length();
        for (i = 0; i < stringLength; i++)
        {
            hashVal = (hashVal << 4) + (s[i]);
            if ((x = hashVal & addr) != 0){
                hashVal ^= (x >> 24);
            }
            hashVal &= ~x;
        }
        return hashVal;
    }

    std::vector<int> search(std::string s) {
        //Searches for element in table
        int i = 0;
        int key = elfhash(s) % capacity();
        while (isActive(key)) {
            if (array[key].key == s) {
                return array[key].value;
            }
            i++;
            //Quadratic probing
            key = (key + ((i * i + i) / 2)) % capacity();
        }
        return {};
    }

    int insert(std::string s, int l) {
        //Inserts into table
        int i =0;
        int probe;
        int limit =100;
        if (currentSize >= capacity() * MAX_LOAD) expand();
        //Calculate hash val
        int key = elfhash(s) % capacity();
        probe = 0;
        while (true) {
            if (!isActive(key)) {
                //Inserts if slot available
                array[key].key = s;
                array[key].value.push_back(l);
                array[key].isActive = true;

                currentSize++;
                return probe;
            }
            else if (array[key].key==s){
                array[key].value.push_back(l);
                return probe;
            }
            //Count prob sequence
            probe++;
            i++;
            //Quadratic Probing
            key = (key + (i*i + i) / 2) % capacity();
        }
    }
};
#endif //CODE_HASHTABLE_H