#include "TFile.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "TROOT.h"
#include <iostream>
#include <omp.h>
#include "TStopwatch.h"
#include "include/functions.h"

#define numEventsD 1e2
#define numThreads 8
UInt_t numEvents = static_cast<UInt_t>(numEventsD); // just to define with e notation 

// Fill seperate and merge
void omp1() {

    omp_set_num_threads(numThreads);

    ROOT::EnableThreadSafety();
    TStopwatch stopwatch;
    TFile *file = new TFile("omp1.root", "RECREATE");
    UInt_t event;
    Float_t variable;
    Double_t elapsedFill, elapsedWrite;
    std::vector<UInt_t> events(numThreads);
    std::vector<Float_t> variables(numThreads);
    TList *list = new TList;

    stopwatch.Start();
    // Create separate trees for each thread
    std::vector<TTree*> trees(numThreads);
    for (UInt_t i = 0; i < numThreads; ++i) {
        trees[i] = new TTree(Form("tree_%d", i), "Example Tree");
        trees[i]->Branch("event", &events[i], "event/i");
        trees[i]->Branch("variable", &variables[i], "variable/F");
    }
    // Fill in parallel
    #pragma omp parallel
    {
        UInt_t tid = omp_get_thread_num();
        for (UInt_t i = tid; i < numEvents; i += numThreads) {
            events[tid] = i;
            variables[tid] = i * 10;
            simulateLoad();
            trees[tid]->Fill();
        }
        list->Add(trees[tid]); 
    }
    // Merge 
    TTree *treeM = TTree::MergeTrees(list);
    treeM->SetName("treeM");
    treeM->SetTitle("Example Tree");
    treeM->SetBranchAddress("event", &event); // Change branch address, dealing with clone trees properly. 
    treeM->SetBranchAddress("variable", &variable);
    stopwatch.Stop();
    elapsedFill = stopwatch.RealTime() * 1000; // Convert to milliseconds
    std::cout << "Fill data: " << elapsedFill << " milliseconds" << std::endl;
    
    // Sort to compare for equality
    std::vector<std::pair<UInt_t, Float_t>> eventOrdering; // Create a vector of pairs containing the event number and corresponding ordering value
    for (UInt_t i = 0; i < treeM->GetEntries(); ++i) {
        treeM->GetEntry(i);
        eventOrdering.emplace_back(event, variable); // Assuming 'variable' as the ordering value
    }
    std::sort(eventOrdering.begin(), eventOrdering.end(), [](const auto& a, const auto& b) { 
        return a.first < b.first;
    });
    auto sortedTree = treeM->CloneTree(0);// Clone empty tree
    for (const auto& pair : eventOrdering) {
        event = pair.first;
        variable = pair.second;
        sortedTree->Fill();
    }
    //delete treeM; // Keep only sorted tree

    // Write data sequentially
    stopwatch.Start();
    file->Write();
    elapsedWrite = stopwatch.RealTime() * 1000; // Convert to milliseconds
    stopwatch.Stop();
    std::cout << "Write data: " << elapsedWrite << " milliseconds" << std::endl;
   
    delete file;
}

// Fill using critical point. Does't work. Multiple threads write the same event in next row. Why?
void omp2() {

    omp_set_num_threads(numThreads);

    ROOT::EnableThreadSafety();
    TStopwatch stopwatch;
    TFile *file = new TFile("omp2.root", "RECREATE");
    TTree *tree = new TTree("tree", "Example Tree");
    UInt_t event;
    Float_t variable;
    Double_t elapsedFill, elapsedWrite;
    tree->Branch("event", &event, "event/i");
    tree->Branch("variable", &variable, "variable/F");

    stopwatch.Start();
    #pragma omp parallel for
    for (UInt_t i = 0; i < numEvents; ++i) {
        UInt_t tid = omp_get_thread_num();
        event = i;
        variable = i * 10;
        simulateLoad();
        #pragma omp critical  // Only one process can fill per time 
        tree->Fill();
    }
    stopwatch.Stop();
    elapsedFill = stopwatch.RealTime() * 1000; // Convert to milliseconds
    std::cout << "Fill data: " << elapsedFill << " milliseconds" << std::endl;
    
    // Sort just to compare
    std::vector<std::pair<UInt_t, Float_t>> eventOrdering; // Create a vector of pairs containing the event number and corresponding ordering value
    for (UInt_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        eventOrdering.emplace_back(event, variable); // Assuming 'variable' as the ordering value
    }
    std::sort(eventOrdering.begin(), eventOrdering.end(), [](const auto& a, const auto& b) { 
        return a.first < b.first;
    });
    auto sortedTree = tree->CloneTree(0);// Clone empty tree
    for (const auto& pair : eventOrdering) {
        event = pair.first;
        variable = pair.second;
        sortedTree->Fill();
    }
    //delete tree; // Keep only sorted tree

    // Write data sequentially
    stopwatch.Start();
    file->Write();
    elapsedWrite = stopwatch.RealTime() * 1000; // Convert to milliseconds
    stopwatch.Stop();
    std::cout << "Write data: " << elapsedWrite << " milliseconds" << std::endl;
   
    delete file;
/* RESULT:
    root [2] sortedtree->Scan()
************************************
*    Row   * event.eve * variable. *
************************************
*        0 *         1 *        10 *
*        1 *         2 *        20 *
*        2 *         3 *        30 *
*        3 *         4 *        40 *
*        4 *         5 *        50 *
*        5 *         6 *        60 *
*        6 *         7 *        70 *
*        7 *         8 *        80 *
*        8 *         9 *        90 *
*        9 *        10 *       100 *
*       10 *        11 *       110 *
*       11 *        12 *       120 *
*       12 *        12 *       120 *
*       13 *        12 *       120 *
*       14 *        12 *       120 *
*       15 *        12 *       120 *
*       16 *        14 *       140 *
*       17 *        15 *       150 **/

}


int main() {
    omp1(); 
    omp2(); 
    return 0;
}
