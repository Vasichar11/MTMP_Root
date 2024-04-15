/// Get tree data sequentially
/// Reference results

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "ROOT/TSeq.hxx"
#include "include/functions.h"

#define numEventsD 1e6
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void getTree_seq() {
    // Fill as in fillTree_seq.C
    TStopwatch stopwatch;
    TFile *file = new TFile("getTree_seq.root", "RECREATE", "getTree_seq", 0 ); 
    auto tree = fillTree(0, numEvents);
    UInt_t event;
    Float_t variable;
    Float_t result;
    // Set address to access later
    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("variable", &variable);

    const auto nEntries = tree->GetEntries();
    stopwatch.Start();
    for (auto i : ROOT::TSeqUL(nEntries)) {
        tree->GetEntry(i);
        result = variable/event;
        if (result!=1) break;
    }
    stopwatch.Stop();
    std::cout << "Read tree data: " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;

    file->Close();
    delete file;
}

int main() {
    getTree_seq(); 
    return 0;
}
