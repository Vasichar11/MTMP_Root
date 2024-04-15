/// Get tree data in parallel using IMT
/// just by calling ROOT::EnableImplicitMT(numThreads);
/// numThreads is automatically decided by ROOT. Any other value is used just as a hint.
/// Works

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "ROOT/TSeq.hxx"
#include "include/functions.h"

#define numEventsD 1e6
#define numThreads 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void getTree_imt() {
    // Fill as in fillTree_seq.C
    TStopwatch stopwatch;
    TFile *file = new TFile("getTree_imt.root", "RECREATE", "getTree_imt", 0 ); 
    auto tree = fillTree(0, numEvents);
    UInt_t event;
    Float_t variable;
    // Set address to access later
    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("variable", &variable);

    ROOT::EnableImplicitMT(numThreads);
    const auto nEntries = tree->GetEntries();
    stopwatch.Start();
    for (auto i : ROOT::TSeqUL(nEntries)) {
        tree->GetEntry(i);
        // Access to event and variable 
        //std::cout << "event " << event << " variable " << variable << std::endl;
    }
    stopwatch.Stop();
    std::cout << " Read tree data (IMT): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    ROOT::DisableImplicitMT();

    file->Close();
    delete file;
}

int main() {
    getTree_imt(); 
    return 0;
}
