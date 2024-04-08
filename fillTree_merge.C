/// Fill in individual trees and then merge
/// Correct results but
/// sometimes can give Segmentation violation!

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

void fillTree_merge() {
    TStopwatch stopwatch;
    TFile *file = new TFile("fillTree_merge.root", "RECREATE", "fillTree_merge", 0 ); // 0 is for the compression algorithm to ensure the same compression each time
    TList *list = new TList;
    std::vector<std::thread> threads;
    
    stopwatch.Start();
    for (int i = 0; i < numThreads; ++i) {
        TTree *t = new TTree(Form("tree%d", i+1), Form("Tree %d", i+1));
        UInt_t start = (i * numEvents) / numThreads;
        UInt_t end = ((i + 1) * numEvents) / numThreads;
        threads.emplace_back(fillTreeThread, start, end, t);
        list->Add(t); 
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
