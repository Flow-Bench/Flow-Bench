#pragma once

// dense partition trie
// used in dense partition algorithm
// we use a trie to solve the problem:
//    given n, h and N, build a trie
//       whose height is h
//       whose internal node represents a solid rule
//       whose leaf node represents a sub-problem
//    and allocate n to the nodes
//    and caculate the maxmimum parameter of the trie

#include <queue>

#include "parameter_calculator.hpp"
#include "rule_set.hpp"
#include "problem_state.hpp"
#include "rule_splitter.hpp"

namespace flowbench {

class DensePartitionTrie : public Singleton<DensePartitionTrie> {
private:
    class TrieNode {
    public:
        std::array<std::shared_ptr<TrieNode>, 2> children;
        uint32_t count;
        bool isLeaf;

        TrieNode() : count(0), isLeaf(false) {
            children[0] = nullptr;
            children[1] = nullptr;
        }

        TrieNode(uint32_t count) : count(count), isLeaf(false) {
            children[0] = nullptr;
            children[1] = nullptr;
        }
    };

    std::shared_ptr<TrieNode> root;

public:
    DensePartitionTrie() : root(nullptr) {}
    
    // return false when:
    // 1. node count is too small (the node count n is smaller than any h-height trie)
    // 2. node count is too large (the node count n is larger than any h-height trie)
    // 3. parameter  is too small (the maximum parameter is smaller than p)
    // 4. parameter  is too large (the minimum parameter is larger than p)
    // otherwise, return true
    bool build(uint32_t n, uint32_t h, uint32_t p, double log2N);

private:
    // build a trie node recursively
    // node: the node to be built
    // n   : the remaining rule count
    // h   : the height of the trie
    // depth: the depth of the node
    // log2N: log2 (the maximum rule count of a sub-problem) (N may be too large to be stored in a uint32_t)
    // return the node count of the trie
    uint32_t buildTrieNode(std::shared_ptr<TrieNode>& node, uint32_t& n, uint32_t h, uint32_t depth, double log2N);

private:
    // the attributes of the trie
    uint32_t leafParamter = 0;      // the sum of the parameters of the leaf nodes (max)
    uint32_t internalParameter = 0; // the sum of the parameters of the internal nodes (fixed)
    uint32_t leafCount = 0;         // the count of leaf nodes
    uint32_t internalCount = 0;     // the count of internal nodes
    uint32_t largePart = 0;         // number of rules in large partitions
    uint32_t smallPart = 0;         // number of rules in small partitions
    uint32_t largeCount = 0;        // the count of large partitions
    uint32_t smallCount = 0;        // the count of small partitions

    // clear the attributes (reset as 0)
    void clearAttributes() {
        leafParamter = internalParameter = 0;
        leafCount = internalCount = 0;
        largePart = smallPart = largeCount = smallCount = 0;
    }

private:
    // we have 3 partitions:
    // 1. full large: the larger partitions of "full" leaf nodes
    // 2. full small: the smaller partitions of "full" leaf nodes
    // 3. partial   : the partitions of "partial" leaf nodes
    uint32_t fullLargeCount = 0;    // the count of full large partitions
    uint32_t fullSmallCount = 0;    // the count of full small partitions
    uint32_t partialCount = 0;      // the count of partial partitions
    uint32_t fullLargeParameter = 0;// the parameter of full large partitions
    uint32_t fullSmallParameter = 0;// the parameter of full small partitions
    uint32_t partialParameter = 0;  // the parameter of partial partitions

    // clear the partition attributes (reset as 0)
    void clearPartitionAttributes() {
        fullLargeCount = fullSmallCount = partialCount = 0;
        fullLargeParameter = fullSmallParameter = partialParameter = 0;
    }

    void arrangePartitions(uint32_t n, uint32_t p);

public:
    // export the origins of the sub-problems (the leaf nodes)
    //        and the solid rules (the internal nodes) recursively
    // use a pre-post-order traversal
    // p: the parameter of the total problem
    // finalSet: the final rule set
    // origins: the queue of the origins of the sub-problems
    bool exportOrigins(UDRuleSet& finalSet, std::queue<std::unique_ptr<ProblemState>> &origins) {
        std::unique_ptr<UDRule> rule = std::make_unique<UDRule>();
        exportNoError = true;
        exportOrigins(root, std::move(rule), finalSet, origins);
        return exportNoError;
    }

private:
    // the pre-post-order traversal
    // pre-order:  rule split
    // reach leaf: export the origin of the sub-problem
    //             there are 3 types of partitions
    // post-order: export the solid rule
    void exportOrigins(std::shared_ptr<TrieNode> node, std::unique_ptr<UDRule> rule, UDRuleSet& finalSet, std::queue<std::unique_ptr<ProblemState>> &origins);
    uint32_t leafIndex = 0;
    bool exportNoError = true;

};

void DensePartitionTrie::arrangePartitions(uint32_t n, uint32_t p) {
    clearPartitionAttributes();
    uint32_t sumOfLeafParts = n - internalCount;
    uint32_t sumOfLeafParameters = p - internalParameter;
    partialCount = smallCount;
    partialParameter = std::min(sumOfLeafParameters / sumOfLeafParts * smallPart, ParameterCalculator::getInstance().at(smallPart));
    sumOfLeafParts -= smallPart;
    sumOfLeafParameters -= partialParameter;
    fullSmallParameter = sumOfLeafParameters / largeCount;
    fullLargeParameter = fullSmallParameter + 1;
    fullLargeCount = sumOfLeafParameters % largeCount;
    fullSmallCount = largeCount - fullLargeCount;
    leafIndex = 0;
}

bool DensePartitionTrie::build(uint32_t n, uint32_t h, uint32_t p, double log2N) {
    clearAttributes();
    uint32_t sumOfCount = n;
    buildTrieNode(root, n, h, 0, log2N);
    if (leafCount == 0                       || // the node count is too small for a h-height trie
        n != 0                               || // the node count is too large for a h-height trie
        leafParamter + internalParameter < p || // the max parameter cannot reach p
        internalParameter > p) {                // the min parameter is too large
        return false;
    }
    arrangePartitions(sumOfCount, p);
    return true;
}

uint32_t DensePartitionTrie::buildTrieNode(std::shared_ptr<TrieNode>& node, uint32_t& n, uint32_t h, uint32_t depth, double log2N) {
    if (n == 0) { // there is no remaining rule to be allocated
        return 0;
    }
    if (depth == h || n == 1) { // build a leaf node
        leafCount++;
        if (std::log2(n) <= log2N) { // can be stored within a single node
            node = std::make_shared<TrieNode>(n);
            if (n < largeCount) {
                smallPart = n;
                smallCount++;
            } else {
                largePart = n;
                largeCount++;
            }
            n = 0;
        } else { // need to be split into multiple nodes, put N rules in the current node
            uint32_t count = std::round(std::exp2(log2N));
            node = std::make_shared<TrieNode>(count);
            n -= count;
            largePart = count;
            largeCount++;
        }
        node->isLeaf = true;
        leafParamter += ParameterCalculator::getInstance().at(node->count);
        return 1;
    } else { // build an internal node
        node = std::make_shared<TrieNode>(1); // the count of an internal node is 1
        internalCount++;
        n--;
        uint32_t left = buildTrieNode(node->children[0], n, h, depth + 1, log2N);
        uint32_t right = buildTrieNode(node->children[1], n, h, depth + 1, log2N);
        internalParameter += left + right;
        return left + right + 1;
    }
}


void DensePartitionTrie::exportOrigins(std::shared_ptr<TrieNode> node, std::unique_ptr<UDRule> rule, UDRuleSet& finalSet, std::queue<std::unique_ptr<ProblemState>> &origins) {
    if (node->isLeaf) {
        uint32_t parameter = 0;
        uint32_t part = 0;
        if (leafIndex < fullLargeCount) {
            parameter = fullLargeParameter;
            part = largePart;
        } else if (leafIndex < fullLargeCount + fullSmallCount) {
            parameter = fullSmallParameter;
            part = largePart;
        } else {
            parameter = partialParameter;
            part = smallPart;
        }
        leafIndex++;
        origins.push(std::make_unique<ProblemState>(part, parameter, true, std::move(rule)));
    } else {
        auto pair = RuleSplitter::getInstance().split(*rule);
        if (node->children[0] != nullptr) {
            if (pair.first != nullptr) {
                exportOrigins(node->children[0], std::move(pair.first), finalSet, origins);
            } else {
                exportNoError = false;
            }
        }
        if (node->children[1] != nullptr) {
            if (pair.second != nullptr) {
                exportOrigins(node->children[1], std::move(pair.second), finalSet, origins);
            } else {
                exportNoError = false;
            }
        }
        finalSet.push_back(std::move(rule));
    }
}

}