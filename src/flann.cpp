/**
 * Flann LSH
 */

#include <flann/flann.hpp>
#include <flann/io/hdf5.h>

#include <stdio.h>

using namespace flann;

int main(int argc, char** argv)
{
    int nn = 3;

    Matrix<float> dataset;
    Matrix<float> query;
    load_from_file(dataset, "data/files/dataset.hdf5","dataset");
    load_from_file(query, "data/files/dataset.hdf5","query");

    Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

    // construct a hierarchical clustering index
    // default paramaters:
    // branching factor: 32
    // centers: random
    // number of parallel trees: 4
    // leaf_max_size: 100
    Index<L2<float> > index(dataset, flann::HierarchicalClusteringIndexParams());
    index.buildIndex();

    // do a knn search, using 128 checks
    // checks : specifies the maximum leafs to visit when searching for neigbors
    index.knnSearch(query, indices, dists, nn, flann::SearchParams(128));

    flann::save_to_file(indices,"data/files/result.hdf5","result");

    delete[] dataset.ptr();
    delete[] query.ptr();
    delete[] indices.ptr();
    delete[] dists.ptr();
    
    return 0;
}
