# qMAXRTC

Problem: given a set of triplets R and a parameter q, build a phylogenetic tree with exactly q internal nodes that induces the maximum number of triplets from R.

A sample generator for creating a set R is provided, as well as implementations of various approximations algorithms.

# Compilation

generator: g++ -O3 -std=c++11 -o generator generator.cpp

qmaxrtc: g++ -O3 -std=c++11 -o qmaxrtc main.cpp

# Creating Sample Inputs

type ./generator model[dc|noisy] n outputFile

dc: builds a random binary tree T with n leafs following the uniform model, extracts all the triplets from T and saves them in outputFile.

noisy: creates n^2 random triplets on a leaf label set of size n and saves the triplets in outputFile.

# Running qMAXRTC

type ./qmaxrtc q inputFile [dot|newick|both]

q: the number of desired internal nodes.

inputFile: the file containing the triplets. A triplet is described by three numbers x y z, which corresponds to the resolved rooted triplet xy|z. The program should work fine as long as numbers are used to for the leaf labels.

output format: the output of the tree with q internal nodes can be saved in both the newick and dot format.

# Examples
