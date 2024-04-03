void mpWrite() {
    TFile *file = new TFile("parallel.root", "RECREATE");
    TTree *tree = new TTree("tree", "Example Tree");

    // Define variables to be stored in the tree
    int event;
    float energy;

    // Create branches in the tree
    tree->Branch("event", &event, "event/I");
    tree->Branch("energy", &energy, "energy/F");

    // Generate and fill some data in parallel
    const int numEvents = 1000000;
    #pragma omp parallel for
    for (int i = 0; i < numEvents; ++i) {
        event = i;
        energy = i * 0.1;
        #pragma omp critical
        tree->Fill();
    }

    // Write the tree to the file and close it
    file->Write();
    file->Close();

    delete file;
}
