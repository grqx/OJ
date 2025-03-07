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

#include "Database.hpp"
#include "Settings.hpp"
#include "Utilities.hpp"

DATABASE::SELECT::SELECT(std::string TableName) {
    this->Connection = DATABASE::CreateConnection();
    this->TableName = TableName;
}
DATABASE::SELECT::~SELECT() { DATABASE::CloseConnection(Connection); }
DATABASE::SELECT &DATABASE::SELECT::Select(std::string Column) {
    this->Columns.push_back(Column);
    return *this;
}
DATABASE::SELECT &DATABASE::SELECT::Where(std::string Column, std::string Value) {
    this->Conditions.push_back({Column, Value});
    return *this;
}
DATABASE::SELECT &DATABASE::SELECT::Where(std::string Column, int Value) { return Where(Column, std::to_string(Value)); }
DATABASE::SELECT &DATABASE::SELECT::Order(std::string Column, bool Ascending) {
    this->Orders.push_back({Column, Ascending});
    this->UseOrder = true;
    return *this;
}
DATABASE::SELECT &DATABASE::SELECT::Limit(int Limits) {
    this->Limits = Limits;
    return *this;
}
DATABASE::SELECT &DATABASE::SELECT::Offset(int Offsets) {
    this->Offsets = Offsets;
    return *this;
}
void DATABASE::SELECT::Execute(std::function<void(std::vector<std::map<std::string, std::string>>)> Callback) {
    if (Connection == nullptr)
        throw EXCEPTION("Not connected");
    if (Columns.empty()) {
        sql::ResultSet *ResultSet(Connection->createStatement()->executeQuery("DESCRIBE " + TableName));
        while (ResultSet->next())
            Columns.push_back(ResultSet->getString("Field"));
        ResultSet->close();
        delete ResultSet;
    }
    std::string Query = "SELECT ";
    for (auto &Column : Columns)
        Query += "`" + Column + "`, ";
    Query.erase(Query.end() - 2, Query.end());
    Query += " FROM `" + TableName + "`";
    if (!Conditions.empty()) {
        Query += " WHERE ";
        for (auto &Condition : Conditions)
            Query += "`" + Condition.first + "`=? AND ";
        Query.erase(Query.end() - 5, Query.end());
    }
    if (!Orders.empty()) {
        Query += " ORDER BY ";
        for (auto &Order : Orders)
            Query += "`" + Order.first + "` " + (Order.second ? "ASC" : "DESC") + ", ";
        Query.erase(Query.end() - 2, Query.end());
    }
    if (Limits)
        Query += " LIMIT " + std::to_string(Limits);
    if (Offsets)
        Query += " OFFSET " + std::to_string(Offsets);
    try {
        sql::PreparedStatement *PreparedStatement(Connection->prepareStatement(Query));
        int Index = 1;
        for (auto &Condition : Conditions)
            PreparedStatement->setString(Index++, Condition.second);
        sql::ResultSet *ResultSet(PreparedStatement->executeQuery());
        PreparedStatement->close();
        delete PreparedStatement;
        std::vector<std::map<std::string, std::string>> Data;
        while (ResultSet->next()) {
            std::map<std::string, std::string> Row;
            for (auto &Column : Columns)
                Row[Column] = ResultSet->getString(Column);
            Data.push_back(Row);
        }
        if (Callback != nullptr)
            Callback(Data);
        ResultSet->close();
        delete ResultSet;
    } catch (sql::SQLException &e) {
        throw EXCEPTION("SQLException " + std::to_string(e.getErrorCode()) + " " + e.what() + " " + e.getSQLState());
    }
}

DATABASE::INSERT::INSERT(std::string TableName) {
    this->Connection = DATABASE::CreateConnection();
    this->TableName = TableName;
}
DATABASE::INSERT::~INSERT() { DATABASE::CloseConnection(Connection); }
DATABASE::INSERT &DATABASE::INSERT::Insert(std::string Column, std::string Value) {
    this->Columns.push_back(Column);
    this->Values.push_back(Value);
    return *this;
}
DATABASE::INSERT &DATABASE::INSERT::Insert(std::string Column, int Value) { return Insert(Column, std::to_string(Value)); }
void DATABASE::INSERT::Execute(std::function<void(int)> Callback) {
    if (Connection == nullptr)
        throw EXCEPTION("Not connected");
    std::string Query = "INSERT INTO " + TableName + " (";
    for (auto &Column : Columns)
        Query += "`" + Column + "`, ";
    Query.erase(Query.end() - 2, Query.end());
    Query += ") VALUES (";
    for (size_t i = 0; i < Values.size(); i++)
        Query += "?, ";
    Query.erase(Query.end() - 2, Query.end());
    Query += ")";
    try {
        sql::PreparedStatement *PreparedStatement(Connection->prepareStatement(Query));
        int Index = 1;
        for (auto &Value : Values)
            PreparedStatement->setString(Index++, Value);
        PreparedStatement->execute();
        PreparedStatement->close();
        delete PreparedStatement;
        sql::ResultSet *ResultSet(Connection->createStatement()->executeQuery("SELECT LAST_INSERT_ID()"));
        ResultSet->next();
        if (Callback != nullptr)
            Callback(ResultSet->getInt(1));
        ResultSet->close();
        delete ResultSet;
    } catch (sql::SQLException &e) {
        throw EXCEPTION("SQLException " + std::to_string(e.getErrorCode()) + " " + e.what() + " " + e.getSQLState());
    }
}

DATABASE::DELETE::DELETE(std::string TableName) {
    this->Connection = DATABASE::CreateConnection();
    this->TableName = TableName;
}
DATABASE::DELETE::~DELETE() { DATABASE::CloseConnection(Connection); }
DATABASE::DELETE &DATABASE::DELETE::Where(std::string Column, std::string Value) {
    this->Conditions.push_back({Column, Value});
    return *this;
}
DATABASE::DELETE &DATABASE::DELETE::Where(std::string Column, int Value) {
    return Where(Column, std::to_string(Value));
}
void DATABASE::DELETE::Execute() {
    if (Connection == nullptr)
        throw EXCEPTION("Not connected");
    std::string Query = "DELETE FROM " + TableName;
    if (!Conditions.empty()) {
        Query += " WHERE ";
        for (auto &Condition : Conditions)
            Query += "`" + Condition.first + "`=? AND ";
        Query.erase(Query.end() - 5, Query.end());
    }
    try {
        sql::PreparedStatement *PreparedStatement(Connection->prepareStatement(Query));
        int Index = 1;
        for (auto &Condition : Conditions)
            PreparedStatement->setString(Index++, Condition.second);
        PreparedStatement->execute();
        PreparedStatement->close();
        delete PreparedStatement;
    } catch (sql::SQLException &e) {
        throw EXCEPTION("SQLException " + std::to_string(e.getErrorCode()) + " " + e.what() + " " + e.getSQLState());
    }
}

DATABASE::UPDATE::UPDATE(std::string TableName) {
    this->Connection = DATABASE::CreateConnection();
    this->TableName = TableName;
}
DATABASE::UPDATE::~UPDATE() { DATABASE::CloseConnection(Connection); }
DATABASE::UPDATE &DATABASE::UPDATE::Set(std::string Column, std::string Value) {
    this->Columns.push_back({Column, Value});
    return *this;
}
DATABASE::UPDATE &DATABASE::UPDATE::Set(std::string Column, int Value) { return Set(Column, std::to_string(Value)); }
DATABASE::UPDATE &DATABASE::UPDATE::Where(std::string Column, std::string Value) {
    this->Conditions.push_back({Column, Value});
    return *this;
}
DATABASE::UPDATE &DATABASE::UPDATE::Where(std::string Column, int Value) { return Where(Column, std::to_string(Value)); }
void DATABASE::UPDATE::Execute() {
    if (Connection == nullptr)
        throw EXCEPTION("Not connected");
    std::string Query = "UPDATE " + TableName + " SET ";
    for (auto &Column : Columns)
        Query += "`" + Column.first + "`=?, ";
    Query.erase(Query.end() - 2, Query.end());
    if (!Conditions.empty()) {
        Query += " WHERE ";
        for (auto &Condition : Conditions)
            Query += "`" + Condition.first + "`=? AND ";
        Query.erase(Query.end() - 5, Query.end());
    }
    try {
        sql::PreparedStatement *PreparedStatement(Connection->prepareStatement(Query));
        int Index = 1;
        for (auto &Column : Columns)
            PreparedStatement->setString(Index++, Column.second);
        for (auto &Condition : Conditions)
            PreparedStatement->setString(Index++, Condition.second);
        PreparedStatement->execute();
        PreparedStatement->close();
        delete PreparedStatement;
    } catch (sql::SQLException &e) {
        throw EXCEPTION("SQLException " + std::to_string(e.getErrorCode()) + " " + e.what() + " " + e.getSQLState());
    }
}

DATABASE::SIZE::SIZE(std::string TableName) {
    this->Connection = DATABASE::CreateConnection();
    this->TableName = TableName;
}
void DATABASE::SIZE::Execute(std::function<void(int)> Callback) {
    if (Connection == nullptr)
        throw EXCEPTION("Not connected");
    try {
        sql::ResultSet *ResultSet(Connection->createStatement()->executeQuery("SELECT COUNT(*) FROM " + TableName));
        ResultSet->next();
        if (Callback != nullptr)
            Callback(ResultSet->getInt(1));
        ResultSet->close();
        delete ResultSet;
    } catch (sql::SQLException &e) {
        throw EXCEPTION("SQLException " + std::to_string(e.getErrorCode()) + " " + e.what() + " " + e.getSQLState());
    }
}
DATABASE::TRUNCATE::TRUNCATE(std::string TableName) {
    this->Connection = DATABASE::CreateConnection();
    this->TableName = TableName;
}
void DATABASE::TRUNCATE::Execute() {
    if (Connection == nullptr)
        throw EXCEPTION("Not connected");
    try {
        Connection->createStatement()->execute("TRUNCATE TABLE " + TableName);
    } catch (sql::SQLException &e) {
        throw EXCEPTION("SQLException " + std::to_string(e.getErrorCode()) + " " + e.what() + " " + e.getSQLState());
    }
}

sql::Connection *DATABASE::CreateConnection() {
    sql::Connection *Connection;
    try {
        sql::Driver *Driver = sql::mysql::get_mysql_driver_instance();
        Connection = Driver->connect(Settings.DatabaseHost.c_str(),
                                     Settings.DatabaseUsername.c_str(),
                                     Settings.DatabasePassword.c_str());
        Connection->setSchema(Settings.DatabaseName.c_str());
    } catch (sql::SQLException &e) {
        std::cout << "SQLException " << std::to_string(e.getErrorCode()) << " " << e.what() << " " << e.getSQLState() << std::endl;
        return nullptr;
    }
    return Connection;
}
void DATABASE::CloseConnection(sql::Connection *Connection) {
    Connection->close();
    delete Connection;
}
