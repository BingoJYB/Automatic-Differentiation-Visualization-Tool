#ifndef DAG_EXTRACTOR_H
#define DAG_EXTRACTOR_H

typedef std::pair<long, long> node_entry_t;
typedef std::pair<long, long> edge_from_to_t;
typedef std::pair<edge_from_to_t, double> edge_with_partial_t;

struct dag_extractor {
    dag _dag;
    std::vector<node_entry_t> _nodes;
    std::vector<edge_with_partial_t> _edges;
    unsigned _maxhop = 1;
    unsigned ttl = 1;

private:
    void recursive_build_up_small_dag_nodes(long nodeidx, unsigned hopcount) {
        if(hopcount == _maxhop) return;

        std::vector<edge_with_partial_t> edges = _dag.gradient_of(nodeidx);

        if (_nodes.empty()) {
            node_entry_t node;
            node.first = ttl;
            node.second = nodeidx;

            _nodes.push_back(node);
        }
        else {
            for (unsigned i = 0; i < _nodes.size(); i++) {
                if (_nodes.at(i).second == nodeidx) {
                    _nodes.at(i).first = ttl;
                    break;
                }

                else if (i == _nodes.size()-1) {
                    node_entry_t node;
                    node.first = ttl;
                    node.second = nodeidx;

                    _nodes.push_back(node);
                    break;
                }
            }
        }

        for (unsigned i = 0; i < edges.size(); i++) {
            if (edges.at(i).first.second == nodeidx) {
                for (unsigned j = 0; j < _nodes.size(); j++) {
                    if (_nodes.at(j).second == edges.at(i).first.first) {
                        _nodes.at(j).first = ttl;
                        break;
                    }

                    if (j == _nodes.size()-1) {
                        node_entry_t node;
                        node.first = ttl;
                        node.second = edges.at(i).first.first;

                        _nodes.push_back(node);
                        break;
                    }
                }

                recursive_build_up_small_dag_nodes(edges.at(i).first.first, hopcount+1);
            }
        }
    }

    void find_missing_edges() {
        _edges.clear();

        for (unsigned i = 0; i < _nodes.size(); i++) {
            std::vector<edge_with_partial_t> edges = _dag.gradient_of(_nodes.at(i).second);
            std::vector<edge_with_partial_t> temp_edges;

            for (unsigned j = 0; j < edges.size(); j++) {
                bool front = false;
                bool end = false;
                long first1 = edges.at(j).first.first;
                long second1 = edges.at(j).first.second;

                for (unsigned k = 0; k < _nodes.size(); k++) {
                    if (_nodes.at(k).second == first1)
                        front = true;

                    if (_nodes.at(k).second == second1)
                        end = true;

                    if (front && end)
                        break;
                }

                if (front && end) {
                    if (_edges.empty())
                        temp_edges.push_back(edges.at(j));
                    else {
                        for (unsigned l = 0; l < _edges.size(); l++) {
                            long first2 = _edges.at(l).first.first;
                            long second2 = _edges.at(l).first.second;

                            if (first1 == first2 && second1 == second2)
                                break;

                            if (l == _edges.size()-1)
                                temp_edges.push_back(edges.at(j));
                        }
                    }
                }
            }

            for (unsigned i = 0; i < temp_edges.size(); i++)
                _edges.push_back(temp_edges.at(i));
        }
    }

    void remove_ttl_zero_nodes() {
        if (!_nodes.empty()) {
            std::vector<node_entry_t> temp_nodes;

            for (unsigned i = 0; i < _nodes.size(); i++) {
                _nodes.at(i).first -= 1;

                if (_nodes.at(i).first != 0)
                    temp_nodes.push_back(_nodes.at(i));
            }

            _nodes = temp_nodes;
        }
    }

public:
   void set_current_node(long nodeidx) {
       remove_ttl_zero_nodes();
       recursive_build_up_small_dag_nodes(nodeidx, 0);
       find_missing_edges();
   }

   void write_dot_file(std::string dotfilename = "small_dag.dot") {
       std::ofstream dotfile;
       dotfile.open(dotfilename);
       dotfile << "digraph {\n";
       for (unsigned i = 0; i < _nodes.size(); i++)
           dotfile << _nodes.at(i).second << "[shape=box,label=\"" << _nodes.at(i).second << "\", color=\"green\"]\n";
       for (unsigned i = 0; i < _edges.size(); i++) {
           dotfile << _edges.at(i).first.first << " -> " << _edges.at(i).first.second << "[label=\""
                   << std::scientific << _edges.at(i).second << "\"]\n";
       }
       dotfile << "}";
       dotfile.close();
   }
};

#endif // DAG_EXTRACTOR_H
