Generally to build this project you need to install dependencies first.
 - ODB (version 2.4.0) https://www.codesynthesis.com/products/odb/,
 - libmysqlclient (version 8.0.22) https://dev.mysql.com/downloads/connector/cpp/,
 - gRPC (version 3.13, including protobuf) https://grpc.io/docs/languages/cpp/quickstart/
 Then you can build and finally run this project.
 Process of installation, build and run varies in different operating systems.
 
 -----------------------------------------------------------------------
 Linux
 -----------------------------------------------------------------------
 // You may open terminall and follow this instructions:
 // Install cmake:
 sudo apt install -y cmake
 
 // Install Git
 sudo apt install -y git
 
 // Install the basic tools required to build gRPC: 
 sudo apt install -y build-essential autoconf libtool pkg-config
 
 // Clone grpc repository (you may change version to the newest one):
 git clone --recurse-submodules -b v1.33.2 https://github.com/grpc/grpc
 cd grpc
 
 // prepare local directory for installing grpc ie.:
 export MY_INSTALL_DIR=$HOME/.local
 export PATH="$PATH:$MY_INSTALL_DIR/bin"
 
 //Build and locally install gRPC and all requisite tools:
 mkdir -p cmake/build
 pushd cmake/build
 cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../..
 make
 make install
 popd
 
 // Install libmysqlclient
  sudo apt install -y libmysqlclient-dev
 // Install ODB (with libodb-2.4, libobd-mysql-2.4)
  sudo apt install -y odb
 
 // After all dependencies are installed you may build this project using cmake. You may create specified folder for build (not necessary)
 mkdir -p cmake/build
 pushd cmake/build
 cmake cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
 make

 // Now you can just run server app. To run this app pass datbase connection arguments ie.:
 ./server --user <userName> --password <password> --host <host> --database <databaseName> --port 3306
 
 // To test this you can run client app too
 ./client
 
 ----------------------------------------------------------------------------
Windows
 ----------------------------------------------------------------------------
 You need to install dependency libraries (check on pointed pages).
 For me only works with mysqlclient library version (version 6.1.11 -> https://downloads.mysql.com/archives/c-c/).
 Additionally you need to you need to install nasm and add it to PATH (choco install nasm) - required by boringssl.
 After you install all dependencies, you may build it using cmake. 
 To build it you need to pass addiionall parameteres to cmake following variables:
 CMAKE_PREFIX_PATH (absolute path for grpc),
 ODB_DIR (absolute path for libodb), 
 ODB_MYSQL_DIR (absolute path for libodb-mysql) , 
 ODB_MYSQLCONNECTOR_DIR (absolute path for myslq client library -> include folder). 

 Then you can run this
 server.exe --user <userName> --password <password> --host <host> --database <databaseName> --port 3306
