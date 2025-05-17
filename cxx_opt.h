#pragma once

#include <map>
#include <string>
#include <stdexcept>

#define CXX_OPT_NAMESPACE cxxopt

namespace CXX_OPT_NAMESPACE {

    class FlagException : public std::runtime_error {
    public:
        FlagException(const std::string &what) : std::runtime_error(what) {}
    };

#define DEFINE_EXCEPTION(className, whatPrefix) \
    class className : public FlagException { \
    public: \
        className(const std::string &whatSuffix = "") \
            : FlagException(whatPrefix + whatSuffix) {} \
    };

    // flag contains = for -name: parse error
    DEFINE_EXCEPTION(FlagContainsEqualError, "flag contains '=' ")
    // flag invalid argument  for register error
    DEFINE_EXCEPTION(FlagInvalidArgumentError, "flag invalid argument ")
    // parse error for xxxx
    DEFINE_EXCEPTION(ParseError, "parse error ")

    /*
     * @param name: name of the flag, e.g. --name
     * @param value: value of the flag, e.g. value
     *
     * @format
     *  -name=value
     *  -name value
     *  --name==value
     *  --name value 
     *
     * @exception
     *  FlagContainsEqualError
     *  InvalidBooleanValueError
     */
    class Flag {
    public:
        Flag();
        ~Flag();

        void banner(const std::string &banner);

        void parse(int argc, char **argv);

        void printDefaults() const noexcept;

        void registerString(const std::string &name, std::string *value, const std::string &help = "");
        void registerInt(const std::string &name, int *value, const std::string &help = "");
        void registerBool(const std::string &name, bool *value, const std::string &help = "");
        void registerFloat(const std::string &name, float *value, const std::string &help = "");

        std::vector<std::string> args();
        std::string arg(size_t index);

    protected:

        enum class FlagType { String, Int, Bool, Float };
        const char *flagTypeToString(FlagType type) const noexcept {
            static const char *types[] = { "string", "int", "bool", "float" };
            return types[(int)type];
        }
        struct FlagInfo {
            FlagType type_;
            std::string name_;
            std::string help_;

            union {
                const char *string_;
                int int_;
                bool bool_;
                float float_;
            } default_;

            bool operator<(const FlagInfo &rhs) const noexcept {
                return name_ < rhs.name_;
            }
        };

    private:
        std::string banner_;
        std::map<FlagInfo/*flag*/, void */*save_pointer*/> flags_;
        std::vector<std::string> args_;
    };
}

