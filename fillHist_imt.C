/// Fill multiple histograms in multiple files sequentially

#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "include/functions.h"

#define numEventsD 1e4
#define numThreads 2
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 


void fillHist_imt() {
    TStopwatch stopwatch;
    ROOT::EnableImplicitMT(numThreads);

    auto workItem = [](const char* fileNamePrefix, const char* histName) {
        TRandom3 rndm;
        TFile f(Form("%s.root", fileNamePrefix), "RECREATE");
        TH1F h(histName, "The Histogram", 64, -4, 4);
        for (UInt_t i = 0; i < numEvents; ++i) {
            h.Fill(rndm.Gaus());
            simulateLoad();
        }
        h.Write();
    };

    stopwatch.Start();
    for (UInt_t i = 0; i < numThreads; ++i) { 
        workItem(Form("fillHist_imt%u", i), Form("hist%u", i));
    }
    stopwatch.Stop();
    std::cout << "Fill histo data (sequential): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
}

int main() {
    fillHist_imt(); 
    return 0;
}