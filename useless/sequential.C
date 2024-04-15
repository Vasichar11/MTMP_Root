#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TTreeReader.h"
#include "include/functions.h"

#define numEventsD 1e2
#define numFiles 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 

void sequential1() {
    std::cout<<"\nExercise 1..."<<std::endl;
    TStopwatch stopwatch;
    TFile *file = new TFile("sequential1.root", "RECREATE", "sequential1", 0 ); 

    // 1) Fill tree sequentially
    stopwatch.Start();
    auto tree = fillTree(0, numEvents);
    stopwatch.Stop();
    std::cout << "Fill tree data: " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    
    // 2) Get tree data sequentially
    stopwatch.Start();
    for (UInt_t i = 0; i < numEvents; ++i) {
        tree->GetEntry(i);
    }
    stopwatch.Stop();
    std::cout << "Read tree data: " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;

    // 3) Write data sequentially
    stopwatch.Start();
    file->Write();
    stopwatch.Stop();
    std::cout << "Write data: " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;

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

void sequential2() {
    std::cout<<"\nExercise 2..."<<std::endl;
    TStopwatch stopwatch;

    // 1) Fill histograms sequentially
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
        workItem(Form("sequential2_%u", i), Form("hist_%u", i));
    }
    stopwatch.Stop();
    std::cout << "Fill histo data (sequential): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
}

int main() {
    sequential1(); 
    sequential2(); 
    return 0;
}
