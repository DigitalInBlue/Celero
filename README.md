#Celero

###C++ Benchmarking Library

Copyright 2014 John Farrier 

Apache 2.0 License

###Overview

Developing consistent and meaningful benchmark results for code is a complex
task. Measurement tools exist (Intel® VTune™ Amplifier, SmartBear AQTime, 
Valgrind) external to applications, but they are sometimes expensive for small 
teams or cumbersome to utilize. This project, Celero, aims to be a small 
library which can be added to a C++ project and perform benchmarks on code in 
a way which is easy to reproduce, share, and compare among individual runs, 
developers, or projects. Celero uses a framework similar to that of GoogleTest 
to make its API easier to use and integrate into a project. Make automated 
benchmarking as much a part of your development process as automated testing.

Celero uses CMake to provide cross-platform builds. It does require a modern 
compiler (Visual C++ 2012 or GCC 4.7+) due to its use of C++11.

Once Celero is added to your project. You can create dedicated benchmark 
projects and source files. For convenience, there is single header file and a 
CELERO_MAIN macro that can be used to provide a main() for your benchmark 
project that will automatically execute all of your benchmark tests.

###Command Line
```
<celeroOutputExecutable> [-g groupNameToRun] [-o outputFileToWriteResultsTo.csv] [-xml junitOutputFile.xml] [-h]
```

-g	Use this option to run only one benchmark group out of all benchmarks 
contained within a test executable.

Example: celeroDemo -g DemoSimple -o SimpleResults.txt

-xml Writes JUnit formatted XML output.

To utilize JUnit output, benchmarks must use the "_TEST" version of the macros and 
specify an expected baseline multiple.  When the test exceeds this multiple, the JUnit 
output will indicate a failure.
