## Exploited Methods:
- IMT 
- TTreeProcessorMT
- TProcessExecutor
- std::threads w/ locks
- omp

## On operations:
1) Fill same tree in same file (fillTree*.C)
2) Fill multiple histos in multiple files (fillHist*.C)
3) Read tree and fill histos (read_fillHist*.C)
4) Get entries from tree (getTree*.C)

## Encoundered issues - race conditions:
- Couldn't write in the same tree using locks 
- Couldn't write in the same tree using omp critical (which uses locks internally)

## Compare trees for equality
- root-diff package
Usage: root-diff [options] a.root b.root

## Results



## TODO:
- exploit TBufferMerger to write in parallel to the same file
- exploit TTreeProcessorMP
- profiling
- mpi
