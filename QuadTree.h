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
    void setVal(int westLong, int eastLong, int southLat, int northLat) {
        root.sector = {westLong,southLat,eastLong,northLat};


    }
    void init(int xcoord, int ycoord, std::vector<int> offset) {
        insert(xcoord,ycoord,offset,&root);


    }

    std::vector<int> search(int xcoord, int ycoord) {
        point temp_root = root;


        while (true)
        {
            if (temp_root.cell == "Empty") return {};
            if (temp_root.cell == "Leaf")
            {
                if (xcoord == temp_root.coord_x && ycoord== temp_root.coord_y)
                    return temp_root.offsets;
                return {};
            }
            while (temp_root.cell == "QUAD");  {
                int count;
                for (count = 0; count < QUAD; count++)
                {
                    //change into 4 variables
                    std::vector<int> sector = temp_root.subtree[count].sector;
                    if (xcoord >= sector[0] && ycoord >= sector[1])
                        if (xcoord <= sector[2] && ycoord <= sector[3])
                        {
                            temp_root= (point)temp_root.subtree[count];
                            break;
                        }
                }
                if (count == 4) return{};

            }
        }

    }

    void insert(int xcoord, int ycoord, std::vector<int> offset, point *p) {
        while (true){
            //Defines parent Node
            if (p->cell == "Empty"){


                if ((xcoord >= p->sector[0] && xcoord <= p->sector[2]) && (ycoord >= p->sector[1] && ycoord <= p->sector[3])) {
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

                int cnt;
                //Checks each child of current Point
                for(cnt = 0; cnt<::QUAD;cnt++){
                    point *curr = &(p->subtree[cnt]);
                    if ((xcoord >= curr->sector[0] && xcoord <= curr->sector[2]) && (ycoord >= curr->sector[1] && ycoord <= curr->sector[3])) {
                        p = curr;
                        break;
                    }
                }
                //error
                if (cnt>3) return;
            }
            //Adds offset to leaf or splits leaf into 4 children
            if (p->cell =="Leaf") {
                if (xcoord == p->coord_x && ycoord == p->coord_y) {

                    for (int i = 0; i<offset.size();i++) {
                        p->offsets.push_back(offset[i]);
                        return;
                    }
                }
                else{
                    point child;
                    child.cell = "Empty";
                    //Calculates new regions
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
