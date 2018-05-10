//
// Created by kostas on 4/24/18.
//

#ifndef QMAXRTC_COMMON_H
#define QMAXRTC_COMMON_H
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <tuple>
#define ulli unsigned long long int
using namespace std;

map<int, int> mapInNew;
vector<int> mapNewIn;

struct triplet{

    int x,y,z; //triplet xy|z

};

bool operator<(const triplet &x, const triplet &y) {
    return std::tie(x.x, x.y, x.z) < std::tie(y.x, y.y, y.z);
}

ulli binom3(ulli n) {
    return n * (n - 1) * (n - 2) / 6;
}

ulli binom2(ulli n) {
    return n * (n - 1)/2;
}


template<class bidiiter>
bidiiter random_unique(bidiiter begin, bidiiter end, size_t num_random, mt19937 &gen) {
    size_t left = std::distance(begin, end);
    while (num_random--) {
        bidiiter r = begin;
        uniform_int_distribution<> dist(0, left-1);
        std::advance(r, dist(gen));
        std::swap(*begin, *r);
        ++begin;
        --left;
    }
    return begin;
}

void writeTripletsToFile(const char *file, vector<triplet> &triplets) {

    ofstream fout;
    fout.open(file, ios_base::out);
    int i;
    for (i = 0; i < triplets.size(); i++) {
        fout << triplets[i].x << " " << triplets[i].y << " " << triplets[i].z << endl;
    }
    fout.close();

}

double compareTripletsRatio(vector<triplet> &R, vector<triplet> &triplets) {

    sort(R.begin(), R.end());
    sort(triplets.begin(), triplets.end());
    vector<triplet> v(R.size());
    std::vector<triplet>::iterator it = std::set_intersection(R.begin(), R.end(), triplets.begin(), triplets.end(),
                                                              v.begin());

    return (double)(it - v.begin())/(double)R.size();

}


int compareTripletsSize(vector<triplet> &R, vector<triplet> &triplets) {

    sort(R.begin(), R.end());
    sort(triplets.begin(), triplets.end());
    vector<triplet> v(R.size());
    std::vector<triplet>::iterator it = std::set_intersection(R.begin(), R.end(), triplets.begin(), triplets.end(),
                                                              v.begin());
    return it - v.begin();

}


#endif //QMAXRTC_COMMON_H
