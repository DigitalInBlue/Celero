#Celero

###C++ Benchmarking Library

Copyright 2014 John Farrier 

Apache 2.0 License

* This will become a table of contents
{:toc}

###Overview

Developing consistent and meaningful benchmark results for code is a complex task. Measurement tools exist (Intel® VTune™ Amplifier, SmartBear AQTime, Valgrind) external to applications, but they are sometimes expensive for small teams or cumbersome to utilize. This project, Celero, aims to be a small library which can be added to a C++ project and perform benchmarks on code in a way which is easy to reproduce, share, and compare among individual runs, developers, or projects. Celero uses a framework similar to that of GoogleTest to make its API easier to use and integrate into a project. Make automated benchmarking as much a part of your development process as automated testing.

Celero uses CMake to provide cross-platform builds. It does require a modern compiler (Visual C++ 2012 or GCC 4.7+) due to its use of C++11.

Once Celero is added to your project. You can create dedicated benchmark projects and source files. For convenience, there is single header file and a CELERO_MAIN macro that can be used to provide a main() for your benchmark project that will automatically execute all of your benchmark tests.

###Command Line

```
<celeroOutputExecutable> [-g groupNameToRun] [-o outputFileToWriteResultsTo.csv] [-xml junitOutputFile.xml] [-h]
```

-g	Use this option to run only one benchmark group out of all benchmarks contained within a test executable.

Example: celeroDemo -g DemoSimple -o SimpleResults.txt

-xml Writes JUnit formatted XML output.

To utilize JUnit output, benchmarks must use the "_TEST" version of the macros and specify an expected baseline multiple.  When the test exceeds this multiple, the JUnit output will indicate a failure.

##Celero Basics

###Background

The goal, generally, of writing benchmarks is to measure the performance of a piece of code.  Benchmarks are useful for comparing multiple solutions to the same problem to select the most appropriate one.  Other times, benchmarks can highlight the performance impact of design or algorithm changes and quantify them in a meaningful way. 

By measuring code performance, you eliminate errors in your assumptions about what the "right" solution is for performance.  Only through measurement can you confirm that using a lookup table, for example, is faster than computing a value.  Such lore (which is often repeated) can lead to bad design decisions and, ultimately, slower code.

The goal in writing good benchmarking code is to eliminate all of the noise and overhead, and measure just the code under test.  Sources of noise in the measurements include clock resolution noise, operating system background operations, test setup/teardown, framework overhead, and other unrelated system activity. 

At a theoretical level we want to measure "t", the time to execute the code under test.  In reality, we measure "t" plus all of this measurement noise.

These extraneous contributors to our measurement of "t" fluctuate over time.  Therefore, we want to try to isolate "t'.  The way this is accomplished is by making many measurements, but only keeping the smallest total.  The smallest total is necessarily the one with the smallest noise contribution and closest to the actual time "t".

Once this measurement is obtained, it has little meaning in isolation.  It is important to create a baseline test by which to compare.  A baseline should generally be a "classic" or "pure" solution to the problem on which you are measuring a solution.   Once you have a baseline, you have a meaningful time to compare your algorithm against.  Simply saying that your fancy sorting algorithm (fSort) sorted a million elements in 10 milliseconds is not sufficient by itself.  However, compare that to a classic sorting algorithm baseline such as quick sort (qSort) and then you can say that fSort is 50% faster than qSort on a million elements.  That is a meaningful and powerful measurement.

###Implementation

Celero heavily utilizes C++11 features that are available in both Visual C++ 2012 and GCC 4.7.  This greatly aided in making the code clean and portable.  To make adopting the code easier, all definitions needed by a user are defined in a celero namespace within a single include file: Celero.h

Celero.h has within it the macro definitions that turn each of the user benchmark cases into its own unique class with the associated test fixture (if any) and then registers the test case within a Factory.  The macros automatically associate baseline test cases with their associated test benchmarks so that, at run time, benchmark-relative numbers can be computed.  This association is maintained by TestVector.

The TestVector utilizes the PImpl idiom to help hide implementation and keep the include overhead of Celero.h to a minimum.

Celero reports its outputs to the command line.  Since colors are nice (and perhaps contribute to the human factors/readability of the results), something beyond std::cout was called for.  Console.h defines a simple color function, SetConsoleColor, which is utilized by the functions in the celero::print namespace to nicely format the program's output.

Measuring benchmark execution time takes place in the TestFixture base class, from which all benchmarks written are ultimately derived.  First, the test fixture setup code is executed.  Then, the start time for the test is retrieved and stored in microseconds using an unsigned long.  This is done to reduce floating point error.  Next, the specified number of operations (iterations) are executed.  When complete, the end time is retrieved, the test fixture is torn down, and the measured time for the execution is returned and the results are saved. 

This cycle is repeated for however many samples were specified.  If no samples were specified (zero), then the test is repeated until it as ran for at least one second or at least 30 samples have been taken.  While writing this specific part of the code, there was a definite "if-else" relationship.  However, the bulk of the code was repeated within the "if" and "else" sections.  An old fashioned function could have been used here, but it was very natural to utilize std::function to define a lambda that could be called and keep all of the code clean.  (C++11 is a fantastic thing.)  Finally, the results are printed to the screen.

###Using the Code

Celero uses CMake to provide cross-platform builds.  It does require a modern compiler (Visual C++ 2012 or GCC 4.7+) due to its use of C++11.

Once Celero is added to your project.  You can create dedicated benchmark projects and source files. For convenience, there is single header file and a CELERO_MAIN macro that can be used to provide a main() for your benchmark project that will automatically execute all of your benchmark tests.

Here is an example of a simple Celero Benchmark:

```C++
#include <celero/Celero.h>
 
CELERO_MAIN;
 
// Run an automatic baseline.  
// Celero will help make sure enough samples are taken to get a reasonable measurement
BASELINE(CeleroBenchTest, Baseline, 0, 7100000)
{
    celero::DoNotOptimizeAway(static_cast(sin(3.14159265)));
}
 
// Run an automatic test.  
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(CeleroBenchTest, Complex1, 0, 7100000)
{
    celero::DoNotOptimizeAway(static_cast(sin(fmod(rand(), 3.14159265))));
}
 
// Run a manual test consisting of 1 sample of 7100000 operations per measurement.
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(CeleroBenchTest, Complex2, 1, 7100000)
{
    celero::DoNotOptimizeAway(static_cast(sin(fmod(rand(), 3.14159265))));
}
 
// Run a manual test consisting of 60 samples of 7100000 operations per measurement.
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(CeleroBenchTest, Complex3, 60, 7100000)
{
    celero::DoNotOptimizeAway(static_cast(sin(fmod(rand(), 3.14159265))));
}
```

The first thing we do in this code is to define a BASELINE test case.  This template takes four arguments:

```C++
BASELINE(GroupName, BaselineName, Samples, Operations)
```

- GroupName - The name of the benchmark group.  This is used to batch together runs and results with their corresponding baseline measurement.
- BaselineName - The name of this baseline for reporting purposes. 
- Samples - The total number of times you want to execute the given number of operations on the test code.
- Operations - The total number of times you want to execute the test code per sample.

Samples and operations here are used to measure very fast code.  If you know the code in your benchmark would take some time less than 100 milliseconds, for example, your operations number would say to execute the code "operations" number of times before taking a measurement.  Samples defines how many measurements to make. 

Celero helps with this by allowing you to specify zero samples.  Zero samples will tell Celero to make some statistically significant number of samples based on how long it takes to complete your specified number of operations.  These numbers will be reported at run time.

The celero::DoNotOptimizeAway template is provided to ensure that the optimizing compiler does not eliminate your function or code.  Since this feature is used in all of the sample benchmarks and their baseline, it's time overhead is canceled out in the comparisons.

After the baseline is defined, various benchmarks are then defined.  They syntax for the BENCHMARK macro is identical to that of the macro.

###Results

The sample project is configured to automatically execute the benchmark code upon successful compilation.  Running this benchmark gave the following output on a PC:

```
[  CELERO  ]
[==========]
[ STAGE    ] Baselining
[==========]
[ RUN      ] CeleroBenchTest.Baseline -- Auto Run, 7100000 calls per run.
[   AUTO   ] CeleroBenchTest.Baseline -- 30 samples, 7100000 calls per run.
[     DONE ] CeleroBenchTest.Baseline  (0.517049 sec) [7100000 calls in 517049 usec] [0.072824 us/call] [13731773.971132 calls/sec]
[==========]
[ STAGE    ] Benchmarking
[==========]
[ RUN      ] CeleroBenchTest.Complex1 -- Auto Run, 7100000 calls per run.
[   AUTO   ] CeleroBenchTest.Complex1 -- 30 samples, 7100000 calls per run.
[     DONE ] CeleroBenchTest.Complex1  (2.192290 sec) [7100000 calls in 2192290 usec] [0.308773 us/call] [3238622.627481 calls/sec]
[ BASELINE ] CeleroBenchTest.Complex1 4.240004
[ RUN      ] CeleroBenchTest.Complex2 -- 1 run, 7100000 calls per run.
[     DONE ] CeleroBenchTest.Complex2  (2.199197 sec) [7100000 calls in 2199197 usec] [0.309746 us/call] [3228451.111929 calls/sec]
[ BASELINE ] CeleroBenchTest.Complex2 4.253363
[ RUN      ] CeleroBenchTest.Complex3 -- 60 samples, 7100000 calls per run.
[     DONE ] CeleroBenchTest.Complex3  (2.192378 sec) [7100000 calls in 2192378 usec] [0.308786 us/call] [3238492.632201 calls/sec]
[ BASELINE ] CeleroBenchTest.Complex3 4.240175
[==========]
[ STAGE    ] Completed.  4 tests complete.
[==========]
```

The first test that executes will be the group's baseline.  This baseline shows that it was an "auto run", indicating that Celero would measure and decide how many times to execute the code.  In this case, it ran 30 samples of 7100000 iterations of the code in our test.  (Each set of 7100000 calls was measured, and this was done 30 times and the smallest time was taken.) This total measurement took 0.517049 seconds.  Given this, it was measured that each individual call of the baseline code took 0.072824 microseconds.

After the baseline is complete, each individual test is ran.  Each test is executed and measured in the same way, however, there is an additional metric reported: Baseline.  This compares the time it takes to compute the benchmark to the baseline.  The data here shows that CeleroBenchTest.Complex1 takes 4.240004 times longer to execute than the baseline.

###Notes

- Benchmarks should always be performed on Release builds.  Never measure the performance of a Debug build and make changes based on the results.  The (optimizing) compiler is your friend with respect to code performance.
- Celero has Doxygen documentation of its API.
- Celero supports test fixtures for each baseline group.

##Celero Charts

###Background

It has been noted many times that writing an algorithm to solve small problems is relatively easy. "Brute force" methods tend to function just as well as more graceful approaches. However, as the size of data increases, truly effective algorithms scale their performance to match.

Theoretically, the best we can hope for with an algorithm is that is scales lineally (Order N, O(N) complexity) with respect to the problem size. That is to say that if the problem set doubles, the time it takes for the algorithm to execute doubles. While this seems obvious, it is often an elusive goal.

Even well performing algorithms eventually run into problems with available memory or CPU cache. When making decisions within our software about algorithms and improvements to existing code, only through measurement and experimentation can we know our complex algorithms perform acceptably.

###Using the Code

While Celero offers simple benchmarking of code and algorithms, it also offers a more complex method or directly producing performance graphs of how the benchmarks change with respect to some independent variable, referred to here as the Problem Set.

Within Celero, a test fixture can push integers into a ProblemSetValues vector which allows for the fixture's own SetUp function to scale a problem set for the benchmarks to run against. For each value pushed into the ProblemSetValues vector, a complete set of benchmarks is executed. These measured values are then stored and can be written out to a CSV file for easy plotting of results.

To demonstrate, we will study the performance of three common sorting algorithms: BubbleSort, SelectionSort, and std::sort. (The source code to this demo is distributed with Celero, available on GitHub.)  First, we will write a test fixture for Celero.

```C++
class DemoSortFixture : public celero::TestFixture
{
public:
    DemoSortFixture()
    {
        // We will run some total number of sets of tests all together. 
        // Each one growing by a power of 2.
        const int totalNumberOfTests = 12;
 
        for(int i = 0; i < totalNumberOfTests; i++)
        {
            // ProblemSetValues is part of the base class and allows us to specify
            // some values to control various test runs to end up building a nice graph.
            this->ProblemSetValues.push_back(static_cast<int32_t>(pow(2, i+1)));
        }
    }
 
    /// Before each run, build a vector of random integers.
    virtual void SetUp(const int32_t problemSetValue)
    {
        this->arraySize = problemSetValue;
 
        for(int i = 0; i < this->arraySize; i++)
        {
            this->array.push_back(rand());
        }
    }
 
    /// After each run, clear the vector of random integers.
    virtual void TearDown()
    {
        this->array.clear();
    }
 
    std::vector array;
    int arraySize;
};
```

The constructor defines the experiment size,  totalNumberOfTests This number can be changed at will to adjust how many values are pushed into the ProblemSetValues array. Celero uses this array internally when calling the fixture's SetUp() virtual function.

The SetUp() virtual function is called before each benchmark test is executed. When using a ProblemSetValues array, the function will be given a value that was previously pushed into the array within the constructor. The function's code can then decide what to do with it. Here, we are using the value to indicate how many elements should be in the array that we intend to sort. For each of the array elements, we simply add a pseudo-random integer.

Now for implementing the actual sorting algorithms. For the baseline case, I implemented the first sorting algorithm I ever learned in school: Bubble Sort. The code for bubble sort is straight forward.

```C++
// For a baseline, I'll choose Bubble Sort.
BASELINE_F(DemoSort, BubbleSort, DemoSortFixture, 0, 10000)
{
    for(int x = 0; x < this->arraySize; x++)
    {
        for(int y = 0; y < this->arraySize - 1; y++)
        {
            if(this->array[y] > this->array[y+1])
            {
                std::swap(this->array[y], this->array[y+1]);
            }
        }
    }
}
```

Celero will use the values from this baseline when computing a base lined measurement for the other two algorithms in the test group "DemoSort". However, when we run this at the command line, we will specify an output file. The output file will contain the measured number of seconds the algorithm took to execute on the given array size.

Next, we will implement the Selection Sort algorithm.

```C++
BENCHMARK_F(DemoSort, SelectionSort, DemoSortFixture, 0, 10000)
{
    for(int x = 0; x < this->arraySize; x++)
    {
        auto minIdx = x;
 
        for(int y = x; y < this->arraySize; y++)
        {
            if(this->array[minIdx] > this->array[y])
            {
                minIdx = y;
            }
        }
 
        std::swap(this->array[x], this->array[minIdx]);
    }
}
```

Finally, for good measure, we will simply use the Standard Library's sorting algorithm: Introsort*. We only need write a single line of code, but here it is for completeness.

```C++
BENCHMARK_F(DemoSort, stdSort, DemoSortFixture, 0, 10000)
{
    std::sort(this->array.begin(), this->array.end());
}
```

###Results

This test was ran on a 4.00 GHz AMD with four cores, eight logical processors, and 32 GB of memory. (Hardware aside, the relative performance of these algorithms should be the same on any modern hardware.) 

Celero outputs timing and benchmark references for each test automatically. However, to write to an output file for easy plotting, simply specify an output file on the command line.

```
celeroDemo -o results.csv
```

While not particularly surprising std::sort is by far the best option with any meaningful problem set size. The results are summarized in the following table output written directly by Celero:

```
DemoTransform,,,,,,,,,,,,
,1,2,4,8,16,32,64,128,256,512,1024,2048
ForLoop,0.0084,0.0125,0.0186,0.0315,0.0595,0.133,0.2353,0.439,0.8607,1.7392,3.4112,7.0522
SelfForLoop,0.0093,0.012,0.0186,0.0304,0.0567,0.1265,0.225,0.4194,0.8096,1.6094,3.2035,6.4252
SelfStdTransform,0.0112,0.0113,0.0167,0.0265,0.0478,0.119,0.1953,0.3788,0.7257,1.4189,2.8082,5.5696
SelfStdTransformLambda,0.0112,0.0111,0.0167,0.0264,0.0474,0.119,0.1953,0.3822,0.7257,1.4192,2.8078,5.5707
StdTransform,0.0074,0.0111,0.0167,0.0288,0.0539,0.1153,0.2046,0.3832,0.7411,1.4555,2.8884,5.9487
StdTransformLambda,0.0074,0.0111,0.0167,0.0288,0.0539,0.1153,0.2046,0.3832,0.7411,1.4556,2.8915,5.9451

```

The data shows first the test group name. Next, all of the data sizes are output. Then each row shows the baseline or benchmark name and the corresponding time for the algorithm to complete measured in useconds. This data, in CSV format, can be directly read by programs such as Microsoft Excel and plotted without any modification.

The point here is not that std::sort is better than more elementary sorting methods, but how easily measureable results can be obtained. In making such measurements more accessible and easier to code, they can become part of the way we code just as automated testing has become.

Test early and test often!

###Notes

- Because I like explicitness as much as the next programmer, I want to note that the actual sorting algorithm used by std::sort is not defined in the standard, but references cite Introsort as a likely contender for how an STL implementation would approach std::sort. http://en.wikipedia.org/wiki/Introsort.
- When choosing a sorting algorithm, start with std::sort and see if you can make improvements from there.
- Don't just trust your experience, measure your code!
