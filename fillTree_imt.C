/// Fill in the same tree in parallel just by enabling IMT
/// just by calling ROOT::EnableImplicitMT(numThreads);
/// All threads spawned but one has increased load
/// numThreads is automatically decided by ROOT. Any other value is used just as a hint.
/// Works



#include <iostream>
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "include/functions.h"

#define numEventsD 1e6
#define numThreads 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void fillTrees_imt() {
    TStopwatch stopwatch;
    TFile *file = new TFile("fillTrees_imt.root", "RECREATE", "fillTrees_imt", 0 ); 
    ROOT::EnableImplicitMT(numThreads);

    stopwatch.Start();
    auto tree = fillTree(0, numEvents); // is IMT exploited?
    stopwatch.Stop();
    
    std::cout << "Fill tree data (IMT): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    ROOT::DisableImplicitMT();

    // Write, close, delete
    file->Write();
    file->Close();
    delete file;
}

int main() {
    fillTrees_imt();  
    return 0;
}
