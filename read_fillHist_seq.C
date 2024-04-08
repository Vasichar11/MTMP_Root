/// Read from tree and fill histograms 
/// Error


#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TTreeReader.h"
#include "include/functions.h"

#define numEventsD 1e2
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 


void read_fillHist_seq() {

    TStopwatch stopwatch;
    // Write the data that will be read later
    TFile *file1 = new TFile("fillTree_seq.root", "RECREATE", "fillTree_seq", 0 ); // 0 is for the compression algorithm to ensure the same compression each time
    auto tree = fillTree(0, numEvents);
    // Write, close, delete
    file1->Write();
    file1->Close();
    delete file1;
    
    // Read and Fill histograms sequentially
    stopwatch.Start();
    TH1F* eventHist = new TH1F("event_dist", "Event Distribution;Event;Count", 100, 0, numEvents);
    TH1F* variableHist = new TH1F("variable_dist", "Variable Distribution;Variable;Count", 100, 0, numEvents*10);
    TTreeReader reader(tree);
    TTreeReaderValue<UInt_t> eventRV(reader, "event");  // <UInt_t> won't work?
    TTreeReaderValue<Float_t> variableRV(reader, "variable");
    
    while (reader.Next()) {
        UInt_t event = *eventRV;
        Float_t variable = *variableRV;
        eventHist->Fill(event);
        variableHist->Fill(variable);
    }
    stopwatch.Stop();
    std::cout << "Read & Fill histos (seq): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;

    // Write  histos  
    TFile *file2 = new TFile("read_fillHist_imt.root", "RECREATE", "read_fillHist_imt", 0 ); // 0 is for the compression algorithm to ensure the same compression each time
    file2->cd();
    eventHist->Write();
    variableHist->Write();

    // Write, close, delete
    file2->Write();
    file2->Close();
    delete file2;
}

int main() {
    read_fillHist_seq(); 
    return 0;
}
