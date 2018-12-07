#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>

struct Background {
    double width;
    double height;
};

struct Edge {
    QString label;
    QPainterPath edgePath;
    QPainterPath arrow;
    double lp_x;
    double lp_y;

    long operator==(const Edge &edge) const;
};

struct Node {
    double height;
    double width;
    double pos_x;
    double pos_y;
    QString label;
    QList<Edge> linkedE;

    long operator==(const Node &node) const;
};

#endif // GRAPH_H
