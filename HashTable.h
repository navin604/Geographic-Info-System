//
// Created by navin on 2022-12-01.
//
#ifndef CODE_HASHTABLE_H
#define CODE_HASHTABLE_H
#include <vector>
#include <math.h>
#include <sstream>

#define MAX_LOAD 0.70

template<typename T>
class HashTable
{
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
        //Prints details for debug command
        std::ostringstream os;
        os << "Format of display is\n";
        os << "Slot number: data record\n";
        os << "Current table size is " << array.size() << "\n";
        os << "Number of elements in table is " << this->currentSize << "\n";
        os << "\n";
        for (int key = 0;key<capacity(); key++) {
            if (array[key].isActive)
                os << key << ": [" << array[key].key<< "], [" << array[key].value <<"]]\n";
        }
        os << std::endl;
        return os.str();
    }

    int search(T s) {
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
        return -1;
    }

    int insert(T s, int l) {
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
                array[key] = std::move(HashEntry{std::move(s), std::move(l), true});
                currentSize++;
                return probe;
            }
            //Count prob sequence
            probe++;
            i++;
            //Quadratic Probing
            key = (key + (i*i + i) / 2) % capacity();

        }
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

private:
    struct HashEntry {
        T key;
        int value;
        bool isActive;
        HashEntry(const T &e = T(), int j = -1, bool a = false): key{e}, value{j}, isActive{a}{}
        HashEntry(T &&e, int &&j, bool a = false): key{std::move(e)}, value{std::move(j)}, isActive{a} {}
    };
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
                insert(std::move(entry.key),std::move(entry.value));
    }

    bool isActive(int currentPos) const { return currentPos != -1 && array[currentPos].isActive; }
    std::vector<HashEntry> array;
    int currentSize;
};

#endif //CODE_HASHTABLE_H
