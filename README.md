#Celero

Develop Branch Status: [![Build Status](https://travis-ci.org/DigitalInBlue/Celero.svg?branch=develop)](https://travis-ci.org/DigitalInBlue/Celero)

###C++ Benchmarking Library

Copyright 2015 John Farrier 

Apache 2.0 License

###Overview

Developing consistent and meaningful benchmark results for code is a complex task. Measurement tools exist (Intel® VTune™ Amplifier, SmartBear AQTime, Valgrind, etc.) external to applications, but they are sometimes expensive for small teams or cumbersome to utilize. This project, Celero, aims to be a small library which can be added to a C++ project and perform benchmarks on code in a way which is easy to reproduce, share, and compare among individual runs, developers, or projects. Celero uses a framework similar to that of GoogleTest to make its API easier to use and integrate into a project. Make automated benchmarking as much a part of your development process as automated testing.

Celero uses CMake to provide cross-platform builds. It does require a modern compiler (Visual C++ 2012+, GCC 4.7+, Clang 2.9+) due to its use of C++11.

Once Celero is added to your project. You can create dedicated benchmark projects and source files. For convenience, there is single header file and a CELERO_MAIN macro that can be used to provide a main() for your benchmark project that will automatically execute all of your benchmark tests.

###Command Line

```
<celeroOutputExecutable> [-g groupNameToRun] [-t resultsTable.csv] [-j junitOutputFile.xml] [-a resultArchive.csv] [-d numberOfIterationsPerDistribution] [-h]
```

-g Use this option to run only one benchmark group out of all benchmarks contained within a test executable.
-t Writes all results to a CSV file.  Very useful when using problem sets to graph performance.
-j Writes JUnit formatted XML output. To utilize JUnit output, benchmarks must use the "_TEST" version of the macros and specify an expected baseline multiple.  When the test exceeds this multiple, the JUnit output will indicate a failure.
-a Builds or updates an archive of historical results, tracking current, best, and worst results for each benchmark.
-d (Experimental) builds a plot of four different sample sizes to investigate the distribution of sample results.

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

#include <random>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

std::random_device RandomDevice;
std::uniform_int_distribution<int> UniformDistribution(0, 1024);

///
/// In reality, all of the "Complex" cases take the same amount of time to run.
/// The difference in the results is a product of measurement error.
///
/// Interestingly, taking the sin of a constant number here resulted in a 
/// great deal of optimization in clang and gcc.
///
BASELINE(DemoSimple, Baseline, 10, 1000000)
{
    celero::DoNotOptimizeAway(static_cast<float>(sin(UniformDistribution(RandomDevice))));
}

///
/// Run a test consisting of 1 sample of 710000 operations per measurement.
/// There are not enough samples here to likely get a meaningful result.
///
BENCHMARK(DemoSimple, Complex1, 1, 710000)
{
    celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
}

///
/// Run a test consisting of 30 samples of 710000 operations per measurement.
/// There are not enough samples here to get a reasonable measurement
/// It should get a Basline number lower than the previous test.
///
BENCHMARK(DemoSimple, Complex2, 30, 710000)
{
    celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
}

///
/// Run a test consisting of 60 samples of 710000 operations per measurement.
/// There are not enough samples here to get a reasonable measurement
/// It should get a Basline number lower than the previous test.
///
BENCHMARK(DemoSimple, Complex3, 60, 710000)
{
    celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
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

Running Celero's simple example experiment (celeroDemoSimple.exe) benchmark gave the following output on a PC:

```
Celero
Timer resolution: 0.069841 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
DemoSimple      | Baseline        |               0 |              10 |         1000000 |         1.00000 |         0.28789 |      3473512.73 |
DemoSimple      | Complex1        |               0 |               1 |          710000 |         1.11028 |         0.31964 |      3128497.53 |
DemoSimple      | Complex2        |               0 |              30 |          710000 |         1.10749 |         0.31884 |      3136388.74 |
DemoSimple      | Complex3        |               0 |              60 |          710000 |         1.10678 |         0.31863 |      3138398.97 |
Complete.
```

The first test that executes will be the group's baseline.  Celero took 10 samples of 1000000 iterations of the code in our test.  (Each set of 1000000 iterations was measured, and this was done 10 times and the smallest time was taken.)  The "Baseline" value for the baseline measurement itself will always be 1.0.

After the baseline is complete, each individual test is ran.  Each test is executed and measured in the same way, however, there is an additional metric reported: Baseline.  This compares the time it takes to compute the benchmark to the baseline.  The data here shows that CeleroBenchTest.Complex1 takes 1.007949 times longer to execute than the baseline.

###Notes

- Benchmarks should always be performed on Release builds.  Never measure the performance of a Debug build and make changes based on the results.  The (optimizing) compiler is your friend with respect to code performance.
- Accuracy is tied very closely to the total number of samples and the sample sizes.  As a general rule, you should aim to execute your baseline code for about as long as your longest benchmark test.  Further, it is helpful if all of the benchmark tests take about the same order of magnitude of execution time.  (Don't compare a baseline that executed in 0.1 seconds with benchmarks that take 60 seconds and an hour, respectivly.)
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
class SortFixture : public celero::TestFixture
{
    public:
        SortFixture()
        {
        }

        virtual std::vector<int64_t> getExperimentValues() const
        {
            std::vector<int64_t> problemSpace;

            // We will run some total number of sets of tests all together. 
            // Each one growing by a power of 2.
            const int totalNumberOfTests = 6;

            for(int i = 0; i < totalNumberOfTests; i++)
            {
                // ExperimentValues is part of the base class and allows us to specify
                // some values to control various test runs to end up building a nice graph.
                problemSpace.push_back(static_cast<int64_t>(pow(2, i+1)));
            }

            return problemSpace;
        }

        /// Before each run, build a vector of random integers.
        virtual void setUp(int64_t experimentValue)
        {
            this->arraySize = experimentValue;

            for(int i = 0; i < this->arraySize; i++)
            {
                this->array.push_back(rand());
            }
        }

        /// After each run, clear the vector of random integers.
        virtual void tearDown()
        {
            this->array.clear();
        }

        std::vector<int64_t> array;
        int64_t arraySize;
};
```

Before the test fixture is utilized by a benchmark, Celero will create an instanciation of the class and call its "getExperimentValues()" function.  The test fixture can then build a vector of int64_t values.  For each value added to this array, benchmarks will be executed following calls to the "setUp" virtual function.  A new test fixture is created for each measurement.

The SetUp() virtual function is called before each benchmark test is executed. When using a problem space values vector, the function will be given a value that was previously pushed into the array within the constructor. The function's code can then decide what to do with it. Here, we are using the value to indicate how many elements should be in the array that we intend to sort. For each of the array elements, we simply add a pseudo-random integer.

Now for implementing the actual sorting algorithms. For the baseline case, I implemented the first sorting algorithm I ever learned in school: Bubble Sort. The code for bubble sort is straight forward.

```C++
// For a baseline, I'll choose Bubble Sort.
BASELINE_F(SortRandInts, BubbleSort, SortFixture, 30, 10000)
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
BENCHMARK_F(SortRandInts, SelectionSort, SortFixture, 30, 10000)
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
BENCHMARK_F(SortRandInts, stdSort, SortFixture, 30, 10000)
{
    std::sort(this->array.begin(), this->array.end());
}
```

###Results

This test was ran on a 4.00 GHz AMD with four cores, eight logical processors, and 32 GB of memory. (Hardware aside, the relative performance of these algorithms should be the same on any modern hardware.) 

Celero outputs timing and benchmark references for each test automatically. However, to write to an output file for easy plotting, simply specify an output file on the command line.

```
celeroExperimentSortingRandomInts.exe -t results.csv
```

While not particularly surprising std::sort is by far the best option with any meaningful problem set size. The results are summarized in the following table output written directly by Celero:

```
Celero
Timer resolution: 0.069841 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
SortRandInts    | BubbleSort      |               2 |              30 |           10000 |         1.00000 |         0.00500 |    200000000.00 |
SortRandInts    | BubbleSort      |               4 |              30 |           10000 |         1.00000 |         0.01820 |     54945054.95 |
SortRandInts    | BubbleSort      |               8 |              30 |           10000 |         1.00000 |         0.08380 |     11933174.22 |
SortRandInts    | BubbleSort      |              16 |              30 |           10000 |         1.00000 |         0.34860 |      2868617.33 |
SortRandInts    | BubbleSort      |              32 |              30 |           10000 |         1.00000 |         1.38200 |       723589.00 |
SortRandInts    | BubbleSort      |              64 |              30 |           10000 |         1.00000 |         5.19970 |       192318.79 |
SortRandInts    | SelectionSort   |               2 |              30 |           10000 |         1.54000 |         0.00770 |    129870129.87 |
SortRandInts    | SelectionSort   |               4 |              30 |           10000 |         0.91758 |         0.01670 |     59880239.52 |
SortRandInts    | SelectionSort   |               8 |              30 |           10000 |         0.81265 |         0.06810 |     14684287.81 |
SortRandInts    | SelectionSort   |              16 |              30 |           10000 |         0.74154 |         0.25850 |      3868471.95 |
SortRandInts    | SelectionSort   |              32 |              30 |           10000 |         0.62171 |         0.85920 |      1163873.37 |
SortRandInts    | SelectionSort   |              64 |              30 |           10000 |         0.60811 |         3.16200 |       316255.53 |
SortRandInts    | stdSort         |               2 |              30 |           10000 |         1.42000 |         0.00710 |    140845070.42 |
SortRandInts    | stdSort         |               4 |              30 |           10000 |         0.51099 |         0.00930 |    107526881.72 |
SortRandInts    | stdSort         |               8 |              30 |           10000 |         0.18377 |         0.01540 |     64935064.94 |
SortRandInts    | stdSort         |              16 |              30 |           10000 |         0.08233 |         0.02870 |     34843205.57 |
SortRandInts    | stdSort         |              32 |              30 |           10000 |         0.03915 |         0.05410 |     18484288.35 |
SortRandInts    | stdSort         |              64 |              30 |           10000 |         0.03831 |         0.19920 |      5020080.32 |
Complete.
```

The data shows first the test group name. Next, all of the data sizes are output. Then each row shows the baseline or benchmark name and the corresponding time for the algorithm to complete measured in useconds. This data, in CSV format, can be directly read by programs such as Microsoft Excel and plotted without any modification.

The point here is not that std::sort is better than more elementary sorting methods, but how easily measureable results can be obtained. In making such measurements more accessible and easier to code, they can become part of the way we code just as automated testing has become.

Test early and test often!

###Notes

- Because I like explicitness as much as the next programmer, I want to note that the actual sorting algorithm used by std::sort is not defined in the standard, but references cite Introsort as a likely contender for how an STL implementation would approach std::sort. http://en.wikipedia.org/wiki/Introsort.
- When choosing a sorting algorithm, start with std::sort and see if you can make improvements from there.
- Don't just trust your experience, measure your code!

## Example and Demo Code

Example and demonstration code is provided under Celero's "experiments" folder.  There are two types of projects.  The first are "Demo" projects.  These are useful for illustrating techniques and ideas, but may not be interesting from a computer-science perspective.  Experiments, on the other hand, have been added which demonstrate real-world questions.  

The addition of real use cases of Celero are encouraged to be submitted to Celero's development branch for inclusion in the Demo and Experiment library.
