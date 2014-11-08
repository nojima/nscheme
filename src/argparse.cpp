#include "argparse.hpp"

namespace nscheme {


void ArgumentParser::addArgument(const std::string& name) {
    argument_names_.push_back(name);
}


void ArgumentParser::addOption(const std::string& name,
                               const std::string& short_name,
                               const std::string& long_name,
                               bool has_value) {
    if (!short_name.empty())
        option_names_.insert(std::make_pair(short_name, name));
    if (!long_name.empty())
        option_names_.insert(std::make_pair(long_name, name));
    if (has_value)
        has_value_.insert(name);
}


std::unordered_map<std::string, std::string>
ArgumentParser::parse(int argc, char** argv) {
    std::unordered_map<std::string, std::string> result;
    std::string option_name;
    size_t n_arguments = 0;
    bool no_options = false;

    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];

        if (!option_name.empty()) {
            // option value
            result.insert(std::make_pair(option_name, arg));
            option_name.clear();
            continue;
        }

        if (arg[0] != '-' || no_options) {
            // argument
            if (n_arguments == argument_names_.size())
                throw ArgumentParseError("Too many arguments");
            result.insert(std::make_pair(argument_names_[n_arguments], arg));
            n_arguments++;
            continue;
        }

        // arg[0] == '-' && !no_options

        if (arg[1] == '-') {
            if (arg[2] == '\0') {
                // '--'
                no_options = true;
                continue;
            }

            // long option
            int j;
            for (j = 2; arg[j]; ++j) {
                if (arg[j] == '=')
                    break;
            }
            if (arg[j] != '=') {
                auto it = option_names_.find(arg + 2);
                if (it == option_names_.end())
                    throw ArgumentParseError("No such option: " + std::string(arg));
                if (has_value_.count(it->second))
                    option_name = it->second;
                else
                    result.insert(std::make_pair(it->second, "true"));
                continue;
            }
            std::string s(std::string(arg + 2, arg + j));
            auto it = option_names_.find(s);
            if (it == option_names_.end())
                throw ArgumentParseError("No such option: --" + s);
            if (!has_value_.count(it->second))
                throw ArgumentParseError("You cannot specify the value of '--" + s + "'");
            std::string value(arg + j + 1);
            result.insert(std::make_pair(it->second, value));
            continue;
        }

        // short option
        int j;
        for (j = 1; arg[j]; ++j) {
            if (arg[j] == '=')
                break;
        }
        if (arg[j] != '=') {
            for (int k = 1; arg[k]; ++k) {
                std::string s(1, arg[k]);
                auto it = option_names_.find(s);
                if (it == option_names_.end())
                    throw ArgumentParseError("No such option: -" + s);
                if (arg[k + 1] == '\0') {
                    if (has_value_.count(it->second))
                        option_name = it->second;
                    else
                        result.insert(std::make_pair(it->second, "true"));
                } else {
                    if (has_value_.count(it->second))
                        throw ArgumentParseError("You must specify the value of '-" + s + "'");
                    result.insert(std::make_pair(it->second, "true"));
                }
            }
            continue;
        }

        for (int k = 1; k < j; ++k) {
            std::string s(1, arg[k]);
            auto it = option_names_.find(s);
            if (it == option_names_.end())
                throw ArgumentParseError("No such option: -" + s);
            if (k == j - 1) {
                if (!has_value_.count(it->second))
                    throw ArgumentParseError("You cannot specify the value of '-" + s + "'");
                result.insert(std::make_pair(it->second, std::string(arg + j + 1)));
            } else {
                if (has_value_.count(it->second))
                    throw ArgumentParseError("You must specify the value of '-" + s + "'");
                result.insert(std::make_pair(it->second, "true"));
            }
        }
        continue;
    }

    return result;
}


}   // namespace nscheme
