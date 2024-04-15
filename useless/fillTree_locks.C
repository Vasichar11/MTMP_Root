/// Fill in the same tree using locks, the conventional way
/// Incorrect results  - Race condition - Fills duplicate entries like in fillTree_ompCritical.C
/// Issue: Concurrent access in conditional thread safe object (TTree). This is not allowed even if we use locks

#include <iostream>
#include <vector>
#include <thread>
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TMutex.h"
#include "include/functions.h"

#define numEventsD 1e2
#define numThreads 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 
TMutex Mutex;

void filltreelocks(const UInt_t start, const UInt_t end, TTree* tree,  UInt_t &local_event, Float_t &local_variable) {
    std::thread::id this_id = std::this_thread::get_id();

    for (UInt_t i = start; i < end; ++i) {
        local_event = i;
        local_variable = i*10;
        simulateLoad();
        Mutex.lock();
        //std::cout<<"Thread: "<<this_id<<" writes."<<std::endl;
        tree->Fill();
        Mutex.unlock();
    }
    //std::cout<<"Thread: "<<this_id<<" done."<<std::endl;
}

void fillTree_locks() {
    TStopwatch stopwatch;
    TFile *file = new TFile("fillTree_locks.root", "RECREATE", "fillTree_locks", 0 ); 

    // ROOT::EnableThreadSafety(); no need
    // Since TTree is Conditionally thread safe
    // We will need to use locks anyways to write in the same tree
    TTree *tree = new TTree("tree", "Example Tree");
    std::vector<std::thread> threads;
    UInt_t event;
    Float_t variable;
    tree->Branch("event", &event, "event/i");
    tree->Branch("variable", &variable, "variable/F");
    stopwatch.Start();
    for (UInt_t i = 0; i < numThreads; ++i) {
        UInt_t start = i * (numEvents / numThreads); 
        UInt_t end = (i + 1) * (numEvents / numThreads); 
        if (i == numThreads - 1) { 
            end = numEvents; // Last thread handles remainder events
        }
        threads.emplace_back(filltreelocks, start, end, tree, std::ref(event), std::ref(variable));
    }
    for (auto& thread : threads) {
        thread.join();
    }
    stopwatch.Stop();
    std::cout << "Fill tree data (locks): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    // Data will be unsorted, as threads are performing unordered filling 

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

    // Write, close, delete
    file->Write();
    file->Close();
    delete file;
}

int main() {
    fillTree_locks();  
    return 0;
}
