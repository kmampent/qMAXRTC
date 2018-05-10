//
// Created by kostas on 4/25/18.
//

#ifndef QMAXRTC_QG_H
#define QMAXRTC_QG_H

#include <vector>
#include <random>
#include "common.h"
#include <deque>

using namespace std;

struct qgNode {

    qgNode *parent;
    vector<qgNode*> children;

    int parentPos;
    int depth;
    int DFSid;
    int minDFSid;
    int maxDFSID;
    ulli numLeafsBelow;
    ulli numDiffPairsDiffSubtree;
    ulli numDiffPairsSameSubtree;

    qgNode() {
        maxDFSID = minDFSid = DFSid = 0;
        numLeafsBelow = numDiffPairsSameSubtree = numDiffPairsDiffSubtree = 0;
        parent = nullptr;
        children.clear();
        parentPos = -1;
    }

};

void bstPrintDotAuxQG(qgNode *cur, ofstream &treeFout) {

    if (cur->children.size() == 0){
        treeFout << "    " << cur->DFSid << " [label=\"" << cur->DFSid << "\"];\n";
        return;
    }

    treeFout << "    " << cur->DFSid << " [label=\"" << "" << "\"];\n";
    for(int i=0;i<cur->children.size();i++){
        treeFout << "    " << cur->DFSid << " -> " << cur->children[i]->DFSid << ";\n";
        bstPrintDotAuxQG(cur->children[i], treeFout);
    }

}

void printDotHelperQG(qgNode *cur, ofstream &treeFout) {
    treeFout << "digraph BST {\n";
    treeFout << "    node [fontname=\"Arial\"];\n";

    if (cur == nullptr) {
        treeFout << "\n";
    } else if (cur->children.size() == 0) {
        treeFout << "    " << cur->DFSid << ";\n";
    } else {
        bstPrintDotAuxQG(cur, treeFout);
    }

    treeFout << "}\n";
}

void printDotQG(const char *file, qgNode *root) {

    ofstream treeFout;
    treeFout.open(file, ios_base::out);
    printDotHelperQG(root, treeFout);
    treeFout.close();

}
void printNewickQGhelper(ofstream& fout, qgNode* cur){

    if(cur->children.size() == 0){
        fout<<cur->DFSid;
        return;
    }

    fout<<"(";

    int i;
    for(i=0;i<cur->children.size()-1;i++){
        printNewickQGhelper(fout, cur->children[i]);
        fout<<",";
    }
    printNewickQGhelper(fout, cur->children[i]);
    fout<<")";

}
void printNewickQG(const char* file, qgNode* tree) {

    ofstream fout;
    fout.open(file, ios_base::out);

    printNewickQGhelper(fout, tree);
    fout<<";"<<endl;

    fout.close();

}

qgNode *generateQG(int numLeafs) {

    deque<qgNode *> v;

    for (int i = 0; i < numLeafs; i++) {
        qgNode *nd = new qgNode();
        v.push_back(nd);
    }

    while (v.size() != 1) {
        qgNode *newInode = new qgNode();
        newInode->children.push_back(v[v.size() - 1]);
        newInode->children.push_back(v[v.size() - 2]);
        v[v.size() - 1]->parent = newInode;
        v[v.size() - 1]->parentPos = 0;
        v[v.size() - 2]->parent = newInode;
        v[v.size() - 2]->parentPos = 1;
        v.pop_back();
        v.pop_back();
        v.push_front(newInode);
    }


    return v[0];

}

void computeDownVariables(qgNode *cur, int &curLID, int &curIID, vector<qgNode *> &backboneLeafs, int depth) {

    cur->depth = depth;

    if (cur->children.size() == 0) {
        cur->DFSid = curLID++;
        backboneLeafs.push_back(cur);
        cur->minDFSid = cur->DFSid;
        cur->maxDFSID = cur->DFSid;
        cur->numLeafsBelow = 1;
        return;
    }

    computeDownVariables(cur->children[0], curLID, curIID, backboneLeafs, depth + 1);
    computeDownVariables(cur->children[1], curLID, curIID, backboneLeafs, depth + 1);

    cur->DFSid = curIID++;
    cur->minDFSid = cur->children[0]->minDFSid;
    cur->maxDFSID = cur->children[1]->maxDFSID;
    cur->numLeafsBelow = cur->children[0]->numLeafsBelow + cur->children[1]->numLeafsBelow;

}

void computeUpVariables(qgNode *cur, int q) {


    if (cur->parent != nullptr) {

        if (cur == cur->parent->children[0]) {
            cur->numDiffPairsSameSubtree =
                    cur->parent->numDiffPairsSameSubtree + binom2(cur->parent->children[1]->numLeafsBelow);
        } else {
            cur->numDiffPairsSameSubtree =
                    cur->parent->numDiffPairsSameSubtree + binom2(cur->parent->children[0]->numLeafsBelow);
        }

        if (cur->parent->parent != nullptr) {

            if (cur == cur->parent->children[0]) {
                cur->numDiffPairsDiffSubtree = cur->parent->numDiffPairsDiffSubtree +
                                               cur->parent->children[1]->numLeafsBelow * (q - cur->parent->numLeafsBelow);
            } else {
                cur->numDiffPairsDiffSubtree = cur->parent->numDiffPairsDiffSubtree +
                                               cur->parent->children[0]->numLeafsBelow * (q - cur->parent->numLeafsBelow);
            }

        }

    }

    if(cur->children.size()!=0) {
        computeUpVariables(cur->children[0], q);
        computeUpVariables(cur->children[1], q);
    }

}

qgNode *lca(qgNode *x, qgNode *y) {
    while (x != y) {
        if (x->depth > y->depth) x = x->parent;
        else y = y->parent;
    }
    return x;
}

double prq(triplet t, vector<bool> &hasAssignment, vector<int> &assignment, vector<qgNode *> backboneLeafs, int q, int numLeafsBackBone) {

    if (hasAssignment[t.x] == false && hasAssignment[t.y] == false && hasAssignment[t.z] == false) {
        //return the probability of a triplet being satisfied given no assignment! Basically the formula from the paper.
        return (double) 1 / 3 - 4 / (3 * (double) (q + 1) * (q + 1));
    }

    if (hasAssignment[t.x] == true && hasAssignment[t.y] == false && hasAssignment[t.z] == false) {
        qgNode *xLeaf = backboneLeafs[assignment[t.x]];
        return 1 / ((double) numLeafsBackBone) * 1 / ((double) numLeafsBackBone) *
               (xLeaf->numDiffPairsDiffSubtree + numLeafsBackBone - 1);
    }

    if (hasAssignment[t.x] == false && hasAssignment[t.y] == true && hasAssignment[t.z] == false) {
        qgNode *yLeaf = backboneLeafs[assignment[t.y]];
        return 1 / ((double) numLeafsBackBone) * 1 / ((double) numLeafsBackBone) *
               (yLeaf->numDiffPairsDiffSubtree + numLeafsBackBone - 1);
    }

    if (hasAssignment[t.x] == false && hasAssignment[t.y] == false && hasAssignment[t.z] == true) {
        qgNode *zLeaf = backboneLeafs[assignment[t.z]];
        return (2*zLeaf->numDiffPairsSameSubtree + numLeafsBackBone-1)/(((double) numLeafsBackBone) * ((double) numLeafsBackBone));
    }

    if (hasAssignment[t.x] == true && hasAssignment[t.y] == true && hasAssignment[t.z] == false) {
        qgNode *xLeaf = backboneLeafs[assignment[t.x]];
        qgNode *yLeaf = backboneLeafs[assignment[t.y]];
        qgNode *p = lca(xLeaf, yLeaf);
        return 1 / ((double) numLeafsBackBone) * (numLeafsBackBone - p->numLeafsBelow);
    }

    if (hasAssignment[t.x] == true && hasAssignment[t.y] == false && hasAssignment[t.z] == true) {
        qgNode *xLeaf = backboneLeafs[assignment[t.x]];
        qgNode *zLeaf = backboneLeafs[assignment[t.z]];
        qgNode *p = lca(xLeaf, zLeaf);
        if(p == xLeaf) return 0;

        qgNode *childContainingX = p->children[1];
        if (xLeaf->DFSid >= p->children[0]->minDFSid && xLeaf->DFSid <= p->children[0]->maxDFSID) {
            childContainingX = p->children[0];
        }

        return 1 / ((double) numLeafsBackBone) * (childContainingX->numLeafsBelow);

    }

    if (hasAssignment[t.x] == false && hasAssignment[t.y] == true && hasAssignment[t.z] == true) {
        qgNode *yLeaf = backboneLeafs[assignment[t.y]];
        qgNode *zLeaf = backboneLeafs[assignment[t.z]];

        qgNode *p = lca(yLeaf, zLeaf);
        if(p == yLeaf) return 0;

        qgNode *childContainingY = p->children[1];
        if (yLeaf->DFSid >= p->children[0]->minDFSid && yLeaf->DFSid <= p->children[0]->maxDFSID) {
            childContainingY = p->children[0];
        }

        return 1 / ((double) numLeafsBackBone) * (childContainingY->numLeafsBelow);

    }

    //all leafs have an assignment

    qgNode *xLeaf = backboneLeafs[assignment[t.x]];
    qgNode *yLeaf = backboneLeafs[assignment[t.y]];
    qgNode *zLeaf = backboneLeafs[assignment[t.z]];

    return (lca(xLeaf, zLeaf) == lca(yLeaf, zLeaf)) && (lca(xLeaf, zLeaf) != lca(xLeaf, yLeaf));

}

vector<int> findTripletsQG(qgNode *cur, vector<triplet> &triplets) {

    if (cur->children.size() == 0) {
        vector<int> p(1);
        p[0] = cur->DFSid;
        return p;
    }

    vector<vector<int>> childrenLeafIDs;

    for (int i = 0; i < cur->children.size(); i++) {
        childrenLeafIDs.push_back(findTripletsQG(cur->children[i], triplets));
    }

    int i, j, k, w, z;

    for (w = 0; w < childrenLeafIDs.size(); w++) {
        //picked one subtree. Now look at every pair of leafs in that subtree.
        for (i = 0; i < childrenLeafIDs[w].size(); i++) {
            for (j = i + 1; j < childrenLeafIDs[w].size(); j++) {
                //look at every other subtree that isn't the same as 'w' and make the triplets
                for (k = 0; k < childrenLeafIDs.size(); k++) {
                    if (k != w) {
                        for (z = 0; z < childrenLeafIDs[k].size(); z++) {
                            triplet newTriplet;
                            newTriplet.x = childrenLeafIDs[w][i];
                            newTriplet.y = childrenLeafIDs[w][j];
                            newTriplet.z = childrenLeafIDs[k][z];
                            if (newTriplet.x > newTriplet.y) swap(newTriplet.x, newTriplet.y);
                            triplets.push_back(newTriplet);
                        }
                    }
                }
            }
        }
    }

    //merge all subtrees and return leafs
    for (i = 1; i < childrenLeafIDs.size(); i++) {
        childrenLeafIDs[0].insert(childrenLeafIDs[0].end(), childrenLeafIDs[i].begin(), childrenLeafIDs[i].end());
    }

    return childrenLeafIDs[0];

}

qgNode *contractLeafsBackBone(qgNode *cur, int &nodesToAdd, int n) {

    if (cur->children.size() == 0) {

        if (cur->DFSid>=n) {
            nodesToAdd++;
            delete cur;
            return nullptr;
        }

        return cur;

    }

    if(cur->children[0]->DFSid < n && cur->children.size() == 1) {
        qgNode* ret = cur->children[0];
        delete cur;
        nodesToAdd++;
        return ret;
    }

    if(cur->children[0]->DFSid < n){
        return cur;
    }


    qgNode *left = contractLeafsBackBone(cur->children[0], nodesToAdd, n);
    qgNode *right = contractLeafsBackBone(cur->children[1], nodesToAdd, n);

    if (left == nullptr && right == nullptr) {
        delete cur;
        nodesToAdd++;
        return nullptr;
    }
    if (left == nullptr && right != nullptr) {
        delete cur;
        nodesToAdd++;
        return right;
    }
    if (left != nullptr && right == nullptr) {
        delete cur;
        nodesToAdd++;
        return left;
    }

    cur->children[0] = left;
    cur->children[1] = right;
    left->parent = cur;
    left->parentPos = 0;
    right->parent = cur;
    right->parentPos = 1;

    return cur;

}

bool compareQGNodePointers(const qgNode* a, const qgNode* b){
    return a->DFSid < b->DFSid;
}

void sortLeafLabelsQ(qgNode* cur, int n, int& nil){

    bool ok = false;
    for(int i=0;i<cur->children.size();i++){
        if(cur->children[i]->DFSid < n){
            ok = true;
            break;
        }
    }
    if(ok){

        int i;
        for(i=0;i<cur->children.size();i++) {
            if (cur->children[i]->DFSid < n){
                int tmp = mapNewIn[cur->children[i]->DFSid];
                cur->children[i]->DFSid = tmp;
                if(tmp > nil) nil = tmp;
            }
        }
        sort(cur->children.begin(), cur->children.end(), compareQGNodePointers);

    }

    int i;
    for(i=0;i<cur->children.size();i++){
        if(cur->children[i]->children.size() != 0) {
            sortLeafLabelsQ(cur->children[i], n, nil);
        }
    }

}

void updateIlabels(qgNode* cur, int& next){

    if(cur->children.size() == 0) return;

    cur->DFSid = next++;

    for(int i=0;i<cur->children.size();i++){
        updateIlabels(cur->children[i], next);
    }


}

void sortLeafs(qgNode* tree, int n){

    int nextIlabel = -1;
    sortLeafLabelsQ(tree, n, nextIlabel);
    nextIlabel++;
    updateIlabels(tree, nextIlabel);
}

void addNodes(qgNode* cur, int& nodesAdded, int nodesToAdd, int n, int& iid){

    if(nodesAdded == nodesToAdd) return;

    if(cur->children.size() > 2){

        while(cur->children.size()!=2 && nodesAdded!=nodesToAdd){
            qgNode* left = cur->children[cur->children.size()-1];
            qgNode* right = cur->children[cur->children.size()-2];

            qgNode * newNode = new qgNode();
            newNode->DFSid = iid++;
            newNode->children.push_back(left);
            newNode->children.push_back(right);
            left->parentPos = 0;
            left->parent = newNode;
            right->parent = newNode;
            right->parentPos = 1;

            cur->children.pop_back();
            cur->children.pop_back();
            cur->children.push_back(newNode);

            newNode->parent = cur;
            newNode->parentPos = cur->children.size()-1;
            nodesAdded++;

        }

        return;
    }

    for(int i=0;i<cur->children.size();i++){
        addNodes(cur->children[i], nodesAdded, nodesToAdd, n, iid);
    }

}

double getGuarantee(int q){

    if(q%2 == 0) q = q-1;

    return (double) 1 / 3 - 4 / (3 * (double) (q + 1) * (q + 1));

}

qgNode* maxrtcq(vector<triplet> &R, int n, int q) {

    //build backbone tree
    int wantedQ = q;

    if (q % 2 == 0)
        q = q - 1;

    int numLeafs = (q - 1) / 2 + 1;

    //build a binary tree with q nodes in total.
    vector<qgNode *> backboneLeafs;
    //qgNode* backboneTree = generateQG(q);
    qgNode *backboneTree = generateQG(numLeafs);
    int lid = n;
    int iid = n+numLeafs;
    computeDownVariables(backboneTree, lid, iid, backboneLeafs, 0);
    computeUpVariables(backboneTree, q);

    vector<vector<triplet>> tripletList(n);

    int i;
    for (i = 0; i < R.size(); i++) {
        tripletList[R[i].x].push_back(R[i]);
        tripletList[R[i].y].push_back(R[i]);
        tripletList[R[i].z].push_back(R[i]);
    }

    vector<bool> hasAssignment(n, false);
    vector<int> assignment(n, -1); //each assignment stores id of leaf to be assigned, you can then find the leaf in backboneLeafs vector

    double prev = ((double) 1 / 3 - 4 / (3 * (double) (q + 1) * (q + 1))) * R.size();
    vector<double> nValue(numLeafs);

    int k, j;
    for (i = 0; i < n; i++) {
        for (k = 0; k < numLeafs; k++) {
            nValue[k] = prev;
        }

        for (j = 0; j < tripletList[i].size(); j++) {

            for (k = 0; k < numLeafs; k++) {
                hasAssignment[i] = false;
                nValue[k] = nValue[k] - prq(tripletList[i][j], hasAssignment, assignment, backboneLeafs, q, numLeafs);
                hasAssignment[i] = true;
                assignment[i] = k;
                nValue[k] = nValue[k] + prq(tripletList[i][j], hasAssignment, assignment, backboneLeafs, q, numLeafs);
            }
        }

        hasAssignment[i] = true;
        assignment[i] = 0;
        double curBest = nValue[0];
        prev = curBest;

        for (k = 1; k < numLeafs; k++) {
            if (nValue[k] > curBest) {
                curBest = nValue[k];
                assignment[i] = k;
                prev = curBest;
            }
        }

            //cout<<prev<<endl;
    }

    int nodesToAdd = wantedQ - q;
    //assign leafs to back bone leafs

    for(i=0;i<n;i++){
        qgNode * parent =  backboneLeafs[assignment[i]];
        qgNode * newnode = new qgNode();
        newnode->DFSid = i;
        newnode->parentPos = parent->children.size();
        parent->children.push_back(newnode);
    }

    backboneTree = contractLeafsBackBone(backboneTree, nodesToAdd, n);
    int nodesAdded = 0;
    addNodes(backboneTree, nodesAdded, nodesToAdd, n, iid);

    return backboneTree;
}


double scoreMaxrtcQ(qgNode* tree, vector<triplet>& R){

    vector<triplet> triplets;
    findTripletsQG(tree, triplets);
    return compareTripletsRatio(R, triplets);

}

#endif //QMAXRTC_QG_H
