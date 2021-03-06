/**
 * Flann LSH
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <flann/flann.hpp>

using namespace flann;
using namespace std;

void load_from_file(Matrix<float>& dataset, const string& filename) {
  ifstream dfile(filename);
  if (!dfile.is_open()) {
    cout << "unable to open data set file: " << filename << endl;
    exit(1);
  }

  string line;

  int i = 0;
  while (dfile >> line) {
    for (unsigned j = 0; j < line.length(); j++) {
      dataset[i][j] = line[j]-'0';
    }
    i++;
  }

  dfile.close();
}

int main(int argc, char** argv) {
    int d = 10;
    Matrix<float> dataset(new float[16*d], 16, d);
    Matrix<float> query(new float[4*d], 4, d);
    load_from_file(dataset, "data/files/d10nd16");
    load_from_file(query, "data/files/d10nq4");

    Matrix<int> indices(new int[query.rows*d], query.rows, d);
    Matrix<float> dists(new float[query.rows*d], query.rows, d);

    // construct a hierarchical clustering index
    // default paramaters:
    // branching factor: 32
    // centers: random
    // number of parallel trees: 4
    // leaf_max_size: 100
    Index<L2<float>> index(dataset, flann::HierarchicalClusteringIndexParams());
    index.buildIndex();

    // do a radius search, using 128 checks
    // checks : specifies the maximum leafs to visit when searching for neigbors
    int n = index.radiusSearch(query, indices, dists, 3, flann::SearchParams(128));
    cout << "number of nearest neighbors: " << n << endl;

    delete[] dataset.ptr();
    delete[] query.ptr();
    delete[] indices.ptr();
    delete[] dists.ptr();
    
    return 0;
}
