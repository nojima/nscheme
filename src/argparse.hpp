#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>


namespace nscheme {


class ArgumentParser {
public:
    void addArgument(const std::string& name);

    void addOption(const std::string& name,
                   const std::string& short_name = "",
                   const std::string& long_name = "",
                   bool has_value = false);

    std::unordered_map<std::string, std::string>
    parse(int argc, char** argv);

private:
    std::vector<std::string> argument_names_;
    std::unordered_map<std::string, std::string> option_names_;
    std::unordered_set<std::string> has_value_;
};


struct ArgumentParseError: public std::runtime_error {
    ArgumentParseError(const std::string& message)
        : std::runtime_error(message) {}
};


}   // namespace nscheme
