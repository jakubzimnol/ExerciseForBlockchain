#include "DatabaseOperator.h"

int
main(int argc, char* argv[])
{
    try
    {
        DatabaseOperator dbOp;
        dbOp.initilizeConnection(argc, argv);//
        Users john("John@mail.com", "John Doe", "John1234", true);
        dbOp.registerNewUser(john);
        Users user = dbOp.getUserById(2); 
        std::unique_ptr<std::list<Users>> vec = dbOp.enumerateUsers(2, 2);
        if (vec->size()>0)
        {
            Users user2 = vec->front();
            std::string str = user2.nickname();
        }

    }
    catch (const odb::exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }
}