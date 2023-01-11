#pragma once

// a Trie class
// part of the pre-computing algorithm
// we use Instantier to generate the solid rules at first
// then we use Trie to generate the virtual rules
// to generate the complete QuadDag profile

#include <functional>

#include "match_field_lpm.hpp"
#include "int32.hpp"

namespace flowbench {

class Trie {
private:
    class TrieNode {
    public:
        LpmField<Int32> field;
        std::array<std::shared_ptr<TrieNode>, 2> children;

        TrieNode() : field(0, 0) {
            children[0] = nullptr;
            children[1] = nullptr;
        }

        TrieNode(const Int32& prefix, uint8_t prefixLength) : field(prefix, prefixLength) {
            children[0] = nullptr;
            children[1] = nullptr;
        }
    };

    std::shared_ptr<TrieNode> root;

public:
    Trie(): root(std::make_shared<TrieNode>()) {}

    Trie(const Trie& other) = delete;
    Trie(Trie&& other) = default;
    Trie& operator=(const Trie& other) = delete;
    Trie& operator=(Trie&& other) = default;

    void clear() {
        root = std::make_shared<TrieNode>();
    }
    
public:
    // insert a field into the Trie
    void insert(const LpmField<Int32>& field);

    // traverse all the leaves
    void traverse(std::function<void(const LpmField<Int32>&)> callback) const {
        traverse(root, callback);
    }

private:
    void traverse(std::shared_ptr<TrieNode> node, std::function<void(const LpmField<Int32>&)> callback) const;
};

void Trie::insert(const LpmField<Int32>& field) {
    auto node = root;
    Int32 prefix = 0, mask = getHighestBitOf<Int32>();
    for (uint8_t i = 1; i <= field.getPrefixLength(); i++) {
        if (node->children[0] == nullptr) {
            node->children[0] = std::make_shared<TrieNode>(prefix, i);
        }
        if (node->children[1] == nullptr) {
            node->children[1] = std::make_shared<TrieNode>(prefix | mask, i);
        }
        if (!(field.getPrefix() & mask).isZero()) {
            prefix |= mask;
            node = node->children[1];
        } else {
            node = node->children[0];
        }
        mask >>= 1;
    }
}

void Trie::traverse(std::shared_ptr<TrieNode> node, std::function<void(const LpmField<Int32>&)> callback) const {
    if (node->children[0] == nullptr && node->children[1] == nullptr) {
        callback(node->field);
    } else {
        traverse(node->children[0], callback);
        traverse(node->children[1], callback);
    }
}


}