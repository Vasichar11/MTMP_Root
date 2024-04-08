/// Fill multiple histograms in multiple files, conventional way with std::threads
/// Works

#include <iostream>
#include <thread>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "ROOT/TSeq.hxx"
#include "include/functions.h"

#define numEventsD 1e2
#define numThreads 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void fillHist_threads() {
    TStopwatch stopwatch;

    auto workItem = [](UInt_t workerID) {
        TRandom3 workerRndm(workerID); 
        TFile f(Form("fillHist_threads%u.root", workerID), "RECREATE");
        TH1F h(Form("hist%u", workerID), "The Histogram", 64, -4, 4);
        for (UInt_t i = 0; i < numEvents; ++i) {
            h.Fill(workerRndm.Gaus());
        }
        h.Write();
        return 0;
    };
    ROOT::EnableThreadSafety();
    stopwatch.Start();
    std::vector<std::thread> pool;
    for (auto workerID : ROOT::TSeqI(numThreads)) {
        pool.emplace_back(workItem, workerID);
    }
    for (auto &&worker : pool)
        worker.join();
    stopwatch.Stop();
    std::cout << "Fill histo data (threads): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;    
}

int main() {
    fillHist_threads();  
    return 0;
}