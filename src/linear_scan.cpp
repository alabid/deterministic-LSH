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

using HamT = vector<bool>; // binary features in hamming space

HamT toHamT(const string& line) {
  HamT ham;
  for (unsigned i = 0; i < line.length(); i++) {
    ham.push_back(line[i]-'0' == 1);
  }
  return ham;
}

const string conv_to_string(const HamT& h) {
  string s("");

  for (const bool bit : h) {
    if (bit) s = s + '1';
    else s = s + '0';
  }

  return s;
}

unsigned hamTDistance(const HamT& h1, const HamT& h2) {
  unsigned dist = 0;
  for (unsigned i = 0; i < h1.size(); i++) {
    dist += static_cast<unsigned>(h1[i] != h2[i]);
  }
  return dist;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    cout << "Usage: " << argv[0] << " R data_set_file query_set_file" << endl;
    exit(1);
  }

  int R = stoi(argv[1]);

  string line;
  ifstream dataset(argv[2]);
  if (!dataset.is_open()) {
    cout << "unable to open data set file: " << argv[2] << endl;
    exit(1);
  }
  vector<HamT> datapoints;
  while (getline(dataset, line)) {
    datapoints.push_back(toHamT(line));
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

  for (auto &qpoint : querypoints) {
    int count = 0;
    cout << "NNs (R=" << R << ") for "
         << conv_to_string(qpoint) << " :" << endl;
    for (auto &dpoint : datapoints) {
      if (hamTDistance(qpoint, dpoint) <= R) {
        cout << conv_to_string(dpoint) << endl;
        count++;
      }
    }
    cout << "Total NNs for " << conv_to_string(qpoint)
         << " : " << count << endl;
    // cout << "Total time for R-NN query: " << endl;
  }

  return 0;
}
