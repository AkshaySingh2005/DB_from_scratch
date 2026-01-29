#include "completion.h"
#include "parser/parser.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <vector>
#include <string>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <set>
#include <cstdlib>
#include <iostream>

static std::vector<std::string> builtins = {
    ".history",
    ".exit",
    ".quit",
    "select",
    "insert",
    "create",
    "use",
    "table"
};

char* command_generator(const char* text, int state) {
    static std::vector<std::string> matches;
    static size_t index;

    if (state == 0) {
        matches.clear();
        index = 0;

        std::set<std::string> seen;

        // Builtins 
        for (const auto& cmd : builtins) {
            if (cmd.rfind(text, 0) == 0) {
                seen.insert(cmd);
            }
        }

        matches.assign(seen.begin(), seen.end());
    }

    if (index < matches.size()) {
        return strdup(matches[index++].c_str());
    }

    return nullptr;
}


char** completion(const char* text, int start, int end) {
    (void)end;

    if (start == 0) {
        return rl_completion_matches(text, command_generator);
    }

    return nullptr;
}

void history_cmd(const std::string input) {

    std::vector<std::string>args = parse_args(input);

    HIST_ENTRY** hist = history_list();
    if (!hist) return;

    int count = history_length;

  
    if (args.size() == 1) {
        for (int i = 0; i < count; i++) {
            std::cout << i + 1 << "  " << hist[i]->line << "\n";
        }
        return;
    }

    int n = 0;
    try {
        n = std::stoi(args[1]);
    } catch (...) {
        return;
    }

    if (n <= 0) return;

    int start = count - n;
    if (start < 0) start = 0;

    for (int i = start; i < count; i++) {
        std::cout << i + 1 << "  " << hist[i]->line << "\n";
    }
}


void setup_readline() {
    rl_attempted_completion_function = completion;
}