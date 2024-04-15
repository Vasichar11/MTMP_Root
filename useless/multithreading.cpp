#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
std::mutex mtx;

void simulateRandomLoad() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(50, 200); // Random delay between 50 and 200 milliseconds
    int delay_ms = distr(gen); 
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms)); 
}

void fillDataLocally(TTree* tree, int start, int end) {
    //std::thread::id this_id = std::this_thread::get_id();
    // Tree variables
    int local_event;
    float local_energy;
    for (int i = start; i < end; ++i) {
        local_event = i;
        local_energy = i * 10;
        // Calculations
        simulateRandomLoad();

        mtx.lock();
        //std::cout<<"Thread: "<<this_id<<" writes."<<std::endl;
        tree->Fill();
        mtx.unlock();
    }

}


void mt1(const int numEvents) {
    // Stopwatch variables
    Double_t elapsedFill, elapsedWrite, elapsedRead;
    TStopwatch stopwatch;

    // Fill data in parallel
    ROOT::EnableThreadSafety();
    TFile *file = new TFile("mt1.root", "RECREATE", "mt1", 0);
    TTree *tree = new TTree("tree", "Example Tree");
    int event;
    float energy;
    tree->Branch("event", &event, "event/i");
    tree->Branch("energy", &energy, "energy/F");
    stopwatch.Start();
    std::thread fillThread1(fillDataLocally, tree,  0, int(numEvents/2));
    std::thread fillThread2(fillDataLocally, tree, int(numEvents/2), numEvents);
    fillThread1.join(); 
    fillThread2.join(); 
    //TList *list = new TList;
    //list->Add(fillDataLocally(0, int(numEvents/2)));
    //list->Add(fillDataLocally(int(numEvents/2), numEvents));
    //TTree *mergedtree = TTree::MergeTrees(list);
    //mergedtree->SetName("mergedtree");
    //mergedtree->Write();
    tree->Write();
    file->Write();
    file->Close();
    elapsedFill = stopwatch.RealTime() * 1000; // Convert to milliseconds
    stopwatch.Stop();
    std::cout << "Fill data: " << elapsedFill << " milliseconds" << std::endl;

    // Write data in parallel
    /*stopwatch.Start();
    file->Write();
    elapsedWrite = stopwatch.RealTime() * 1000; // Convert to milliseconds
    stopwatch.Stop();
    std::cout << "Write data: " << elapsedWrite << " milliseconds" << std::endl;

    // Read data in parallel
    stopwatch.Start();
    for (int i = 0; i < numEvents; ++i) {
        tree->GetEntry(i);
    }
    stopwatch.Stop();
    elapsedRead = stopwatch.RealTime() * 1000; // Convert to milliseconds
    std::cout << "Read data: " << elapsedRead << " milliseconds" << std::endl;
    file->Close();
    delete file;
    */

}


int main(int argc, char* argv[]) {
    mt1(std::stod(argv[1])); 
    return 0;
}


/*
void mt2_TTreeProcessorMT(const int numEvents, const int numThreads){

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
        float energy = i * 0.1;
        tree->Fill();
    }
}
void mt1(const int numEvents, const int numThreads) {
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
    float energy;
    tree->Branch("event", &event, "event/i");
    tree->Branch("energy", &energy, "energy/F");

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