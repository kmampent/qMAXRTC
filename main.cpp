//
// Created by kostas on 4/24/18.
//

#include <iostream>
#include "generator.h"
#include "parser.h"
#include "q2.h"
#include "qg.h"

using namespace std;

int main(int argc, char *argv[]){

    if(argc!=4){
        cout<<"type ./qmaxrtc q inputFile [dot|newick|both]"<<endl;
        return 1;
    }

    int n;
    vector<triplet> R;
    parse(argv[2], R, n);
    int q = atoi(argv[1]);
    if(q > n-1){
        cout<<"the value of q must be between 2 and "<<n-1<<endl;
        return 1;
    }

    string outputType = argv[3];

    if(q == 2) {

        vector<int> down, up;
        maxrtc2(R, n, down, up);
        cout << "Theoretical guarantee: "<< (double)4/27<<endl;
        cout << "Performance: "<<scoreMaxrtc2(down, up, R) << endl;

        sortLeafLabels2(down, up);
        if(outputType == "dot") {
            printDotQ2("dot", down, up);
        }
        else if (outputType == "newick"){
            printNewickQ2("newick", down, up);
        }
        else if (outputType == "both"){
            printDotQ2("dot", down, up);
            printNewickQ2("newick", down, up);
        }

    }

    else{

        qgNode* tree = maxrtcq(R, n, q);

        cout << "Theoretical guarantee: "<<getGuarantee(q)<<endl;
        cout << "Performance: "<<scoreMaxrtcQ(tree, R)  << endl;

        sortLeafs(tree, n);

        if(outputType == "dot") {
            printDotQG("dot", tree);
        }
        else if (outputType == "newick"){
            printNewickQG("newick", tree);
        }
        else if(outputType == "both"){
            printDotQG("dot", tree);
            printNewickQG("newick", tree);
        }

    }

    return 0;
}
