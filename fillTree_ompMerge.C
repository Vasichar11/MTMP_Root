/// Fill in individual trees using OpenMP parallel region and then merge
/// Works

#include <iostream>
#include <omp.h>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "include/functions.h"

#define numEventsD 1e6
#define numThreads 8
UInt_t numEvents = static_cast<UInt_t>(numEventsD);

// Fill seperate and merge
void fillTree_ompMerge() {

    omp_set_num_threads(numThreads);

    ROOT::EnableThreadSafety();
    TStopwatch stopwatch;
    TFile *file = new TFile("fillTree_ompMerge.root", "RECREATE");
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
    // Every thread has its own tree object which is being filled
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
    TTree *tree = TTree::MergeTrees(list);
    tree->SetName("tree");
    tree->SetTitle("Example Tree");
    tree->SetBranchAddress("event", &event); // Change branch address, dealing with clone trees properly. 
    tree->SetBranchAddress("variable", &variable);
    stopwatch.Stop();
    elapsedFill = stopwatch.RealTime() * 1000; // Convert to milliseconds
    std::cout << "Fill data (OMP + merge): " << elapsedFill << " milliseconds" << std::endl;
    
    // Sort to compare for equality
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
    delete tree; // Keep only sorted tree

    // Write, close, delete
    file->Write();
    file->Close();
    delete file;
}

int main() {
    fillTree_ompMerge(); 
    return 0;
}
