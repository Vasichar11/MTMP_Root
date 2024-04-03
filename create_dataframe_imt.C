{
    ROOT::EnableImplicitMT();
    // Cresate a DataFrame from a ROOT file
    ROOT::RDataFrame df("treeName", "filename.root");

    // Filter the DataFrame
    auto filtered_df = df.Filter([](double x) { return x > 0.5; }, {"x"});

    return 0;
}
