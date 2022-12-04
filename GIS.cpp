#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "BufferPool.h"
#include "HashTable.h"
#include "GISRecord.h"
#include "QuadTree.h"
#include <ctime>
#include <chrono>

struct DMS {};

struct {int westLong;
        int eastLong;
        int southLat;
        int northLat;
}world;
int avg = 0;


class CommandProcessor {

public:
    static int validate_record(std::string s){
        std::vector<std::string> record = GISRecord::convert(s);
        avg += record[1].size();
        int lon = GISRecord::longitude_convert(record[8]);
        int lat = GISRecord::latitude_convert(record[7]);

        if ((lon > ::world.westLong) && (lon < ::world.eastLong) && (lat > ::world.southLat) && (lat < ::world.northLat)) {
            return 1;
        }
        return 0;


    }
    static void process_cmd(char* filename, char* database,char* logfile) {
        HashTable<std::string> table;
        QuadTree *tree;
        std::fstream log;
        log.open(logfile, std::ios_base::app);
        std::cout << logfile << "\n";
        std::vector<std::string> vect;
        std::cout << filename << " process\n";
        std::fstream cmd_file;
        int cnt = 1;
        std::cout << "Attempting to open: " << filename << "\n";
        cmd_file.open(filename, std::ios::in);
        if (cmd_file.is_open()) {
            std::cout << "File open, now reading\n";
            std::string tp;
            std::cout << "---------------------------------------------------------\n";
            while(getline(cmd_file,tp)) {
                if (tp[0] == ';'){
                    log << tp <<"\n";
                    continue;
                }

                std::cout << tp << "\n";
                ex(tp, vect);
                std::cout << "---------------------------------------------------------\n";
                if (vect[0] == "import") {
                    log << "Command " <<cnt<<": import " << vect[1] << "\n";
                    import(vect[1],database,table, log,tree);

                }
                else if(vect[0] == "world") {
                    world(vect,log,tree);

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
    static void import(std::string file,char* database,HashTable<std::string> &table, std::fstream &log,QuadTree* tree) {
        std::cout << "filename to read " << file << "||| database now" << database << "\n";
        int ignoreLine = 0;
        std::fstream input;
        std::fstream output;
        input.open(file, std::ios::in);
        output.open(database, std::ios::out);
        int imported = 0;
        int cur_probe = 0;
        int max_probe = 0;




        if (input.is_open() and output.is_open()) {
           std::string tp;
           std::cout << "Both Files are open\n";
           while (getline(input,tp)) {
               if (ignoreLine == 0) {
                   ignoreLine++;
                   continue;
               }

               if (validate_record(tp) == 1) {
                   output << tp << "\n";
                   cur_probe = GISRecord::asses_name(tp,table,ignoreLine);
                   ignoreLine++;
                   imported++;

               }
               if (cur_probe > max_probe) max_probe = cur_probe;

           }
        } else std::cout << "Files did not open";

        input.close();
        output.close();
        ::avg = ::avg/imported;
        std::cout << "Finished Reading\n";
        log <<"Command 1: import\t./VA_Monterey.txt\n";
        log << "Imported Features by name: " << imported << "\n";
        log << "Longest probe sequence:    " << max_probe << "\n";
        log << "Imported Locations:        " << imported << "\n";
        log << "Average name length:       " << ::avg << "\n";
        log << "-------------------------------------------------------------------------------------------" << "\n";



    }
    static void world(std::vector<std::string> &vec, std::fstream &log,QuadTree* tree) {

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
        tree = new QuadTree(::world.westLong,::world.eastLong,::world.northLat,::world.southLat);

        log << "------------------------------------------------------------------------------------------\n";
        log << " Latitude/longitude values in index entries are shown as signed integers, in total seconds.\n";
        log << "------------------------------------------------------------------------------------------\n";
        log << "                        World boundaries are set to:\n";
        log << "                                    "<< ::world.northLat << "\n";
        log << "                        "<<::world.westLong<< "                "<<::world.eastLong << "\n";
        log << "                                    "<< ::world.southLat << "\n";
        log << "------------------------------------------------------------------------------------------\n";

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

        auto time = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(time);

        std::fstream logfile;
        logfile.open(argv[3]);
        logfile << "Course Project for COMP 8042\n";
        logfile << "Student Name: Navin Parmar, Student Id: A01044425\n";
        logfile << "Begin of GIS Program log:\n";
        logfile << "    dbFile: " << argv[1] <<"\n";
        logfile << "    script: " << argv[2] <<"\n";
        logfile << "    log: " << argv[3] <<"\n";
        logfile << "    Start Time:  " << std::ctime(&end_time) <<"\n";

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


    SystemManager::validate_args(argc,argv);
    CommandProcessor::process_cmd(argv[2], argv[1],argv[3]);
    std::cout << "back in main";


    return 0;
}

