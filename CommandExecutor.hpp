// -*- sys -*-
//===--------------------------- `target name` ---------------------------------===//
//
// CommandExecutor.hpp
//
// Created by Molybdenum on 3/18/23.
//===----------------------------------------------------------------------===//

#ifndef SYS_COMMANDEXECUTOR_HPP
#define SYS_COMMANDEXECUTOR_HPP

namespace Ash {

class BuiltinCommandExecutor {
public:

    BuiltinCommandExecutor() noexcept = default;

    virtual int execute(int argc, const char * argv[]) = 0;

    virtual ~BuiltinCommandExecutor() = default;

};


}

#endif//SYS_COMMANDEXECUTOR_HPP
