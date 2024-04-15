#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "TTreeReader.h"
#include "TMutex.h"
//#include <mutex>
//#include "ROOT/TBufferMerger.hxx"
#include "ROOT/TSeq.hxx"
#include "ROOT/TProcessExecutor.hxx"
#include "ROOT/TThreadedObject.hxx"
#include "ROOT/TTreeProcessorMT.hxx"
#include "Math/Vector4Dfwd.h"
#include "include/functions.h"

#define numEventsD 1e2
#define numThreads 8
const UInt_t numEvents = static_cast<UInt_t>(numEventsD); 
TMutex Mutex; 

void fillTreeLocks(const UInt_t start, const UInt_t end, TTree* tree,  UInt_t &local_event, Float_t &local_variable) {
    std::thread::id this_id = std::this_thread::get_id();

    for (UInt_t i = start; i < end; ++i) {
        local_event = i;
        local_variable = i*10;
        simulateLoad();
        Mutex.lock();
        //std::cout<<"Thread: "<<this_id<<" writes."<<std::endl;
        tree->Fill();
        Mutex.unlock();
    }
    //std::cout<<"Thread: "<<this_id<<" done."<<std::endl;
}

void parallel1() {
    TStopwatch stopwatch;
    std::cout<<"Exercise 1..."<<std::endl;
    TFile *file = new TFile("parallel1.root", "RECREATE", "parallel1", 0 ); 
    ROOT::EnableThreadSafety();
    // 1) Fill tree in parallel

    // ---------------1a) Fill in the same tree using Implicit MultiThreading, as TTree::Fill() is supported for IMT
    ROOT::EnableImplicitMT(numThreads);
    stopwatch.Start();
    auto tree = fillTree(0, numEvents); // is IMT exploited?
    stopwatch.Stop();
    std::cout << "1a) Fill tree data (IMT): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    ROOT::DisableImplicitMT();

    // --------------1b) Fill in the same tree using locks, the conventional way
    // Won't work. Filling duplicates entries like in omp1.C
    // ROOT::EnableThreadSafety(); // TTree is Conditionally thread safe, so we will need to use locks anyways to write in the same tree
    // Data will be unsorted, as threads are performing unordered filling 
    TTree *treeL = new TTree("treeL", "Example Tree");
    std::vector<std::thread> threads;
    UInt_t event;
    Float_t variable;
    treeL->Branch("event", &event, "event/i");
    treeL->Branch("variable", &variable, "variable/F");
    stopwatch.Start();
    for (UInt_t i = 0; i < numThreads; ++i) {
        UInt_t start = i * (numEvents / numThreads); //0 ,50
        UInt_t end = (i + 1) * (numEvents / numThreads); //50, 100
        if (i == numThreads - 1) { 
            end = numEvents; // Last thread handles remainder events
        }
        threads.emplace_back(fillTreeLocks, start, end, treeL, std::ref(event), std::ref(variable));
    }
    for (auto& thread : threads) {
        thread.join();
    }
    stopwatch.Stop();
    std::cout << "1b) Fill tree data (locks): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;

    // -------------1c) Fill individual trees and merge
    threads.clear();
    stopwatch.Start();
    TList *list = new TList;
    for (int i = 0; i < numThreads; ++i) {
        TTree *t = new TTree(Form("tree%d", i+1), Form("Tree %d", i+1));
        UInt_t start = (i * numEvents) / numThreads;
        UInt_t end = ((i + 1) * numEvents) / numThreads;
        threads.emplace_back(fillTreeThread, start, end, t);
        list->Add(t); 
    }
    for (auto& thread : threads) {
        thread.join();
    }
    TTree *treeM = TTree::MergeTrees(list);
    treeM->SetName("treeM");
    treeM->SetTitle("Merged Tree");
    stopwatch.Stop();
    std::cout << "1c) Fill tree data (merging): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;


    // 2) Get tree data in parallel (implicit)
    ROOT::EnableImplicitMT(numThreads);
    const auto nEntries = treeL->GetEntries();
    stopwatch.Start();
    for (auto i : ROOT::TSeqUL(nEntries)) {
        treeL->GetEntry(i);
    }
    stopwatch.Stop();
    std::cout << "2) Read tree data (IMT): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    ROOT::DisableImplicitMT();

    // 3) Write data sequentially
    stopwatch.Start();
    file->Write();
    stopwatch.Stop();
    std::cout << "3) Write data: " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;

    // Compare trees to see if they are equal
    equalTrees(tree, treeM);
    // equalTrees(tree, treeL); //treeL unsorted

    // 4) Read tree and fill histograms in parallel (implicit)
    stopwatch.Start();
    ROOT::TThreadedObject<TH1F> eventHist("event_dist", "Event Distribution;Event;Count", 100, 0, numEvents);
    ROOT::TThreadedObject<TH1F> variableHist("variable_dist", "Variable Distribution;Variable;Count", 100, 0, numEvents*10);
    ROOT::TTreeProcessorMT tp("parallel1.root", "tree");
    auto ReadFill = [&](TTreeReader &myReader) {
        TTreeReaderValue<UInt_t> eventRV(myReader, "event"); // <UInt_t> won't work?
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
    std::cout << "4) Read tree & Fill histos (IMT): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    // Write  histos  
    file->cd();
    eventHistMerged->Write();
    variableHistMerged->Write();

    // Close and delete
    file->Close();
    delete file;

}


void parallel2() {
    TStopwatch stopwatch;
    std::cout<<"\nExercise 2..."<<std::endl;
    
    // 1) Fill different histograms in parallel 
    // ------------------- 1a) Fill in parallel, work pool with TProcessExecutor
    auto workItem1 = [](UInt_t workerID) {
        TRandom3 workerRndm(workerID); 
        TFile f(Form("parallel1_%u.root", workerID), "RECREATE");
        TH1F h(Form("hist1_%u", workerID), "The Histogram", 64, -4, 4);
        for (UInt_t i = 0; i < numEvents; ++i) {
            h.Fill(workerRndm.Gaus());
        }
        h.Write();
        return 0;
    };
    auto workItem2 = [](UInt_t workerID) {
        TRandom3 workerRndm(workerID); 
        TFile f(Form("parallel2_%u.root", workerID), "RECREATE");
        TH1F h(Form("hist2_%u", workerID), "The Histogram", 64, -4, 4);
        for (UInt_t i = 0; i < numEvents; ++i) {
            h.Fill(workerRndm.Gaus());
        }
        h.Write();
        return 0;
    };
    stopwatch.Start();
    ROOT::TProcessExecutor pool1(numThreads); 
    pool1.Map(workItem1, ROOT::TSeq(numThreads));
    stopwatch.Stop();
    std::cout << "Fill histo data (mp): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;
    // ------------------- 1b) Fill concurently, conventional way
    ROOT::EnableThreadSafety();
    stopwatch.Start();
    std::vector<std::thread> pool2;
    for (auto workerID : ROOT::TSeqI(numThreads)) {
        pool2.emplace_back(workItem2, workerID);
    }
    for (auto &&worker : pool2)
        worker.join();
    stopwatch.Stop();
    std::cout << "Fill histo data (mt): " << stopwatch.RealTime() * 1000 << " milliseconds" << std::endl;    
}

int main() {
    parallel1();  
    parallel2();  
    return 0;
}


/*
void mt2_TTreeProcessorMT(const UInt_t numEvents, const int numThreads){

    ROOT::EnableImplicitMT(numThreads);

    TFile *file = new TFile("test.root", "RECREATE");
    ROOT::TThreadedObject<TTree> tree("local_tree", "Local Tree");
    ROOT:

    
}
void FillTreeThread(void* arg) {
    TTree *tree = (TTree*)arg;

    // Generate and fill some data in the thread
    const int numEvents = 1000000;
    for (int i = 0; i < numEvents; ++i) {
        int event = i;
        Float_t variable = i * 0.1;
        tree->Fill();
    }
}
void parallel1(const int numEvents, const int numThreads) {
    // Create the file
    TFile *file = new TFile("threaded.root", "RECREATE");
    if (!file || !file->IsOpen()) {
        std::cerr << "Error: Unable to open file for writing!" << std::endl;
        return;
    }

    // Create the tree and associate it with the file
    TTree *tree = new TTree("tree", "Example Tree");
    if (!tree) {
        std::cerr << "Error: Unable to create tree!" << std::endl;
        delete file;
        return;
    }

    // Create branches in the tree
    int event;
    Float_t variable;
    tree->Branch("event", &event, "event/i");
    tree->Branch("variable", &variable, "variable/F");

    // Create threads for filling the tree
    TThread *threads[numThreads];
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = new TThread(Form("Thread%d", i), FillTreeThread, (void*)tree);
        threads[i]->Run();
    }

    // No need to join threads, they are detached

    // Write the tree to the file and close it
    file->Write();
    file->Close();

    // Clean up
    delete file;
}
*/