//
// Created by Navin on 2022-12-03.
//

#ifndef CODE_RECORD_H
#define CODE_RECORD_H
#include <iostream>
#include <vector>
#include "HashTable.h"
#include "QuadTree.h"

class GISRecord {
public:
    static long int longitude_convert(std::string l) {
        //Converts string DMS coords into seconds
        int total = 0;
        if (l=="Unknown" or l==""){
            return 0;
        }
        std::string deg = l.substr(0,3);
        std::string min = l.substr(3,2);
        std::string sec = l.substr(5,2);
        total = (stol(deg)*3600) + (stol(min)*60) +stol(sec) ;
        if (l[7] == 'W') total = total*-1;
        return total;
    }
    static long int latitude_convert(std::string l) {
        //Converts string DMS coords into seconds
        if (l=="Unknown" or l==""){
            return 0;
        }
        std::string deg = l.substr(0,2);
        std::string min = l.substr(2,2);
        std::string sec = l.substr(4,2);
        int total = (stol(deg)*3600) + (stol(min)*60) +stol(sec) ;
        if (l[6] == 'S') total = total*-1;
        return total;

    }

    static std::string lon_str(std::string l) {
        //Converts string DMS coords into seconds, puts in formatted string
        std::string dir = "";
        int deg = stoi(l.substr(0,3));
        int min = stoi(l.substr(3,2));
        int sec = stoi(l.substr(5,2));
        if (l[7] == 'W') {
            dir = "West";
        }else {
            dir = "East";
        }
        std::string temp = std::to_string(deg) + "d " +std::to_string(min) + "m " + std::to_string(sec) + "s " + dir;
        return temp;

    }
    static std::string lat_str(std::string l) {
        //Converts string DMS coords into seconds, puts in formatted string
        std::string dir = "";
        int deg = stoi(l.substr(0,2));
        int min = stoi(l.substr(2,2));
        int sec = stoi(l.substr(4,2));
        if (l[6] == 'N') {
            dir = "North";
        }else {
            dir = "South";
        }
        std::string temp = std::to_string(deg) + "d " +std::to_string(min) + "m " + std::to_string(sec) + "s " + dir;
        return temp;
    }
    static int check_filter(std::string s,std::string filter) {
        //Checks if records meets filter parameter
        if (filter == "structure"){
            if (s =="Airport" or s =="Bridge" or s =="Building" or s =="Church" or s =="Dam" or s =="Hospital" or s == "Levee" or s =="Park" or s =="Post Office" or s =="School" or s =="Tower" or s =="Tunnel" ){
                return 1;
            } else return -1;
        }
        if (filter == "pop"){
            if (s =="Populated Place" ){
                return 1;
            } else return -1;
        }
        if (filter == "water"){
            if (s =="Arroyo" or s =="Bay" or s =="Bend" or s =="Canal" or s =="Channel" or s =="Falls" or s == "Glacier" or s =="Gut" or s =="Harbor" or s =="Lake" or s =="Rapids" or s =="Reservoir" or s =="Stream"  or s =="Sea" or s =="Spring" or s=="Swamp" or s=="Well"   ){
                return 1;
            } else return -1;
        }
    }

    static std::vector<std::string> convert(std::string s) {
        //Converts record into vector
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
        return l;
    }

    static int asses_name(std::string s,HashTable &table,int i){
        //Concatenate string for hashtable key
        std::vector<std::string> l = convert(s);
        std::string temp = l[1] +":"+ l[3];
        int val = table.insert(temp,i);
        return val;
    }

    static void asses_coord(std::string s,QuadTree* tree,int offset){
        //Converts string coords into seconds for quad tree
        std::vector<std::string> l = convert(s);
        int longitude = longitude_convert(l[8]);
        int latitude =latitude_convert(l[7]);
        tree->init(longitude,latitude,{offset});
    }
};

#endif //CODE_RECORD_H
