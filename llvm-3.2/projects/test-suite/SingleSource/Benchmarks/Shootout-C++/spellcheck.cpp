// -*- mode: c++ -*-
// $Id: spellcheck.cpp 8907 2003-10-07 01:08:48Z lattner $
// http://www.bagley.org/~doug/shootout/
// STL spell checker from Bill Lear

#include <iostream>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;

typedef std::pair<const char*, const char*> span;

class spell_checker {
public:
    spell_checker() {
        std::ifstream in("Usr.Dict.Words");
        char line[32];
        while (in.getline(line, 32)) {
            const char* begin = line;
            const char* end = line + in.gcount() - 1;
            if (dict.end() == dict.find(span(begin, end))) {
                const size_t len = end - begin;
                char* word = new char[len];
                copy(begin, end, word);
                ++dict[span(word, word + len)];
            }
         }
    }

    void process(std::istream& in) {
        char line[32];
        while (in.getline(line, 32)) {
            if (dict.end() == dict.find(span(line, line + in.gcount() - 1))) {
                cout << line << '\n';
            }
        }
    }

private:
    std::map<span, int> dict;
};

int main() {
    spell_checker spell;
    char buff[4096];
    cin.rdbuf()->pubsetbuf(buff, 4096); // enable buffering
    spell.process(cin);
}
