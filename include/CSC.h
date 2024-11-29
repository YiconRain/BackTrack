#ifndef CSC_H
#define CSC_H
#include<string>
#include"utils.h"

class CSC{
public:
    int numVertices,numEdges;
    int *col_indices,*row_indices;
    int *edge_weight;
    CSC(){
    }
    CSC(int vertices,int edges): numVertices(vertices),numEdges(edges) {
        col_indices = new int[numVertices+1];
        row_indices = new int[numEdges];
        edge_weight = new int[numEdges];
    }
    ~CSC(){
        delete[] row_indices;
        delete[] col_indices;
        delete[] edge_weight;
    }
    
};

#endif