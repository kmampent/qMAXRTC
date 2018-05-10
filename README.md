# qMAXRTC

Problem: given a set of triplets R and a parameter q, build a phylogenetic tree with exactly q internal nodes that induces the maximum number of triplets from R.

A sample generator for creating a set R is provided, as well as implementations of various approximations algorithms.

# Compilation

generator: g++ -O3 -std=c++11 -o generator generator.cpp
qmaxrtc: g++ -O3 -std=c++11 -o qmaxrtc main.cpp
