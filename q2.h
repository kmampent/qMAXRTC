//
// Created by kostas on 4/25/18.
//

#ifndef QMAXRTC_Q_H
#define QMAXRTC_Q_H

#include "common.h"

void makeTriplets2(vector<int>& down, vector<int>& up, vector<triplet>& triplets){

    for (int i = 0; i < down.size(); i++) {
        for (int j = i + 1; j < down.size(); j++) {
            triplet t;
            t.x = down[i];
            t.y = down[j];
            if (t.x > t.y) swap(t.x, t.y);
            for (int k = 0; k < up.size(); k++) {
                t.z = up[k];
                triplets.push_back(t);
            }
        }
    }

}

double scoreMaxrtc2(vector<int>& down, vector<int>& up, vector<triplet>& R){

    vector<triplet> triplets;
    makeTriplets2(down, up, triplets);

    return compareTripletsRatio(R, triplets);

}

void printDotQ2(const char* file, vector<int>& down, vector<int>& up){

    ofstream treeFout;
    treeFout.open(file, ios_base::out);

    treeFout << "digraph BST {\n";
    treeFout << "    node [fontname=\"Arial\"];\n";

    int mx = down[0];
    int i;
    for(i=1;i<down.size();i++) if(down[i] > mx ) mx = down[i];
    for(i=0;i<up.size();i++) if(up[i] > mx ) mx = up[i];

    treeFout << "    " << mx+1 << " -> " << mx+2 << ";\n";
    treeFout << "    " << mx+1 << " [label=\""<<""<<"\"];\n";
    treeFout << "    " << mx+2 << " [label=\""<<""<<"\"];\n";

    for(i=0;i<up.size();i++){

        treeFout << "    " << mx+1 << " -> " << up[i] << ";\n";
        treeFout << "    " << up[i] << " [label=\"" << up[i] <<"\"];\n";
    }

    for(i=0;i<down.size();i++){
        treeFout << "    " << mx+2 << " -> " << down[i] << ";\n";
        treeFout << "    " << down[i] << " [label=\"" << down[i] << "\"];\n";
    }

    treeFout << "}\n";

    treeFout.close();

}

void printNewickQ2(const char* file, vector<int> & down, vector<int> & up){

    int i;
    ofstream fout;
    fout.open(file, ios_base::out);

    fout<<"((";
    for(i=0;i<down.size()-1;i++){
        fout<<down[i]<<",";
    }
    fout<<down[i]<<"),";
    for(i=0;i<up.size()-1;i++){
        fout<<up[i]<<",";
    }
    fout<<up[i]<<");"<<endl;

    fout.close();

}

double pr2(triplet t, vector<bool>& hasAssignment, vector<bool>& assignment){

    if((hasAssignment[t.x] && !assignment[t.x]) || (hasAssignment[t.y] && !assignment[t.y]) || (hasAssignment[t.z] && assignment[t.z])) return 0;

    double res = (double)4/27;
    if(hasAssignment[t.x]) res *= (double)3/2;
    if(hasAssignment[t.y]) res *= (double)3/2;
    if(hasAssignment[t.z]) res *= 3;

    return res;

}

void sortLeafLabels2(vector<int>& down, vector<int> & up){

    int i;
    for(i=0;i<down.size();i++){
        down[i] = mapNewIn[down[i]];
    }
    for(i=0;i<up.size();i++){
        up[i] = mapNewIn[up[i]];
    }

    sort(down.begin(), down.end());
    sort(up.begin(), up.end());

}

void maxrtc2(vector<triplet>& R, int n, vector<int>& down, vector<int>& up){

    vector<vector<triplet>> tripletList(n);

    int i;
    for(i=0;i<R.size();i++){
        tripletList[R[i].x].push_back(R[i]);
        tripletList[R[i].y].push_back(R[i]);
        tripletList[R[i].z].push_back(R[i]);
    }

    vector<bool> hasAssignment(n, false);
    vector<bool> assignment(n, false); //true: down, false: up

    double prev = (double)4*R.size()/27; //probability triplet is satisfied given the assignment, but no assignment is initially.

    for(i=0;i<n;i++){
        double downValue = prev;
        double upValue = prev;
        int j;
        for(j=0;j<tripletList[i].size();j++){
            //remove the contribution of triplet[i][j] to cur
            hasAssignment[i] = false;
            downValue -= pr2(tripletList[i][j], hasAssignment, assignment);
            upValue -= pr2(tripletList[i][j], hasAssignment, assignment);
            hasAssignment[i] = true;
            assignment[i] = true;
            downValue += pr2(tripletList[i][j], hasAssignment, assignment);
            assignment[i] = false;
            upValue +=  pr2(tripletList[i][j], hasAssignment, assignment);
        }

        hasAssignment[i] = true;
        if(downValue > upValue){
            assignment[i] = true;
            prev = downValue;
        }
        else{
            assignment[i] = false;
            prev = upValue;
        }
    }

    for(i=0;i<n;i++){
        if(assignment[i]) down.push_back(i);
        else up.push_back(i);
    }

}



#endif //QMAXRTC_Q_H

