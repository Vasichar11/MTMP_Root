## Compilation
- g++ -o program program.C functions.cpp `root-config --cflags --libs`
- if omp: g++ -o program program.C functions.cpp `root-config --cflags --libs` -fopenmp
- if mpi: mpic++ -o program program.C functions.cpp `root-config --cflags --libs` 

## Execute 5 times and take average
- for i in {1..5}; do ./program ; done

## Exploited to achieve parallelism:
- IMT 
- TTreeProcessorMT
- TProcessExecutor
- ROOT::TThreadedObject
- std::threads w/ locks
- OMP
- MPI

## On operations:
1) TTree::Fill() - Fill same tree in same file (fillTree*.C)
2) TH1::Fill() - Fill multiple histos in multiple files (fillHist*.C)
3) TTreeReader() - Read tree and fill histos (read_fillHist*.C)
4) TTree::GetEntry() - TTree::Get entries from tree (getTree*.C)

## Encoundered issues - race conditions:
- Couldn't write in the same tree using locks 
- Couldn't write in the same tree using omp critical (which uses locks internally)
- Implicit multithreading (IMT) is automatically decided by ROOT. The parameter numthreads is just a suggestion for ROOT. The program can decide to use any number of threads.

## Compare trees for equality
- root-diff package
https://pkg.go.dev/go-hep.org/x/hep/groot/cmd/root-diff
Usage: root-diff [options] a.root b.root

## Results
- .ods or .csv file
- system Memory: 12GB 
- system Processor: Intel© Core™ i5-8250U CPU @ 1.60GHz × 4 
- each execution time (ms) written in the.csv file is the AVG of 5 same executions
- resulting trees were compared for equality

## General advice from experts
- Use IMT (implicit multithreading) with ROOT::EnableImplicitMT 
- If no speedup, do trivial parallelism (different threads-different objects) and merge them in single thread.
- Rare crashes is very problematic, need to ensure that program runs all times!
- The following can result in weird behaviour:
1) Using same TFile from different threads, even by using mutexes(I encountered race conditions).
2) Mixing IMT with explicit user-level multi-threading

https://root-forum.cern.ch/t/multicore-multithreading/32205/4

## TODO:
- exploit TBufferMerger to write in parallel to the same file
- exploit TTreeProcessorMP
- exploit TThreadExecutor
- exploit TBB
