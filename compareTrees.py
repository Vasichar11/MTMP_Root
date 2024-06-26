from ROOT import RDataFrame
import numpy as np
import ROOT


## Simple script to compare two trees by converting them to numpy arrays first
## Will not work for all trees!
## root-diff package can compare any tree 

rdf1 = RDataFrame("tree","mt1.root")
rdf2 = RDataFrame("mergedtree","mt1.root")
#convert rdataframes to numpy arrays
numpyArray1 = rdf1.AsNumpy()
numpyArray2 = rdf2.AsNumpy()
print("numpyArray1 type:", type(numpyArray1))
print("numpyArray2 type:", type(numpyArray2))
print("numpyArray1 shape:", np.shape(numpyArray1))
print("numpyArray2 shape:", np.shape(numpyArray2))
np.allclose(numpyArray1, numpyArray2)