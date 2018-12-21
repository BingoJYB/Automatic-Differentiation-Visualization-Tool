#ifndef DAG_H
#define DAG_H

#include <fstream>
#include <sstream>

typedef std::pair<long, long> edge_from_to_t;
typedef std::pair<edge_from_to_t, double> edge_with_partial_t;

struct dag {
    std::vector<long> _nodes;
    std::vector<edge_with_partial_t> _edges;

    void read_huge_dot_file(std::string dotfilename = "tape.dot") {
        _nodes.clear();
        _edges.clear();
        std::ifstream dotfile(dotfilename);

        std::string line;
        while (std::getline(dotfile, line))
        {
            std::size_t found1 = line.find("shape");
            std::size_t found2 = line.find("->");
            std::size_t found3 = line.find("[");
            std::size_t found4 = line.find("]");

            if (found1 != std::string::npos) {
                _nodes.push_back(std::stol(line.substr(0, found3)));
            }
            else if (found1 == std::string::npos && found2 != std::string::npos) {
                edge_from_to_t edge;
                edge_with_partial_t edge_with_p;

                edge.first = std::stol(line.substr(0, found2-1));
                edge.second = std::stol(line.substr(found2+3, found3-found2-3));

                for (unsigned i = 0; i < _nodes.size(); i++) {
                    if (_nodes[i] == edge.first)
                        break;

                    if (i == _nodes.size()-1)
                        _nodes.push_back(edge.first);
                }

                for (unsigned i = 0; i < _nodes.size(); i++) {
                    if (_nodes[i] == edge.second)
                        break;

                    if (i == _nodes.size()-1)
                        _nodes.push_back(edge.second);
                }

                edge_with_p.first = edge;
                edge_with_p.second = std::stold(line.substr(found3+8, found4-found3-9));
                _edges.push_back(edge_with_p);
            }
        }
    }

    std::vector<edge_with_partial_t> gradient_of(long nodeidx) {
        std::vector<edge_with_partial_t> edges_with_partial_t;

        for (unsigned i = 0; i < _edges.size(); i++)
            if (_edges.at(i).first.first == nodeidx || _edges.at(i).first.second == nodeidx)
                edges_with_partial_t.push_back(_edges.at(i));

        return edges_with_partial_t;
    }
};

#endif // DAG_H
