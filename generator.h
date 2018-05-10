//
// Created by kostas on 4/25/18.
//

#ifndef QMAXRTC_GENERATOR_H
#define QMAXRTC_GENERATOR_H

#include "common.h"
#include <vector>
#include <random>
#include <fstream>
#include <set>
#include <chrono>       // std::chrono::system_clock
#include <algorithm>
#include <iostream>
using namespace std;

struct node {

    struct node *parent;
    int parentPos;
    vector<node*> children;
    int key;

    node(){
        parent = nullptr;
        parentPos = -1;
        children.clear();
        key = -1;
    }

};

vector<int> findTriplets(node *cur, vector<triplet> &triplets) {

    if (cur->children.size() == 0) {
        vector<int> p(1);
        p[0] = cur->key;
        return p;
    }

    vector<int> pLeft = findTriplets(cur->children[0], triplets);
    vector<int> pRight = findTriplets(cur->children[1], triplets);

    int i, j, k;
    for (i = 0; i < pLeft.size(); i++) {
        for (j = i + 1; j < pLeft.size(); j++) {
            for (k = 0; k < pRight.size(); k++) {
                triplet newTriplet;
                newTriplet.x = pLeft[i];
                newTriplet.y = pLeft[j];
                newTriplet.z = pRight[k];
                if (newTriplet.x > newTriplet.y) swap(newTriplet.x, newTriplet.y);
                triplets.push_back(newTriplet);
            }
        }
    }

    for (i = 0; i < pRight.size(); i++) {
        for (j = i + 1; j < pRight.size(); j++) {
            for (k = 0; k < pLeft.size(); k++) {
                triplet newTriplet;
                newTriplet.x = pRight[i];
                newTriplet.y = pRight[j];
                newTriplet.z = pLeft[k];
                if (newTriplet.x > newTriplet.y) swap(newTriplet.x, newTriplet.y);
                triplets.push_back(newTriplet);
            }
        }
    }

    pLeft.insert(pLeft.end(), pRight.begin(), pRight.end());
    return pLeft;

}

node *generate(int n) {

    struct edge {
        struct node *cur;
        struct node *parent;
    };

    struct node *root;
    vector<edge> edges;

    int ikey = n;
    int lkey = 0;
    root = new node();
    root->parentPos = -1;
    root->key = lkey++;

    edge e;
    e.cur = root;
    e.parent = nullptr;
    edges.push_back(e);

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());

    while (edges.size() != 2 * n - 1) {

        std::uniform_int_distribution<int> distr(0, edges.size() - 1);
        //pick a random edge to expand with the same probability

        int randomEdgeIndex = distr(generator);
        edge randomEdge = edges[randomEdgeIndex];
        //make one internal node and one leaf
        node *inode = new node();
        node *lnode = new node();
        inode->key = ikey++;
        lnode->key = lkey++;
        inode->parent = randomEdge.parent;
        lnode->parent = inode;
        randomEdge.cur->parent = inode;

        if (randomEdge.cur == root) {

            //by convention assume that  the root edge is a left going edge
            inode->children.push_back(randomEdge.cur);
            randomEdge.cur->parentPos = 0;
            inode->children.push_back(lnode);
            lnode->parentPos = 1;
            root = inode;

        } else {

            if (randomEdge.cur == randomEdge.parent->children[0]) {
                //current node on the edge is the left child of the parent
                inode->parent->children[0] = inode;
                inode->parentPos = 0;
                inode->children.push_back(randomEdge.cur);
                randomEdge.cur->parentPos = 0;
                inode->children.push_back(lnode);
                lnode->parentPos = 1;
            } else {
                //current node on the edge is the right child of the parent
                inode->parent->children[1] = inode;
                inode->parentPos = 1;
                inode->children.push_back(lnode);
                lnode->parentPos = 0;
                inode->children.push_back(randomEdge.cur);
                randomEdge.cur->parentPos = 1;

            }

        }

        edge eUP, eDOWN, eDIRECTION;
        eUP.cur = inode;
        eUP.parent = inode->parent;
        eDOWN.cur = randomEdge.cur;
        eDOWN.parent = inode;
        eDIRECTION.cur = lnode;
        eDIRECTION.parent = inode;

        edges[randomEdgeIndex] = eUP;
        edges.push_back(eDOWN);
        edges.push_back(eDIRECTION);

    }

    return root;

}

node* generateDC(int n, vector<triplet>& R){

    node* tree = generate(n);
    findTriplets(tree, R);
    return tree;

}

void generateNOISY(int n, vector<triplet>& R){

    int noiseSize = n*n;
    vector<int> labels(n);
    int i;
    for (i = 0; i < n; i++) labels[i] = i;
    set<triplet> s;

    //make n random triplets for every label

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());

    std::uniform_int_distribution<int> dist2(0, 1);
    for(i=0;i<n;i++){

        triplet t;
        bool type = dist2(rand_dev) == 0;
        if(type) t.x = i;
        else t.z = i;
        bool cont = true;
        while(cont) {
            //pick two random labels

            bool ok = false;
            while(!ok) {
                random_unique(labels.begin(), labels.end(), 2, generator);
                if(labels[0] == i || labels[1] == i) ok = false;
                else ok = true;
            }
            if (type) {
                t.y = labels[0];
                t.z = labels[1];
            } else {
                t.x = labels[0];
                t.y = labels[1];
            }

            if(t.x > t.y) swap(t.x, t.y);

            if (s.find(t) == s.end()) {
                s.insert(t);
                cont = false;
                R.push_back(t);
            } else {
                cont = true;
            }

        }
    }


    triplet t;
    for(i=R.size();i<noiseSize;i++){
        bool cont = true;
        while(cont) {
            random_unique(labels.begin(), labels.end(), 3, generator);
            t.x = labels[0];
            t.y = labels[1];
            if(t.x > t.y) swap(t.x, t.y);
            t.z = labels[2];
            if(s.find(t) == s.end()){
                cont = false;
                s.insert(t);
                R.push_back(t);
            }
            else{
                cont = true;
            }
        }

    }

}

#endif //QMAXRTC_GENERATOR_H
