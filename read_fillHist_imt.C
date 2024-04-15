/// Fill histograms while reading from tree in parallel using IMT
/// With the help of ROOT::TThreadedObject and ROOT::TTreeProcessorMT
/// numThreads is automatically decided by ROOT. Any other value is used just as a hint.
/// Works

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TTreeReader.h"
#include "TH1F.h"
#include "ROOT/TThreadedObject.hxx"
#include "ROOT/TTreeProcessorMT.hxx"
#include "include/functions.h"

#define numEventsD 1e6
#define numThreads 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void read_fillHist_imt() {
    // Write the data that will be read later
    TFile *file1 = new TFile("read_fillHist_imt.root", "RECREATE", "read_fillHist_imt", 0 ); 
    auto tree = fillTree(0, numEvents);
    // Write, close, delete
    file1->Write();
    file1->Close();
    delete file1;

    // Read and Fill histograms in parallel
    //ROOT::EnableImplicitMT(numThreads);
    TStopwatch stopwatch;
    stopwatch.Start();
    ROOT::TThreadedObject<TH1F> eventHist("event_dist", "Event Distribution;Event;Count", 100, 0, numEvents);
    ROOT::TThreadedObject<TH1F> variableHist("variable_dist", "Variable Distribution;Variable;Count", 100, 0, numEvents*10);
    ROOT::TTreeProcessorMT tp("read_fillHist_imt.root", "tree");
    auto ReadFill = [&](TTreeReader &myReader) {
        TTreeReaderValue<UInt_t> eventRV(myReader, "event"); 
        TTreeReaderValue<Float_t> variableRV(myReader, "variable");
        auto myEventHist = eventHist.Get();
        auto myVariableHist = variableHist.Get();
        while (myReader.Next()) {
            UInt_t event = *eventRV;
            Float_t variable = *variableRV;
            myEventHist->Fill(event);
            myVariableHist->Fill(variable);
        }
    };
    tp.Process(ReadFill);
    auto eventHistMerged = eventHist.Merge();
    auto variableHistMerged = variableHist.Merge();
    stopwatch.Stop();
    std::cout << "Read tree & Fill histos (imt): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    
    // Write  histos  
    TFile *file2 = new TFile("read_fillHist_imt.root", "RECREATE", "read_fillHist_imt", 0 ); 
    file2->cd();
    eventHistMerged->Write();
    variableHistMerged->Write();

    // Write, close, delete
    file2->Write();
    file2->Close();
    delete file2;
}

int main() {
    read_fillHist_imt(); 
    return 0;
}
