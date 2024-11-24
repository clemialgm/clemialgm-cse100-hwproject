#include "Helper.hpp"
#include "HCTree.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "Usage: ./uncompress <compressed_file> <uncompressed_file>\n" << endl;
    return 0;
  }
  FancyInputStream is(argv[1]);
  FancyOutputStream os(argv[2]);
  if (!is.good()) {
    error("Cannot open compressed_file!");
  }
  if (!os.good()) {
    error("Cannot open uncompressed_file!");
  }
  if (is.filesize() == 0) {
    return 0;
  }
  int ch;
  HCTree tree;
  vector<int> freqs;
  freqs.resize(256, 0);
  int filesize = is.read_int();
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 24; j++) {
      freqs[i] |= (is.read_bit()) << j;
     }
  }
  tree.build(freqs);
  int count = 0;
  while (count < filesize) {
    ch = tree.decode(is);
    os.write_byte(ch);
    count ++;
  }
}
