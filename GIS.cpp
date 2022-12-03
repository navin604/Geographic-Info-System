
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "BufferPool.h"
#include "HashTable.h"

struct DMS {};

struct {int westLong;
        int eastLong;
        int southLat;
        int northLat;
}world;

class GISRecord {
public:
    static int validate_record() {
        return 1;

    }
    static void asses_record(std::string s,HashTable<std::string> &table,int i){
        std::vector<std::string> l;
        std::string temp = "";
        for (auto x: s){
            if (x=='|') {
                l.push_back(temp);
                temp = "";
            }
            else {
                temp += x;

            }
        }
        temp = l[1] +":"+ l[3];
        //std::cout << temp <<" "<< i<< "\n";
        table.insert(temp,i);

    }
};

class CommandProcessor {
public:
    static void process_cmd(char* filename, char* database,HashTable<std::string> &table) {
        std::vector<std::string> vect;
        std::cout << filename << " process\n";
        std::fstream cmd_file;

        std::cout << "Attempting to open: " << filename << "\n";
        cmd_file.open(filename, std::ios::in);
        if (cmd_file.is_open()) {
            std::cout << "File open, now reading\n";
            std::string tp;
            std::cout << "---------------------------------------------------------\n";
            while(getline(cmd_file,tp)) {
                if (tp[0] == ';'){
                    continue;
                }

                std::cout << tp << "\n";
                ex(tp, vect);
//                for (std::string i: vect)
//                    std::cout << i << ' ';
//                std::cout<<"\n";
                std::cout << "---------------------------------------------------------\n";
                if (vect[0] == "import") {
                    import(vect[1],database,table);

                }
                else if(vect[0] == "world") {
                    world(vect);

                }
                //else if() {
//
//                } else if() {
//
//                } else if() {
//
//                } else if() {
//
//                } else if() {
//
//                }
                vect.clear();

            }
            cmd_file.close();
        }
    }
    static void import(std::string file,char* database,HashTable<std::string> &table) {
        std::cout << "filename to read " << file << "||| database now" << database << "\n";
        int ignoreLine = 0;
        std::fstream input;
        std::fstream output;
        input.open(file, std::ios::in);
        output.open(database, std::ios::out);
        if (input.is_open() and output.is_open()) {
           std::string tp;
           std::cout << "Both Files are open\n";
           while (getline(input,tp)) {
               if (ignoreLine == 0) {
                   ignoreLine++;
                   continue;
               }
               if (GISRecord::validate_record() == 1) {
                   output << tp << "\n";
                   GISRecord::asses_record(tp,table,ignoreLine);
                   ignoreLine++;

               }

           }
        } else std::cout << "Files did not open";

        input.close();
        output.close();
        std::cout << "Finished Reading\n";
        table.print();
        exit(1);

    }
    static void world(std::vector<std::string> &vec) {

        int total = 0;
        std::string deg = vec[1].substr(0,3);
        std::string min = vec[1].substr(3,2);
        std::string sec = vec[1].substr(5,2);
        total = (stoi(deg)*3600) + (stoi(min)*60) +stoi(sec) ;
        if (vec[1][7] == 'W') total = total*-1;
        ::world.westLong = total;

        deg = vec[2].substr(0,3);
        min = vec[2].substr(3,2);
        sec = vec[2].substr(5,2);
        total = (stoi(deg)*3600) + (stoi(min)*60) +stoi(sec) ;
        if (vec[2][7] == 'W') total = total*-1;
        ::world.eastLong = total;

        deg = vec[3].substr(0,2);
        min = vec[3].substr(2,2);
        sec = vec[3].substr(4,2);
        total = (stoi(deg)*3600) + (stoi(min)*60) +stoi(sec) ;
        if (vec[3][6] == 'S') total = total*-1;
        ::world.southLat = total;

        deg = vec[4].substr(0,2);
        min = vec[4].substr(2,2);
        sec = vec[4].substr(4,2);
        total = (stoi(deg)*3600) + (stoi(min)*60) +stoi(sec) ;
        if (vec[4][6] == 'S') total = total*-1;
        ::world.northLat = total;



    }






    static void ex(std::string s,std::vector<std::string> &vec) {
        std::stringstream ss(s);
        std::string word;
        while (ss >> word) {
            vec.push_back(word);
        }
    }
};

class Logger {
public:
    static void log_init(char* argv[]) {
        std::fstream logfile;
        logfile.open(argv[3]);
        logfile << "I am Navin Parmar\n";
        logfile << "My student number is: A01044425\n";
        logfile << "Input files are: <" << argv[1] << "> <"<< argv[2] << "> <"<<  argv[3] <<">\n";
        logfile.close();
    }
    static void add_cmd() {

    }
};

class SystemManager {
public:
    static void validate_args(int argc, char *argv[]) {
        // ./GIS <database file> <command script> <log file>
        if (argc!=4) {
            std::cout << "Error: Program should be invoked as ->"
                         " ./GIS <database file> <command script> <log file>\n";
            exit(0);
        }
        file_setup(argv[1]);
        file_setup(argv[3]);
        Logger::log_init(argv);
    }
    static void file_setup(char* filename) {
        // Deletes and creates database & log files
        if (remove(filename)!=0) {
            std::cout << filename << " does not exist, now creating..." << "\n";
        } else {
            std::cout << filename << " exists, has been deleted\n";
        }

        std::fstream file;
        file.open(filename, std::ios::out);
        if (!file) {
            std:: cout<<"Error in creating file!!!\n Now exiting!\n";
            std::exit(0);
        }
        std::cout << filename << " has been created\n";


    }

};

int main(int argc, char *argv[]) {
//    HashTable<std::string> table;
//    std::cout << table.elfhash("Little Doe Hill VA");
//    BufferPool pool;
//    pool.addElement("5");
//    pool.addElement("4");
//    pool.addElement("3");
//    pool.addElement("2");
//    pool.addElement("1");
//    pool.addElement("0");
//    std::cout << pool.output();

    HashTable<std::string> table;
    SystemManager::validate_args(argc,argv);
    CommandProcessor::process_cmd(argv[2], argv[1],table);
    std::cout << "back in main";
    return 0;
}

