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

        std::vector<point>  subtree;
        std::vector<int> sector;
        std::vector<int> offsets;
        int coord_x;
        int coord_y;


        std::string cell;

    };

    std::vector<point*> traversal;
    std::vector<point> subset;
    point root;

public:

    QuadTree(int westLong, int eastLong, int southLat, int northLat) {
        root.sector = {westLong,southLat,eastLong,northLat};
        root.cell = "Empty";

    }
    void setVal(int westLong, int eastLong, int southLat, int northLat) {
        root.sector = {westLong,southLat,eastLong,northLat};


    }
    void init(int xcoord, int ycoord, std::vector<int> offset) {
        insert(xcoord,ycoord,offset,&root);


    }

    std::vector<int> search(int xcoord, int ycoord) {
        point temp_root = root;
        int count;
        while (true) {
            if (temp_root.cell == "Empty") return {};
            if (temp_root.cell == "Leaf") {
                if (xcoord == temp_root.coord_x and ycoord== temp_root.coord_y)
                    return temp_root.offsets;
                return {};
            }
            do {
                for (count = 0; count < QUAD; count++){
                    std::vector<int> sector = temp_root.subtree[count].sector;
                    if ((xcoord >= sector[0] and xcoord <= sector[2]) and ( ycoord >= sector[1] and ycoord <= sector[3])) {
                        temp_root = (point) temp_root.subtree[count];
                        break;
                    }
                }
                if (count == 4) return{};

            } while (temp_root.cell == "Quad");
        }

    }
    void dfs(point point) {
        if (point.cell == "Quad"){
            for (int i = 0; i< QUAD;i++) {
                dfs(point.subtree[i]);
            }
        }
        else if (point.cell == "Leaf") subset.push_back(point);
    }
    void dfs_print(point point, std::ostringstream &os,int &depth,bool &check) {
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
            int depth = 0;
            dfs_print(root,os,depth,check);

        }
        return os.str();

    }

    std::vector<int> search_rectangle(int xlow, int xhigh, int ylow, int yhigh) {
        point index = root;
        point child;
        int i;
        while (index.cell == "Quad" && i != QUAD){
            for (i = 0; i < 4; i++){
                child = index.subtree[i];
                if ((xlow >= child.sector[0] and xhigh<= child.sector[2]) and (ylow >= child.sector[1] and yhigh <= child.sector[3]))                    {
                        index = child;
                        break;
                }
            }
        }
        dfs(index);
        std::vector<int> nodes;
        for (point location : subset){
            if ((location.coord_x >= xlow and xhigh >= location.coord_x) and  (location.coord_y >= ylow and yhigh >= location.coord_y)) {
                for ( int x = 0; x<location.offsets.size();x++)
                    nodes.push_back(location.offsets[x]);
            }
        }
        subset.clear();
        return nodes;

    }

    void insert(int xcoord, int ycoord, std::vector<int> offset, point *p) {
        while (true){

            if (p->cell == "Empty"){
                if ((xcoord >= p->sector[0] and xcoord <= p->sector[2]) and (ycoord >= p->sector[1] and ycoord <= p->sector[3])) {

                    p->coord_x = xcoord;
                    p->coord_y = ycoord;

                    for (int i = 0; i<offset.size();i++) {
                        p->offsets.push_back(offset[i]);
                    }

                    p->cell = "Leaf";
                    return;
                }
                return;
            }

            if (p->cell =="Quad") {
                int cnt;
                for(cnt = 0; cnt<::QUAD;cnt++){
                    point *curr = &(p->subtree[cnt]);
                    if ((xcoord >= curr->sector[0] and xcoord <= curr->sector[2]) and (ycoord >= curr->sector[1] and ycoord <= curr->sector[3])) {
                        p = curr;
                        break;
                    }
                }
                if (cnt>3) return;
            }

            if (p->cell =="Leaf") {
                if (xcoord == p->coord_x and ycoord == p->coord_y) {
                    for (int i = 0; i<offset.size();i++) {
                        p->offsets.push_back(offset[i]);
                        return;
                    }
                }
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

                    insert(p->coord_x,p->coord_y,p->offsets,p);

                }

            }

        }
    }




};




#endif //CODE_QUADTREE_H
