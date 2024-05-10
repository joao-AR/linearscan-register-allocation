#ifndef LIFE_RANGE_H
#define LIFE_RANGE_H
    
    #include <iostream>
    #include <map>
    #include <set>
    using namespace std; 

    struct Life_range_Node {
        int start;
        int end;
        int reg = -1;
        bool spill;
    }; 

    struct Life_range {
        map<int, Life_range_Node> nodes;
        int k;
    };
#endif
