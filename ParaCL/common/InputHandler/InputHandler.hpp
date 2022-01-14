#ifndef INPUT_HANDLER_H__
#define INPUT_HANDLER_H__

#include <fstream>
#include <iostream>
#include <string>

struct InputHandler final {
    int argc_ = 0;
    char **argv_ = nullptr;

    std::fstream code_;

    std::string msg_;

    InputHandler () {}

    void setParams (const int argc, char **argv)
    {
        argv_ = argv;
        argc_ = argc;

        if (argc_ != 2) {
            throw std::runtime_error ("You have to enter input filename only!");
        }

        code_.open (argv_[1], std::ios::in);
        if (!code_.is_open ()) {
            throw std::runtime_error ("Can't open input file");
        }
    }
};

#endif
