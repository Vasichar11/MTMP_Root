#include <iostream>
#include <vector>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TFile.h>
#include <TTree.h>

// Define the data structure to be stored in the TTree
struct EventData {
    float x;
    float y;
    float z;
};

int main() {
    // Enable ROOT's implicit multithreading with specified number of threads
    ROOT::EnableImplicitMT(8); // Specify the number of threads here

    // Create a ROOT file to store the TTree
    TFile file("output.root", "RECREATE");

    // Create a TTree
    TTree tree("tree", "Example Tree");
    
    // Create branches in the TTree
    EventData eventData;
    tree.Branch("x", &eventData.x, "x/F");
    tree.Branch("y", &eventData.y, "y/F");
    tree.Branch("z", &eventData.z, "z/F");

    // Generate some data
    constexpr int kNumEvents = 10e7;
    std::vector<float> xData(kNumEvents);
    std::vector<float> yData(kNumEvents);
    std::vector<float> zData(kNumEvents);
    for (int i = 0; i < kNumEvents; ++i) {
        xData[i] = i * 0.1;
        yData[i] = i * 0.2;
        zData[i] = i * 0.3;
    }

    // Fill the TTree using implicit multithreading
    for (int i = 0; i < kNumEvents; ++i) {
        eventData.x = xData[i];
        eventData.y = yData[i];
        eventData.z = zData[i];
        tree.Fill();
    }

    // Write the TTree to the ROOT file
    tree.Write();
    file.Close();

    std::cout << "Data written to output.root successfully." << std::endl;

    return 0;
}
