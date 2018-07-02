# Celero

### C++ Benchmarking Library

Copyright 2017-2018 John Farrier 

Apache 2.0 License

#### Builds and Testing

Branch                 | Status
---------------------- | --------------------------------------------------------------------------------------------------------------------------------------
```origin/master: ```  | [![Build Status (Master)](https://travis-ci.org/DigitalInBlue/Celero.svg?branch=master)](https://travis-ci.org/DigitalInBlue/Celero)
```origin/develop: ``` | [![Build Status (Develop)](https://travis-ci.org/DigitalInBlue/Celero.svg?branch=develop)](https://travis-ci.org/DigitalInBlue/Celero)

<a href="https://scan.coverity.com/projects/celero">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/910/badge.svg"/>
</a>

### Overview

Developing consistent and meaningful benchmark results for code is a complex task. Measurement tools exist (Intel® VTune™ Amplifier, SmartBear AQTime, Valgrind, etc.) external to applications, but they are sometimes expensive for small teams or cumbersome to utilize. This project, Celero, aims to be a small library which can be added to a C++ project and perform benchmarks on code in a way which is easy to reproduce, share, and compare among individual runs, developers, or projects. Celero uses a framework similar to that of GoogleTest to make its API easier to use and integrate into a project. Make automated benchmarking as much a part of your development process as automated testing.

Celero uses CMake to provide cross-platform builds. It does require a modern compiler (Visual C++ 2012+, GCC 4.7+, Clang 2.9+) due to its use of C++11.

Once Celero is added to your project. You can create dedicated benchmark projects and source files. For convenience, there is single header file and a `CELERO_MAIN` macro that can be used to provide a `main()` for your benchmark project that will automatically execute all of your benchmark tests.

### Key Features

- Supports Windows, Linux, and OSX using C++11.
- The timing utilities can be used directly in production code (independent of benchmarks).
- Console table output is formatted as Markdown to easily copy/paste into documents.
- Archive results to track performance over time.
- Integrates into CI/CT/CD environments with JUnit-formatted output.
- User-defined Experiment Values can scale test results, sample sizes, and user-defined properties for each run.
- Supports Test Fixtures.
- Supports fixed-time benchmark baselines.
- Capture a rich set of timing statistics to a file.
- Easily installed using CMake, Conan, or VCPkg.

### Command Line

```
<celeroOutputExecutable> [-g groupNameToRun] [-t resultsTable.csv] [-j junitOutputFile.xml] [-a resultArchive.csv] [-d numberOfIterationsPerDistribution] [-h]
```

- `-g` Use this option to run only one benchmark group out of all benchmarks contained within a test executable.
- `-t` Writes all results to a CSV file.  Very useful when using problem sets to graph performance.
- `-j` Writes JUnit formatted XML output. To utilize JUnit output, benchmarks must use the `_TEST` version of the macros and specify an expected baseline multiple.  When the test exceeds this multiple, the JUnit output will indicate a failure.
- `-a` Builds or updates an archive of historical results, tracking current, best, and worst results for each benchmark.
- `-d` (Experimental) builds a plot of four different sample sizes to investigate the distribution of sample results.

## Celero Basics

### Background

The goal, generally, of writing benchmarks is to measure the performance of a piece of code.  Benchmarks are useful for comparing multiple solutions to the same problem to select the most appropriate one.  Other times, benchmarks can highlight the performance impact of design or algorithm changes and quantify them in a meaningful way. 

By measuring code performance, you eliminate errors in your assumptions about what the "right" solution is for performance.  Only through measurement can you confirm that using a lookup table, for example, is faster than computing a value.  Such lore (which is often repeated) can lead to bad design decisions and, ultimately, slower code.

The goal in writing good benchmarking code is to eliminate all of the noise and overhead, and measure just the code under test.  Sources of noise in the measurements include clock resolution noise, operating system background operations, test setup/teardown, framework overhead, and other unrelated system activity. 

At a theoretical level we want to measure "t", the time to execute the code under test.  In reality, we measure "t" plus all of this measurement noise.

These extraneous contributors to our measurement of "t" fluctuate over time.  Therefore, we want to try to isolate "t'.  The way this is accomplished is by making many measurements, but only keeping the smallest total.  The smallest total is necessarily the one with the smallest noise contribution and closest to the actual time "t".

Once this measurement is obtained, it has little meaning in isolation.  It is important to create a baseline test by which to compare.  A baseline should generally be a "classic" or "pure" solution to the problem on which you are measuring a solution.   Once you have a baseline, you have a meaningful time to compare your algorithm against.  Simply saying that your fancy sorting algorithm (fSort) sorted a million elements in 10 milliseconds is not sufficient by itself.  However, compare that to a classic sorting algorithm baseline such as quick sort (qSort) and then you can say that fSort is 50% faster than qSort on a million elements.  That is a meaningful and powerful measurement.

### Implementation

Celero heavily utilizes C++11 features that are available in both Visual C++ 2012 and GCC 4.7.  This greatly aided in making the code clean and portable.  To make adopting the code easier, all definitions needed by a user are defined in a celero namespace within a single include file: `Celero.h`

`Celero.h` has within it the macro definitions that turn each of the user benchmark cases into its own unique class with the associated test fixture (if any) and then registers the test case within a Factory.  The macros automatically associate baseline test cases with their associated test benchmarks so that, at run time, benchmark-relative numbers can be computed.  This association is maintained by TestVector.

The `TestVector` utilizes the PImpl idiom to help hide implementation and keep the include overhead of `Celero.h` to a minimum.

Celero reports its outputs to the command line.  Since colors are nice (and perhaps contribute to the human factors/readability of the results), something beyond `std::cout` was called for.  `Console.h` defines a simple color function, `SetConsoleColor`, which is utilized by the functions in the `celero::print` namespace to nicely format the program's output.

Measuring benchmark execution time takes place in the `TestFixture` base class, from which all benchmarks written are ultimately derived.  First, the test fixture setup code is executed.  Then, the start time for the test is retrieved and stored in microseconds using an unsigned long.  This is done to reduce floating point error.  Next, the specified number of operations (iterations) are executed.  When complete, the end time is retrieved, the test fixture is torn down, and the measured time for the execution is returned and the results are saved. 

This cycle is repeated for however many samples were specified.  If no samples were specified (zero), then the test is repeated until it as ran for at least one second or at least 30 samples have been taken.  While writing this specific part of the code, there was a definite "if-else" relationship.  However, the bulk of the code was repeated within the "if" and "else" sections.  An old fashioned function could have been used here, but it was very natural to utilize std::function to define a lambda that could be called and keep all of the code clean.  (C++11 is a fantastic thing.)  Finally, the results are printed to the screen.

### General Program Flow
To summarize, this pseudo-code illustrates how the tests are executed internally:

```cpp
for(Each Experiment)
{
	for(Each Sample)
	{
		// Call the virtual function
		// and DO NOT include its time in the measurement.
		experiment->setUp();

		// Start the Timer
		timer->start();

		// Run all iterations
		for(Each Iteration)
		{
			// Call the virtual function
			// and include its time in the measurement.
			experiment->onExperimentStart(x);

			// Run the code under test
			experiment->run(threads, iterations, experimentValue);
		
			// Call the virtual function
			// and include its time in the measurement.
			experiment->onExperimentEnd();
		}

		// Stop the Timer
		timer->stop();

		// Record data...

		// Call the virtual teardown function
		// and DO NOT include its time in the measurement.
		experiment->tearDown();
	}
}

```

### Using the Code

Celero uses CMake to provide cross-platform builds.  It does require a modern compiler (Visual C++ 2012 or GCC 4.7+) due to its use of C++11.

Once Celero is added to your project.  You can create dedicated benchmark projects and source files. For convenience, there is single header file and a `CELERO_MAIN` macro that can be used to provide a `main()` for your benchmark project that will automatically execute all of your benchmark tests.

Here is an example of a simple Celero Benchmark. (Note: This is a complete, runnable example.)

```cpp
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

The first thing we do in this code is to define a `BASELINE` test case.  This template takes four arguments:

```cpp
BASELINE(GroupName, BaselineName, Samples, Operations)
```

- `GroupName` - The name of the benchmark group.  This is used to batch together runs and results with their corresponding baseline measurement.
- `BaselineName` - The name of this baseline for reporting purposes. 
- `Samples` - The total number of times you want to execute the given number of operations on the test code.
- `Operations` - The total number of times you want to execute the test code per sample.

Samples and operations here are used to measure very fast code.  If you know the code in your benchmark would take some time less than 100 milliseconds, for example, your operations number would say to execute the code "operations" number of times before taking a measurement.  Samples defines how many measurements to make. 

Celero helps with this by allowing you to specify zero samples.  Zero samples will tell Celero to make some statistically significant number of samples based on how long it takes to complete your specified number of operations.  These numbers will be reported at run time.

The `celero::DoNotOptimizeAway` template is provided to ensure that the optimizing compiler does not eliminate your function or code.  Since this feature is used in all of the sample benchmarks and their baseline, it's time overhead is canceled out in the comparisons.

After the baseline is defined, various benchmarks are then defined.  They syntax for the `BENCHMARK` macro is identical to that of the macro.

### Results

Running Celero's simple example experiment (`celeroDemoSimple.exe`) benchmark gave the following output on a PC:

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

After the baseline is complete, each individual test is ran.  Each test is executed and measured in the same way, however, there is an additional metric reported: Baseline.  This compares the time it takes to compute the benchmark to the baseline.  The data here shows that `CeleroBenchTest.Complex1` takes 1.007949 times longer to execute than the baseline.

#### Statistically Sound Results

In order to use Celero for real science, there are three primary factors to consider when reviewing results.  Firstly, you MUST check the generated assembly for your test.  There are different paths to viewing the assembly for different compilers, but essentially this must be done to ensure that you did not optimize out critical code.  You must also verify, via assembly, that you are comparing apples to apples.  

Once that is sorted out, you should run just the "Baseline" case several times.  The "us/Iteration" and "Iterations/sec" should not fluctuate by any significant degree between runs.  If they do, then ensure that your number of iterations is sufficiently large as to overcome the timer resolution on your machine.  Once the number of iterations is high enough, ensure that you are performing a statistically significant number of samples.  Lore has it that 30 samples is good, but use your own science to figure out the best number for your situation.

Finally, you need to ensure that the number of iterations and samples is producing stable output for your experiment cases.  These numbers may be the same as your now-stable baseline case.

One factor that can impact the number of samples and iterations required is the amount of work that your experiment is doing.  For cases where you are utilizing Celero's "problem space" functionality to scale up the algorithms, you can corresponding scale down the number of iterations.  Doing so can reduce the total run time of the larger experiments by doing less iterations, buy while still maintaining a statistically meaningful measurement.  (It saves you time.)

### Threaded Benchmarks
Celero can automatically run threaded benchmarks.  `BASELINE_T` and `BENCHMARK_T` can be used to launch the given code on its own thread using a user-defined number of concurrent executions.  `celeroDemoMultithread` illustrates using this feature.  When defining these macros, the use the following format:

```cpp
BASELINE_T(groupName, baselineName, fixtureName, samples, iterations, threads);
BASELINE_FIXED_T(groupName, baselineName, fixtureName, samples, iterations, threads, useconds);

BENCHMARK_T(groupName, benchmarkName, fixtureName, samples, iterations, threads);
BENCHMARK_TEST_T(groupName, benchmarkName, fixtureName, samples, iterations, threads, target);
```

### Fixed Measurement Benchmarks
While Celero normally measures the baseline time and then executes benchmark cases for comparison, you can also specify a fixed measurement time.  This is useful for measuring performance against a real-time requirement.  To use, utilize the ```_FIXED_``` version of the `BASELINE` and `BENCHMARK` macros.

```cpp
// No threads or test fixtures.
BASELINE_FIXED(groupName, baselineName, samples, iterations, useconds);

// For using test fixtures:
BASELINE_FIXED_F(groupName, baselineName, fixtureName, samples, iterations, useconds);

// For using threads and test fixtures.
BASELINE_FIXED_T(groupName, baselineName, fixtureName, samples, iterations, threads, useconds);
```

Example:

```cpp
BASELINE_FIXED_F(DemoTransform, FixedTime, DemoTransformFixture, 30, 10000, 100)
{ /* Nothing to do */ }
```

### Notes

- Benchmarks should always be performed on Release builds.  Never measure the performance of a Debug build and make changes based on the results.  The (optimizing) compiler is your friend with respect to code performance.
- Accuracy is tied very closely to the total number of samples and the sample sizes.  As a general rule, you should aim to execute your baseline code for about as long as your longest benchmark test.  Further, it is helpful if all of the benchmark tests take about the same order of magnitude of execution time.  (Don't compare a baseline that executed in 0.1 seconds with benchmarks that take 60 seconds and an hour, respectively.)
- Celero has Doxygen documentation of its API.
- Celero supports test fixtures for each baseline group.

## Celero Charts

### Background

It has been noted many times that writing an algorithm to solve small problems is relatively easy. "Brute force" methods tend to function just as well as more graceful approaches. However, as the size of data increases, truly effective algorithms scale their performance to match.

Theoretically, the best we can hope for with an algorithm is that is scales lineally (Order N, O(N) complexity) with respect to the problem size. That is to say that if the problem set doubles, the time it takes for the algorithm to execute doubles. While this seems obvious, it is often an elusive goal.

Even well performing algorithms eventually run into problems with available memory or CPU cache. When making decisions within our software about algorithms and improvements to existing code, only through measurement and experimentation can we know our complex algorithms perform acceptably.

### Using the Code

While Celero offers simple benchmarking of code and algorithms, it also offers a more complex method or directly producing performance graphs of how the benchmarks change with respect to some independent variable, referred to here as the Problem Set.

Within Celero, a test fixture can push integers into a `ProblemSetValues` vector which allows for the fixture's own SetUp function to scale a problem set for the benchmarks to run against. For each value pushed into the `ProblemSetValues` vector, a complete set of benchmarks is executed. These measured values are then stored and can be written out to a CSV file for easy plotting of results.

To demonstrate, we will study the performance of three common sorting algorithms: BubbleSort, SelectionSort, and std::sort. (The source code to this demo is distributed with Celero, available on [GitHub](https://github.com/DigitalInBlue/Celero).)  First, we will write a test fixture for Celero.

```cpp
class SortFixture : public celero::TestFixture
{
public:
    SortFixture()
    {
    }

    virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        // We will run some total number of sets of tests all together. 
        // Each one growing by a power of 2.
        const int totalNumberOfTests = 6;

        for(int i = 0; i < totalNumberOfTests; i++)
        {
            // ExperimentValues is part of the base class and allows us to specify
            // some values to control various test runs to end up building a nice graph.
            problemSpace.push_back({int64_t(pow(2, i+1)});
        }

        return problemSpace;
    }

    /// Before each run, build a vector of random integers.
    virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue)
    {
        this->arraySize = experimentValue.Value;
        this->array.reserve(this->arraySize);
    }

    /// Before each iteration. A common utility function to push back random ints to sort.
    void randomize()
    {
        for(int i = 0; i < this->arraySize; i++)
        {
            this->array.push_back(rand());
        }
    }

    /// After each iteration, clear the vector of random integers.
    void clear()
    {
        this->array.clear();
    }

    std::vector<int64_t> array;
    int64_t arraySize;
};
```

Before the test fixture is utilized by a benchmark, Celero will create an instanciation of the class and call its "getExperimentValues()" function.  The test fixture can then build a vector of TestFixture::ExperimentValue values.  For each value added to this array, benchmarks will be executed following calls to the "setUp" virtual function.  A new test fixture is created for each measurement.

The `SetUp()` virtual function is called before each benchmark test is executed. When using a problem space values vector, the function will be given a value that was previously pushed into the array within the constructor. The function's code can then decide what to do with it. Here, we are using the value to indicate how many elements should be in the array that we intend to sort. For each of the array elements, we simply add a pseudo-random integer.

Now for implementing the actual sorting algorithms. For the baseline case, I implemented the first sorting algorithm I ever learned in school: Bubble Sort. The code for bubble sort is straight forward.

```cpp
// For a baseline, I'll choose Bubble Sort.
BASELINE_F(SortRandInts, BubbleSort, SortFixture, 30, 10000)
{
    this->randomize();

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

	this->clear();
}
```

Celero will use the values from this baseline when computing a base lined measurement for the other two algorithms in the test group _DemoSort_. However, when we run this at the command line, we will specify an output file. The output file will contain the measured number of seconds the algorithm took to execute on the given array size.

Next, we will implement the Selection Sort algorithm.

```cpp
BENCHMARK_F(SortRandInts, SelectionSort, SortFixture, 30, 10000)
{
    this->randomize();

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

	this->clear();
}
```

Finally, for good measure, we will simply use the Standard Library's sorting algorithm: `Introsort`. We only need write a single line of code, but here it is for completeness.

```cpp
BENCHMARK_F(SortRandInts, stdSort, SortFixture, 30, 10000)
{
    this->randomize();
    std::sort(this->array.begin(), this->array.end());
    this->clear();
}
```

### Results

This test was ran on a 4.00 GHz AMD with four cores, eight logical processors, and 32 GB of memory. (Hardware aside, the relative performance of these algorithms should be the same on any modern hardware.) 

Celero outputs timing and benchmark references for each test automatically. However, to write to an output file for easy plotting, simply specify an output file on the command line.

```
celeroExperimentSortingRandomInts.exe -t results.csv
```

While not particularly surprising std::sort is by far the best option with any meaningful problem set size. The results are summarized in the following table output written directly by Celero:

```
Celero
Celero: CPU processor throttling disabled.
Timer resolution: 0.254288 us
Writing results to: results.csv
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
SortRandInts    | BubbleSort      |               2 |              30 |           10000 |         1.00000 |         0.05270 |     18975332.07 |
SortRandInts    | BubbleSort      |               4 |              30 |           10000 |         1.00000 |         0.12060 |      8291873.96 |
SortRandInts    | BubbleSort      |               8 |              30 |           10000 |         1.00000 |         0.31420 |      3182686.19 |
SortRandInts    | BubbleSort      |              16 |              30 |           10000 |         1.00000 |         1.09130 |       916338.31 |
SortRandInts    | BubbleSort      |              32 |              30 |           10000 |         1.00000 |         3.23470 |       309147.68 |
SortRandInts    | BubbleSort      |              64 |              30 |           10000 |         1.00000 |        10.82530 |        92376.19 |
SortRandInts    | SelectionSort   |               2 |              30 |           10000 |         1.09108 |         0.05750 |     17391304.35 |
SortRandInts    | SelectionSort   |               4 |              30 |           10000 |         1.03317 |         0.12460 |      8025682.18 |
SortRandInts    | SelectionSort   |               8 |              30 |           10000 |         1.01464 |         0.31880 |      3136762.86 |
SortRandInts    | SelectionSort   |              16 |              30 |           10000 |         0.72253 |         0.78850 |      1268230.82 |
SortRandInts    | SelectionSort   |              32 |              30 |           10000 |         0.63771 |         2.06280 |       484777.97 |
SortRandInts    | SelectionSort   |              64 |              30 |           10000 |         0.54703 |         5.92180 |       168867.57 |
SortRandInts    | InsertionSort   |               2 |              30 |           10000 |         1.07021 |         0.05640 |     17730496.45 |
SortRandInts    | InsertionSort   |               4 |              30 |           10000 |         1.05970 |         0.12780 |      7824726.13 |
SortRandInts    | InsertionSort   |               8 |              30 |           10000 |         1.00382 |         0.31540 |      3170577.05 |
SortRandInts    | InsertionSort   |              16 |              30 |           10000 |         0.74104 |         0.80870 |      1236552.49 |
SortRandInts    | InsertionSort   |              32 |              30 |           10000 |         0.61508 |         1.98960 |       502613.59 |
SortRandInts    | InsertionSort   |              64 |              30 |           10000 |         0.45097 |         4.88190 |       204838.28 |
SortRandInts    | QuickSort       |               2 |              30 |           10000 |         1.18027 |         0.06220 |     16077170.42 |
SortRandInts    | QuickSort       |               4 |              30 |           10000 |         1.16169 |         0.14010 |      7137758.74 |
SortRandInts    | QuickSort       |               8 |              30 |           10000 |         1.01400 |         0.31860 |      3138731.95 |
SortRandInts    | QuickSort       |              16 |              30 |           10000 |         0.65060 |         0.71000 |      1408450.70 |
SortRandInts    | QuickSort       |              32 |              30 |           10000 |         0.48542 |         1.57020 |       636861.55 |
SortRandInts    | QuickSort       |              64 |              30 |           10000 |         0.34431 |         3.72730 |       268290.72 |
SortRandInts    | stdSort         |               2 |              30 |           10000 |         1.08539 |         0.05720 |     17482517.48 |
SortRandInts    | stdSort         |               4 |              30 |           10000 |         0.94776 |         0.11430 |      8748906.39 |
SortRandInts    | stdSort         |               8 |              30 |           10000 |         0.76926 |         0.24170 |      4137360.36 |
SortRandInts    | stdSort         |              16 |              30 |           10000 |         0.45954 |         0.50150 |      1994017.95 |
SortRandInts    | stdSort         |              32 |              30 |           10000 |         0.33573 |         1.08600 |       920810.31 |
SortRandInts    | stdSort         |              64 |              30 |           10000 |         0.23979 |         2.59580 |       385237.69 |
```

The data shows first the test group name. Next, all of the data sizes are output. Then each row shows the baseline or benchmark name and the corresponding time for the algorithm to complete measured in useconds. This data, in CSV format, can be directly read by programs such as Microsoft Excel and plotted without any modification.  The CSV contains the following data:

Group | Experiment | Problem Space | Samples | Iterations | Baseline | us/Iteration | Iterations/sec | Min (us) | Mean (us) | Max (us) | Variance | Standard Deviation | Skewness | Kurtosis | Z Score
----- | ---------- | ------------- | ------- | ---------- | -------- | ------------ | -------------- | -------- | --------- | -------- | -------- | ------------------ | -------- | -------- | -------
SortRandInts | BubbleSort | 2 | 30 | 10000 | 1 | 0.0527 | 1.89753e+07 | 527 | 532.533 | 582 | 118.74 | 10.8968 | 3.64316 | 13.0726 | 0.507794
SortRandInts | BubbleSort | 4 | 30 | 10000 | 1 | 0.1206 | 8.29187e+06 | 1206 | 1230.77 | 1455 | 1941.22 | 44.0593 | 4.60056 | 20.9542 | 0.562122
SortRandInts | BubbleSort | 8 | 30 | 10000 | 1 | 0.3142 | 3.18269e+06 | 3142 | 3195.73 | 3425 | 3080.41 | 55.5014 | 2.48383 | 7.72605 | 0.968143
SortRandInts | BubbleSort | 16 | 30 | 10000 | 1 | 1.0913 | 916338 | 10913 | 11022.1 | 11228 | 5450.26 | 73.8259 | 0.71778 | 0.387441 | 1.47825
SortRandInts | BubbleSort | 32 | 30 | 10000 | 1 | 3.2347 | 309148 | 32347 | 32803.9 | 36732 | 650545 | 806.563 | 4.1236 | 17.2616 | 0.566519
SortRandInts | BubbleSort | 64 | 30 | 10000 | 1 | 10.8253 | 92376.2 | 108253 | 110999 | 133389 | 2.8152e+07 | 5305.85 | 3.15455 | 9.60246 | 0.517542
SortRandInts | SelectionSort | 2 | 30 | 10000 | 1.09108 | 0.0575 | 1.73913e+07 | 575 | 620.167 | 753 | 2170.97 | 46.5937 | 1.33794 | 1.19871 | 0.969373
SortRandInts | SelectionSort | 4 | 30 | 10000 | 1.03317 | 0.1246 | 8.02568e+06 | 1246 | 1339.57 | 1413 | 2261.7 | 47.5574 | -0.263592 | -0.727621 | 1.96745
SortRandInts | SelectionSort | 8 | 30 | 10000 | 1.01464 | 0.3188 | 3.13676e+06 | 3188 | 3500.63 | 3742 | 20181.2 | 142.061 | -0.438792 | -0.522354 | 2.2007
SortRandInts | SelectionSort | 16 | 30 | 10000 | 0.722533 | 0.7885 | 1.26823e+06 | 7885 | 8504.67 | 9482 | 322584 | 567.965 | 0.274438 | -1.43741 | 1.09103
SortRandInts | SelectionSort | 32 | 30 | 10000 | 0.63771 | 2.0628 | 484778 | 20628 | 20826.7 | 21378 | 26307.7 | 162.196 | 1.64431 | 2.96239 | 1.22526
SortRandInts | SelectionSort | 64 | 30 | 10000 | 0.547033 | 5.9218 | 168868 | 59218 | 59517.7 | 60308 | 55879.5 | 236.389 | 1.42419 | 2.38341 | 1.26783
SortRandInts | InsertionSort | 2 | 30 | 10000 | 1.07021 | 0.0564 | 1.77305e+07 | 564 | 585.4 | 814 | 2239.42 | 47.3225 | 4.06868 | 16.6254 | 0.452216
SortRandInts | InsertionSort | 4 | 30 | 10000 | 1.0597 | 0.1278 | 7.82473e+06 | 1278 | 1312 | 1574 | 3857.17 | 62.1061 | 3.06791 | 9.38706 | 0.54745
SortRandInts | InsertionSort | 8 | 30 | 10000 | 1.00382 | 0.3154 | 3.17058e+06 | 3154 | 3208.57 | 3617 | 8053.91 | 89.7436 | 3.40649 | 12.5161 | 0.608029
SortRandInts | InsertionSort | 16 | 30 | 10000 | 0.741043 | 0.8087 | 1.23655e+06 | 8087 | 8198.43 | 8556 | 11392.8 | 106.737 | 1.66984 | 3.10417 | 1.044
SortRandInts | InsertionSort | 32 | 30 | 10000 | 0.61508 | 1.9896 | 502614 | 19896 | 20088.9 | 20593 | 20955.8 | 144.761 | 1.97818 | 4.12296 | 1.33254
SortRandInts | InsertionSort | 64 | 30 | 10000 | 0.450971 | 4.8819 | 204838 | 48819 | 49152 | 50253 | 129327 | 359.62 | 1.7583 | 2.51588 | 0.925884
SortRandInts | QuickSort | 2 | 30 | 10000 | 1.18027 | 0.0622 | 1.60772e+07 | 622 | 647.4 | 836 | 2492.52 | 49.9252 | 2.83628 | 7.08836 | 0.508761
SortRandInts | QuickSort | 4 | 30 | 10000 | 1.16169 | 0.1401 | 7.13776e+06 | 1401 | 1450 | 1655 | 4476.21 | 66.9045 | 1.94538 | 2.90363 | 0.732388
SortRandInts | QuickSort | 8 | 30 | 10000 | 1.014 | 0.3186 | 3.13873e+06 | 3186 | 3245.8 | 3549 | 5043.89 | 71.0203 | 2.88396 | 9.36231 | 0.842012
SortRandInts | QuickSort | 16 | 30 | 10000 | 0.6506 | 0.71 | 1.40845e+06 | 7100 | 7231.07 | 7670 | 17248.2 | 131.332 | 1.93858 | 3.21011 | 0.997977
SortRandInts | QuickSort | 32 | 30 | 10000 | 0.485424 | 1.5702 | 636862 | 15702 | 15863.2 | 16469 | 33518 | 183.079 | 2.01833 | 3.2763 | 0.880494
SortRandInts | QuickSort | 64 | 30 | 10000 | 0.344314 | 3.7273 | 268291 | 37273 | 37554.4 | 37999 | 34113.3 | 184.698 | 0.822276 | -0.0186633 | 1.52339
SortRandInts | stdSort | 2 | 30 | 10000 | 1.08539 | 0.0572 | 1.74825e+07 | 572 | 591.233 | 764 | 1863.15 | 43.1642 | 2.86875 | 7.63924 | 0.445585
SortRandInts | stdSort | 4 | 30 | 10000 | 0.947761 | 0.1143 | 8.74891e+06 | 1143 | 1185.33 | 1385 | 3435.4 | 58.6123 | 2.53277 | 5.69826 | 0.72226
SortRandInts | stdSort | 8 | 30 | 10000 | 0.769255 | 0.2417 | 4.13736e+06 | 2417 | 2459.47 | 2838 | 6555.84 | 80.9682 | 3.78132 | 14.5264 | 0.524486
SortRandInts | stdSort | 16 | 30 | 10000 | 0.459544 | 0.5015 | 1.99402e+06 | 5015 | 5120.97 | 5283 | 6486.65 | 80.5398 | 0.55161 | -0.798651 | 1.31571
SortRandInts | stdSort | 32 | 30 | 10000 | 0.335734 | 1.086 | 920810 | 10860 | 13398 | 24592 | 8.85889e+06 | 2976.39 | 2.1597 | 4.93241 | 0.852722
SortRandInts | stdSort | 64 | 30 | 10000 | 0.23979 | 2.5958 | 385238 | 25958 | 27384.8 | 35800 | 4.88819e+06 | 2210.92 | 2.24632 | 5.15422 | 0.645326

The point here is not that std::sort is better than more elementary sorting methods, but how easily measurable results can be obtained. In making such measurements more accessible and easier to code, they can become part of the way we code just as automated testing has become.

Test early and test often!

### Notes

- Because I like explicitness as much as the next programmer, I want to note that the actual sorting algorithm used by `std::sort` is not defined in the standard, but references cite Introsort as a likely contender for how an STL implementation would approach std::sort. http://en.wikipedia.org/wiki/Introsort.
- When choosing a sorting algorithm, start with std::sort and see if you can make improvements from there.
- Don't just trust your experience, measure your code!

## FAQ

### Q: I asked for `N` iterations, but Celero ran `N+1` iterations.
The internal code will do one un-measured "warm up" pass.  This helps account for caching which may otherwise influence measurements.

### Q: As my problem space increases in size, my runs take longer and longer.  How do I account for the increased complexity?
When defining a problem space, you set up a `celero::TestFixture::ExperimentValue`.  If the `Iterations` member in the class is greater than zero, that number will be used to control the number of iterations for the corresponding `celero::TestFixture::ExperimentValue`.

```cpp
class MyFixture : public celero::TestFixture
{
public:
	virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const override
	{
		std::vector<std::pair<int64_t, uint64_t>> problemSpaceValues;

		// We will run some total number of sets of tests all together.
		// Each one growing by a power of 2.
		const int totalNumberOfTests = 12;

		for(int i = 0; i < totalNumberOfTests; i++)
		{
			// ExperimentValues is part of the base class and allows us to specify
			// some values to control various test runs to end up building a nice graph.
			// We make the number of iterations decrease as the size of our problem space increases
			// to demonstrate how to adjust the number of iterations per sample based on the
			// problem space size.
			problemSpaceValues.push_back(std::make_pair(int64_t(pow(2, i + 1)), uint64_t(pow(2, totalNumberOfTests - i))));
		}

		return problemSpaceValues;
	}
```

## Example and Demo Code

Example and demonstration code is provided under Celero's "experiments" folder.  There are two types of projects.  The first are "Demo" projects.  These are useful for illustrating techniques and ideas, but may not be interesting from a computer-science perspective.  Experiments, on the other hand, have been added which demonstrate real-world questions.  

The addition of real use cases of Celero are encouraged to be submitted to Celero's development branch for inclusion in the Demo and Experiment library.
