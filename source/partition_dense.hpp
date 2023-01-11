#pragma once

// the dense partition algorithm
// dense partition:
//       build a trie, each node is a solid rule
//       and, each leaf node is a origin of a sub-problem
// NOTE:
// dense partition has no relation with the dense mode
//                 which is related with what QuadDag we use

// there are two problems in this algorithm:
// 1. if the trie is too inbalanced, we will use too many bits to represent the trie
//    e.g. when the trie is a linked list, we will use n bits
//         though the DAG is very dense with O(n^2) parameter
//         in general, there is no chance that we have so many available bits
// 2. if the trie is too balanced, the DAG may not be dense enough
//    e.g. when the trie is a complete binary tree, we will use O(logn) bits
//         but the DAG is not dense enough with O(nlogn) parameter

// to solve the two problems, we will use a greedy algorithm
// our goal:
// 1. ensure that the trie can satisfy the parameter requirement
// 2. use as few bits as possible

// we have notice that the leaf node has a maximum parameter of MP(n) (calculated by parameter_calculator.hpp)
//                     where MP(n) = O(nlogn), is a convex function
//                     so the parameter is larger when the trie is less balanced
// we have notice that rule count of a sub-problem on a leaf node is limited by the bit width
//                     when the height of trie is h, we have w - h remaining bits (w is the total bit width)
//                     because we need at most 5 bits for a solid layer during the search
//                     so the rule count is limited by about (1 ~ 4/3) * 4^((w - h) / 5)
// from the information above, we design the following algorithm:
// 1. at the beginning, h = 0 (partCount p = 2^h, internal trie node number t = 2^h-1)
// 2. besides the internal trie nodes, we have n - t nodes to be allocated to p sub-problems
// 3. from the formula above, we can calculate the maximum rule count of a sub-problem N
// 4. if N * p >= n - t, we can satisfy the rule count requirement
//    otherwise, the partition fails
// 5. try to allocate the n - t nodes to p sub-problems
//    guarantee that the maximum parameter reaches the requirement

#include "partition_dense_trie.hpp"
#include "partition.hpp"

namespace flowbench {

class DensePartition : public Partition, public Singleton<DensePartition> {
private:
    uint32_t h; // height of the trie

public:
    DensePartition(): Partition(), h(0) {}

    // at the beginning, p = 1 = 2^h
    // if the maximum parameter is too small, we will increase p (multiply 2)
    //    until the maximum parameter reaches the requirement
    // if p is too large (N * p < n - p + 1 or h - 1 > n), return false
    // otherwise, return true
    bool addPartition();

    // export the origins of the sub-problems
    // if the partition is not finished, return false
    // otherwise, return true
    bool exportOrigins(UDRuleSet& finalSet, std::queue<std::unique_ptr<ProblemState>> &origins) const override;
};

bool DensePartition::addPartition() {
    h++;
    if (totalWidth < h) { // we have no bits to represent the trie
        return false;
    }
    double log2N = 0.4 * (totalWidth - h); // N >= 4 ^ (w - h) / 5
    if (n > p - 1 && log2N < std::log2(static_cast<double>(n - p + 1) / p)) { // we use too many bits to represent the trie
        return false;
    }
    if (DensePartitionTrie::getInstance().build(n, h, p, log2N)) { // the trie is too balanced
        return true;
    }
    return addPartition();
}

bool DensePartition::exportOrigins(UDRuleSet& finalSet, std::queue<std::unique_ptr<ProblemState>> &origins) const {
    return DensePartitionTrie::getInstance().exportOrigins(finalSet, origins);
}

}