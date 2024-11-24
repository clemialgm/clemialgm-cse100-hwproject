#include "HCTree.hpp"

HCTree::~HCTree() {
  deleteTree(root);
}

void HCTree::deleteTree(HCNode* node) {
  if (!node) {
    return;
  }
  deleteTree(node->c0);
  deleteTree(node->c1);
  delete node;
}

void HCTree::build(const vector<int>& freqs) {
  priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;

  for (int i = 0; i < (int)freqs.size(); ++i) {
    if (freqs[i]) {
      HCNode* node = new HCNode(freqs[i], i);
      leaves[i] = node;
      pq.push(node);
    }
  }

  while (pq.size() > 1) {
    HCNode* c0 = pq.top();
    pq.pop();
    HCNode* c1 = pq.top();
    pq.pop();

    HCNode* parent = new HCNode(c0->count + c1->count, c0->symbol);
    parent->c0 = c0;
    parent->c1 = c1;
    c0->p = parent;
    c1->p = parent;

    pq.push(parent);
  }

  if (!pq.empty()) {
    root = pq.top();
  }
}

void HCTree::encode(unsigned char symbol, FancyOutputStream& out) const {
  HCNode* node = leaves[symbol];
  vector<int> bits;
  while (node != root) {
    if (node->p->c0 == node) {
      bits.push_back(0);
    } else {
      bits.push_back(1);
    }
    node = node->p;
  }

  reverse(bits.begin(), bits.end());
  for (int i = 0; i < (int)bits.size(); i++) {
    out.write_bit(bits[i]);
  }
}

unsigned char HCTree::decode(FancyInputStream& in) const {
  HCNode* node = root;

  // Traverse down from root to leaf based on input bits
  while (node->c0 != nullptr || node->c1 != nullptr) {
    int bit = in.read_bit();
    if (bit == 0) {
      node = node->c0;
    } else {
      node = node->c1;
    }
  }

  return node->symbol;
}
