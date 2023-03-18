// -*- sys -*-
//===--------------------------- `target name` ---------------------------------===//
//
// Shell.hpp
// 
// Created by Molybdenum on 3/18/23.
//===----------------------------------------------------------------------===//

#ifndef SYS_SHELL_HPP
#define SYS_SHELL_HPP

#include "typedef.h"
#include "CommandExecutor.hpp"

#include <unordered_map>
#include <string_view>
#include <vector>
#include <memory>

namespace Ash {

enum class ShellInputTokenKind {
    Unknown,
    Eof,
    Text,
    Pipe, // '|'
    ReIn, // '<'
    ReOut, // '>'
    Num
};

class ShellInputToken {
    ShellInputTokenKind kind;
    const char *ptr;
    size_t length;

    friend class Shell;

public:
    ShellInputToken() : kind(ShellInputTokenKind::Unknown), ptr(), length() {}

    bool is(ShellInputTokenKind K) const { return kind == K; }

    bool isNot(ShellInputTokenKind K) const { return kind != K; }

    bool isOneOf(ShellInputTokenKind K1, ShellInputTokenKind K2) const {
        return is(K1) || is(K2);
    }

    template<typename... Ts>
    bool isOneOf(ShellInputTokenKind K1, ShellInputTokenKind K2,
                 Ts... Ks) const {
        return is(K1) || isOneOf(K2, Ks...);
    }

    void setKind(ShellInputTokenKind aKind) {
        kind = aKind;
    }

    ShellInputTokenKind getKind() const {
        return kind;
    }

    const size_t &getLength() const {
        return length;
    }

    std::string_view getRawData() {
        return {ptr, length};
    }

};

class Shell {

    std::unordered_map<std::string_view, std::unique_ptr<BuiltinCommandExecutor>> builtinMap;

    const char *curPtr;
    std::vector<ShellInputToken> inputTokens;

    void formToken(ShellInputToken &Result, const char *TokEnd,
                   ShellInputTokenKind Kind);

    void formText(ShellInputToken &result);

    void nextToken(ShellInputToken &Result);

public:

    static Shell &SharedShell();

    void run(int argc, const char * argv[]);

    template<typename T>
    void registerBuiltinCommand(std::string_view name) {
        builtinMap.insert({ name, std::make_unique<T>() });
    }

};

namespace detail {
template<typename T>
struct BuiltinCommandRegister {
    explicit BuiltinCommandRegister(std::string_view name) {
        Shell::SharedShell().registerBuiltinCommand<T>(name);
    }
};
}

#define REGISTER_BUILTIN_COMMAND(cls, name) static Ash::detail::BuiltinCommandRegister<cls> TOKENPASTE2(__##cls##Regsister, __LINE__)(name)

}

#endif //SYS_SHELL_HPP
