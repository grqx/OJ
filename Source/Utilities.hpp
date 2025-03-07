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

#include "Exception.hpp"
#include "Logger.hpp"
#include <vector>

#define DEBUG_HERE                                                                                                    \
    {                                                                                                                 \
        Logger.Info(std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + std::string(__PRETTY_FUNCTION__)); \
        std::cout << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << std::endl;                          \
    }

class UTILITIES {
  private:
    static size_t UploadFunction(char *ptr, size_t size, size_t nmemb, void *userp);

  public:
    static std::string RandomToken();
    static std::string StringReplaceAll(std::string Data, std::string Search, std::string Replace);
    static std::vector<std::string> StringSplit(std::string Data, std::string Delimiter);
    static std::string StringJoin(std::vector<std::string> Data, std::string Delimiter);
    static void LoadFile(std::string Filename, std::string &Output);
    static void LoadFile(std::string Filename, int &Output);
    static void SaveFile(std::string Filename, std::string Data);
    static void SaveFile(std::string Filename, int Data);
    static std::string RemoveSpaces(std::string Input);
    static void SendEmail(std::string To, std::string Subject, std::string Body);
    static time_t StringToTime(std::string String);
    static bool VerifySignature(std::string Data, std::string Signature, std::string PublicKey);
    static std::string HashData(std::string Data);
};
