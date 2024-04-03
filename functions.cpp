#include "include/functions.h"

void simulateLoad() {
    double x = 0.0;
    for (UInt_t j = 0; j < 100 ; ++j) {
        x += (std::sin(j) + std::cos(j));
    }
}
/* Avoid random nunmber generations to compare the files for equallity later
void simulateRandomLoad() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 10); // Random delay between 1 and 10 milliseconds
    UInt_t delay = distr(gen); 
    std::this_thread::sleep_for(std::chrono::milliseconds(delay)); 
}
void fillTree(TTree* tree, const UInt_t start, const UInt_t end, UInt_t &local_event, Float_t &local_variable) {
    for (UInt_t i = start; i < end; ++i) {
        local_event = i;
        local_variable = i*10;
        simulateLoad();
        tree->Fill();
    }
}*/
TTree* fillTree(const UInt_t start, const UInt_t end) {
    TTree *tree = new TTree("tree", "Example Tree");
    UInt_t event;
    Float_t variable;
    tree->Branch("event", &event, "event/I");
    tree->Branch("variable", &variable, "variable/F");
    for (UInt_t i = start; i < end; ++i) {
        event = i;
        variable = i*10;
        simulateLoad();
        tree->Fill();
    }
    return tree;
}
void fillTreeThread(const UInt_t start, const UInt_t end, TTree* tree) {
    UInt_t event;
    Float_t variable;
    tree->Branch("event", &event, "event/i");
    tree->Branch("variable", &variable, "variable/F");
    for (UInt_t i = start; i < end; ++i) {
        event = i;
        variable = i*10;
        simulateLoad();
        tree->Fill();
    }
}
void equalTrees(TTree* tree1, TTree* tree2) {
    std::cout << "Comparing trees: "<< tree1->GetName() << " " << tree2->GetName() << std::endl;
    const char* treeName = "tree";
    
    // Write in temporary files and compore
    auto file1 = std::unique_ptr<TFile> {TFile::Open("file1.root", "RECREATE")};
    TTree *t1 = tree1->CloneTree();
    t1->SetName(treeName);
    file1->Write();

    auto file2 = std::unique_ptr<TFile> {TFile::Open("file2.root", "RECREATE")};
    TTree *t2 = tree2->CloneTree();
    t2->SetName(treeName);
    file2->Write();

    std::string cmpr = "root-diff -k " + std::string(treeName) + " file1.root file2.root";
    int result = std::system(cmpr.c_str());
    if (result==0) {
        std::cout <<"Trees identical"<<std::endl;
    }
    else {
        std::cout << "Trees differ" << std::endl;
    }

    // Remove temporary files 
    std::string rmv = "rm -rf file1.root file2.root";
    std::system(rmv.c_str());
}
