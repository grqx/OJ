/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2025  langningchen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**********************************************************************/

#pragma once

#include <string>

enum JUDGE_RESULT {
    UNKNOWN_ERROR,
    ACCEPTED,
    PRESENTATION_ERROR,
    WRONG_ANSWER,
    TIME_LIMIT_EXCEEDED,
    MEMORY_LIMIT_EXCEEDED,
    OUTPUT_LIMIT_EXCEEDED,
    RUNTIME_ERROR,
    RESTRICTED_FUNCTION,
    COMPILATION_ERROR,
    SYSTEM_ERROR,
    WAITING,
    FETCHED,
    COMPILING,
    COMPILED,
    JUDGING,
    JUDGED,
    COMPARING,
    SKIPPED,
    REJECTED
};
