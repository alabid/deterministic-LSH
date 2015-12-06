/**
 * Exact Nearest Neigbor by linear scan.
 *
 * Usage: [filename] R data_set_file query_set_file
 */

#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 4) {
    cout << "Usage: " << argv[0] << " R data_set_file query_set_file" << endl;
    exit(1);
  }

  cout << "exact LSH using linear scan!" << endl;

  return 0;
}
