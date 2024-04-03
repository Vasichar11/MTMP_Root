void seqWrite(const int numEvents) {

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
    
    // Fill data sequentially
    TStopwatch stopwatch;
    stopwatch.Start();
    for (int i = 0; i < numEvents; ++i) {
        event = i;
        energy = i * 0.1;
        //energy = rand->Uniform(0.0, 100.0); 
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
