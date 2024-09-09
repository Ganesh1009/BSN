# shared_libs

This is a submodule for shared code related to nordic microcontrollers

## Cmake project for testing

the cmake project can be used for testing the different libraries on your development machine and the gitlab runners. There are some tools needed to get the most out of the test results:

## test tools
### lcov - coverage analysis
Checks your test coverage, on an ubuntu machine you install it via:

```bash
 sudo apt-get install lcov
```
#### adding your library and test to the coverage target
if you want to add your library or test target to the ccov targets have a look at the CMakeLists.txt file in /tests

it's as simple as:
target_code_coverage(yourtargetorlibname ALL)

#### running the coverage reports
to create coverage reports you just need to run:

```bash
#makefile
make ccov-all
#ninja
ninja ccov-all
```

after running this command, at the end of the build outpout there will be an lint to a html report. just open the file with your browser and you get a nice visualization which line of your code have not been covered by your testcases. 

### cppcheck - static code analysis
code for static code analyis, on an ubuntu machine you install it via:

```bash
 sudo apt-get install cppcheck
```

#### running cppcheck
there's acutally nothing to do here. 
when adding the snippet from our top level CMakeLists.txt file it automatically analysis any c/cpp file it builds.
