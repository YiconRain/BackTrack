#ifndef UTILS_H
#define UTILS_H
#include<cstdio>
#include<iostream>
#include<stack>
using namespace std;

// 由于只需找到点边关系，且只需要
struct Entity{
    int order_node;//按顺序第几个出现的点
    int id;
    std::string name;
    std::string type;
    std::string exepath;
    std::string pid;
};

struct Relation{
    int order_edge; //按顺序出现的第几条边
    int id;
    int subject;
    int object;
    std::string type;
    std::string stime;
    std::string etime;
    bool SubjToObj;//True表示数据从subject流向object，false相反
};



#endif