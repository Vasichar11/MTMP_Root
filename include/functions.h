#pragma once
#include <cstdlib>
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <typeinfo>
#include <cmath>
#include <stdexcept>
#include "TFile.h"
#include "TH2F.h"
#include "TTree.h"


// Function to simulate computational load for some iterations
void simulateLoad();

// Function to simulate random computational load
//void simulateRandomLoad();

TTree* fillTree(const UInt_t start, const UInt_t end);

void fillTreeThread(const UInt_t start, const UInt_t end, TTree* tree);

// Function to compare two root trees
void equalTrees(TTree* tree1, TTree* tree2);


