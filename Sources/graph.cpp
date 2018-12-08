#include "graph.h"

long Edge::operator==(const Edge &edge) const {
    return (label == edge.label && lp_x == edge.lp_x && lp_y == edge.lp_y);
}

long Node::operator==(const Node &node) const {
    return (label == node.label && pos_x == node.pos_x && pos_y == node.pos_y);
}
