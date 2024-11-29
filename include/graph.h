#ifndef GRAPH_H
#define GRAPH_H
#include<string>
#include "CSC.h"
#include "utils.h"
#include <set>
/*
Number of entity types: 3
Entity types:
file，network，process

Number of relation types: 10
Relation types:
accept，clone，execve，read，recvmsg，rename_in，rename_out，sendmsg，write，writev
*/

class Graph{
private:
    int numEdges,numVertices;
    struct Entity *entities;
    struct Relation *relations;
    int *inDegree;//记录每个点的入度（subject<-的已经矫正过来了）
    int *check_node_order_from_id;//根据id查询点的序号
    int *check_edge_order_from_id;//根据id查询边的序号
    CSC csc_graph;
public:
    Graph(const int num_vertices,const int num_edges):numVertices(num_vertices),numEdges(num_edges),csc_graph(num_vertices,num_edges){
        entities=new Entity[numVertices];
        relations=new Relation[numEdges];
        inDegree=new int[numVertices];
        check_node_order_from_id=new int[numEdges+1+numVertices+1];
        check_edge_order_from_id=new int[numEdges+1+numVertices+1];
        for(int i=0;i<numVertices;i++)
            inDegree[i]=0;
    }
    ~Graph(){
        delete [] entities;
        delete [] relations;
        delete [] check_edge_order_from_id;
        delete [] check_node_order_from_id;
        delete [] inDegree;
    }
    void BuildFromJson(std::string JsonPath);
    void register_CSC();//根据Relations建立CSC
    int getNumEdges()const;
    int getNumVertices()const;
    int get_node_order_from_id(int node_id)const;//根据id查询点的序号
    int get_edge_order_from_id(int edge_id)const;//根据id查询边的序号
    Entity get_entitie_from_node_order(int node_order)const;
    Relation get_relation_from_edge_order(int edge_order)const; 
    void find_Influence(set<pair<int,int>> &influnce_set,int this_entity_order,string min_edge_et);//找到对应entity_id的entity的相关影响
    static bool JudgeDataFlow(std::string type);//根据类型判断数据流向
    Entity findPOI(string POI_name)const;//找到POI_name匹配的node_order
    void show()const;
};


#endif