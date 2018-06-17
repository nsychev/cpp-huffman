#ifndef HUFFMAN_TREE
#define HUFFMAN_TREE

#include <array>
#include <queue>

class huffman_tree {
private:
    typedef std::array<size_t, 256> table;
public:
    struct node {
        virtual bool is_leaf() const = 0;

        virtual void extract_to(table&, size_t, table&, size_t) const = 0;

        virtual ~node() = default;
    };

    struct inner_node : public node {
    private:
        node *left, *right;
    public:
        inner_node() = delete;
        inner_node(node*, node*);

        bool is_leaf() const override;

        node* get_left() const;
        node* get_right() const;

        void extract_to(table&, size_t, table&, size_t) const override;

        ~inner_node() override;
    };

    struct leaf : public node {
    private:
        char symbol;
    public:
        leaf() = delete;
        explicit leaf(char);

        bool is_leaf() const override;

        char get_symbol() const;
        void extract_to(table&, size_t, table&, size_t) const override;

        ~leaf() override = default;
    };

    huffman_tree() = delete;
    explicit huffman_tree(const table&);

    void extract_to(table&, table&) const;
    node *get_root() const;

    ~huffman_tree();
private:
    node *root;

    struct queue_item {
        node *vertex;
        size_t freq;
        size_t step;

        queue_item() = delete;
        queue_item(node*, size_t, size_t);

        bool operator>(const queue_item& other) const;
    };
};

#endif // HUFFMAN_TREE
