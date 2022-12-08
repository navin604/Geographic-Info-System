//
// Created by Navin on 2022-12-02.
//
#include <iostream>
#include <sstream>
#ifndef CODE_QUADTREE_H
#define CODE_QUADTREE_H

int QUAD = 4;
class QuadTree{
private:
    struct point {
        //Child Nodes of parent node
        std::vector<point>  subtree;
        //4 corner coords of this node
        std::vector<int> sector;
        //Offsets of records in this location
        std::vector<int> offsets;
        //Coords of location
        int coord_x;
        int coord_y;
        //Represents whether node is empty, a leaf, or has kids(quad)
        std::string cell;
    };
    //Holds nodes in DFS traversal of tree
    std::vector<point> dfs_nodes;
    point root;

public:

    QuadTree(int westLong, int eastLong, int southLat, int northLat) {
        //Initialize tree
        root.sector = {westLong,southLat,eastLong,northLat};
        root.cell = "Empty";

    }
    void setVal(int westLong, int eastLong, int southLat, int northLat) {
        //Adjust region of specific coord (square coords)
        root.sector = {westLong,southLat,eastLong,northLat};


    }
    void init(int xcoord, int ycoord, std::vector<int> offset) {
        //Receive coords to insert, passes coords with root to insert function
        insert(xcoord,ycoord,offset,&root);
    }

    std::vector<int> search(int xcoord, int ycoord) {
        //Searches for specific coords
        point temp_root = root;
        int count;
        while (true) {
            //If node empty, return
            if (temp_root.cell == "Empty") return {};
            //If node has no kids, check if coords are in this leaf
            if (temp_root.cell == "Leaf") {
                if (xcoord == temp_root.coord_x and ycoord== temp_root.coord_y)
                    return temp_root.offsets;
                return {};
            }
            do {
                //If node has 4 kids, search each child to determine if coords can be found
                for (count = 0; count < QUAD; count++){
                    std::vector<int> sector = temp_root.subtree[count].sector;
                    if ((xcoord >= sector[0] and xcoord <= sector[2]) and ( ycoord >= sector[1] and ycoord <= sector[3])) {
                        temp_root = (point) temp_root.subtree[count];
                        break;
                    }
                }
                //Reached last child, no results
                if (count == 4) return{};
            } while (temp_root.cell == "Quad");
        }

    }
    void dfs(point point) {
        //dfs traversal for searching rectangle regions. Bottom up search
        if (point.cell == "Quad"){
            for (int i = 0; i< QUAD;i++) {
                dfs(point.subtree[i]);
            }
        }
        else if (point.cell == "Leaf") dfs_nodes.push_back(point);
    }

    void dfs_print(point point, std::ostringstream &os,int &depth,bool &check) {
        //prints for debug command
        //Uses @ to represent which level the node is at
        //Uses DFS.
        os  << "    " ;
        for (int j = 0; j < 4; j++){
            if (point.subtree[j].cell == "Empty")
                os  << "" ;
            else if (point.subtree[j].cell== "Leaf"){

                os << " [(" << point.subtree[j].coord_x;
                os << ", " << point.subtree[j].coord_y;
                os << ", [ ";
                for (auto &offset : point.subtree[j].offsets)
                    os << offset << " ";
                os << "]) ";
            }
            else  {
                os << "\n";
                depth+=1;
                for (int i= 0; i<depth; i++) {
                    if (check ==false and depth==1){
                        check=true;
                    }
                    os << "@";
                }
                os <<"\n";
                dfs_print(point.subtree[j],os,depth,check);
                depth-=1;
                os <<"\n";
                for (int i= 0; i<depth; i++) {
                    if (check == true and depth==1){
                        check = false;
                        return;
                    }
                    os << "@";
                }
                os <<"\n";
            }
        }
        return;


    }
    std::string print() {
        //Prints root node if no kids
        bool check = false;
        std::ostringstream os;
        if (root.cell == "Empty") {
            os << "This tree is empty!\n";
        } else if (root.cell == "Leaf") {
            os << "This is a single node(leaf) tree!\n";
            os << "[(\"" << root.coord_x <<"," << root.coord_y << "), ";
            for (int i = 0; i < root.offsets.size();i++){
                os << "[" <<root.offsets[i] << "], ";
            }
            os << "\n";
        }
        else {
            //Has kids, DFS search
            int depth = 0;
            dfs_print(root,os,depth,check);
        }
        return os.str();

    }

    std::vector<int> search_rectangle(int xlow, int xhigh, int ylow, int yhigh) {
        point index = root;
        point child;
        dfs(index);
        std::vector<int> nodes;
        //DFS, work bottom up and check each node to see if they fit in range
        for (point dummy : dfs_nodes){
            if ((dummy.coord_x >= xlow and xhigh >= dummy.coord_x) and  (dummy.coord_y >= ylow and yhigh >= dummy.coord_y)) {
                for ( int x = 0; x<dummy.offsets.size();x++)
                    nodes.push_back(dummy.offsets[x]);
            }
        }
        dfs_nodes.clear();
        return nodes;
    }

    void insert(int xcoord, int ycoord, std::vector<int> offset, point *p) {
        while (true){
            //Insert in node if empty
            if (p->cell == "Empty"){
                if ((xcoord >= p->sector[0] and xcoord <= p->sector[2]) and (ycoord >= p->sector[1] and ycoord <= p->sector[3])) {
                    //if coords in boundary, insert
                    p->coord_x = xcoord;
                    p->coord_y = ycoord;
                    //Store offsets
                    for (int i = 0; i<offset.size();i++) {
                        p->offsets.push_back(offset[i]);
                    }
                    //Set as leaf
                    p->cell = "Leaf";
                    return;
                }
                return;
            }
            //if node has 4 kids, search each node to see if coords can fit,
            if (p->cell =="Quad") {
                int cnt;
                for(cnt = 0; cnt<::QUAD;cnt++){
                    point *dummy = &(p->subtree[cnt]);
                    if ((xcoord >= dummy->sector[0] and xcoord <= dummy->sector[2]) and (ycoord >= dummy->sector[1] and ycoord <= dummy->sector[3])) {
                        //If fit, set node as child, repeat until leaf or empty node is found
                        p = dummy;
                        break;
                    }
                }
                if (cnt>3) return;
            }
            //If leaf, check if coords can fit, insert if they do
            if (p->cell =="Leaf") {
                if (xcoord == p->coord_x and ycoord == p->coord_y) {
                    for (int i = 0; i<offset.size();i++) {
                        p->offsets.push_back(offset[i]);
                        return;
                    }
                }
                //Coords do not fit, split current region into 4 smaller regions.
                else{
                    point child;
                    child.cell = "Empty";
                    child.sector = {p->sector[0],p->sector[1],p->sector[0] +(p->sector[2]-p->sector[0])/2,p->sector[1]+(p->sector[3]-p->sector[1])/2};
                    p->subtree.push_back(child);
                    child.sector = {p->sector[0] +(p->sector[2]-p->sector[0])/2,p->sector[1],p->sector[2],p->sector[1]+(p->sector[3]-p->sector[1])/2};
                    p->subtree.push_back(child);
                    child.sector = {p->sector[0],p->sector[1]+(p->sector[3]-p->sector[1])/2,p->sector[0] +(p->sector[2]-p->sector[0])/2,p->sector[3]};
                    p->subtree.push_back(child);
                    child.sector = {p->sector[0] +(p->sector[2]-p->sector[0])/2,p->sector[1]+(p->sector[3]-p->sector[1])/2,p->sector[2],p->sector[3]};
                    p->subtree.push_back(child);
                    p->cell = "Quad";
                    //Reinsert leaf node values, and then the original coords being inserted
                    insert(p->coord_x,p->coord_y,p->offsets,p);
                }
            }
        }
    }

};

#endif //CODE_QUADTREE_H
