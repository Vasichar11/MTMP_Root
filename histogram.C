#include <iostream>
#include <TH1F.h>
#include <TCanvas.h>
#include <TRandom3.h>

void createAndPlotHistogram() {
    // Create a canvas to draw on
    TCanvas *canvas = new TCanvas("canvas", "Histogram Example", 800, 600);

    // Create a histogram
    TH1F *histogram = new TH1F("histogram", "Random Numbers", 100, 0, 10);

    // Generate some random numbers and fill the histogram
    TRandom3 randomGenerator;
    for (int i = 0; i < 1000; ++i) {
        double randomNumber = randomGenerator.Uniform(0, 10); // Generate random number between 0 and 10
        histogram->Fill(randomNumber); // Fill the histogram with random numbers
    }

    // Draw the histogram
    histogram->Draw();

    // Update the canvas
    canvas->Update();

    // Keep the program running until user closes the canvas
    canvas->WaitPrimitive();
}

int main() {
    createAndPlotHistogram();
    return 0;
}
