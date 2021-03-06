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
                             const vector<Point>& data) {
        // compute LSH parameters
        assert(param_r + 1 < 30);                       // TODO larger r requires too much memory
        const int param_L = (1 << (param_r + 1)) - 1;   // use L = 2^(r+1)-1 hash functions
                                                        // assuming cr=log(n), param_c is not used in basic algorithm

        // initialize hamming projection family
        projection.resize(param_L);
        auto dice = bind(uniform_int_distribution<int>(0, param_L), default_random_engine());
        for (int i {1}; i <= param_d; ++i) {
                // m(i) randomly chosen from {0,1}^(r+1)
                int m = dice();
                // use all v in {0,1}^(r+1)\{0}
                for (int v {1}; v <= param_L; ++v) {
                        int p = (m & v);        // bitwise conjunction
                        bool w = false;         // compute parity
                        while (p) {
                                p &= (p - 1);
                                w = !w;
                        }
                        if (w) {
                                projection[v - 1].push_back(i - 1);
                        }
                }
        }

        // add data points (indices) to hash tables
        hash_table.resize(param_L);
        for (int i {0}; i < param_n; ++i) {
                for (int j {0}; j < param_L; ++j) {
                        int64_t bucket {0};
                        for (const auto& k : projection[j]) {
                                bucket = bucket * 2 + data[i][k];       // TODO bucket may overflow
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
                        const int param_c) {                            // c-approximate
        const vector<Point> data {readPointsFromFile(data_file)};       // data points
        const vector<Point> query {readPointsFromFile(query_file)};     // query points
        const int param_n {static_cast<int>(data.size())};              // number of data points
        assert(param_n > 0);
        const int param_d {static_cast<int>(data.at(0).size())};        // dimension of points
        assert(param_r > 0);

        // echo input parameters
        cerr << "r = " << param_r << endl
             << "c = " << param_c << endl
             << "d = " << param_d << endl
             << "n = " << param_n << endl
             << "#query = " << query.size() << endl;

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
        if (argc != 5) {
                cerr << "Usage: " << argv[0] << " R C DataFile QueryFile\n"
                     << "       R               retrieve all points within hamming distance R\n"
                     << "       C               approximation factor\n"
                     << "       DataFile        file containing all data points of the same dimension\n"
                     << "                       each point represented as a binary string in a line\n"
                     << "       QueryFile       file containing all query points\n";
                return EXIT_FAILURE;
        }

        const int param_r {stoi(argv[1])};
        const int param_c {stoi(argv[2])};
        const string data_file {argv[3]};
        const string query_file {argv[4]};

        NearNeighborSearch(data_file, query_file, param_r, param_c);

        return EXIT_SUCCESS;
}
