#include "Helper.hpp"
#include "HCTree.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "Usage: ./compress <original_file> <compressed_file>\n" << endl;
    return 0;
  }
  FancyInputStream is(argv[1]);
  FancyOutputStream os(argv[2]);
  if (!is.good()) {
    error("Cannot open original_file!");
  }
  if (!os.good()) {
    error("Cannot open compressed_file!");
  }
  if (is.filesize() == 0) {
    return 0;
  }
  int ch;
  HCTree tree;
  vector<int> freqs;
  freqs.resize(256, 0);
  while ((ch = is.read_byte()) != EOF) {
    freqs[ch] ++;
  }
  tree.build(freqs);
  is.reset();
  os.write_int(is.filesize());
  // output the header
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 24; j++) {
      os.write_bit(freqs[i] >> j & 1);
    }
  }
  // output file info

  int t = 0;
  while ((ch = is.read_byte()) != EOF) {
    tree.encode(ch, os);
    t++;
  }
}
