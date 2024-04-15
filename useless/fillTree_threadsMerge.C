/// Fill in individual trees and then merge
/// Doesn't work

#include <iostream>
#include <vector>
#include <thread>
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "include/functions.h"

#define numEventsD 1e2
#define numThreads 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void fillTreeThread(const UInt_t start, const UInt_t end, const UInt_t i, TList& list) {
    TTree *tree = new TTree(Form("tree%d", i+1), Form("Tree %d", i+1));
    UInt_t event;
    Float_t variable;
    tree->Branch("event", &event, "event/i");
    tree->Branch("variable", &variable, "variable/F");
    for (UInt_t j = start; j < end; ++j) {
        event = j;
        variable = j*10;
        simulateLoad();
        tree->Fill();
    }
    list.Add(tree); 
}

void fillTree_merge() {
    TStopwatch stopwatch;
    TFile *file = new TFile("fillTree_merge.root", "RECREATE", "fillTree_merge", 0 ); 
    TList *list = new TList;
    std::vector<std::thread> threads;
    
    stopwatch.Start();
    for (UInt_t i = 0; i < numThreads; ++i) {
        UInt_t start = (i * numEvents) / numThreads;
        UInt_t end = ((i + 1) * numEvents) / numThreads;
        threads.emplace_back(fillTreeThread, start, end, i, std::ref(*list));
    }
    for (auto& thread : threads) {
        thread.join();
    }
    TTree *tree = TTree::MergeTrees(list);
    tree->SetName("tree");
    tree->SetTitle("Merged Tree");
    stopwatch.Stop();
    std::cout << "Fill tree data (merge): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
   
    // Write, close, delete
    file->Write();
    file->Close();
    delete file;
}

int main() {
    fillTree_merge();  
    return 0;
}
