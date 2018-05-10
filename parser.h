//
// Created by kostas on 4/24/18.
//

#ifndef QMAXRTC_PARSER_H
#define QMAXRTC_PARSER_H

#include "common.h"
#include <set>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/*
 *
 * parse a file containing triplets, assume that each line contains three numbers x y z for the triplet xy|z.
 * No restriction on the numbers for the leaf labels, 32 bit integers should be OK.
 * no triplets of the form xx|z, xy|x, xx|x are allowed in the input => undefined behaviour.
 *
 */

void parse(const char* file, vector<triplet>& R, int& n){

    ifstream fin;
    fin.open(file, ios_base::in);

    n = 0;
    triplet t;
    while(fin >> t.x >> t.y >> t.z){

        R.push_back(t);

        if(mapInNew.find(t.x) == mapInNew.end()){
            mapInNew[t.x] = n++;
        }
        if(mapInNew.find(t.y) == mapInNew.end()){
            mapInNew[t.y] = n++;
        }

        if(mapInNew.find(t.z) == mapInNew.end()){
            mapInNew[t.z] = n++;
        }

    }

    fin.close();

    mapNewIn.resize(n);

    map<int, int>::iterator it = mapInNew.begin();
    while(it!=mapInNew.end()){
        mapNewIn[it->second] = it->first;
        it++;
    }

    int i;
    for(i=0;i<R.size();i++){
        R[i].x = mapInNew[R[i].x];
        R[i].y = mapInNew[R[i].y];
        if (R[i].x > R[i].y) swap(R[i].x, R[i].y);
        R[i].z = mapInNew[R[i].z];
    }

}

#endif //QMAXRTC_PARSER_H
