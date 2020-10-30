#pragma once
#include <memory>
#include <iostream>

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include <odb/mysql/database.hxx>

#include "Users.hxx"
#include "Users-odb.hxx"

using namespace std;
using namespace odb::core;

using namespace odb::core;

class DatabaseOperator
{
    typedef odb::query<Users> query;
    typedef odb::result<Users> result;
public:
    std::string initilizeConnection(int argc, char* argv[]);//std::string user, std::string password, std::string host, std::string database, unsigned port = 3306
    void registerNewUser(Users user);
    std::unique_ptr<std::list<Users>> enumerateUsers(unsigned int pageSize, unsigned int pageNumber);
    Users getUserById(unsigned long int id);

private: 
    unique_ptr <database> db;
};

