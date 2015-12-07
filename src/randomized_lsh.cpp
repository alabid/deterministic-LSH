#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

using Point = vector<bool>;

// LSH data structure
vector<vector<int>> projection;         // random projection family
vector<unordered_map<int64_t, vector<int>>> hash_table;

// build LSH constructions from input data points
void buildNearNeighborStruct(const int param_c,
                             const int param_r,
                             const int param_d,
                             const int param_n,
                             const vector<Point>& data) {
        // compute LSH parameters: randomly select k bits; use L hash tables
        // P2 = 1-cr/d, and P2^k = 1/n
        int param_k = static_cast<int>(ceil(log(1.0/param_n) / log(1-static_cast<double>(param_c)*param_r/param_d)));
        assert(param_k < 64);           // guarantee that bucket is within int64_t, or perhaps 32-bit is enough for now?
                                        // for n=1M, r=d/4 and c=2, k is 20
                                        // TODO make it more flexible for larger #buckets
        // L = n^\pho = n^(1/c)
        int param_L = static_cast<int>(ceil(pow(param_n, 1.0/param_c)));
        //cerr << "k = " << param_k << endl;
        //cerr << "L = " << param_L << endl;

        // initialize hamming projection family
        projection.resize(param_L);
        vector<int> dimension;
        for (int i {1}; i <= param_d; ++i)
                dimension.push_back(i);
        for (int i {0}; i < param_L; ++i) {
                random_shuffle(dimension.begin(), dimension.end());     // TODO use faster knuth shuffle?
                for (int j {0}; j < param_k; ++j) {
                        projection[i].push_back(dimension[j]);
                }
        }

        // add data points (indices) to hash tables
        hash_table.resize(param_L);
        for (int i {0}; i < param_n; ++i) {
                for (int j {0}; j < param_L; ++j) {
                        int64_t bucket {0};
                        for (int k {0}; k < param_k; ++k) {
                                bucket = bucket * 2 + data[i][projection[j][k]];
                        }
                        if (hash_table[j].find(bucket) == hash_table[j].end()) {
                                hash_table[j].emplace(bucket, vector<int>());
                        }
                        hash_table[j][bucket].push_back(i);
                }
        }
}

// return all indices of near neighbors within distance r
vector<int> getNearNeighbors(const Point& point,
                             const int distance_threshold,
                             const vector<Point>& data) {
        unordered_set<int> candidates;
        for (int i {0}, L {static_cast<int>(projection.size())}; i < L; ++i) {
                int64_t bucket {0};
                for (const auto& j : projection[i]) {
                        bucket = bucket * 2 + point[j];
                }
                if (hash_table[i].find(bucket) == hash_table[i].end()) {
                        continue;
                }
                candidates.insert(hash_table[i][bucket].begin(), hash_table[i][bucket].end());
        }

        // validate if near neighbors are within r
        vector<int> result;
        for (const auto& j : candidates) {
                int distance {0};
                for (int i {0}, d {static_cast<int>(point.size())}; i < d; ++i)
                        distance += (point[i] != data[j][i]);
                if (distance <= distance_threshold)
                        result.push_back(j);
        }
        return result;
}

// convert from bit string to bit vector
Point toPoint(const string& s) {
        vector<bool> point(s.length());
        for (int i {0}; s[i]; ++i) {
                point[i] = s[i] - '0';
        }
        return point;
}

// convert from bit vector to bit string
string toString(const Point& point) {
        string s(point.size(), '0');
        for (int i {0}; s[i]; ++i) {
                if (point[i])
                        s[i] = '1';
        }
        return s;
}

// read points from file, where each line is a point in hamming space
// each point is represented by a bit string of 0 and 1, deliminated by new lines
// assuming all points have same dimension, i.e. the length of the bit string
vector<Point> readPointsFromFile(const string& file) {
        ifstream fin {file};
        assert(fin.is_open());

        vector<Point> points;
        string point_str;
        while (fin >> point_str) {
                points.push_back(toPoint(point_str));   // no checking for point dimension
        }
        return points;
}

// perform r-near neighbor search
int main(int argc, char* argv[]) {
        assert(argc == 4);

        const int param_c {2};                                          // c-approximate
        const int param_r {stoi(argv[1])};                              // r-near
        const vector<Point> data {readPointsFromFile(argv[2])};         // data points
        const vector<Point> query {readPointsFromFile(argv[3])};        // query points
        const int param_n {static_cast<int>(data.size())};              // number of data points
        assert(param_n > 0);
        const int param_d {static_cast<int>(data.at(0).size())};        // dimension of points

        // echo input parameters
        // TODO add parameter \theta for success probability in randomized LSH
        cerr << "c = " << param_c << endl;
        cerr << "r = " << param_r << endl;
        cerr << "d = " << param_d << endl;
        cerr << "n = " << param_n << endl;
        cerr << "#query = " << query.size() << endl;

        // build LSH construction and add data points
        using namespace std::chrono;
        auto build_start = high_resolution_clock::now();
        buildNearNeighborStruct(param_c, param_r, param_d, param_n,
                                data);
        auto build_end = high_resolution_clock::now();
        auto build_duration = duration_cast<milliseconds>(build_end - build_start);
        cerr << "Data structure built in " << build_duration.count() << "ms" << endl;

        // query and output results
        auto query_start = high_resolution_clock::now();
        const int distance = param_r;
        for (int i {0}, sz {static_cast<int>(query.size())}; i < sz; ++i) {
                vector<int> result {getNearNeighbors(query[i], distance,
                                                     data)};  // result is a vector of index for points in data

                // TODO should disable output for measuring query performance
                cout << "Query point " << i << ": found " << result.size() << " NNs\n";
                for (const auto& p : result) {
                        cout << toString(data.at(p)) << '\n';
                }
        }
        auto query_end = high_resolution_clock::now();
        auto query_duration = duration_cast<milliseconds>(query_end - query_start);
        cerr << "Querying completed in " << query_duration.count() << "ms" << endl;

        return EXIT_SUCCESS;
}