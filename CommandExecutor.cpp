// -*- sys -*-
//===--------------------------- `target name` ---------------------------------===//
//
// CommandExecutor.cpp
//
// Created by Molybdenum on 3/18/23.
//===----------------------------------------------------------------------===//

#include "CommandExecutor.hpp"
#include "Shell.hpp"

#include <cstdlib>
#include <unistd.h>

namespace Ash {


class BuiltinExit : public BuiltinCommandExecutor {

public:
    int execute(int argc, const char **argv) override {
        exit(0);
    }
};

REGISTER_BUILTIN_COMMAND(BuiltinExit, "exit");

class BuiltinCd : public BuiltinCommandExecutor {
public:
    int execute(int argc, const char **argv) override {
        if (argc != 2) {
            printf("cd with illegal arguments\n");
            return -1;
        }
        if (chdir(argv[1]) < 0) {
            printf("cd error\n");
            return -1;
        }
        return 0;
    }
};

REGISTER_BUILTIN_COMMAND(BuiltinCd, "cd");


class BuiltinPwd : public BuiltinCommandExecutor {

public:
    virtual int execute(int arc, const char * argv[]) override {
        char *wd = getcwd(nullptr, 0);
        puts(wd);
        free(wd);
        return 0;
    }

};

REGISTER_BUILTIN_COMMAND(BuiltinPwd, "pwd");


class BuiltinClear : public BuiltinCommandExecutor {

public:
    virtual int execute(int arc, const char * argv[]) override {
        printf("\e[1;1H\e[2J");
        return 0;
    }
};

REGISTER_BUILTIN_COMMAND(BuiltinClear, "clear");

}