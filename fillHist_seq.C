/// Fill multiple histograms in multiple files sequentially
/// Reference results

#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"

#define numEventsD 1e2
#define numFiles 8 
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 


void fillHist_seq() {
    TStopwatch stopwatch;

    auto workItem = [](const char* fileNamePrefix, const char* histName) {
        TRandom3 rndm;
        TFile f(Form("%s.root", fileNamePrefix), "RECREATE");
        TH1F h(histName, "The Histogram", 64, -4, 4);
        for (UInt_t i = 0; i < numEvents; ++i) {
            h.Fill(rndm.Gaus());
        }
        h.Write();
    };

    stopwatch.Start();
    for (UInt_t i = 0; i < numFiles; ++i) { 
        workItem(Form("fillHist_seq%u", i), Form("hist%u", i));
    }
    stopwatch.Stop();
    std::cout << "Fill histo data (sequential): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
}

int main() {
    fillHist_seq(); 
    return 0;
}