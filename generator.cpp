//
// Created by kostas on 5/10/18.
//

#include "generator.h"

using namespace std;

int main(int argc, char *argv[]){

    if(argc!=4){
        cout<<"type ./generator model[dc|noisy] n outputFile"<<endl;
        return 1;
    }

    string model = argv[1];
    int n = atoi(argv[2]);

    vector<triplet> R;
    if(model == "dc")
        generateDC(n, R);
    else
        generateNOISY(n, R);

    writeTripletsToFile(argv[3],R);

    cout<<R.size()<<" triplets successfully generated and saved in "<<argv[3]<<endl;

}