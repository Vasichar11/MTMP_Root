#include "/usr/lib/gcc/x86_64-linux-gnu/9/include/omp.h"
void OMPWrite(const int numEvents, const int numThreads) {

    TFile *file = new TFile("test.root", "RECREATE");
    TTree *tree = new TTree("tree", "Example Tree");

    // Tree variables
    int event;
    float energy;

    // Stopwatch variables
    Double_t elapsedFill, elapsedWrite;
    
    // Branches in the tree
    tree->Branch("event", &event, "event/I");
    tree->Branch("energy", &energy, "energy/F");

    //TRandom *rand = new TRandom();
    
    // Fill data in parallel
    TStopwatch stopwatch;
    stopwatch.Start();
    omp_set_num_threads(4);
    #pragma omp parallel for
    int tid = omp_get_thread_num();
    std::cout << "Thread " << tid << std::endl;
    for (int i = 0; i < numEvents; ++i) {
        event = i;
        energy = i * 0.1;
        //energy = rand->Uniform(0.0, 100.0); 
        #pragma omp critical // Only one process can fill per time
        tree->Fill();
    }
    stopwatch.Stop();
    elapsedFill = stopwatch.RealTime() * 1000; // Convert to milliseconds
    std::cout << "Fill data: " << elapsedFill << " milliseconds" << std::endl;
    
    // Write data sequentially
    stopwatch.Start();
    file->Write();
    file->Close();
    elapsedWrite = stopwatch.RealTime() * 1000; // Convert to milliseconds
    stopwatch.Stop();
    std::cout << "Write data: " << elapsedWrite << " milliseconds" << std::endl;
   
   
    delete file;

}
