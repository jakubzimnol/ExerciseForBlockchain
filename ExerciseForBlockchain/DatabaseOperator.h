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
	 // Initialize connection to database
	 // returns empty string if success otherwise error message
    std::string initilizeConnection(int argc, char* argv[]);
	 
	 // register new user to database    
    // returns id off newly created user
    unsigned long int registerNewUser(Users user);
    
    // returns users regarding pagination data (pageNumber must be higher than 0)
    std::unique_ptr<std::list<Users>> enumerateUsers(unsigned int pageSize, unsigned int pageNumber);
    
    // returns user by id
    Users getUserById(unsigned long int id);

private: 
    unique_ptr <database> db;
};

