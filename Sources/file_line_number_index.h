#ifndef FILE_LINE_NUMBER_INDEX_HPP
#define FILE_LINE_NUMBER_INDEX_HPP

#include <fstream>

struct file_line_number_index {
    std::vector<int> _lineoffsets;

    int get_position(const int linenumber, const int col) const {
        return _lineoffsets[linenumber-1] + col - 1;
    }

    void get_mapping(const std::string &filename) {
        _lineoffsets.clear();

        std::ifstream infile(filename);
        _lineoffsets.push_back(0);

        char c;
        int offset = 0;
        while(infile.read(&c, 1)) {
            if(c == '\n') {
                _lineoffsets.push_back(offset+1);
            }
            offset++;
        }
    }
};

#endif // FILE_LINE_NUMBER_INDEX_HPP
