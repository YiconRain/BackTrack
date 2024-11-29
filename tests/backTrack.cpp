#include "graph.h"
#include "CSC.h"
#include "utils.h"
#include <string>
#include <set>
#include <utility>
#include <fstream>
#include <iostream>
using namespace std;

#define MAX_ETIME "1600000000.000000000"
void show_entity(Entity node) {
    cout << "Entity: node_order=" << node.order_node
         << ", id=" << node.id
         << ", name=" << node.name
         << ", type=" << node.type
         << endl;
}

void show_relation(Relation edge) {
    cout << "Relation: edge_order=" << edge.order_edge
         << ", id=" << edge.id
         << ", subject=" << edge.subject
         << ", object=" << edge.object
         << ", type=" << edge.type
         << ", stime=" << edge.stime
         << ", etime=" << edge.etime
         << endl;
}

int main() {
    std::string JsonPath = "../data/experiment.json";
    int numVertices = 72420;
    int numEdges = 5000001;
    string POI_name = "redis-server"; // 待匹配的POI

    Graph g(72420, 5000001);
    g.BuildFromJson(JsonPath);
    Entity POI_node = g.findPOI(POI_name); // 找到是第几个node
    show_entity(POI_node);
    g.show();

    // influence_set存储最终的答案，分别是<node_order,edge_order>,可以唯一表示重边
    set<pair<int, int>> influence_set;
    set<int> NODE_ID;
    set<int> EDGE_ID;
    g.find_Influence(influence_set, POI_node.order_node, MAX_ETIME);

    ofstream node_file("../output/node.txt");
    ofstream edge_file("../output/edge.txt");
    if (!node_file.is_open() || !edge_file.is_open()) {
        cerr << "Failed to open output files." << endl;
        return 1;
    }

    for (const auto& element : influence_set) {
        int node_order = element.first;
        int edge_order = element.second;

        Entity node = g.get_entitie_from_node_order(node_order);
        Relation edge = g.get_relation_from_edge_order(edge_order);

        NODE_ID.insert(node.id);
        EDGE_ID.insert(edge.id);

        // show_entity(node);
        // show_relation(edge);
    }


    cout << "NODE_ID:" << endl;
    for (auto it = NODE_ID.begin(); it != NODE_ID.end(); ++it) {
        node_file << *it <<endl;
    }

    cout << "EDGE_ID:" << endl;
    for (auto it = EDGE_ID.begin(); it != EDGE_ID.end(); ++it) {
        edge_file << *it <<endl;
    }
    

    // 关闭文件
    node_file.close();
    edge_file.close();

    cout << "Node IDs written to node.txt" << endl;
    cout << "Edge IDs written to edge.txt" << endl;

    return 0;
}


