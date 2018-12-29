#ifndef DAG_LAYOUTER_H
#define DAG_LAYOUTER_H

#include <graphviz/gvc.h>

struct dag_layouter {
    Background bg;
    QList<Node> nodes;
    QList<Edge> edges;

    void parseDotFile(QString dotfilename) {
        nodes.clear();
        edges.clear();

        static GVC_t *gvc=0;
        if (!gvc)
            gvc = gvContext();

        FILE *fp;
        QByteArray fn = dotfilename.toLatin1();
        const char *c = fn.data();
        fp = fopen (c, "r");

        Agraph_t *g = 0;
        g = agread(fp, 0);
        gvLayout(gvc, g, "dot");
        gvRender(gvc, g, "dot", stdout);

        std::string bb(agget(g, "bb"));
        bg = parsebb(bb);

        Agnode_t *curn = agfstnode(g);
        while(curn) {
            QList<Edge> linkedE;
            std::string nodeHeight(agget(curn, "height"));
            std::string nodeWidth(agget(curn, "width"));
            std::string nodeLabel(agget(curn, "label"));
            std::string nodePos(agget(curn, "pos"));

            Agedge_t* cure = agfstedge(g, curn);
            while(cure) {
                std::string edgeLabel(agget(cure, "label"));
                std::string edgeLp(agget(cure, "lp"));
                std::string edgePos(agget(cure, "pos"));
                Edge edge = parseEdge(bg, edgeLabel, edgeLp, edgePos);

                if (edges.indexOf(edge) == -1)
                    edges << edge;

                linkedE << edge;
                cure = agnxtedge(g, cure, curn);
            }

            nodes << parseNode(bg, nodeHeight, nodeWidth, nodeLabel, nodePos, linkedE);
            curn = agnxtnode(g, curn);
        }
    }

    Background parsebb(std::string bb) {
        std::stringstream bg_ss(bb);
        std::string bg_item;
        std::vector<std::string> bg_tokens;
        while (getline(bg_ss, bg_item, ','))
            bg_tokens.push_back(bg_item);
        double width = atof(bg_tokens.at(2).c_str()) * 96 / 72;
        double height = atof(bg_tokens.at(3).c_str()) * 96 / 72;

        Background temp;
        temp.width = width;
        temp.height = height;

        return temp;
    }

    Node parseNode(Background bg, std::string height, std::string width, std::string label, std::string pos, QList<Edge> le) {
        double h = atof(height.c_str()) * 96;
        double w = atof(width.c_str()) * 96;

        std::stringstream node_ss(pos);
        std::string node_item;
        std::vector<std::string> node_tokens;
        while (getline(node_ss, node_item, ','))
            node_tokens.push_back(node_item);
        double pos_x = atof(node_tokens.at(0).c_str()) * 96 / 72;
        double pos_y = bg.height - atof(node_tokens.at(1).c_str()) * 96 / 72;

        QString Qlabel = QString::fromStdString(label);

        Node temp;
        temp.height = h;
        temp.width = w;
        temp.pos_x = pos_x;
        temp.pos_y = pos_y;
        temp.label = Qlabel;
        temp.linkedE = le;

        return temp;
    }

    Edge parseEdge(Background bg, std::string label, std::string lp, std::string pos) {
        QPainterPath path;
        QPainterPath arrow;

        std::stringstream edge_ss(pos);
        std::string edge_item;
        std::vector<std::string> edge_tokens;
        while (getline(edge_ss, edge_item, ' '))
            edge_tokens.push_back(edge_item);

        std::stringstream edgeS_ss(edge_tokens.at(1));
        std::string edgeS_item;
        std::vector<std::string> edgeS_tokens;
        while (getline(edgeS_ss, edgeS_item, ','))
            edgeS_tokens.push_back(edgeS_item);
        double edgeS_x = atof(edgeS_tokens.at(0).c_str()) * 96 / 72;
        double edgeS_y = bg.height - atof(edgeS_tokens.at(1).c_str()) * 96 / 72;
        path.moveTo(edgeS_x, edgeS_y);

        std::stringstream edgeE_ss(edge_tokens.at(0));
        std::string edgeE_item;
        std::vector<std::string> edgeE_tokens;
        while (getline(edgeE_ss, edgeE_item, ','))
            edgeE_tokens.push_back(edgeE_item);
        double edgeE_x = atof(edgeE_tokens.at(1).c_str()) * 96 / 72;
        double edgeE_y = bg.height - atof(edgeE_tokens.at(2).c_str()) * 96 / 72;

        double edgeP_x = edgeS_x, edgeP_y = edgeS_y;
        double edgeC_x, edgeC_y;
        double edgeN_x, edgeN_y;
        for (unsigned i=2; i < edge_tokens.size(); i++) {
            std::stringstream edgeC_ss(edge_tokens.at(i));
            std::string edgeC_item;
            std::vector<std::string> edgeC_tokens;
            while (getline(edgeC_ss, edgeC_item, ','))
                edgeC_tokens.push_back(edgeC_item);
            edgeC_x = atof(edgeC_tokens.at(0).c_str()) * 96 / 72;
            edgeC_y = bg.height - atof(edgeC_tokens.at(1).c_str()) * 96 / 72;

            if (i+1 < edge_tokens.size()) {
                std::stringstream edgeN_ss(edge_tokens.at(i+1));
                std::string edgeN_item;
                std::vector<std::string> edgeN_tokens;
                while (getline(edgeN_ss, edgeN_item, ','))
                    edgeN_tokens.push_back(edgeN_item);
                edgeN_x = atof(edgeN_tokens.at(0).c_str()) * 96 / 72;
                edgeN_y = bg.height - atof(edgeN_tokens.at(1).c_str()) * 96 / 72;
            }
            else {
                edgeN_x = edgeE_x;
                edgeN_y = edgeE_y;
            }

            QPointF p1 = QPointF(edgeC_x+(edgeP_x-edgeC_x)*0.5, edgeC_y+(edgeP_y-edgeC_y)*0.5);
            QPointF p2 = QPointF(edgeC_x-(edgeC_x-edgeN_x)*0.5, edgeC_y-(edgeC_y-edgeN_y)*0.5);
            QPointF c = QPointF(edgeC_x, edgeC_y);

            path.lineTo(p1);
            path.quadTo(c, p2);
            edgeP_x = edgeC_x;
            edgeP_y = edgeC_y;
        }
        path.lineTo(edgeE_x, edgeE_y);

        QLineF centerLine(edgeP_x, edgeP_y, edgeE_x, edgeE_y);
        QPolygonF arrowHead;
        arrowHead.push_back(QPointF(0,0));
        arrowHead.push_back(QPointF(arrowHead.first().x()-7.5, arrowHead.first().y()-15));
        arrowHead.push_back(QPointF(arrowHead.first().x()+7.5, arrowHead.first().y()-15));
        arrowHead.push_back(QPointF(arrowHead.first().x(), arrowHead.first().y()));
        QTransform transform = QTransform().translate(edgeE_x, edgeE_y).rotate(270-centerLine.angle(), Qt::ZAxis);;
        arrow.addPolygon(transform.map(arrowHead));

        if (label == "-0.000000e+00")
            label = "0.000000e+00";
        QString edge_l = QString::fromStdString(label);

        std::stringstream edgeL_ss(lp);
        std::string edgeL_item;
        std::vector<std::string> edgeL_tokens;
        while (getline(edgeL_ss, edgeL_item, ','))
            edgeL_tokens.push_back(edgeL_item);
        double edgeL_x = atof(edgeL_tokens.at(0).c_str()) * 96 / 72;
        double edgeL_y = bg.height - atof(edgeL_tokens.at(1).c_str()) * 96 / 72;

        Edge temp;
        temp.lp_x = edgeL_x;
        temp.lp_y = edgeL_y;
        temp.edgePath = path;
        temp.arrow = arrow;
        temp.label = edge_l;

        return temp;
    }
};

#endif // DAG_LAYOUTER_H
