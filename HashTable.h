//
// Created by navin on 2022-12-01.
//
#ifndef CODE_HASHTABLE_H
#define CODE_HASHTABLE_H
#include <vector>
#include <math.h>

#define MAX_LOAD 0.70

template<typename T>
class HashTable
{
public:
    explicit HashTable(int size = 1024) : array(size) {
        currentSize = 0;
        makeEmpty();
    }
    bool isPrime(int n) {
        if (n == 2 || n == 3)
            return true;

        if (n == 1 || n % 2 == 0)
            return false;

        for (int i = 3; i * i <= n; i += 2)
            if (n % i == 0)
                return false;

        return true;
    }

/**
 * Internal method to return a prime number at least as large as n.
 * Assumes n > 0.
 */
    int nextPrime(int n) {
        if (n % 2 == 0)
            ++n;

        for (; !isPrime(n); n += 2);

        return n;
    }

    void makeEmpty() {
        currentSize = 0;
        for (auto &entry: array)
            entry.isActive = false;
    }


    int capacity() const {
        return array.size();
    }




    void print(){

        std::cout << "Contents of hash table:\n" << std::endl;
        std::cout << "Total elements " << this->currentSize << "\n";
        for (int key = 0;key<capacity(); key++) {
            if (array[key].isActive)
                std::cout << key << " " << array[key].key<< " " << array[key].value <<"\n";

        }





    }

    bool insert(T s, int l) {

        if (currentSize >= capacity() * MAX_LOAD) expand();
        //Calculate hash val

        int key = elfhash(s) % capacity();
        if (!isActive(key)){

            array[key] = std::move(HashEntry{std::move(s),std::move(l),true});

            currentSize++;


            return true;
        } else {


                // If there is a collision
                // iterating through all
                // possible quadratic values
                for (int j = 0; j < array.capacity(); j++) {
                    // Computing the new hash value
                    int key = ((pow(j,2)+j)/2);

                    if (key>capacity()) key = key%capacity();
                    if (!isActive(key)) {

                        array[key] = std::move(HashEntry{std::move(s),std::move(l),true});
                        currentSize++;


                        return true;
                    }
                }

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
            if ((x = hashVal & addr) != 0)
            {
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
