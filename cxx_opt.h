/*
 * =============================================================================
 *  File Name    : cxx_opt.h
 *  Description  : Lightweight flag parsing utility for C++ (command-line flags)
 *  Author       : Ouzw
 *  Email        : ouzw.mail@gmail.com
 *  Created Date : Mon Sep 9 22:40:40 2024 +0800
 *  Version      : 1.0
 *
 *  Copyright (c) 2025 Ouzw
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 * =============================================================================
 */
#pragma once

#include <map>
#include <vector>
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
        void registerHandler(const std::string &name, std::function<void (void *)> handler, void *context, const std::string &help = "");

        std::vector<std::string> args();
        std::string arg(size_t index);

    protected:
        void showHelp() const noexcept;

        enum class FlagType { String, Int, Bool, Float, Handler };
        const char *flagTypeToString(FlagType type) const noexcept {
            static const char *types[] = { "string", "int", "bool", "float", "" };
            return types[(int)type];
        }
        struct FlagInfo {
            FlagType type_;
            std::string name_;
            std::string help_;
            std::function<void (void *)> handler_;
            void *context;

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
        std::string cmd_;
        std::string banner_;
        std::map<FlagInfo/*flag*/, void */*save_pointer*/> flags_;
        std::vector<std::string> args_;
    };
}

