#ifndef MAPPING_HELPER_HPP
#define MAPPING_HELPER_HPP

#include <iostream>
#include <fstream>

#include "mainwindow.h"

struct mapping_helper {
    struct position_info {
        std::string filename;
        int line;
        int col;
    };

    std::vector<position_info> _positions;
    position_info dummy;

    const position_info& location(const unsigned index) {
        if(index - 1 > _positions.size()) {
            dummy.col = 0;
            dummy.line = 0;
            return dummy;
        }
        return _positions[index-1];
    }

    void set_path(const std::string root_path) {
        _positions.clear();
        std::string filename = root_path + std::string("instrumented/mapping.txt");

        std::ifstream infile(filename.c_str());
        if(infile.is_open()==false) {
            std::cout << "file not Found: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(infile, line))
        {
            std::istringstream linestream(line);
            std::string tok;
            int k = 0;
            position_info cur;
            while(getline(linestream, tok, ';')) {
                if(k==1) {
                    cur.filename = tok;
                }
                else if(k==2) {
                    cur.line = std::stoi(tok);
                }
                else if(k==3) {
                    cur.col = std::stoi(tok);
                    _positions.push_back(cur);
                }

                k++;
            }
        }
    }
};

#endif // MAPPING_HELPER_HPP
