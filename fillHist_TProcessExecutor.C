/// Fill multiple histograms in multiple files, work pool with TProcessExecutor
/// Works

#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "ROOT/TProcessExecutor.hxx"
#include "ROOT/TSeq.hxx"
#include "include/functions.h"

#define numEventsD 1e2
#define numThreads 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void fillHist_TProcessExecutor() {
    TStopwatch stopwatch;

    auto workItem = [](UInt_t workerID) {
        TRandom3 workerRndm(workerID); 
        TFile f(Form("fillHist_TProcessExecutor%u.root", workerID), "RECREATE");
        TH1F h(Form("hist%u", workerID), "The Histogram", 64, -4, 4);
        for (UInt_t i = 0; i < numEvents; ++i) {
            h.Fill(workerRndm.Gaus());
        }
        h.Write();
        return 0;
    };
    stopwatch.Start();
    ROOT::TProcessExecutor pool(numThreads); 
    pool.Map(workItem, ROOT::TSeq(numThreads));
    stopwatch.Stop();
    std::cout << "Fill histo data (TProcessExecutor): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
}

int main() {
    fillHist_TProcessExecutor();  
    return 0;
}