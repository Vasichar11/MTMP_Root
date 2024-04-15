/// Fill tree sequentially
/// Reference results

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "include/functions.h"

#define numEventsD 1e6

const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void fillTree_seq() {
    TStopwatch stopwatch;
    TFile *file = new TFile("fillTree_seq.root", "RECREATE", "fillTree_seq", 0 ); 
    stopwatch.Start();
    auto tree = fillTree(0, numEvents);
    stopwatch.Stop();
    std::cout << "Fill tree data (seq): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    // Write, close, delete
    file->Write();
    file->Close();
    delete file;
}

int main() {
    fillTree_seq(); 
    return 0;
}
