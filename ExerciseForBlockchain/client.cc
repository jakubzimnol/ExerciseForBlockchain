/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

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
   std::string getUserById(unsigned long int usersId) {
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
    if (status.ok()) {
      return toString(reply);
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }

  }

    std::string registerNewUser(const std::string& nickname, const std::string& email, const std::string& password, const bool& isActive ) {
      // Follows the same pattern as SayHello.
      User request;
      request.set_email(email);
      request.set_nickname(nickname);
      request.set_password(password);
      request.set_isactive(isActive);
      RegistrationResult reply;
      ClientContext context;

      // Here we can use the stub's newly available method we just added.
      Status status = stub_->registerNewUser(&context, request, &reply);
      if (status.ok()) {
          if (reply.message())
              return "User was added successfully";
          else
            return "Adding user failed";
      } 
      else 
      {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return "RPC failed";
      }
    }

    std::unique_ptr<std::list<std::string>> enumerateUsers(unsigned int pageSize, unsigned int pageNumber) {
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
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
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
  std::string user("world");
  std::string reply = databaseClient.registerNewUser("John@mail.com", "John Doe", "John1234", true);
  std::cout << "DatabaseClient results: " << reply << std::endl;

  reply = databaseClient.getUserById(1);
  std::cout << "DatabaseServer received: " << reply << std::endl;

  int pageSize=5, pageNumber=2;
  std::unique_ptr<std::list<std::string>> usersList = databaseClient.enumerateUsers(pageSize, pageNumber);
  std::cout << "DatabaseServer received users from page "<< pageNumber << std::endl;
  std::for_each(usersList->begin(), usersList->end(), [](std::string& user)
      {
          std::cout << user;
      });

  return 0;
}
