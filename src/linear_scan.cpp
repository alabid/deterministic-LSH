/**
 * Exact Nearest Neigbor by linear scan.
 *
 * Usage: [filename] R data_set_file query_set_file
 */

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

using HamT = long long; // binary features in hamming space

HamT toHamT(const string& line) {
  HamT num = 0;
  int d = line.length();
  for (int i = 0; i < d; i++) {
    if (line[i]-'0' == 1) {
      num += pow(2, d-i-1);
    }
  }
  return num;
}

const string conv_to_string(const HamT h, const int d) {
  string s("");
  HamT num = h;

  for (int i = 0; i < d; i++) {
    char ch;
    if (num % 2 == 0) ch = '0';
    else ch = '1';
    s = ch + s;

    num /= 2;
  }

  return s;
}

int hamTDistance(const HamT h1, const HamT h2) {
  int dist = 0;
  long long distNum = h1 ^ h2;
  while (distNum != 0) {
    distNum &= distNum-1;
    dist++;
  }
  return dist;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    cout << "Usage: " << argv[0] << " R data_set_file query_set_file" << endl;
    exit(1);
  }

  int R = stoi(argv[1]);
  int d = 0;

  string line;
  ifstream dataset(argv[2]);
  if (!dataset.is_open()) {
    cout << "unable to open data set file: " << argv[2] << endl;
    exit(1);
  }
  vector<HamT> datapoints;
  while (getline(dataset, line)) {
    datapoints.push_back(toHamT(line));
    if (d == 0) {
      for (auto ch : line) {
        if (ch == '0' || ch == '1') d++;
      }
    }
  }
  dataset.close();

  ifstream queryset(argv[3]);
  if (!queryset.is_open()) {
    cout << "unable to open query set file: " << argv[3] << endl;
    exit(1);
  }
  vector<HamT> querypoints;
  while (getline(queryset, line)) {
    querypoints.push_back(toHamT(line));
  }
  queryset.close();

  for (auto qpoint : querypoints) {
    int count = 0;
    cout << "NNs (R=" << R << ") for "
         << conv_to_string(qpoint, d) << " :" << endl;
    for (auto dpoint : datapoints) {
      if (hamTDistance(qpoint, dpoint) <= R) {
        cout << conv_to_string(dpoint, d) << endl;
        count++;
      }
    }
    cout << "Total NNs for " << conv_to_string(qpoint, d)
         << " : " << count << endl;
    // cout << "Total time for R-NN query: " << endl;
  }

  return 0;
}
