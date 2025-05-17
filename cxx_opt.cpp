#include <cstdio>
#include <cstring>
#include "cxx_opt.h"

#define PARAM_STRING_NOT_EMPTY_ASSERT(param, error) \
    do { \
    if (param.empty()) { \
        throw CXX_OPT_NAMESPACE::FlagInvalidArgumentError(error); \
    } } while (0)
#define PARAM_NOT_NIL_ASSERT(param, error) \
    do { \
    if (param == nullptr) { \
        throw CXX_OPT_NAMESPACE::FlagInvalidArgumentError(error); \
    } } while (0) 

#define REGISTER_PARAM_CHECK_ASSERT(name, value) \
    do { \
    PARAM_STRING_NOT_EMPTY_ASSERT(name, "for register parameter name empty"); \
    PARAM_NOT_NIL_ASSERT(value, "for register parameter value point == nil"); \
    } while (0)

static std::string toLower(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

CXX_OPT_NAMESPACE::Flag::Flag()
    : banner_("") {
}

CXX_OPT_NAMESPACE::Flag::~Flag() {
}

void CXX_OPT_NAMESPACE::Flag::parse(int argc, char **argv) {
    char *cmd = argv[0];
    char **arg_list = &argv[1];
    int arg_count = argc - 1;

    for (int i = 0; i < arg_count; i++) {
        std::string arg = arg_list[i];
        
        auto match = std::find_if(flags_.begin(), flags_.end(), [&](const std::pair<const FlagInfo, void*>& entry) {
                const std::string &src = entry.first.name_;
                const std::string &dst = arg;

                if (dst[0] == '-' && dst[1] != '-')
                    return dst.compare(1, src.length(), src) == 0 &&
                           (dst.length() == src.length() + 1 || dst[src.length() + 1] == '=');
                else if (dst[0] == '-' && dst[1] == '-')
                    return dst.compare(2, src.length(), src) == 0 &&
                           (dst.length() == src.length() + 2 || dst[src.length() + 2] == '=');

                // if (dst[0] == '-' && dst[1] != '-')
                //     return dst.compare(1, src.length(), src) == 0;
                // else if (dst[0] == '-' && dst[1] == '-')
                //     return dst.compare(2, src.length(), src) == 0;

                return false;
            });

        // unkown flag insert to args_.
        if (match == flags_.end()) {
            args_.push_back(std::move(arg));
            continue;
        }

        std::string argument;
        if (arg.find("==") != std::string::npos) {
            argument = arg.substr(arg.find("==") + 2);      
        } else if (arg.find('=') != std::string::npos) {
            argument = arg.substr(arg.find('=') + 1);
        } else {
            if (match->first.type_ == FlagType::Bool) {
                argument = "true";
            } else if ((i + 1) >= arg_count) {
                throw CXX_OPT_NAMESPACE::FlagInvalidArgumentError(arg + " argument not found");
            } else {
                argument = arg_list[i + 1];
                i++;
            }
        }

        argument = toLower(argument);

        try {
            switch (match->first.type_) {
                case FlagType::String: {
                    *static_cast<std::string*>(match->second) = argument;
                } break;
                case FlagType::Int: {
                    *static_cast<int*>(match->second) = std::stoi(argument); 
                } break;
                case FlagType::Bool: {
                    bool *save_ptr = static_cast<bool*>(match->second);
                    if (argument == "true" || argument == "1") {
                        *save_ptr = true;
                    } else if (argument == "false" || argument == "0") {
                        *save_ptr = false;
                    } else {
                        throw CXX_OPT_NAMESPACE::FlagInvalidArgumentError(arg + " Bool argument is invalid");
                    }
                } break;
                case FlagType::Float: {
                    *static_cast<float*>(match->second) = std::stof(argument);
                } break;
            }

        } catch (const FlagException &e) {
            throw e;
        } catch (const std::exception &e) {
            throw CXX_OPT_NAMESPACE::FlagInvalidArgumentError(arg + " " + e.what());
        }
    }
}

void CXX_OPT_NAMESPACE::Flag::banner(const std::string &banner) {
    banner_ = banner;
}

void CXX_OPT_NAMESPACE::Flag::printDefaults() const noexcept {
    for (const auto &flag : flags_) {
        const FlagInfo &info = flag.first;
        std::fprintf(stderr, "  -%s %s\n", info.name_.c_str(), flagTypeToString(info.type_));

        // help (default "default")
        switch (info.type_) {
        case FlagType::String: {
            std::fprintf(stderr, "    %s (default: %s)\n", info.help_.c_str(), info.default_.string_);
        } break;
        case FlagType::Int: {
            std::fprintf(stderr, "    %s (default: %d)\n", info.help_.c_str(), info.default_.int_);
        } break;
        case FlagType::Bool: {
            std::fprintf(stderr, "    %s (default: %s)\n", info.help_.c_str(), info.default_.bool_ ? "true" : "false");
        } break;
        case FlagType::Float: {
            std::fprintf(stderr, "    %s (default: %0.2f)\n", info.help_.c_str(), info.default_.float_);
        } break;
        }
    }
}

void CXX_OPT_NAMESPACE::Flag::registerString(const std::string &name, std::string *value, const std::string &help) {
    REGISTER_PARAM_CHECK_ASSERT(name, value);

    FlagInfo info = {
        .type_ = FlagType::String,
        .name_ = name,
        .help_ = help,
        .default_ = {
            .string_ = value->c_str()
        }
    };

    flags_[info] = value;
}

void CXX_OPT_NAMESPACE::Flag::registerInt(const std::string &name, int *value, const std::string &help) {
    REGISTER_PARAM_CHECK_ASSERT(name, value);

    FlagInfo info = {
        .type_ = FlagType::Int,
        .name_ = name,
        .help_ = help,
        .default_ = {
            .int_ = *value
        }
    };

    flags_[info] = value;
}

void CXX_OPT_NAMESPACE::Flag::registerBool(const std::string &name, bool *value, const std::string &help) {
    REGISTER_PARAM_CHECK_ASSERT(name, value);

    FlagInfo info = {
        .type_ = FlagType::Bool,
        .name_ = name,
        .help_ = help,
        .default_ = {
            .bool_ = *value
        }
    };

    flags_[info] = value;
}

void CXX_OPT_NAMESPACE::Flag::registerFloat(const std::string &name, float *value, const std::string &help) {
    REGISTER_PARAM_CHECK_ASSERT(name, value);

    FlagInfo info = {
        .type_ = FlagType::Float,
        .name_ = name,
        .help_ = help,
        .default_ = {
            .float_ = *value
        }
    };

    flags_[info] = value;
}

std::vector<std::string> CXX_OPT_NAMESPACE::Flag::args() {
    return std::vector<std::string>();
}

std::string CXX_OPT_NAMESPACE::Flag::arg(size_t index) {
    return args_.at(index);
}
