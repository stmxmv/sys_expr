// -*- sys -*-
//===--------------------------- `target name` ---------------------------------===//
//
// typedef.h
// 
// Created by Molybdenum on 3/18/23.
//===----------------------------------------------------------------------===//

#ifndef SYS_TYPEDEF_H
#define SYS_TYPEDEF_H

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#define UNIX_GUARD(stmt) \
    do {\
    if ((long long int)(stmt) < 0) {    \
        perror(#stmt);   \
        exit(-1); \
    } } while (0)\

#endif //SYS_TYPEDEF_H
