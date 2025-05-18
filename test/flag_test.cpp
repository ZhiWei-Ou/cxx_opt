/*
 * =============================================================================
 *  File Name    : flag_test.cpp
 *  Description  : Lightweight flag parsing utility for C++ (command-line flags)
 *  Author       : Ouzw
 *  Email        : ouzw.mail@gmail.com
 *  Created Date : Mon Sep 9 22:47:11 2024 +0800
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

#include <gtest/gtest.h>
#include "../cxx_opt.h"

TEST(Flag, exception) {
    bool occur = false;

    CXX_OPT_NAMESPACE::Flag flag;
    try {
        flag.registerInt("age", nullptr);
    } catch (...) {
        occur = true;
    }
    EXPECT_TRUE(occur) << "TEST: nil pointer exception.";

    try {
        occur = false;
        std::string str;
        flag.registerString("", &str);
    } catch (...) {
        occur = true;
    }
    EXPECT_TRUE(occur) << "TEST: empty flag exception.";

    try {
        occur = false;
        int age = 0;
        flag.registerInt("age=", &age);
    } catch (...) {
        occur = true;
    }
    EXPECT_TRUE(occur) << "TEST: flag contains '='";

    try {
        const char *cmd[] = {
            "./mock",
            "-age=stub",
        };

        occur = false;
        int age = 0;
        flag.registerInt("age", &age);
        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
    } catch (...) {
        occur = true;
    }
    EXPECT_TRUE(occur) << "TEST: './mock -age=stub' for invalid integer value.";

    try {
        const char *cmd[] = {
            "./mock",
            "-adult=2",
        };

        occur = false;
        bool adult = false;
        flag.registerBool("adult", &adult);
        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
    } catch (...) {
        occur = true;
    }
    EXPECT_TRUE(occur) << "TEST: './mock -adult=2' for invalid boolean value.";
}

TEST(Flag, printDefaults) {
    std::string name = "def";
    int int_ = 10;
    bool bool_ = true;
    float float_ = 1.0f;

    CXX_OPT_NAMESPACE::Flag flag;
    flag.banner("banner");
    flag.registerString("Str", &name, "string help");
    flag.registerInt("Int", &int_, "Int help");
    flag.registerBool("Bool", &bool_, "Bool help");
    flag.registerFloat("Float", &float_, "Float help");
    flag.printDefaults();

    /* Correct output: 
      -Bool bool
        Bool help (default: true)
      -Float float
        Float help (default: 1.00)
      -Int int
        Int help (default: 10)
      -Str bool
        string help (default: false)
    */
}

TEST(Flag, parse_capability_of_rules) {

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -name=value";

        char *cmd[] = {
            (char *)"cmd",
            (char *)"-name=value"
        };
        CXX_OPT_NAMESPACE::Flag flag;
        std::string name = "default_name";
        flag.registerString("name", &name);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);

        EXPECT_EQ(name, "value");
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -name value";

        char *cmd[] = {
            (char *)"cmd",
            (char *)"-name",
            (char *)"value"

        };

        CXX_OPT_NAMESPACE::Flag flag;
        std::string name = "default_name";
        flag.registerString("name", &name);
        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);

        EXPECT_EQ(name, "value");

    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd --name==value";

        char *cmd[] = {
            (char *)"cmd",
            (char *)"--name==value"
        };
        CXX_OPT_NAMESPACE::Flag flag;
        std::string name = "default_name";
        flag.registerString("name", &name);
        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);

        EXPECT_EQ(name, "value");
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd --name value";

        char *cmd[] = {
            (char *)"cmd",
            (char *)"--name",
            (char *)"value"
        };
        CXX_OPT_NAMESPACE::Flag flag;
        std::string name = "default_name";
        flag.registerString("name", &name);
        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);

        EXPECT_EQ(name, "value");
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd --xx value, expect use default name";

        char *cmd[] = {
            (char *)"cmd",
            (char *)"-xx",
            (char *)"value"
        };
        CXX_OPT_NAMESPACE::Flag flag;
        std::string name = "default_name";
        flag.registerString("name", &name);
        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);

        EXPECT_EQ(name, "default_name");
    }

}

TEST(Flag, parse_multi_duplicate) {
    /*
     * @format
     *  -name=value
     *  -name value
     *  --name==value
     *  --name value 
     */

    char *cmd[] = {
        (char *)"cmd",
        (char *)"-name=val1",
        (char *)"-name",
        (char *)"val2",
        (char *)"--name==val3",
        (char *)"--name",
        (char *)"val4"
    };

    CXX_OPT_NAMESPACE::Flag flag;
    std::string name = "default_name";
    flag.registerString("name", &name);
    flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);

    EXPECT_EQ(name, "val4");
}

TEST(Flag, parse_type_int) {

    const char *cmd[] = {
        "./cmd",
        "-age",
        "20"
    };

    CXX_OPT_NAMESPACE::Flag flag;
    int age = 1314; // default
    flag.registerInt("age", &age);

    flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
    EXPECT_EQ(age, 20);

    /* ./cmd -age_ 20 */
    cmd[1] = "-age_";
    cmd[2] = "12306";

    flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
    EXPECT_EQ(age, 20);

}

TEST(Flag, parse_type_bool) {
    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male, expect true.";
        const char *cmd[] = {
            "./cmd",
            "-male",
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = false; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, true);
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male 1, expect true.";
        const char *cmd[] = {
            "./cmd",
            "-male",
            "1"
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = false; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, true);
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male 0, expect true.";
        const char *cmd[] = {
            "./cmd",
            "-male",
            "0"
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = false; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, true);
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male=0, expect false.";
        const char *cmd[] = {
            "./cmd",
            "-male=0",
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = true; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, false);
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male==0, expect false.";
        const char *cmd[] = {
            "./cmd",
            "-male==0",
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = true; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, false);
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male true, expect true.";
        const char *cmd[] = {
            "./cmd",
            "-male",
            "true"
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = false; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, true);
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male False, expect true.";
        const char *cmd[] = {
            "./cmd",
            "-male",
            "False"
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = true; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, true);
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male=False, expect false.";
        const char *cmd[] = {
            "./cmd",
            "-male=False",
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = true; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, false);
    }

    {
        GTEST_LOG_(INFO) << "TEST: ./cmd -male==FALSE, expect false.";
        const char *cmd[] = {
            "./cmd",
            "-male==FALSE",
        };

        CXX_OPT_NAMESPACE::Flag flag;
        bool male = true; // default
        flag.registerBool("male", &male);

        flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
        EXPECT_EQ(male, false);
    }
}

TEST(Flag, parse_type_float) {

    const char *cmd [] = {
        "./cmd",
        "-pi",
        "3.141592"
    };

    CXX_OPT_NAMESPACE::Flag flag;
    float pi = 0; // default
    flag.registerFloat("pi", &pi);

    flag.parse(sizeof cmd / sizeof cmd[0], (char **)&cmd);
    EXPECT_EQ(pi, 3.141592f);

}
