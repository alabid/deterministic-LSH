from pyflann import *
from numpy import *
from numpy.random import *

dataset = rand(10000, 128)
testset = rand(1000, 128)

flann = FLANN()
# algorithm can be 'linear', 'kmeans', 'kdtree', 'composite', or 'autotuned'
result, dists = flann.nn(dataset, testset, 5, algorithm="kdtree",
                         branching=32, iterations=7, checks=16)
print result, dists
