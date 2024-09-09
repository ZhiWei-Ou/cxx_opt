#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <strings.h>
#include <vector>

#define OPT_SHORT_PREFIX "-"
#define OPT_LONG_PREFIX "--"
#define OPT_OPERATION "="

class Opt {
public:
    Opt() {}
    ~Opt() {}

    typedef void (*optionHandle)(void *);
    enum Type { Type_String = 0x1, Type_Bool = 0x2, Type_Int = 0x4, Type_Function = 0x8, Type_Unkown = (0x0 | ( 0x1 << sizeof(void *) * 8) ) };
    struct option {
        std::string short_;
        std::string long_;
        std::string description_;
        Type type_;
        void *value_;
        union { const char *string_; bool bool_; int int_; void *closure; } default_value_;

        option(const std::string &short_,
                const std::string &long_,
                const std::string &description_,
                Type type_,
                void *value_,
                const char *default_value_)
            : short_(short_), long_(long_), description_(description_),
              type_(type_), value_(value_), default_value_{.string_ = default_value_} { }

        option(const std::string &short_,
                const std::string &long_,
                const std::string &description_,
                Type type_,
                void *value_,
                bool default_value_)
            : short_(short_), long_(long_), description_(description_),
              type_(type_), value_(value_), default_value_{.bool_ = default_value_} { }
        
        option(const std::string &short_,
                const std::string &long_,
                const std::string &description_,
                Type type_,
                void *value_,
                int default_value_)
            : short_(short_), long_(long_), description_(description_),
              type_(type_), value_(value_), default_value_{.int_ = default_value_} { }

        option(const std::string &short_,
                const std::string &long_,
                const std::string &description_,
                Type type_,
                void *func_,
                void *closure_)
            : short_(short_), long_(long_), description_(description_),
              type_(type_), value_(func_), default_value_{.closure = closure_} { }
    };


    Opt &String(const std::string &short_,
            const std::string &long_,
            const std::string &description_,
            std::string &value_,
            const std::string &default_value_ = "")
    { options_.emplace_back(short_, long_, description_, Type_String, &value_, default_value_.c_str()); return *this; }

    Opt &Bool(const std::string &short_,
            const std::string &long_,
            const std::string &description_,
            bool &value_,
            bool default_value_ = false)
    { options_.emplace_back(short_, long_, description_, Type_Bool, &value_, default_value_); return *this; }

    Opt &Int(const std::string &short_,
            const std::string &long_,
            const std::string &description_,
            int &value_,
            int default_value_ = 0)
    { options_.emplace_back(short_, long_, description_, Type_Int, &value_, default_value_); return *this; }

    Opt &Handle(const std::string &short_,
                const std::string &long_,
                const std::string &description_,
                optionHandle func,
                void *closure = nullptr)
    { options_.emplace_back(short_, long_, description_, Type_Function, (void *)func, closure); return *this; }

    Opt &Banner(const std::string &);
    bool Banner() const
    { return banner_; }
    std::string BannerText() const
    { return banner_ ? bannerText_ : ""; }

    Opt &Version(const std::string &);
    bool Version() const
    { return version_; }
    std::string VersionText() const
    { return version_ ? versionText_ : ""; }

    Opt &Helper(bool enable = true);
    void PrintAllOpt(std::ostream &os = std::cout, const char *indent = "\t") const; 

    void Parse(int argc, char *argv[]);

private:
    void todo(Opt::option &opt, char *para);

    bool help_ = false;
    bool version_ = false;
    char versionText_[37];
    bool banner_ = false;
    char bannerText_[37];
    std::vector<Opt::option> options_;
};

inline Opt & Opt::Banner(const std::string &text) {
    banner_ = true;

    strncpy(bannerText_, text.c_str(), sizeof(bannerText_) - 1);
    bannerText_[sizeof(bannerText_) - 1] = '\0';

    return *this;
}

inline Opt &Opt::Helper(bool enable) {
    if (help_) {
        return *this;
    }

    auto helper = [] (void *self) -> void {
        Opt *_this = static_cast<Opt *>(self);

        if (_this->Banner()) {
            printf("%s\n", _this->BannerText().c_str());
        }
        _this->PrintAllOpt();
        std::exit(0);
    };

    if (!help_ && enable) {
       this->Handle("h", "help", "show help.", static_cast<Opt::optionHandle>(helper), this); 
       help_ = true;
    }

    return *this;
}

inline Opt & Opt::Version(const std::string &version) {
    strncpy(versionText_, version.c_str(), sizeof(versionText_) - 1);
    versionText_[sizeof(versionText_) - 1] = '\0';

    auto ver = [] (void *self) -> void {
        Opt *_this = static_cast<Opt *>(self);
        printf("%s\n", _this->VersionText().c_str());
        std::exit(0);
    };

    if (!version_) {
        this->Handle("v", "version", "show version.", static_cast<Opt::optionHandle>(ver), this);
        version_ = true;
    }

    return *this;
}

inline void Opt::PrintAllOpt(std::ostream &os, const char *indent) const {
    for (auto &opt : options_) {
        os << indent << '-' << opt.short_
            << ", --" << opt.long_ << ", Description: " << opt.description_ << '\n';
    }
}

inline void Opt::todo(Opt::option &opt, char *para) {
    if (opt.type_ & Type_String) {
        *(static_cast<std::string *>(opt.value_)) = para;
    } else if (opt.type_ & Type_Bool) {
        *(static_cast<bool *>(opt.value_)) = strcasecmp(para, "true") ? false : true;
    } else if (opt.type_ & Type_Int) {
        *(static_cast<int *>(opt.value_)) = atoi(para);
    } else if (opt.type_ & Type_Function) {
        ( (optionHandle)(opt.value_) )(opt.default_value_.closure);
    }
}


inline void Opt::Parse(int argc, char *argv[])
{
    char *prog = argv[0];

    for (int i=1;i < argc;++i) {
        char *arg = argv[i]; 

        if (arg[0] == '-') {
            if (arg[1] == '-') {
                // It is long option
                char *_opt = &arg[2];
                char *para = strtok(_opt, OPT_OPERATION);

                for (auto &opt : options_) {
                    if (opt.long_ == para) {
                        if (para != nullptr) {
                            para = strtok(nullptr, OPT_OPERATION);
                            todo(opt, para);
                        }
                        break;
                    }
                }

                continue;
            }

            // It is short option
            char *_opt = &arg[1];            
            for (auto &opt : options_) {
                if (opt.short_[0] == _opt[0]) {
                    char *para = strtok(_opt, OPT_OPERATION);
                    if (para != nullptr) {
                        para = strtok(nullptr, OPT_OPERATION);
                        todo(opt, para);
                    }
                    break;
                }
            }
        }
    }
}
