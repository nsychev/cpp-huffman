#include "tree.hpp"

/* huffman_tree */

huffman_tree::huffman_tree(const table& freq) {
    std::priority_queue<queue_item, std::vector<queue_item>, std::greater<>> nodes;

    size_t step = 0;

    for (size_t i = 0; i < 256; i++)
        if (freq[i] > 0)
            nodes.push(queue_item(new leaf(static_cast<char>(i)), freq[i], step++));

    if (nodes.size() == 0) {
        root = nullptr;
        return;
    }

    while (nodes.size() > 1) {
        queue_item left_node = nodes.top();
        nodes.pop();
        queue_item right_node = nodes.top();
        nodes.pop();

        size_t sum_freq = left_node.freq + right_node.freq;
        node *parent = new inner_node(left_node.vertex, right_node.vertex);

        nodes.push(queue_item(parent, sum_freq, step++));
    }

    root = nodes.top().vertex;
}

void huffman_tree::extract_to(table& codes, table& lens) const {
    if (root)
        root->extract_to(codes, 0, lens, 0);
}

huffman_tree::node *huffman_tree::get_root() const {
    return root;
}

huffman_tree::~huffman_tree() {
    delete root;
}

/* inner_node */

huffman_tree::inner_node::inner_node(node *left, node *right):
    left(left), right(right) {}

bool huffman_tree::inner_node::is_leaf() const {
    return false;
}

huffman_tree::node* huffman_tree::inner_node::get_left() const {
    return left;
}

huffman_tree::node* huffman_tree::inner_node::get_right() const {
    return right;
}

void huffman_tree::inner_node::extract_to(table& codes, size_t code, table& lens, size_t len) const {
    left->extract_to(codes, code, lens, len + 1);
    right->extract_to(codes, code + (1U << len), lens, len + 1);
}

huffman_tree::inner_node::~inner_node() {
    delete left;
    delete right;
}

/* leaf */

huffman_tree::leaf::leaf(char sym): symbol(sym) {}

bool huffman_tree::leaf::is_leaf() const {
    return true;
}

char huffman_tree::leaf::get_symbol() const {
    return symbol;
}

void huffman_tree::leaf::extract_to(table& codes, size_t code, table& lens, size_t len) const {
    codes[static_cast<unsigned char>(symbol)] = code;
    lens[static_cast<unsigned char>(symbol)] = len;
}

/* queue_item */
huffman_tree::queue_item::queue_item(huffman_tree::node *vertex, size_t freq, size_t step):
    vertex(vertex), freq(freq), step(step) {}

bool huffman_tree::queue_item::operator>(const queue_item& other) const {
    if (freq != other.freq)
        return freq > other.freq;
    return step < other.step;
}