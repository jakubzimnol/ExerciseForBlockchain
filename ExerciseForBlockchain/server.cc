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

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "Users.grpc.pb.h"

// ODB
#include "DatabaseOperator.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerWriter;
using usersProto::User; 
using usersProto::Id;
using usersProto::Pagination;
using usersProto::RegistrationResult;
using usersProto::DatabaseService;

void transformToProtoUser(Users& odbUser, User* protobufUser)
{
    protobufUser->set_nickname(odbUser.nickname());
    protobufUser->set_email(odbUser.email());
    protobufUser->set_password(odbUser.password());
    protobufUser->set_isactive(odbUser.isActive());
    protobufUser->set_id(odbUser.id());
}

// Logic and data behind the server's behavior.
class DatabaseServiceImpl final : public DatabaseService::Service {

    public:
        DatabaseServiceImpl::DatabaseServiceImpl(int argc, char* argv[])
        {
            dbOp.initilizeConnection(argc, argv);
        }
    private:
        //methods
      Status getUserById(ServerContext* context, const Id* request, User* reply) override 
      {
        Users requestedUser = dbOp.getUserById(request->id());
        transformToProtoUser(requestedUser, reply);
        return Status::OK;
      }
      Status registerNewUser(ServerContext* context, const User* request, RegistrationResult* reply) override 
      {
        Users createdUser(request->email(), request->nickname(), request->password(), request->isactive());
        dbOp.registerNewUser(createdUser); // this shuld return status or ID created user 
        reply->set_message(true);
        return Status::OK;
      }
      Status enumerateUsers(ServerContext* context, const Pagination* request, ServerWriter<User>* writer) override
      {
          std::unique_ptr<std::list<Users>> usersList = dbOp.enumerateUsers(request->pagesize(), request->pagenumber());
          std::for_each(usersList->begin(), usersList->end(), [&writer](Users& user)
              {
                  User replyUser;
                  transformToProtoUser(user, &replyUser);
                  writer->Write(replyUser);
              });
          return Status::OK;
      }
      // members
      DatabaseOperator dbOp;
};

void RunServer(int argc, char* argv[]) {
  std::string server_address("0.0.0.0:50051");
  DatabaseServiceImpl service(argc, argv);

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer(argc, argv);

  return 0;
}
