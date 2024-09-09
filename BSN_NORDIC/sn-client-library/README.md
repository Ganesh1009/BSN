# sn-client-library

Library for accessing the DFKI BSN and BIONIC BSN without the need of running the bsn_server application

## building the client

The library includes the shared-library as a git submodule so please run first:

``` shell
#init submodules
git submodule update --init --recursive
```

after the submodules have bin initialized it's just a standard cmake project. Just choose your preferred generator and compile it.

### release - build

``` shell
mkdir build 
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
### debug build - including testing

for a debug build configure your project with the deubg option:

``` shell	
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

if you want to additionally run the tests and cppcheck you can configure it like this:

``` shell	
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON -DENABLE_CPPCHECK=ON ..
```

#### run unit tests
``` shell	
ctest
```

#### run code coverage
this creates a code coverage report: 

``` shell	
make coverage_snclient
```

the report can be viewed with any browser by opening ./coverage_snclient/index.html

``` shell
#example	
firefox ./coverage_snclient/index.html
```