void create_file() {
    TFile *file = new TFile("filename_small.root", "RECREATE");
    TTree *tree = new TTree("treeName", "Tree Title");
    double x;
    tree->Branch("x", &x);
    for (int i = 0; i < 100000000; ++i) {
        x = i * i ;
        tree->Fill();
    }
    tree->Write();

    file->Close();

    delete file; 
}

