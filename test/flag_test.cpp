#include <gtest/gtest.h>
#include "../cxx_opt.h"

TEST(Flag, exception) {

    GTEST_LOG_(INFO) << "TODO: nil pointer exception";

    GTEST_LOG_(INFO) << "TODO: invalid boolean value exception";

    GTEST_LOG_(INFO) << "TODO: flag contains '=' exception";
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
