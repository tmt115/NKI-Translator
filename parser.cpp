#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>

using namespace std;

string parse_for_loop(curr_line, line_num) {
    regex loop_stat("for (\\w+) in range\((\\d+\))");
    smatch match;
    if (regex_search(curr_line, match, loop_stat)) {
        string loop_var = match[1];
        string loop_range = match[2];
        string nki_forloop = "for " + loop_var + " in nl.affine_range(" + loop_range + "):";
        return nki_forloop;
    }
    cerr << "Unparsable syntax at line " + to_string(line_num) + ": " + curr_line;
    return "";
}

vector<string> parse_line(curr_line, line_num) {
    regex for_det("for");
    smatch match;
    if (regex_search(curr_line, match, for_det)) {
        string nfl = parse_for_loop(curr_line, line_num);
        vector<string> vec = {nfl, "for loop"};
        return vec;
    }
}

int main(int argc, char * argv[]) {
    string filename = argv[1];
    string outfile = argv[2];
    ifstream file(filename);
    if (!file) {
        cerr << "Failed to open file" << endl;
        return 1;
    }
    string line;
    size_t line_num{};
    bool toindent = false;
    while(getline(file, line)) {
        line_num++;
        vector<string> nvec = parse_line(line, line_num);
        if (toindent) {
            out << "    ";
            toindent = false;
        }
        if (nvec[1] == "for loop") {
            out << nvec[0] << endl;
            toindent = true;
        }
    }
    return 0;
}
