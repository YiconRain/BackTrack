#include "graph.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unistd.h>
#include <set>
using namespace std;



int Graph::getNumVertices()const{
    return numVertices;
}

int Graph::getNumEdges()const{
    return numEdges;
}

//根据id查询点的序号
int Graph::get_node_order_from_id(int node_id)const{
    return check_node_order_from_id[node_id];
}

//根据id查询边的序号
int Graph::get_edge_order_from_id(int edge_id)const{
    return check_edge_order_from_id[edge_id];
}

Entity Graph::get_entitie_from_node_order(int node_order)const{
    return entities[node_order];
}

Relation Graph::get_relation_from_edge_order(int edge_order)const{
    return relations[edge_order];
}

Entity Graph::findPOI(string POI_name)const{
    for(int i=0;i<numVertices;i++)
        if(entities[i].name == POI_name)return entities[i];
     return Entity{-1, -1, "Not Found", "Unknown", "", ""}; 
}

bool Graph::JudgeDataFlow(string type){
    static const unordered_map<string, bool> dataFlowMap = {
        {"accept", false},    // object -> subject
        {"clone", false},     // object -> subject
        {"execve", true},     // subject -> object
        {"read", false},      // object -> subject
        {"recvmsg", false},   // object -> subject
        {"rename_in", false}, // object -> subject
        {"rename_out", true}, // subject -> object
        {"sendmsg", true},    // subject -> object
        {"write", true},      // subject -> object
        {"writev", true}      // subject -> object
    };
    auto it = dataFlowMap.find(type);
    if (it != dataFlowMap.end()) {
        return it->second;  // 返回对应的数据流方向
    } else {
        // 如果找不到该类型，打印错误并返回默认值
        cerr << "Unknown type: " << type << endl;
        return false;  // 默认假设数据流从 object 到 subject
    }
}

void Graph::BuildFromJson(string JsonPath) {
    Json::Value json_tuples;
    Json::CharReaderBuilder reader;
    ifstream file(JsonPath, ifstream::binary);

    if (!file.is_open()) {
        return;
    }

    string line;
    int cnt_vertices = 0;
    int cnt_edges = 0;
    int cnt=0;
    while (getline(file, line)) {

        istringstream stream(line);
        string errs;
        if (!Json::parseFromStream(reader, stream, &json_tuples, &errs)) {
            continue;
        }
        //entity
        if (json_tuples.isObject() && json_tuples.isMember("name")) {
            entities[cnt_vertices].order_node = cnt_vertices;
            entities[cnt_vertices].id = json_tuples["id"].asInt();
            entities[cnt_vertices].name = json_tuples["name"].asString();
            entities[cnt_vertices].type = json_tuples["type"].asString();

            check_node_order_from_id[entities[cnt_vertices].id]=cnt_vertices;

            if (entities[cnt_vertices].type == "process") {
                entities[cnt_vertices].exepath = json_tuples["exepath"].asString();
                entities[cnt_vertices].pid = json_tuples["pid"].asString();
            }
            cnt_vertices++;
        }
        //relation
        else if (json_tuples.isObject() && json_tuples.isMember("subject") && json_tuples.isMember("object")) {
            relations[cnt_edges].order_edge = cnt_edges;
            relations[cnt_edges].id = json_tuples["id"].asInt();
            relations[cnt_edges].subject = json_tuples["subject"].asInt();
            relations[cnt_edges].object = json_tuples["object"].asInt();
            relations[cnt_edges].type = json_tuples["type"].asString();
            relations[cnt_edges].stime = json_tuples["stime"].asString();
            relations[cnt_edges].etime = json_tuples["etime"].asString();

            int subject_order=check_node_order_from_id[relations[cnt_edges].subject];
            int object_order=check_node_order_from_id[relations[cnt_edges].object];
            
            if(JudgeDataFlow(relations[cnt_edges].type)){
                //subject -> object
                inDegree[object_order]++;
                /*
                if(object_order==0){
                    //cout<<"id:"<<relations[cnt_edges].id<<",order:"<<relations[cnt_edges].order_edge<<",subject:"<<relations[cnt_edges].subject<<",object:"<<relations[cnt_edges].object<<endl;
                    //usleep(100000);  // 延迟 100 毫秒（100000 微秒）
                }
                */
                relations[cnt_edges].SubjToObj = true;
            }
            else{
                //object -> subject
                inDegree[subject_order]++;
                /*
                if(subject_order==0){
                    cout<<"id:"<<relations[cnt_edges].id<<",order:"<<relations[cnt_edges].order_edge<<",subject:"<<relations[cnt_edges].subject<<",object:"<<relations[cnt_edges].object<<endl;
                    usleep(100000);  // 延迟 100 毫秒（100000 微秒）
                }
                */
                relations[cnt_edges].SubjToObj = false;
            }
            cnt_edges++;
        }
        cnt++;
        if(cnt%800000==0)cout<<"yes";
    }
    cout << "Total Vertices: " << cnt_vertices << endl;
    cout << "Total Edges: " << cnt_edges << endl;

    file.close();
    register_CSC();
}

//根据Relations构造CSC
void Graph::register_CSC(){
    int* cnt = new int[numVertices]();
    // 构建col_indices数组
    csc_graph.col_indices[0] = 0;
    for(int i=1;i<=numVertices;i++){
        csc_graph.col_indices[i]=csc_graph.col_indices[i-1]+inDegree[i-1];
    }
    // 构建row_indices和edge_weight(edge_weight表示边的order_edge)
    int* current_pos = new int[numVertices]();
    int col,pos,from,to;
    for(int i=0;i<numEdges;i++){
        // 有重边也存了
        if(relations[i].SubjToObj){
            //subject -> object
            to = check_node_order_from_id[relations[i].object];
            from = check_node_order_from_id[relations[i].subject];
            col = to;
            pos = csc_graph.col_indices[col] + current_pos[col];
            /*
            if(to == 2){
                cout<<"object==0:  "<<"from=("<<relations[i].subject<<","<<from<<")  to=("<<relations[i].object<<","<<to<<")  pos=="<<pos<<endl;
            }*/
            csc_graph.row_indices[pos] = from;
            csc_graph.edge_weight[pos] = relations[i].order_edge;
            current_pos[col]++;
        }
        else{
            // object -> subject
            to = check_node_order_from_id[relations[i].subject];
            from = check_node_order_from_id[relations[i].object];
            col = to;
            pos = csc_graph.col_indices[col] + current_pos[col];
            /*
            if(to == 2){
                cout<<"object==0:  "<<"from=("<<relations[i].object<<","<<from<<")  to=("<<relations[i].subject<<","<<to<<")  pos=="<<pos<<endl;
            }*/
            csc_graph.row_indices[pos] = from;
            csc_graph.edge_weight[pos] = relations[i].order_edge;
            current_pos[col]++;
        }
    }
    delete[] current_pos;
    delete[] cnt;
}

void Graph::show()const{
    /*
    for(int i=0;i<50;i++){
        cout<<"id:"<<entities[i].id<<"  name:"<<entities[i].name<<endl;
    }
    cout<<"ENTITY\n";
    for(int i=0;i<50;i++){
        cout<<"id:"<<relations[i].id<<"  subject:"<<relations[i].subject<<"object:"<<relations[i].object<<endl;
    }
    for(int i=0;i<500;i++)cout<<inDegree[i]<<",";
    cout<<endl<<"\n\nDEGREE\n";
    for(int i=0;i<500;i++)cout<<csc_graph.col_indices[i]<<",";
    cout<<endl<<endl<<"COLCOL\n";
    for(int i=0;i<5000;i++)cout<<csc_graph.row_indices[i]<<",";
    cout<<"\n\n"<<"ROWROW\n";
    for(int i=0;i<500;i++)cout<<csc_graph.edge_weight[i]<<",";
    cout<<"\n\n"<<"WEIGHT\n";
   for(int i=csc_graph.col_indices[2];i<csc_graph.col_indices[3];i++){
    cout<<csc_graph.row_indices[i]<<","<<relations[csc_graph.row_indices[i]].id<<endl;
    usleep(10000);
   }
   */
  int temp=0;
    for(int i=csc_graph.col_indices[0];i<csc_graph.col_indices[1];i++){
        temp++;
        cout<<"temp:"<<temp<<endl;
        cout<<"node:"<<csc_graph.row_indices[i]<<","<<entities[csc_graph.row_indices[i]].id<<endl;
        cout<<"edge:"<<csc_graph.edge_weight[i]<<","<<relations[csc_graph.edge_weight[i]].id<<endl;
        usleep(100000);
    }
}

void Graph::find_Influence(set<pair<int,int>> &influnce_set,int this_entity_order,string min_edge_et){

    Entity node = entities[this_entity_order];
    cout << "Entity: node_order=" << node.order_node
              << ", id=" << node.id
              << ", name=" << node.name
              << ", type=" << node.type
              << endl;
    //找出入边即相关信息

    for(int i=csc_graph.col_indices[this_entity_order];i<csc_graph.col_indices[this_entity_order+1];i++){
        int in_vertex_order = csc_graph.row_indices[i];//找到入边node_order
        
        //从edge_weight中取出边的edge_order
        int in_edge_order = csc_graph.edge_weight[i];
        if(relations[in_edge_order].stime < min_edge_et){
            influnce_set.insert({in_vertex_order,in_edge_order});
            string min_et;
            if(relations[in_edge_order].etime > min_edge_et)min_et =min_edge_et;
            else min_et=relations[in_edge_order].etime;
            find_Influence(influnce_set,in_vertex_order,min_et);
        }
    }
    return;
}

