#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
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
                             const double param_delta,
                             const vector<Point>& data) {
        // compute LSH parameters: randomly select k bits; use L hash tables
        // P2^k = 1/n, where P2 = 1-cr/d
        // k = -log(n) / log(P2)
        // TODO set k to minimize expected query running time?
        int param_k = static_cast<int>(ceil(-log(param_n) / log(1-static_cast<double>(param_c)*param_r/param_d)));
        assert(param_k > 0 && param_k < 64);    // guarantee that bucket is within int64_t, or perhaps 32-bit is enough for now?
                                                // for n=1M, r=d/4 and c=2, k is 20
                                                // TODO make it more flexible for larger #buckets
        // 1 - (1-P1^k)^L >= 1 - delta, where P1 = 1-r/d
        // L >= log(delta) / log(1 - P1^k)
        // if no delta, a reasonable setting is L = n^\pho = n^(1/c)
        int param_L = static_cast<int>(ceil(log(param_delta) / log(1 - pow(1-static_cast<double>(param_r)/param_d, param_k))));
        assert(param_L > 0);
        cerr << "k = " << param_k << endl
             << "L = " << param_L << endl;

        // initialize hamming projection family
        projection.resize(param_L);
        auto dice = bind(uniform_int_distribution<int>(0, param_d - 1), default_random_engine());
        for (int i {0}; i < param_L; ++i) {
                for (int j {0}; j < param_k; ++j) {
                        projection[i].push_back(dice());
                }
        }

        // add data points (indices) to hash tables
        hash_table.resize(param_L);
        for (int i {0}; i < param_n; ++i) {
                for (int j {0}; j < param_L; ++j) {
                        int64_t bucket {0};
                        for (int k {0}; k < param_k; ++k) {     // AND concatenation of k primitive functions
                                bucket = bucket * 2 + data[i][projection[j][k]];
                        }
                        if (hash_table[j].find(bucket) == hash_table[j].end()) {
                                hash_table[j].emplace(bucket, vector<int>());
                        }
                        hash_table[j][bucket].push_back(i);
                }
        }
}

// return all indices of near neighbors within distance threshold r
vector<int> getNearNeighbors(const Point& point,
                             const int threshold,
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
                if (distance <= threshold)
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
void NearNeighborSearch(const string& data_file,
                        const string& query_file,
                        const int param_r,                              // r-near
                        const int param_c,                              // c-approximate
                        const double param_delta) {                     // failure probability
        const vector<Point> data {readPointsFromFile(data_file)};       // data points
        const vector<Point> query {readPointsFromFile(query_file)};     // query points
        const int param_n {static_cast<int>(data.size())};              // number of data points
        assert(param_n > 0);
        const int param_d {static_cast<int>(data.at(0).size())};        // dimension of points
        assert(param_r > 0);
        assert(param_delta > 0 && param_delta < 1);

        // echo input parameters
        cerr << "r = " << param_r << endl
             << "c = " << param_c << endl
             << "d = " << param_d << endl
             << "n = " << param_n << endl
             << "delta = " << param_delta << endl
             << "#query = " << query.size() << endl;

        // build LSH construction and add data points
        using namespace std::chrono;
        auto build_start = high_resolution_clock::now();
        buildNearNeighborStruct(param_c, param_r, param_d, param_n, param_delta,
                                data);
        auto build_end = high_resolution_clock::now();
        auto build_duration = duration_cast<milliseconds>(build_end - build_start);
        cerr << "Data structure built in " << build_duration.count() << "ms" << endl;

        // query and output results
        auto query_start = high_resolution_clock::now();
        for (int i {0}, sz {static_cast<int>(query.size())}; i < sz; ++i) {
                vector<int> result {getNearNeighbors(query[i], param_r,
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
}

int main(int argc, char* argv[]) {
        if (argc != 5 && argc != 6) {
                cerr << "Usage: " << argv[0] << " R C DataFile QueryFile [SuccessProb]\n"
                     << "       R               retrieve all points within hamming distance R\n"
                     << "       C               approximation factor\n"
                     << "       DataFile        file containing all data points of the same dimension\n"
                     << "                       each point represented as a binary string in a line\n"
                     << "       QueryFile       file containing all query points\n"
                     << "       SuccessProb     (optional) success probability that a r-near neighbor is returned\n"
                     << "                       default success probability is 0.9\n";
                return EXIT_FAILURE;
        }

        const int param_r {stoi(argv[1])};
        const int param_c {stoi(argv[2])};
        const string data_file {argv[3]};
        const string query_file {argv[4]};
        double param_delta {1 - 0.9};           // default success probability 0.9
        if (argc == 6)
                param_delta = 1-stod(argv[5]);

        NearNeighborSearch(data_file, query_file, param_r, param_c, param_delta);

        return EXIT_SUCCESS;
}
