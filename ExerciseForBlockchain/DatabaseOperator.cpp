#include "DatabaseOperator.h"

std::string DatabaseOperator::initilizeConnection(int argc, char* argv[])
{
    try
    {
        db = std::make_unique<odb::mysql::database>(argc, argv);
    }
    catch (const odb::exception& e)
    {
       return e.what();
    }
    return "";
}
unsigned long int DatabaseOperator::registerNewUser(Users user)
{
    transaction t(db->begin());
    unsigned long int userid = db->persist(user);
    t.commit();
    return userid;
}

std::unique_ptr<std::list<Users>> DatabaseOperator::enumerateUsers(unsigned int pageSize, unsigned int pageNumber)
{
	//if (pageSize < 1)
	 //std::throw "Error: page size lower than 1";
    unsigned long int offset = (pageNumber-1) * pageSize;
    transaction t(db->begin());
    
    // Note that for most databases to return consistent result the
    // query has to have the ORDER BY clause.
    query q("ORDER BY" + query::id +
        +"LIMIT" + query::_val(offset) + ","
        + query::_val(pageSize));

    result r(db->query<Users>(q));
    std::unique_ptr<std::list<Users>> vecUsersPtr = std::make_unique<std::list<Users>>( r.begin(), r.end());
    t.commit();
    return vecUsersPtr;
}

Users DatabaseOperator::getUserById(unsigned long int id)
{
    transaction t(db->begin());
    result r(db->query<Users>(query::id == id));
    Users resultUser = *(r.begin());
    t.commit();
    return resultUser;
}