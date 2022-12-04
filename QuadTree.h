//
// Created by navin on 2022-12-02.
//
#include <iostream>
#include <sstream>
#ifndef CODE_QUADTREE_H
#define CODE_QUADTREE_H

int QUAD = 4;
class QuadTree{
private:
    struct point {
        //size_t height = 0;
        std::vector<point>  subtree;
        std::vector<int> sector;
        std::vector<int> offsets;
        int coord_x;
        int coord_y;

        // States if cell is empty/full/subsector
        std::string cell;

    };
    //Vector used for str() method
    std::vector<point*> traversal;

    point root;

public:

    QuadTree(int westLong, int eastLong, int southLat, int northLat) {
        root.sector = {westLong,southLat,eastLong,northLat};
        root.cell = "Empty";

    }
    void init(int xcoord, int ycoord, std::vector<int> offset) {
        insert(xcoord,ycoord,offset,&root);


    }
    void insert(int xcoord, int ycoord, std::vector<int> offset, point *p) {
        while (true){
            //Defines parent Node
            if (p->cell == "Empty"){
                std::cout << "empty\n";

                if ((xcoord >= p->sector[0] and xcoord <= p->sector[2]) and (ycoord >= p->sector[1] and ycoord <= p->sector[3])) {
                    //Sets Coordinates
                    p->coord_x = xcoord;
                    p->coord_y = ycoord;
                    //Adds offsets
                    for (int i = 0; i<offset.size();i++) {
                        p->offsets.push_back(offset[i]);
                    }
                    //Classify node as leaf(non empty)
                    p->cell = "Leaf";
                    return;
                }
                //Error
                return;
            }
            //Locates correct child node
            if (p->cell =="Quad") {
                std::cout << "quads\n";

                int cnt;
                //Checks each child of current Point
                for(cnt = 0; cnt<::QUAD;cnt++){
                    point *curr = &(p->subtree[cnt]);
                    if ((xcoord >= curr->sector[0] and xcoord <= curr->sector[2]) and (ycoord >= curr->sector[1] and ycoord <= curr->sector[3])) {
                        p = curr;
                        break;
                    }
                }
                //error
                if (cnt>3) return;
            }
            //Adds offset to leaf or splits leaf into 4 children
            if (p->cell =="Leaf") {
                //std::cout << "leaf\n";
                if (xcoord == p->coord_x and ycoord == p->coord_y) {

                    for (int i = 0; i<offset.size();i++) {
                        p->offsets.push_back(offset[i]);
                        return;
                    }
                }
                else{
                    point child;
                    child.cell = "Empty";
                    std::cout << "wdadawdawdawda\n";
                    child.sector = {p->sector[0],p->sector[1],p->sector[0] +(p->sector[2]-p->sector[0])/2,p->sector[1]+(p->sector[3]-p->sector[1])/2};
                    p->subtree.push_back(child);

                    child.sector = {p->sector[0] +(p->sector[2]-p->sector[0])/2,p->sector[1],p->sector[2],p->sector[1]+(p->sector[3]-p->sector[1])/2};
                    p->subtree.push_back(child);

                    child.sector = {p->sector[0],p->sector[1]+(p->sector[3]-p->sector[1])/2,p->sector[0] +(p->sector[2]-p->sector[0])/2,p->sector[3]};
                    p->subtree.push_back(child);

                    child.sector = {p->sector[0] +(p->sector[2]-p->sector[0])/2,p->sector[1]+(p->sector[3]-p->sector[1])/2,p->sector[2],p->sector[3]};
                    p->subtree.push_back(child);

                    std::cout << p->cell;
                    p->cell = "Quad";
                    std::cout << p->cell;
                    insert(p->coord_x,p->coord_y,p->offsets,p);

                }

            }

        }
    }


};


#endif //CODE_QUADTREE_H
