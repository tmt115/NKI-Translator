#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>

using namespace std;

string parse_for_loop(curr_line, line_num) {
    regex loop_stat(R"(for (\w+) in range\((\d+)\))");
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

string parse_init(curr_line, line_num) {
    regex init_stat(R"((\w+) = zeros\(\(([\d,]+)\)\))");
    smatch match;
    if (regex_search(curr_line, match, init_stat)) {
        string init_var = match[1];
        string dim_nums = match[2];
        regex nums(R"(\d+)");
        smatch match2;
        regex_search(match[2], match2, nums);
        string dim_str = "";
        for (string s : match2) {
            dim_str += s;
        }
        string fin_string = init_var + " = nl.zeros(" + dim_str + ", dtype=np.float32, buffer=nl.sbuf)";
        return fin_string;
    }
    cerr << "Unparsable syntax at line " + to_string(line_num) + ": " + curr_line;
    return "";
}

string parse_matmul(curr_line, line_num) {
    regex matmul_stat_pe(R"((/w+) += matmul\((\w+), (\w+)\))");
    regex matmul_stat_p(R"((/w+) = matmul\((\w+), (\w+)\))");
    smatch match;
    if (regex_search(curr_line, match, matmul_stat_pe)) {
        string res_var = match[1];
        string lhs = match[2];
        string rhs = match[3];
        string nmm = res_var + " += nl.matmul(" + lhs + ", " + rhs + ")";
        return nmm
    }
    else if (regex_search(curr_line, match, matmul_stat_p)) {
        string res_var = match[1];
        string lhs = match[2];
        string rhs = match[3];
        string nmm = res_var + " += nl.matmul(" + lhs + ", " + rhs + ")";
        return nmm
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
    regex zero_det("zeros");
    else if (regex_search(curr_line, match, zero_det)) {
        string nz = parse_init(curr_line, line_num);
        vector<string> vec = {nz, "NI"}
        return vec;
    }
    regex matmul_det("matmul");
    else if (regex_search(curr_line, match, matmul_det)) {
        string nmm = parse_matmul(curr_line, line_num);
        vector<string> vec = {nmm, "NI"};
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
    string curr_indent = "";
    size_t indent_level{};
    while(getline(file, line)) {
        line_num++;
        vector<string> nvec = parse_line(line, line_num);
        if (curr_indent != "") {
            regex spacing(curr_indent);
            smatch match;
            if (!regex_search(line, match, spacing)) {indent_level--;}
        }
        for (size_t i{}; i < indent_level; i++) {
            out << "    ";
        }
        if (nvec[1] == "for loop") {
            out << nvec[0] << endl;
            indent_level++;
        }
        else {
            out << nvec[0] << endl;
        }
    }
    return 0;
}
