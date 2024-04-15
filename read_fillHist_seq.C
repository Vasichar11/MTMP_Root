/// Fill histograms while reading from tree sequentially
/// Reference results


#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TTreeReader.h"
#include "include/functions.h"

#define numEventsD 1e6
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 


void read_fillHist_seq() {

    TStopwatch stopwatch;
    TFile *file = new TFile("read_fillHist_seq.root", "RECREATE", "read_fillHist_seq", 0 ); 

    // 1) Fill tree sequentially
    stopwatch.Start();
    auto tree = fillTree(0, numEvents);
    stopwatch.Stop();
    std::cout << "Fill tree data: " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    
    
    // 4) Read and Fill histograms sequentially
    stopwatch.Start();
    TH1F* eventHist = new TH1F("event_dist", "Event Distribution;Event;Count", 100, 0, numEvents);
    TH1F* variableHist = new TH1F("variable_dist", "Variable Distribution;Variable;Count", 100, 0, numEvents*10);
    TTreeReader reader(tree);
    TTreeReaderValue<UInt_t> eventRV(reader, "event");  
    TTreeReaderValue<Float_t> variableRV(reader, "variable");
    while (reader.Next()) {
        UInt_t event = *eventRV;
        Float_t variable = *variableRV;
        eventHist->Fill(event);
        variableHist->Fill(variable);
    }
    stopwatch.Stop();
    // Write  histos  
    eventHist->Write();
    variableHist->Write();

    // Close and delete
    file->Close();
    delete file;
}

int main() {
    read_fillHist_seq(); 
    return 0;
}
