/// Fill in the same tree using OpenMP work sharing with a critical region
/// Incorrect results - Race condition - Fills duplicate entries like in fillTree_locks.C
/// Issue: Concurrent access in conditional thread safe object (TTree) which is not allowed even if we use locks

#include <iostream>
#include <omp.h>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "include/functions.h"

#define numEventsD 1e2
#define numThreads 8
UInt_t numEvents = static_cast<UInt_t>(numEventsD);  

void fillTree_ompCritical() {

    omp_set_num_threads(numThreads);

    // ROOT::EnableThreadSafety(); // We will consider TTree as unsafe object and use locks
    TStopwatch stopwatch;
    TFile *file = new TFile("fillTree_ompCritical.root", "RECREATE", "fillTree_ompCritical", 0);
    TTree *tree = new TTree("tree", "Example Tree");
    UInt_t event;
    Float_t variable;
    Double_t elapsedFill, elapsedWrite;
    tree->Branch("event", &event, "event/i");
    tree->Branch("variable", &variable, "variable/F");
    
    //omp_lock_t lock;
    //omp_init_lock(&lock);
    //std::vector <UInt_t> events(numEvents);
    //std::vector <Float_t> variables(numEvents);
    stopwatch.Start();
    #pragma omp parallel for
    for (UInt_t i = 0; i < numEvents; ++i) {
        UInt_t tid = omp_get_thread_num();
        event = i;
        variable = i * 10;
        simulateLoad();
        //omp_set_lock(&lock); // Tried using omp locks instead, didn't work
        #pragma omp critical  // Only one process can fill per time 
        {
            //events[i]=event; // No race condition if we fill vectors
            //variables[i]=variable; 
            tree->Fill(); // Race condition if we fill the same tree using locks or critical region
            //omp_unset_lock(&lock);
        }

    }

    //omp_destroy_lock(&lock);
    stopwatch.Stop();
    elapsedFill = stopwatch.RealTime() * 1000; // Convert to milliseconds
    std::cout << "Fill data (omp critical): " << elapsedFill << " milliseconds" << std::endl;
    
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
    fillTree_ompCritical(); 
    return 0;
}
