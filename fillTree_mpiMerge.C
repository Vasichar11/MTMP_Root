/// Fill in individually vectors, gather them in master rank and create merged tree
/// Works

#include <iostream>
#include <mpi.h>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "include/functions.h"

#define numEventsD 1e6
UInt_t numEvents = static_cast<UInt_t>(numEventsD);
// define number of processes when executing:
// mpiexec -n <numProcesses> ./fillTree_mpiMerge


void fillTree_mpiMerge(int rank, int size) {

    //ROOT::EnableThreadSafety(); //should it be enabled?
    TStopwatch stopwatch;
    UInt_t event;
    Float_t variable;
    Double_t elapsedFill, elapsedWrite;

    if (rank == 0) stopwatch.Start();

    // Determine start and end indices for each process
    UInt_t eventsPerProcess = numEvents / size;
    int remainder = numEvents % size; // int type to use std::min(const int& a, const int& b )
    UInt_t startEvent = rank * eventsPerProcess + std::min(rank, remainder);
    UInt_t endEvent = startEvent + eventsPerProcess + (rank < remainder ? 1 : 0);

    // Fill the events and variables vectors for this process
    std::vector<UInt_t> events;
    std::vector<Float_t> variables;
    for (UInt_t i = startEvent; i < endEvent; ++i) {
        events.push_back(i);
        variables.push_back(i * 10);
        simulateLoad();
    }

    // Wait for all processes to reach this point
    MPI_Barrier(MPI_COMM_WORLD);
    
    // --Gathering TTree object wouldn't work to merge it later to the mergedTree--
    // Instead gather the data vectors 
    // Gather the event and variable data from each process onto the master
    std::vector<UInt_t> allEvents(numEvents);
    std::vector<Float_t> allVariables(numEvents);
    MPI_Gather(events.data(), events.size(), MPI_UNSIGNED, allEvents.data(), events.size(), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(variables.data(), variables.size(), MPI_FLOAT, allVariables.data(), variables.size(), MPI_FLOAT, 0, MPI_COMM_WORLD);
    // Rank 0 fills tree with the gathered data
    if (rank == 0) {
        TFile *file = new TFile("fillTree_mpiMerge.root", "RECREATE", "fillTree_mpiMerge", 0);
        TTree *mergedTree = new TTree("tree", "Example Tree");
        UInt_t event;
        Float_t variable;
        mergedTree->Branch("event", &event, "event/i");
        mergedTree->Branch("variable", &variable, "variable/F");
        for (int i = 0; i < numEvents; ++i) {
            event = allEvents[i];
            variable = allVariables[i];
            mergedTree->Fill();
        }
        stopwatch.Stop();
        elapsedFill = stopwatch.RealTime() * 1000; // Convert to milliseconds
        std::cout << "Fill data (MPI): " << elapsedFill << " milliseconds" << std::endl;
        file->Write();
        delete mergedTree;
        delete file;
    }
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    fillTree_mpiMerge(rank, size);

    MPI_Finalize();
    return 0;
}
