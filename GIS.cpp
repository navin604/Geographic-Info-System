//
// Created by Navin on 2022-12-07.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "BufferPool.h"
#include "HashTable.h"
#include "GISRecord.h"
#include "QuadTree.h"
#include <ctime>
#include <chrono>

struct {long int westLong;
    long int eastLong;
    long int southLat;
    long int northLat;
} world;

int global_offset = 0;

class CommandProcessor {
public:
    static int validate_record(std::string s,int &avg){
        //Checks if record is in world boundaries;
        std::vector<std::string> record = GISRecord::convert(s);
        long int lon = GISRecord::longitude_convert(record[8]);
        long int lat = GISRecord::latitude_convert(record[7]);
        if (lon == 0 or lat == 0){
            return 0;
        }
        else if ((lon > ::world.westLong) and (lon < ::world.eastLong) and (lat > ::world.southLat) and (lat < ::world.northLat)) {
            //Calculates average name length per valid record
            avg += record[1].length();
            return 1;
        }
        return 0;


    }
    static void process_cmd(char* filename, char* database, std::fstream &log) {
        //Initializes data structures
        HashTable table;
        BufferPool pool;
        QuadTree* tree = new QuadTree(0,0,0,0);
        std::vector<std::string> vect;
        std::fstream cmd_file;
        
        int cnt = 1;
        
        //Processes script file
        cmd_file.open(filename, std::ios::in);
        if (cmd_file.is_open()) {
            std::string tp;
            while(getline(cmd_file,tp)) {
                //Ignore comments
                if (tp[0] == ';'){
                    log << tp <<"\n";
                    continue;
                }
                //Process command into vector
                ex(tp, vect);
                if (vect[0] == "import") {
                    //Imports records
                    log << "Command " <<cnt<<": import " << vect[1] << "\n\n";
                    import(vect[1],database,table, log,tree);
                    cnt++;
                }
                else if(vect[0] == "world") {
                    //Sets world
                    log << tp << "\n\n";
                    world(vect,log,tree);
                }
                else if(vect[0] == "debug") {
                    //Visualizes data structures
                    log << "Command " <<cnt<<": debug " << vect[1] << "\n\n";
                    debug(vect,log,table,pool,tree);
                    cnt++;
                }
                else if(vect[0] == "what_is") {
                    //Searches for records based on feature name and abbreviation
                    log << "Command " <<cnt<<": what_is " << vect[1] << " " << vect[2] << "\n\n";
                    what_is(vect,log,table,pool,database);
                    cnt++;
                }
                else if(vect[0] == "what_is_at"){
                    //Searches for records at given coordinates
                    log << "Command " <<cnt<<": what_is_at " << vect[1] << " " << vect[2] << "\n\n";
                    what_is_at(vect,log,tree,pool,database);
                    cnt++;
                }
                else if(vect[0] == "what_is_in"){
                    //Searches for records in closed rectangle
                    log << "Command " <<cnt<<": "<< tp << "\n\n";
                    what_is_in(vect,log,tree,pool,database);
                    cnt++;
                }
                else if(vect[0] == "quit"){
                    //Stops processing commands and exits.
                    auto time = std::chrono::system_clock::now();
                    std::time_t end_time = std::chrono::system_clock::to_time_t(time);
                    log << "Command " <<cnt<<": QUIT " << "\n\n";
                    log << "Terminating execution of commands.\n";
                    log << "------------------------------------------------------------------------------------------\n";
                    log<< "Exit Time:  " << std::ctime(&end_time) <<"\n";
                    log.close();
                    exit(1);
                } else {
                    //Skips invalid command
                    log << "INVALID COMMAND!\n";
                }
                //Clears command vector
                vect.clear();
            }
            cmd_file.close();
        } else{
            log << "INVALID SCRIPT! CLOSING!\n";
            log.close();
            exit(1);
        }
    }

    static void what_is_in(std::vector<std::string> &vec, std::fstream &log,QuadTree* tree,BufferPool &pool,char* database){
        bool long_option = false;
        bool filter_option = false;
        std::string filter ="";
        //Validates arguments for what_is_in
        if (vec.size() < 5) {
            log << "Invalid arguments for command: what_is_in\nSkipping Command\n\n";
            return;
        }
        else if (vec.size() == 6) {
            if (vec[1] == "-long") {
                //Sets long option
                long_option = true;
            }
            else {
                log << "Invalid arguments for command: what_is_in\nSkipping Command\n\n";
                return;
            }
        }
        else if (vec.size() == 7){
            if ((vec[1] == "-filter") and ((vec[2] == "structure") or (vec[2] == "water") or (vec[2] == "pop"))) {
                //Sets filter
                filter_option = true;
                filter =vec[2];
            }
            else {
                log << "Invalid arguments for command: what_is_in\nSkipping Command\n\n";
                return;
            }
        }
        std::string str_long;
        std::string str_lat;
        int longitude;
        int latitude;
        int half_height;
        int half_width;
        //Process arguments based on parameter
        if (long_option) {
            str_long=vec[3];
            str_lat=vec[2];
            longitude = GISRecord::longitude_convert(vec[3]);
            latitude = GISRecord::latitude_convert(vec[2]);
            half_height = stoi(vec[4]);
            half_width =  stoi(vec[5]);
        }
        else if (filter_option){
            str_long=vec[4];
            str_lat=vec[3];
            longitude = GISRecord::longitude_convert(vec[4]);
            latitude = GISRecord::latitude_convert(vec[3]);
            half_height = stoi(vec[5]);
            half_width =  stoi(vec[6]);
        }
        else {
            str_long=vec[2];
            str_lat=vec[1];
            longitude = GISRecord::longitude_convert(vec[2]);
            latitude = GISRecord::latitude_convert(vec[1]);
            half_height = stoi(vec[3]);
            half_width =  stoi(vec[4]);
        }
        //Searches for records
        std::vector<int> offsets = tree->search_rectangle(longitude-half_width, longitude+half_width,latitude-half_height,latitude+half_height);

        if (offsets.size() == 0) {
            log << "Nothing was found in (" << GISRecord::lat_str(str_lat) << " +/- " <<half_height << ", "<< GISRecord::lon_str(str_long) << " +/- " <<half_width << ")\n";
            log << "\n-------------------------------------------------------------------------------------------" << "\n\n";
            return;
        }
        std::string offset;
        std::string query;
        //Searches for full record from pool and/or database
        std::vector<std::vector<std::string>> temp;
        if (filter_option) {
            for (int i=0; i<offsets.size();i++) {
                offset = std::to_string(offsets[i]);
                query = pool.filter_search(offset, database, filter);
                if (query == "") {
                    continue;
                }
                temp.push_back(GISRecord::convert(query));
            }
        }
        //Outputs records based on parameters specified above
        std::string lon = GISRecord::lon_str(str_long);
        std::string lat = GISRecord::lat_str(str_lat);
        int filterNum = 0;
        if (filter_option) {
            log << "The following features matching your criteria were found in  (" <<lat<< " +/- " <<half_height << ", "<<lon << " +/- " <<half_width << ")\n\n";
            for (int i = 0; i<temp.size(); i++) {
                if (GISRecord::check_filter(temp[i][2],filter)==1){
                    filterNum++;
                    log << "    " << offsets[i] <<": \""<<temp[i][1] <<"\" \""<<temp[i][3] <<"\"   \"("<< GISRecord::lat_str(temp[i][7]) <<", "<<GISRecord::lon_str(temp[i][8]) << ")\n";
                }
            }
            log << "\nThere were " << filterNum << " features of type " << filter <<"\n";
            log << "\n-------------------------------------------------------------------------------------------" << "\n\n";
            return;
        }

        //Searches for all records if filter not specified
        for (int i=0; i<offsets.size();i++){
            offset = std::to_string(offsets[i]);
            query = pool.search(offset,database);
            temp.push_back(GISRecord::convert(query));

        }

        //Verbose output
        if (long_option) {
            log << "The following "<<offsets.size() << " feature(s) were found at (" <<lat<< " +/- " <<half_height << ", "<<lon << " +/- " <<half_width << ")\n\n";
            for (int i = 0;i<temp.size();i++){
                log << "Feature ID  :  " <<temp[i][0] <<"\n";
                log << "Feature Name  :  " <<temp[i][1] <<"\n";
                log << "Feature Cat :  " <<temp[i][2] <<"\n";
                log << "State  :  " <<temp[i][3] <<"\n";
                log << "County  :  " <<temp[i][5] <<"\n";
                log << "Longitude   :  " <<GISRecord::lon_str(temp[i][8]) <<"\n";
                log << "Latitude   :  " <<GISRecord::lat_str(temp[i][7]) <<"\n";
                log << "Elev in ft  :  " <<temp[i][16] <<"\n";
                log << "USGS Quad   :  " <<temp[i][17] <<"\n";
                log << "Date created  :  " <<temp[i][18] <<"\n";
                log << "\n";
            }
            log << "-------------------------------------------------------------------------------------------" << "\n\n";
            return;
        }
        //Standard output
        log << "The following "<<offsets.size() << " feature(s) were found at (" <<lat<< " +/- " <<half_height << ", "<<lon << " +/- " <<half_width << ")\n\n";
        for (int i = 0;i<offsets.size();i++){
            log << "    " << offsets[i] <<": \""<<temp[i][1] <<"\" \""<<temp[i][3] <<"\"   \"("<< GISRecord::lat_str(temp[i][7]) <<", "<<GISRecord::lon_str(temp[i][8]) << ")\n";
        }
        log << "\n-------------------------------------------------------------------------------------------" << "\n\n";
    }

    static void what_is_at(std::vector<std::string> &vec, std::fstream &log,QuadTree* tree,BufferPool &pool,char* database){
        //Searches for all records at given coordinate
        if (vec.size() != 3) {
            log << "Invalid arguments for command: what_is_at\n";
            return;
        }
        //Convert string DMS coords into seconds
        int longitude = GISRecord::longitude_convert(vec[2]);
        int latitude = GISRecord::latitude_convert(vec[1]);
        std::vector<int> offsets = tree->search(longitude, latitude);
        std::string offset;
        std::string query;
        std::vector<std::vector<std::string>> temp;
        for (int i=0; i<offsets.size();i++){
            offset = std::to_string(offsets[i]);
            query = pool.search(offset,database);
            temp.push_back(GISRecord::convert(query));

        }
        //Converts coordinates to DMS ouput
        std::string lon = GISRecord::lon_str(vec[2]);
        std::string lat = GISRecord::lat_str(vec[1]);
        if (offsets.size() == 0) {
            log << "Nothing was found at (" <<lat<<", "<<lon << ")\n";
            log << "-------------------------------------------------------------------------------------------" << "\n\n";
            return;
        }
        log << "The following feature(s) were found at (" <<lat<<", "<<lon << ")\n";
        for (int i = 0;i<offsets.size();i++){
            log << "    " << offsets[i] <<": \""<<temp[i][1] <<"\" \"" <<temp[i][5] << "\"  \"" <<temp[i][3] <<"\"\n";

        }
        log << "-------------------------------------------------------------------------------------------" << "\n\n";
    }

    static void what_is(std::vector<std::string> &vec, std::fstream &log,HashTable &table,BufferPool &pool,char* database){
        //Searches for records with matching name
        std::string query = vec[1] +":"+ vec[2];
        std::vector<int> result = table.search(query);
        if (result.size() == 0) {
            log << "No records match \""<<vec[1] <<"\" and \""<< vec[2] << "\"\n";
            log << "-------------------------------------------------------------------------------------------" << "\n\n";
            return;
        }
        for (int z = 0; z<result.size();z++){
            std::string offset = std::to_string(result[z]);
            query = pool.search(offset,database);
            std::vector<std::string> temp = GISRecord::convert(query);
            std::string lon = GISRecord::lon_str(temp[8]);
            std::string lat = GISRecord::lat_str(temp[7]);
            log << offset << ": " << temp[5] << " (" <<lat<<", "<<lon << ")\n";
        }

        log << "-------------------------------------------------------------------------------------------" << "\n\n";
    }

    static void debug(std::vector<std::string> &vec, std::fstream &log,HashTable &table,BufferPool &pool,QuadTree* tree){
        //Visualize data structures
        if (vec[1] == "hash"){
            log << table.print();
            log << "-------------------------------------------------------------------------------------------" << "\n\n";
        }else if(vec[1] == "pool"){
            log << pool.print();
            log << "-------------------------------------------------------------------------------------------" << "\n\n";

        }
        else if(vec[1] == "quad"){
            log << "This command attempts to print using DFS. I apologize for poor formatting!\n";
            log << tree->print();
            log << "-------------------------------------------------------------------------------------------" << "\n\n";

        }
        else if(vec[1] == "world"){
            log << "This was a optional command. I did not implement it!" << "\n";
            log << "-------------------------------------------------------------------------------------------" << "\n\n";
        }
    }

    static void import(std::string file,char* database,HashTable &table, std::fstream &log,QuadTree* tree) {
        //Imports files
        int ignoreLine = -1;
        std::fstream input;
        std::fstream output;
        input.open(file, std::ios::in);
        output.open(database, std::fstream::app);
        int imported = 0;
        int cur_probe = 0;
        int max_probe = 0;
        int avg = 0;
        if (input.is_open() and output.is_open()) {
            std::string tp;
            while (getline(input,tp)) {
                if (ignoreLine == -1) {
                    ignoreLine++;
                    continue;
                }
                if (validate_record(tp,avg) == 1) {
                    //Checks if record within world coordinates
                    output << tp << "\n";
                    cur_probe = GISRecord::asses_name(tp,table,::global_offset);
                    GISRecord::asses_coord(tp,tree,::global_offset);
                    ignoreLine++;
                    imported++;
                    ::global_offset++;
                }
                if (cur_probe > max_probe) max_probe = cur_probe;
            }
        } else {
            log << "Files did not open (check path) - Exiting program\n";
            log.close();
            exit(1);
        }
        input.close();
        output.close();
        if (imported !=0){
            avg = avg/imported;
        }
        else {
            log << "No files were imported, exiting!\n";
            log.close();
            exit(1);
        }
        //Log stats for import
        log << "Imported Features by name: " << imported << "\n";
        log << "Longest probe sequence:    " << max_probe << "\n";
        log << "Imported Locations:        " << imported << "\n";
        log << "Average name length:       " << avg << "\n";
        log << "-------------------------------------------------------------------------------------------" << "\n\n";
        imported = 0;
        max_probe = 0;
        avg = 0;
    }
    static void world(std::vector<std::string> &vec, std::fstream &log,QuadTree* tree) {
        //Sets world boundaries
        if (vec.size() != 5) {
            log << "Invalid arguments for command: world\nExiting program\n";
            return;
        }
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

        tree->setVal(::world.westLong,::world.eastLong,::world.southLat,::world.northLat);

        //Log details for world command
        log << "------------------------------------------------------------------------------------------\n";
        log << " Latitude/longitude values in index entries are shown as signed integers, in total seconds.\n";
        log << "------------------------------------------------------------------------------------------\n";
        log << "                        World boundaries are set to:\n";
        log << "                                    "<< ::world.northLat << "\n";
        log << "                        "<<::world.westLong<< "                "<<::world.eastLong << "\n";
        log << "                                    "<< ::world.southLat << "\n";
        log << "------------------------------------------------------------------------------------------\n\n";

    }

    static void ex(std::string s,std::vector<std::string> &vec) {
        //process string command
        if (s==""){
            vec.push_back("F");
        }
        std::istringstream iss(s);
        std::string word;
        while(std::getline(iss, word, '\t'))   // but we can specify a different one
            vec.push_back(word);

    }
};

class Logger {
public:
    static void log_init(char* argv[]) {
        //Initialize log with basic data
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
};

class SystemManager {
public:
    static void validate_args(int argc, char *argv[]) {
        //Validates args
        if (argc!=4) {
            exit(0);
        }
        file_setup(argv[1]);
        file_setup(argv[3]);
        Logger::log_init(argv);
    }
    static void file_setup(char* filename) {
        // Deletes and creates database & log files
        if (remove(filename)!=0) {

        } else {

        }
        std::fstream file;
        file.open(filename, std::ios::out);
        if (!file) {
            std::exit(0);
        }

    }

};

int main(int argc, char *argv[]) {
    SystemManager::validate_args(argc,argv);
    std::fstream log;
    log.open(argv[3], std::ios_base::app);
    CommandProcessor::process_cmd(argv[2], argv[1],log);
    return 0;
}
