void FillTreeThread(void* arg) {
    TTree *tree = (TTree*)arg;

    // Define variables to be stored in the tree
    int event;
    float energy;

    // Generate and fill some data in the thread
    const int numEvents = 1000000;
    for (int i = 0; i < numEvents; ++i) {
        event = i;
        energy = i * 0.1;
        tree->Fill();
    }
}

void mtWrite() {
    TFile *file = new TFile("threaded.root", "RECREATE");
    TTree *tree = new TTree("tree", "Example Tree");

    // Create branches in the tree
    tree->Branch("event", &event, "event/I");
    tree->Branch("energy", &energy, "energy/F");

    // Create threads for filling the tree
    const int numThreads = 4; 
    TThread *threads[numThreads];
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = new TThread(Form("Thread%d", i), FillTreeThread, (void*)tree);
        threads[i]->Run();
    }

    // Join threads
    for (int i = 0; i < numThreads; ++i) {
        threads[i]->Join();
        delete threads[i];
    }

    // Write the tree to the file and close it
    file->Write();
    file->Close();

    delete file;
}
