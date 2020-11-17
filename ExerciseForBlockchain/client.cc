#include <iostream>
#include <memory>
#include <string>
#include <sstream> 
#include <grpcpp/grpcpp.h>

#include "Users.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;
using usersProto::User;
using usersProto::Id;
using usersProto::Pagination;
using usersProto::RegistrationResult;
using usersProto::DatabaseService;

class DatabaseClient {
 public:
  DatabaseClient(std::shared_ptr<Channel> channel)
      : stub_(DatabaseService::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
   std::string getUserById(unsigned long int usersId, std::string& statusMsg) {
    // Data we are sending to the server.
   Id request;
    request.set_id(usersId);

    // Container for the data we expect from the server.
    User reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->getUserById(&context, request, &reply);

    // Act upon its status.
    if (status.ok())
      return toString(reply);
    else {
        std::stringstream sstr;
        sstr << "RPC failed with status code: " << status.error_code() << " and message: " << status.error_message() <<std::endl;
        statusMsg = sstr.str();
        return "";
    }
  }

    bool registerNewUser(const std::string& nickname, const std::string& email, const std::string& password, const bool& isActive, unsigned long int& resultId, std::string& statusMsg) {
      User request;
      request.set_email(email);
      request.set_nickname(nickname);
      request.set_password(password);
      request.set_isactive(isActive);
      RegistrationResult reply;
      ClientContext context;

      Status status = stub_->registerNewUser(&context, request, &reply);
      if (status.ok()) {
      	resultId = reply.resultid();
          if (resultId)
              return true;
          else
          {
				statusMsg = "Adding user failed";
				return false;
          }
      }
      else
      {
        std::stringstream sstr;
        sstr << "RPC failed with status code: " << status.error_code() << " and message: " << status.error_message() <<std::endl;
		  statusMsg = sstr.str();
        return false;
      }
    }

    std::unique_ptr<std::list<std::string>> enumerateUsers(unsigned int pageSize, unsigned int pageNumber, std::string& statusMsg ) {
        Pagination request;
        request.set_pagenumber(pageNumber);
        request.set_pagesize(pageSize);
        User reply;
        ClientContext context;
        std::unique_ptr<std::list<std::string>> returnedStingList = std::make_unique<std::list<std::string>>();
        std::unique_ptr<ClientReader<User>> reader(stub_->enumerateUsers(&context, request));
        while (reader->Read(&reply)) {
            returnedStingList->push_back(toString(reply));
        }
        Status status = reader->Finish();
        if (!status.ok()) {
        	  std::stringstream sstr;
           sstr << "RPC failed with status code: " << status.error_code() << " and message: " << status.error_message() <<std::endl;
		     statusMsg = sstr.str();
           return returnedStingList;
        }
        return returnedStingList;
    }

 private:
     std::string toString(User& user)
     {
         std::stringstream sstr;
         sstr << "Id: " << user.id() << std::endl
              << "nickname: " << user.nickname() << std::endl
              << "email: " << user.email() << std::endl
              << "password: " << user.password() << std::endl
              << "isActive: " << user.isactive() << std::endl;
         return sstr.str();
     }

  std::unique_ptr<DatabaseService::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target=" << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  
  DatabaseClient databaseClient(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  
  //registering new user
  ///////////////////////  
  unsigned long int id;
  std::string statusMsg ="";
  if(databaseClient.registerNewUser("John@mail.com", "John Doe", "John1234", true, id, statusMsg))
    std::cout << "Registered new user with id: "<< id << std::endl;
  else
    std::cout << statusMsg << std::endl;
  
  // getting user by id
  /////////////////////
  statusMsg = "";
  std::string replyUser = databaseClient.getUserById(1, statusMsg);
   if(statusMsg.empty())
  		std::cout << "Received user: " << replyUser << std::endl;
  	else
  		std::cout << statusMsg << std::endl;

  //enumerate users by pagination
  ///////////////////////////////
  statusMsg = "";
  int pageSize=5, pageNumber=2;
  std::unique_ptr<std::list<std::string>> usersList = databaseClient.enumerateUsers(pageSize, pageNumber, statusMsg);
  if (statusMsg.empty())
  {
  	 std::cout << "DatabaseServer received users from page "<< pageNumber << std::endl;
  	 std::for_each(usersList->begin(), usersList->end(), [](std::string& user)
      {
          std::cout << user;
      });
	}
	else
		std::cout << statusMsg << std::endl;
  return 0;
}
