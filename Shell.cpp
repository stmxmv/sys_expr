// -*- sys -*-
//===--------------------------- `target name` ---------------------------------===//
//
// Shell.cpp
//
// Created by Molybdenum on 3/18/23.
//===----------------------------------------------------------------------===//

#include "Shell.hpp"

#include <iostream>
#include <string>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <cstring>

namespace Ash {

namespace charinfo {

inline bool isASCII(char Ch) {
    return static_cast<unsigned char>(Ch) <= 127;
}

inline bool isVerticalWhitespace(char Ch) {
    return isASCII(Ch) && (Ch == '\r' || Ch == '\n');
}

inline bool isHorizontalWhitespace(char Ch) {
    return isASCII(Ch) && (Ch == ' ' || Ch == '\t' ||
                           Ch == '\f' || Ch == '\v');
}

inline bool isWhitespace(char Ch) {
    return isHorizontalWhitespace(Ch) ||
           isVerticalWhitespace(Ch);
}

inline bool isDigit(char Ch) {
    return isASCII(Ch) && Ch >= '0' && Ch <= '9';
}

inline bool isHexDigit(char Ch) {
    return isASCII(Ch) &&
           (isDigit(Ch) || (Ch >= 'A' && Ch <= 'F'));
}

inline bool isIdentifierHead(char Ch) {
    return isASCII(Ch) &&
           (Ch == '_' || (Ch >= 'A' && Ch <= 'Z') ||
            (Ch >= 'a' && Ch <= 'z'));
}

inline bool isIdentifierBody(char Ch) {
    return isIdentifierHead(Ch) || isDigit(Ch);
}

}// namespace charinfo

Shell &Shell::SharedShell() {
    static Shell sh;
    return sh;
}


void Shell::run(int _argc, const char **_argv) {
    std::string line_buffer;
    for (;;) {

        {
            std::unique_ptr<char, decltype(&free)> wd(getcwd(nullptr, 0), &free);
            std::string_view wd_view(wd.get());
            int pos = (int) wd_view.find_last_of("/\\");
            printf("[ash] %s $ ", wd.get() + pos + 1);
        }


        std::getline(std::cin, line_buffer);

        ShellInputToken token;
        inputTokens.clear();
        curPtr = line_buffer.c_str(); //NOLINT

        int pd[2] = { -1, -1 };
        int in = STDIN_FILENO;

        for (;;) {
            nextToken(token);

            if (token.isOneOf(ShellInputTokenKind::Eof, ShellInputTokenKind::Pipe, ShellInputTokenKind::ReIn, ShellInputTokenKind::ReOut)) {
                if (inputTokens.empty()) {
                    /// no input
                    break;
                }

                /// create pipe if input '|'
                if (token.is(ShellInputTokenKind::Pipe)) {
                    UNIX_GUARD(pipe(pd));
                }

                /// process argc and argv
                int argc = (int) inputTokens.size();
                std::vector<const char *> argv;

                std::vector<std::string> temp_strings;

                temp_strings.reserve(inputTokens.size());
                for (auto &inputToken : inputTokens) {
                    temp_strings.emplace_back(inputToken.getRawData());
                }
                argv.reserve(temp_strings.size());
                for (auto &&string : temp_strings) {
                    argv.push_back(string.c_str());
                }
                argv.push_back(nullptr);

                /// check if builtin command
                if (auto iter = builtinMap.find(inputTokens.front().getRawData()); iter != builtinMap.end()) {
                    int stdin_backup = dup(STDOUT_FILENO);
                    if (token.isNot(ShellInputTokenKind::Eof)) {
                        UNIX_GUARD(dup2(pd[1], STDOUT_FILENO));
                        UNIX_GUARD(close(pd[1]));
                        pd[1] = -1;
                    }
                    iter->second->execute(argc, argv.data());
                    if (token.isNot(ShellInputTokenKind::Eof)) {
                        UNIX_GUARD(dup2(stdin_backup, STDOUT_FILENO));
                        UNIX_GUARD(close(stdin_backup));
                    }
                } else {
                    pid_t pid;
                    switch (pid = fork()) {
                        // fork子进程失败
                        case -1:
                            printf("Failed to fork child process");
                            return;
                        case 0: {

                            if (in != STDIN_FILENO) {
                                UNIX_GUARD(dup2(in, STDIN_FILENO));
                                UNIX_GUARD(close(in));
                            } else if (pd[0] != -1) {
                                UNIX_GUARD(close(pd[0]));
                            }

                            if (token.isNot(ShellInputTokenKind::Eof) && pd[1] != -1) {
                                UNIX_GUARD(dup2(pd[1], STDOUT_FILENO));
                                UNIX_GUARD(close(pd[1]));
                            }

                            do {

                                if (token.is(ShellInputTokenKind::ReIn)) {

                                    nextToken(token);
                                    if (token.isNot(ShellInputTokenKind::Text)) {
                                        printf("Not provide redirect input\n");
                                        exit(0);
                                    }

                                    std::string fileName(token.getRawData());

                                    int fd;
                                    UNIX_GUARD(fd = open(fileName.c_str(), O_RDONLY, 0600));
                                    UNIX_GUARD(dup2(fd, STDIN_FILENO));
                                    UNIX_GUARD(close(fd));

                                } else if (token.is(ShellInputTokenKind::ReOut)) {

                                    nextToken(token);
                                    if (token.isNot(ShellInputTokenKind::Text)) {
                                        printf("Not provide redirect output\n");
                                        exit(0);
                                    }

                                    std::string fileName(token.getRawData());
                                    int fd;
                                    UNIX_GUARD(fd = open(fileName.c_str(), O_WRONLY|O_APPEND|O_CREAT|O_APPEND, 0600));
                                    UNIX_GUARD(dup2(fd, STDOUT_FILENO));
                                    UNIX_GUARD(close(fd));
                                }

                                nextToken(token);

                            } while (token.isOneOf(ShellInputTokenKind::ReIn, ShellInputTokenKind::ReOut));


                            execvp(argv[0], (char *const *) argv.data());

                            printf("%s: command error\n", argv[0]);
                            exit(1);
                        }
                        default: {
                            int status;
                            signal(SIGINT, [](int sig) {
                                signal(sig, SIG_DFL);
                                puts("");
                            });

                            if (pd[1] != -1) {
                                UNIX_GUARD(close(pd[1])); /// no need to write to pipe
                                pd[1] = -1;
                            }

                            waitpid(pid, &status, 0);
                            int err = WEXITSTATUS(status);

                            if (err) {
                                printf("Error: %s\n", strerror(err));
                            }
                        }
                    }
                }

                if (in != STDIN_FILENO) {
                    UNIX_GUARD(close(in));
                }
                in = pd[0]; /// next input pipe

                /// parent process not handle reIn and reOut
                if (token.isOneOf(ShellInputTokenKind::ReIn, ShellInputTokenKind::ReOut, ShellInputTokenKind::Eof)) {
                    break;
                }

                inputTokens.clear();
                continue;
            }
            inputTokens.push_back(token);
        }
    }
}


void Shell::nextToken(ShellInputToken &result) {
    /// skip all kinds of white space
    while (*curPtr && charinfo::isWhitespace(*curPtr)) {
        ++curPtr;
    }

    /// check if end of string
    if (!*curPtr) {
        result.setKind(ShellInputTokenKind::Eof);
        return;
    }

    switch (*curPtr) {
        case '|':
            formToken(result, curPtr + 1, ShellInputTokenKind::Pipe);
            return;
        case '<':
            formToken(result, curPtr + 1, ShellInputTokenKind::ReIn);
            return;
        case '>':
            formToken(result, curPtr + 1, ShellInputTokenKind::ReOut);
            return;
    }

    formText(result);
}

void Shell::formToken(ShellInputToken &Result, const char *TokEnd, ShellInputTokenKind Kind) {
    size_t TokLen = TokEnd - curPtr;
    Result.ptr    = curPtr;

    Result.length = TokLen;
    Result.kind   = Kind;
    curPtr        = TokEnd;
}

void Shell::formText(ShellInputToken &result) {
    const char *start = curPtr;
    const char *end   = curPtr + 1;
    while (*end && !charinfo::isWhitespace(*end)) {
        ++end;
    }

    formToken(result, end, ShellInputTokenKind::Text);
}

}// namespace Ash