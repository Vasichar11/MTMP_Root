void execT(const char* macroName) {
    TString command = TString::Format("root -l -q '%s'", macroName);
    gSystem->Exec(Form("time %s", command.Data()));
}
