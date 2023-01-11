#pragma once

#include <ostream>
#include <algorithm>

#include "constants.hpp"
#include "edge_type.hpp"

namespace flowbench {

// The sequence of edges in QuadDag is:
// 0, 1
// 0, 2
// 1, 2
// 0, 3
// 1, 3
// 2, 3

static constexpr uint8_t getIndex(uint8_t src, uint8_t dst) {
    return dst * (dst - 1) / 2 + src;
}

class QuadDag {
protected:
    std::array<EdgeType, QD_VPAIR_CNT> edges;

public:
    QuadDag() {
        std::fill(edges.begin(), edges.end(), EdgeType::None);
    }
    QuadDag(const std::string& str);
    std::string toString() const;

public:
    EdgeType getEdge(uint8_t index) const {
        return edges[index];
    }

    EdgeType getEdge(uint8_t src, uint8_t dst) const {
        return edges[getIndex(src, dst)];
    }

    void setEdge(uint8_t index, EdgeType edge) {
        edges[index] = edge;
    }

    void setEdge(uint8_t src, uint8_t dst, EdgeType edge) {
        edges[getIndex(src, dst)] = edge;
    }
};

QuadDag::QuadDag(const std::string& str) {
    static constexpr std::array<EdgeType, 3> edgeTypes = {
        EdgeType::None,
        EdgeType::Overlap,
        EdgeType::Cover
    };
    std::transform(str.begin(), str.end(), edges.begin(), [&](char c) {
        return edgeTypes[c - '0'];
    });
}

std::string QuadDag::toString() const {
    std::string str;
    str.reserve(QD_VPAIR_CNT);
    std::transform(edges.begin(), edges.end(), std::back_inserter(str), [](EdgeType edge) {
        return getEnumName<EdgeType>(edge)[0];
    });
    return str;
}

}