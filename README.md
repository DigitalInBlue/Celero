# Celero

### C++ Benchmarking Library

Copyright 2017-2023 John Farrier 

Apache 2.0 License

#### Community Support

A Special Thanks to the following corporations for their support:

-   [Hellebore Consulting Group](http://www.hellebore.com)
-   [Araxis](https://www.araxis.com/)
-   [Axosoft](https://www.gitkraken.com/)
-   [Microsoft](https://www.microsoft.com)
-   [Travis.CI](https://travis-ci.org)

#### Builds and Testing

| Branch                 | Status                                                                                                                                 |
|:-----------------------|:---------------------------------------------------------------------------------------------------------------------------------------|
| ```origin/master: ```  | [![Build Status (Master)](https://travis-ci.org/DigitalInBlue/Celero.svg?branch=master)](https://travis-ci.org/DigitalInBlue/Celero)   |
| ```origin/develop: ``` | [![Build Status (Develop)](https://travis-ci.org/DigitalInBlue/Celero.svg?branch=develop)](https://travis-ci.org/DigitalInBlue/Celero) |

Celero has been successfully built on the following platforms during development.  See [Travis.CI](https://travis-ci.org/DigitalInBlue/Celero) for more details.

- GCC v6.0.0
- GCC v7.0.0
- GCC v8.0.0
- GCC v9.0.0
- LLVM v3.9.0
- LLVM v5.0.1
- LLVM v7.0.0
- LLVM v8.0.0
- LLVM v9.0.0
- Visual Studio 2019 (16.8.4)
- Visual Studio 2019 (16.11.5)
- Visual Studio 2022 (17.0.5)
- Visual Studio 2022 (17.4.3)
- XCode v10.1
- XCode v10.3
- XCode v11.0
- XCode v12.0

As of v2.7, Celero requres the developer to provide GoogleTest in order to build unit tests.  We suggest using a package manager such as VCPKG or Conan to provide the latest version of the library.

#### Quality Control

| Tooling          | Status                                                                                                                                                                                                                                                            |
|:-----------------|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ```Codacy```     | [![Codacy Badge](https://api.codacy.com/project/badge/Grade/2008db9f4fc4413d878474441f7a1511)](https://www.codacy.com/manual/john.farrier/Celero?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DigitalInBlue/Celero&amp;utm_campaign=Badge_Grade) |
| ```Statistics``` | [View on OpenHub](https://www.openhub.net/p/CeleroBenchmark)                                                                                                                                                                                                      |

### Overview

Developing consistent and meaningful benchmark results for code is a complicated task. Measurement tools exist (Intel® VTune™ Amplifier, SmartBear AQTime, Valgrind, etc.) external to applications, but they are sometimes expensive for small teams or cumbersome to utilize. This project, Celero, aims to be a small library which can be added to a C++ project and perform benchmarks on code in a way which is easy to reproduce, share, and compare among individual runs, developers, or projects. Celero uses a framework similar to that of GoogleTest to make its API more natural to use and integrate into a project. Make automated benchmarking as much a part of your development process as automated testing.

Celero uses CMake to provide cross-platform builds. It does require a modern compiler (Visual C++ 2012+, GCC 4.7+, Clang 2.9+) due to its use of C++11.

Once Celero is added to your project. You can create dedicated benchmark projects and source files. For convenience, there is single header file and a `CELERO_MAIN` macro that can be used to provide a `main()` for your benchmark project that will automatically execute all of your benchmark tests.

### Key Features

-   Supports Windows, Linux, and OSX using C++11.
-   The timing utilities can be used directly in production code (independent of benchmarks).
-   Automatically tracks RAM usage during the experiments
-   Console table output is formatted as Markdown to easily copy/paste into documents.
-   Archive results to track performance over time.
-   Integrates into CI/CT/CD environments with JUnit-formatted output.
-   User-defined Experiment Values can scale test results, sample sizes, and user-defined properties for each run.
-   User-defined Measurements allow for measuring anything in addition to timing.
-   Supports Test Fixtures.
-   Supports fixed-time benchmark baselines.
-   Capture a rich set of timing statistics to a file.
-   Easily installed using CMake, Conan, or VCPkg.

### Command Line

```
<celeroOutputExecutable> [-g groupNameToRun] [-t resultsTable.csv] [-j junitOutputFile.xml] [-a resultArchive.csv] [-d numberOfIterationsPerDistribution] [-h]
```

-   `-g` Use this option to run only one benchmark group out of all benchmarks contained within a test executable.
-   `-t` Writes all results to a CSV file.  Very useful when using problem sets to graph performance.
-   `-j` Writes JUnit formatted XML output. To utilize JUnit output, benchmarks must use the `_TEST` version of the macros and specify an expected baseline multiple.  When the test exceeds this multiple, the JUnit output will indicate a failure.
-   `-a` Builds or updates an archive of historical results, tracking current, best, and worst results for each benchmark.
-   `-d` (Experimental) builds a plot of four different sample sizes to investigate the distribution of sample results.

## Celero Basics

### Background

The goal, generally, of writing benchmarks is to measure the performance of a piece of code.  Benchmarks are useful for comparing multiple solutions to the same problem to select the most appropriate one.  Other times, benchmarks can highlight the performance impact of design or algorithm changes and quantify them in a meaningful way.

By measuring code performance, you eliminate errors in your assumptions about what the "right" solution is for performance.  Only through measurement can you confirm that using a lookup table, for example, is faster than computing a value.  Such lore (which is often repeated) can lead to bad design decisions and, ultimately, slower code.

The goal of writing correct benchmarking code is to eliminate all of the noise and overhead, and measure just the code under test.  Sources of noise in the measurements include clock resolution noise, operating system background operations, test setup/teardown, framework overhead, and other unrelated system activity.

At a theoretical level, we want to measure `t`, the time to execute the code under test.  In reality, we measure `t` plus all of this measurement noise.

These extraneous contributors to our measurement of `t` fluctuate over time.  Therefore, we want to try to isolate `t`.  The way this is accomplished is by making many measurements, but only keeping the smallest total.  The smallest total is necessarily the one with the smallest noise contribution and closest to the actual time `t`.

Once this measurement is obtained, it has little meaning in isolation.  It is essential to create a baseline test by which to compare.  A baseline should generally be a "classic" or "pure" solution to the problem on which you measure a solution.   Once you have a baseline, you have a meaningful time to compare your algorithm against. Merely saying that your fancy sorting algorithm (fSort) sorted a million elements in 10 milliseconds is not sufficient by itself.  However, compare that to a classic sorting algorithm baseline such as quicksort (qSort) and then you can say that fSort is 50% faster than qSort on a million elements.  That is a meaningful and powerful measurement.

### Implementation

Celero heavily utilizes C++11 features that are available in both Visual C++ 2012 and GCC 4.7.  C++11 greatly aided in making the code clean and portable.  To make adopting the code more manageable, all definitions needed by a user are defined in a celero namespace within a single include file: `Celero.h`.

`Celero.h` has within it the macro definitions that turn each of the user benchmark cases into its own unique class with the associated test fixture (if any) and then registers the test case within a Factory.  The macros automatically associate baseline test cases with their associated test benchmarks so that, at run time, benchmark-relative numbers can be computed.  This association is maintained by TestVector.

The `TestVector` utilizes the PImpl idiom to help hide implementation and keep the include overhead of `Celero.h` to a minimum.

Celero reports its outputs to the command line.  Since colors are nice (and perhaps contribute to the human factors/readability of the results), something beyond `std::cout` was called for.  `Console.h` defines a simple color function, `SetConsoleColor`, which is utilized by the functions in the `celero::print` namespace to nicely format the program's output.

Measuring benchmark execution time takes place in the `TestFixture` base class, from which all benchmarks are written are ultimately derived.  First, the test fixture setup code is executed.  Then, the start time for the test is retrieved and stored in microseconds using an unsigned long.  This is done to reduce floating point error.  Next, the specified number of operations (iterations) is executed.  When complete, the end time is retrieved, the test fixture is torn down, and the measured time for the execution is returned, and the results are saved.

This cycle is repeated for however-many samples were specified.  If no samples were specified (zero), then the test is repeated until it as ran for at least one second or at least 30 samples have been taken.  While writing this specific part of the code, there was a definite "if-else" relationship.  However, the bulk of the code was repeated within the `if` and `else` sections.  An old fashioned function could have been used here, but it was very natural to utilize std::function to define a lambda that could be called and keep all of the code clean.  (C++11 is a fantastic thing.)  Finally, the results are printed to the screen.

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

#ifndef _WIN32
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
/// It should get a Baseline number lower than the previous test.
///
BENCHMARK(DemoSimple, Complex2, 30, 710000)
{
    celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
}

///
/// Run a test consisting of 60 samples of 710000 operations per measurement.
/// There are not enough samples here to get a reasonable measurement
/// It should get a Baseline number lower than the previous test.
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

-   `GroupName` - The name of the benchmark group.  This is used to batch together runs and results with their corresponding baseline measurement.
-   `BaselineName` - The name of this baseline for reporting purposes.
-   `Samples` - The total number of times you want to execute the given number of operations on the test code.
-   `Operations` - The total number of times you want to run the test code per sample.

Samples and operations here are used to measure very fast code.  If you know the code in your benchmark will take some time less than 100 milliseconds, for example, your operations number would say to execute the code "operations" number of times before taking a measurement.  Samples define how many measurements to make.

Celero helps with this by allowing you to specify zero samples.  Zero samples will tell Celero to make some statistically significant number of samples based on how long it takes to complete your specified number of operations.  These numbers will be reported at run time.

The `celero::DoNotOptimizeAway` template is provided to ensure that the optimizing compiler does not eliminate your function or code.  Since this feature is used in all of the sample benchmarks and their baseline, it's time overhead is canceled out in the comparisons.

After the baseline is defined, various benchmarks are then defined.  The syntax for the `BENCHMARK` macro is identical to that of the macro.

### Results

Running Celero's simple example experiment (`celeroDemoSimple.exe`) benchmark gave the following output on a PC:

```
Celero
Timer resolution: 0.277056 us
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |   RAM (bytes)   |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|DemoSimple      | Baseline        |            Null |              30 |         1000000 |         1.00000 |         0.09320 |     10729498.61 |          892928 |
|DemoSimple      | Complex1        |            Null |               1 |          710000 |         0.99833 |         0.09305 |     10747479.64 |          897024 |
|DemoSimple      | Complex2        |            Null |              30 |          710000 |         0.97898 |         0.09124 |     10959834.52 |          897024 |
|DemoSimple      | Complex3        |            Null |              60 |          710000 |         0.98547 |         0.09185 |     10887733.66 |          897024 |
Completed in 00:00:10.315012
```

The first test that executes will be the group's baseline.  Celero took 30 samples of 1000000 iterations of the code in our test.  (Each set of 1000000 iterations was measured, and this was done ten times and the shortest time was taken.)  The "Baseline" value for the baseline measurement itself will always be 1.0.

After the baseline is complete, each individual test runs.  Each test is executed and measured in the same way. However, there is an additional metric reported: Baseline.  This compares the time it takes to compute the benchmark to the baseline.  The data here shows that `CeleroBenchTest.Complex1` takes 1.007949 times longer to execute than the baseline.

### Automatically computing the number of Iterations and Samples

If you do want Celero to figure out a reasonable number of iterations to run, you can set the iteration value to ```0``` for your experiment.  You can also set the number of samples to ```0``` to have it compute a statistically valid number of samples.  (Note that the current implementation uses ```30``` as the default number of samples, but does calculate a reasonable number of iterations.)

Update the previous "DemoSimple" code's ```Complex1``` case to use this feature as follows:

```cpp
/// Run a test consisting of 0 samples of 0 iterations per measurement.
/// Since the sample size is equal to 0, celero will compute a number to use for both samples and iterations.
BENCHMARK(DemoSimple, Complex1, 0, 0)
{
    celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
}
```

Now, when this executes, you will see a different number automatically computed for the number of iterations, and the sample size has been increased.

```
Celero
Timer resolution: 0.100000 us
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |   RAM (bytes)   |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|DemoSimple      | Baseline        |            Null |              30 |         1000000 |         1.00000 |         0.09076 |     11017948.24 |          892928 |
|DemoSimple      | Complex1        |            Null |              30 |         2097152 |         1.01148 |         0.09180 |     10892938.02 |          897024 |
|DemoSimple      | Complex2        |            Null |              30 |          710000 |         0.98926 |         0.08979 |     11137604.32 |          897024 |
|DemoSimple      | Complex3        |            Null |              60 |          710000 |         0.99908 |         0.09068 |     11028098.35 |          897024 |
Completed in 00:00:15.889099
```

#### Statistically Sound Results

To use Celero for real science, there are three primary factors to consider when reviewing results.  Firstly, you MUST check the generated assembly for your test.  There are different paths to viewing the assembly for various compilers, but essentially this must be done to ensure that you did not optimize-out critical code.  You must also verify, via assembly, that you are comparing apples to apples.  

Once that is sorted out, you should run just the "Baseline" case several times.  The "us/Iteration" and "Iterations/sec" should not fluctuate by any significant degree between runs.  If they do, then ensure that your number of iterations is sufficiently large as to overcome the timer resolution on your machine.  Once the number of iterations is high enough, ensure that you are performing a statistically significant number of samples.  Lore has it that 30 samples are good, but use your own science to figure out the best number for your situation.

Finally, you need to ensure that the number of iterations and samples is producing stable output for your experiment cases.  These numbers may be the same as your now-stable baseline case.

One factor that can impact the number of samples and iterations required is the amount of work that your experiment is doing.  For cases where you are utilizing Celero's "problem space" functionality to scale up the algorithms, you can correspondingly scale down the number of iterations.  Doing so can reduce the total run time of the more extensive experiments by doing fewer iterations, buy while still maintaining a statistically meaningful measurement.  (It saves you time.)

### Threaded Benchmarks

Celero can automatically run threaded benchmarks.  `BASELINE_T` and `BENCHMARK_T` can be used to launch the given code on its own thread using a user-defined number of concurrent executions.  `celeroDemoMultithread` illustrates using this feature.  When defining these macros, they use the following format:

```cpp
BASELINE_T(groupName, baselineName, fixtureName, samples, iterations, threads);
BASELINE_FIXED_T(groupName, baselineName, fixtureName, iterations, threads, useconds);

BENCHMARK_T(groupName, benchmarkName, fixtureName, samples, iterations, threads);
BENCHMARK_TEST_T(groupName, benchmarkName, fixtureName, samples, iterations, threads, target);
```

### Fixed Measurement Benchmarks

While Celero typically measures the baseline time and then executes benchmark cases for comparison, you can also specify a fixed measurement time.  This is useful for measuring performance against a real-time requirement.  To use, utilize the ```_FIXED_``` version of the `BASELINE` and `BENCHMARK` macros.

```cpp
// No threads or test fixtures.
BASELINE_FIXED(groupName, baselineName, iterations, useconds);

// For using test fixtures:
BASELINE_FIXED_F(groupName, baselineName, fixtureName, iterations, useconds);

// For using threads and test fixtures.
BASELINE_FIXED_T(groupName, baselineName, fixtureName, iterations, threads, useconds);
```

Example:

```cpp
BASELINE_FIXED_F(DemoTransform, FixedTime, DemoTransformFixture, 1, 100)
{ /* Nothing to do */ }
```

It is important that if your measurements use a test fixture, that your baseline (even if fixed) should use a test fixture as well.  Features such as User-Defined Measurements (UDMs) look to the baseline class to detect if other features are present.  If the baseline does not use a test fixture, Celero will not know that other classes do use a test fixture that offers a UDM.

### User-Defined Measurements (UDM)

Celero, by default, measures the execution time of your experiments. If you want to measure anything else, say for example the number of page faults via [PAPI](http://icl.cs.utk.edu/projects/papi/wiki/PAPIC:Overview), *user-defined measurements* are for you.

Adding user-defined measurements consists of three steps:

- Define a class for your user-defined measurement.  (One per type of measurement.) This class must derive from `celero::UserDefinedMeasurement`. Celero provides a convenience class `celero::UserDefinedMeasurementTemplate<>` which will be sufficient for most uses.
- Add (an) instance(s) of your class(es) to your test fixture. Implement `getUserDefinedMeasurements` to return these instances.
- At the appropriate point (most likely `tearDown()`), record your measurements in your user-defined measurement instances.

As a rough example, say you want to measure the number of page faults. The class for your user-defined measurement could be as simple as this:

```cpp
class PageFaultUDM : public celero::UserDefinedMeasurementTemplate<size_t>
{
  virtual std::string getName() const override
  {
    return "Page Faults";
  }

  // Optionally turn off some statistical reporting.
  virtual bool reportKurtosis() const override
  {
    return false;
  }
};
```

The only thing you *need* to implement in this case is a unique name. Other virtual functions are available inside `celero::UserDefinedMeasurementTemplate` and `celero::UserDefinedMeasurement` that you can leverage as needed.  There are optional virtual functions that you can override to turn off specific statistical measurements in the output.  These are:

```cpp
    virtual bool reportSize() const;
    virtual bool reportMean() const;
    virtual bool reportVariance() const;
    virtual bool reportStandardDeviation() const;
    virtual bool reportSkewness() const;
    virtual bool reportKurtosis() const;
    virtual bool reportZScore() const;
    virtual bool reportMin() const;
    virtual bool reportMax() const;
```

(By default, all of the ```report``` functions inside ```UserDefinedMeasurementTemplate``` return ```true```.)

Now, add it to your regular Celero test fixure:

```cpp
class SortFixture : public celero::TestFixture
{
public:
    SortFixture()
    {
        this->pageFaultUDM.reset(new PageFaultUDM());
    }

    [...]

    virtual std::vector<std::shared_ptr<celero::UserDefinedMeasurement>> getUserDefinedMeasurements() const override
    {
        return { this->pageFaultUDM };
    }

private:
    std::shared_ptr<CopyCountUDM> pageFaultUDM;
};
```

Finally, you need to record your results. For this pseud-code example, assume two functions exist: `resetPageFaultCounter()` and `getPageFaults()`.  These reset the number of page faults and return the number of page faults since last reset, respectively. Then, add these to the `setUp` and `tearDown` methods:

```cpp
class SortFixture : public celero::TestFixture
{
public:
    SortFixture()
    {
        this->pageFaultUDM.reset(new PageFaultUDM());
    }

    [...]

    // Gather page fault statistics inside the UDM.
    virtual void onExperimentEnd() override
    {
        [...]
        this->pageFaultUDM->addValue(this->getPageFaults());
    }

    [...] 

    // Reset the page fault counter.
    virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        [...]
        this->resetPageFaultCounter();
    }

    [...]

    virtual std::vector<std::shared_ptr<celero::UserDefinedMeasurement>> getUserDefinedMeasurements() const override
    {
        return { this->pageFaultUDM };
    }

private:
    std::shared_ptr<CopyCountUDM> pageFaultUDM;
    [...]
};
```

You will now be reporting statistics on the number of page faults that occurred during your experiments. See the `ExperimentSortingRandomIntsWithUDM` example for a complete example.

A note on User-Defined Measurements: This capability was introduced well after the creation of Celero.  While it is a great enhancement to the library, it was not designed-in to the library.  As such, the next major release of the library (v3.x) may change the way this is implemented and exposed to the library's users.  

### Frequency Scaling

CPU Frequency Scaling should be disabled if possible when executing benchmarks.  While there is code in Celero to attempt to do this, it may not have sufficient privileges to be effective.  On Linux systems, this can be accomplished as follows:

```bash
sudo cpupower frequency-set --governor performance
./celeroBenchmarkExecutable
sudo cpupower frequency-set --governor powersave
```

### Notes

-   Benchmarks should always be performed on Release builds.  Never measure the performance of a Debug build and make changes based on the results.  The (optimizing) compiler is your friend concerning code performance.
-   Accuracy is tied very closely to the total number of samples and the sample sizes.  As a general rule, you should aim to execute your baseline code for about as long as your longest benchmark test.  Further, it is helpful if all of the benchmark tests take about the same order of magnitude of execution time.  (Don't compare a baseline that executed in 0.1 seconds with benchmarks that take 60 seconds and an hour, respectively.)
-   Celero has Doxygen-style documentation of its API.  (The Doxyfile is provided.  The user must generate the documentation.)
-   Celero supports test fixtures for each baseline group.

## Celero Charts

### Background

It has been noted many times that writing an algorithm to solve small problems is relatively easy. "Brute force" methods tend to function just as well as more agile approaches. However, as the size of data increases, beneficial algorithms scale their performance to match.

Theoretically, the best we can hope for with an algorithm is that is scales linearly (Order N, O(N) complexity) with respect to the problem size. That is to say that if the problem set doubles, the time it takes for the algorithm to execute doubles. While this seems obvious, it is often an elusive goal.

Even well-performing algorithms eventually run into problems with available memory or CPU cache. When making decisions within our software about algorithms and improvements to existing code, only through measurement and experimentation, can we know our complex algorithms perform acceptably.

### Using the Code

While Celero offers simple benchmarking of code and algorithms, it also provides a more sophisticated method or directly producing performance graphs of how the benchmarks change with respect to some independent variable, referred to here as the Problem Set.

Within Celero, a test fixture can push integers into a `ProblemSetValues` vector which allows for the fixture's own SetUp function to scale a problem set for the benchmarks to run against. For each value pushed into the `ProblemSetValues` vector, a complete set of benchmarks is executed. These measured values are then stored and can be written out to a CSV file for easy plotting of results.

To demonstrate, we will study the performance of three common sorting algorithms: BubbleSort, SelectionSort, and `std::sort`. (The source code to this demo is distributed with Celero, available on [GitHub](https://github.com/DigitalInBlue/Celero).)  First, we will write a test fixture for Celero.

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

        // We will run some total number of sets of tests together. 
        // Each one growing by a power of 2.
        const int totalNumberOfTests = 6;

        for(int i = 0; i < totalNumberOfTests; i++)
        {
            // ExperimentValues is part of the base class and allows us to specify
            // some values to control various test runs to end up building a nice graph.
            problemSpace.push_back({int64_t(pow(2, i+1))});
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

Before the test fixture is utilized by a benchmark, Celero will create an instantiation of the class and call its `getExperimentValues()` function.  The test fixture can then build a vector of `TestFixture::ExperimentValue` values.  For each value added to this array, benchmarks will be executed following calls to the `setUp` virtual function.  A new test fixture is created for each measurement.

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

Finally, for good measure, we will simply use the Standard Library's sorting algorithm: `Introsort`. We only need to write a single line of code, but here it is for completeness.

```cpp
BENCHMARK_F(SortRandInts, stdSort, SortFixture, 30, 10000)
{
    this->randomize();
    std::sort(this->array.begin(), this->array.end());
    this->clear();
}
```

### Results

This test was run on a 4.00 GHz AMD with four cores, eight logical processors, and 32 GB of memory. (Hardware aside, the relative performance of these algorithms should be the same on any modern hardware.)

Celero outputs timing and benchmark references for each test automatically. However, to write to an output file for easy plotting, simply specify an output file on the command line.

```
celeroExperimentSortingRandomInts.exe -t results.csv
```

While not particularly surprising `std::sort` is by far the best option with any meaningful problem set size. The results are summarized in the following table output written directly by Celero:

```
Celero
Timer resolution: 0.100000 us
Writing results to: results.csv
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |   RAM (bytes)   |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|SortRandInts    | BubbleSort      |              64 |            2000 |               2 |         1.00000 |         6.50000 |       153846.15 |          905216 |
|SortRandInts    | BubbleSort      |             128 |            2000 |               2 |         1.00000 |        21.50000 |        46511.63 |          909312 |
|SortRandInts    | BubbleSort      |             256 |            2000 |               2 |         1.00000 |        72.50000 |        13793.10 |          909312 |
|SortRandInts    | BubbleSort      |             512 |            2000 |               2 |         1.00000 |       248.50000 |         4024.14 |          917504 |
|SortRandInts    | BubbleSort      |            1024 |            2000 |               2 |         1.00000 |       917.00000 |         1090.51 |          917504 |
|SortRandInts    | BubbleSort      |            2048 |            2000 |               2 |         1.00000 |      3607.50000 |          277.20 |          937984 |
|SortRandInts    | BubbleSort      |            4096 |            2000 |               2 |         1.00000 |     16567.00000 |           60.36 |          909312 |
|SortRandInts    | SelectionSort   |              64 |            2000 |               2 |         0.46154 |         3.00000 |       333333.33 |          909312 |
|SortRandInts    | SelectionSort   |             128 |            2000 |               2 |         0.44186 |         9.50000 |       105263.16 |          909312 |
|SortRandInts    | SelectionSort   |             256 |            2000 |               2 |         0.44138 |        32.00000 |        31250.00 |          909312 |
|SortRandInts    | SelectionSort   |             512 |            2000 |               2 |         0.43863 |       109.00000 |         9174.31 |          917504 |
|SortRandInts    | SelectionSort   |            1024 |            2000 |               2 |         0.43730 |       401.00000 |         2493.77 |          987136 |
|SortRandInts    | SelectionSort   |            2048 |            2000 |               2 |         0.42245 |      1524.00000 |          656.17 |         1122304 |
|SortRandInts    | SelectionSort   |            4096 |            2000 |               2 |         0.35749 |      5922.50000 |          168.85 |         1159168 |
|SortRandInts    | InsertionSort   |              64 |            2000 |               2 |         0.23077 |         1.50000 |       666666.67 |         1159168 |
|SortRandInts    | InsertionSort   |             128 |            2000 |               2 |         0.18605 |         4.00000 |       250000.00 |         1159168 |
|SortRandInts    | InsertionSort   |             256 |            2000 |               2 |         0.12414 |         9.00000 |       111111.11 |         1159168 |
|SortRandInts    | InsertionSort   |             512 |            2000 |               2 |         0.09256 |        23.00000 |        43478.26 |         1159168 |
|SortRandInts    | InsertionSort   |            1024 |            2000 |               2 |         0.06161 |        56.50000 |        17699.12 |         1159168 |
|SortRandInts    | InsertionSort   |            2048 |            2000 |               2 |         0.04435 |       160.00000 |         6250.00 |         1159168 |
|SortRandInts    | InsertionSort   |            4096 |            2000 |               2 |         0.03084 |       511.00000 |         1956.95 |         1159168 |
|SortRandInts    | QuickSort       |              64 |            2000 |               2 |         0.15385 |         1.00000 |      1000000.00 |         1159168 |
|SortRandInts    | QuickSort       |             128 |            2000 |               2 |         0.11628 |         2.50000 |       400000.00 |         1159168 |
|SortRandInts    | QuickSort       |             256 |            2000 |               2 |         0.07586 |         5.50000 |       181818.18 |         1159168 |
|SortRandInts    | QuickSort       |             512 |            2000 |               2 |         0.05433 |        13.50000 |        74074.07 |         1159168 |
|SortRandInts    | QuickSort       |            1024 |            2000 |               2 |         0.03162 |        29.00000 |        34482.76 |         1159168 |
|SortRandInts    | QuickSort       |            2048 |            2000 |               2 |         0.01746 |        63.00000 |        15873.02 |         1159168 |
|SortRandInts    | QuickSort       |            4096 |            2000 |               2 |         0.00803 |       133.00000 |         7518.80 |         1159168 |
|SortRandInts    | stdSort         |              64 |            2000 |               2 |         0.07692 |         0.50000 |      2000000.00 |         1159168 |
|SortRandInts    | stdSort         |             128 |            2000 |               2 |         0.09302 |         2.00000 |       500000.00 |         1159168 |
|SortRandInts    | stdSort         |             256 |            2000 |               2 |         0.06207 |         4.50000 |       222222.22 |         1159168 |
|SortRandInts    | stdSort         |             512 |            2000 |               2 |         0.04225 |        10.50000 |        95238.10 |         1159168 |
|SortRandInts    | stdSort         |            1024 |            2000 |               2 |         0.02508 |        23.00000 |        43478.26 |         1159168 |
|SortRandInts    | stdSort         |            2048 |            2000 |               2 |         0.01358 |        49.00000 |        20408.16 |         1159168 |
|SortRandInts    | stdSort         |            4096 |            2000 |               2 |         0.00637 |       105.50000 |         9478.67 |         1159168 |
Completed in 00:02:09.698721
```

The data shows first the test group name. Next, all of the data sizes are output. Then each row shows the baseline or benchmark name and the corresponding time for the algorithm to complete measured in useconds. This data, in CSV format, can be directly read by programs such as Microsoft Excel and plotted without any modification.  The CSV contains the following data:

```
Group,Experiment,Problem Space,Samples,Iterations,Failure,Baseline,us/Iteration,Iterations/sec,T Min (us),T Mean (us),T Max (us),T Variance,T Standard Deviation,T Skewness,T Kurtosis,T Z Score,R Min (us),R Mean (us),R Max (us),R Variance,R Standard Deviation,R Skewness,R Kurtosis,R Z Score,
SortRandInts,BubbleSort,64,2000,2,0,1,6.5,153846,13,14.2415,32,2.30133,1.51701,5.71041,47.2301,0.818385,905216,905216,905216,0,0,-nan(ind),0,0,
SortRandInts,BubbleSort,128,2000,2,0,1,21.5,46511.6,43,49.269,267,91.7625,9.57928,7.62785,136.8,0.654434,909312,909312,909312,0,0,-nan(ind),0,0,
SortRandInts,BubbleSort,256,2000,2,0,1,72.5,13793.1,145,158.413,645,1031.51,32.1171,5.04721,38.9898,0.417612,909312,909312,909312,0,0,-nan(ind),0,0,
SortRandInts,BubbleSort,512,2000,2,0,1,248.5,4024.14,497,577.158,1790,18081.1,134.466,2.67151,8.19469,0.59612,917504,917504,917504,0,0,-nan(ind),0,0,
SortRandInts,BubbleSort,1024,2000,2,0,1,917,1090.51,1834,1982.06,4802,76681.5,276.914,4.82492,27.7115,0.534671,917504,917504,917504,0,0,-nan(ind),0,0,
SortRandInts,BubbleSort,2048,2000,2,0,1,3607.5,277.2,7215,7684.98,14256,353860,594.862,5.38843,37.5894,0.790058,937984,937984,937984,0,0,-nan(ind),0,0,
SortRandInts,BubbleSort,4096,2000,2,0,1,16567,60.361,33134,34810.3,46135,2.0815e+06,1442.74,2.56028,8.76086,1.16188,909312,945914,974848,1.05957e+09,32551.1,-0.235618,-1.94448,1.12444,
SortRandInts,SelectionSort,64,2000,2,0,0.461538,3,333333,6,7.1075,19,2.17303,1.47412,3.61017,16.7992,0.751296,909312,909312,909312,0,0,-nan(ind),0,0,
SortRandInts,SelectionSort,128,2000,2,0,0.44186,9.5,105263,19,21.222,86,14.62,3.82361,5.59879,54.0056,0.581126,909312,909312,909312,0,0,-nan(ind),0,0,
SortRandInts,SelectionSort,256,2000,2,0,0.441379,32,31250,64,66.1445,169,29.8065,5.45954,10.3044,130.96,0.392799,909312,909312,909312,0,0,-nan(ind),0,0,
SortRandInts,SelectionSort,512,2000,2,0,0.438632,109,9174.31,218,233.505,693,1386.03,37.2294,5.6463,40.5119,0.416485,917504,985813,987136,9.04186e+07,9508.87,-7.04634,47.6509,7.18372,
SortRandInts,SelectionSort,1024,2000,2,0,0.437296,401,2493.77,802,851.35,1897,11262.6,106.125,4.88785,29.1835,0.465021,987136,1.11974e+06,1122304,3.40712e+08,18458.4,-7.04634,47.6509,7.18372,
SortRandInts,SelectionSort,2048,2000,2,0,0.422453,1524,656.168,3048,3222.06,6809,102590,320.296,5.38706,35.6438,0.543445,1122304,1.1223e+06,1122304,0,0,-nan(ind),0,0,
SortRandInts,SelectionSort,4096,2000,2,0,0.357488,5922.5,168.848,11845,12433,22307,496117,704.356,5.3611,45.0827,0.834836,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,InsertionSort,64,2000,2,0,0.230769,1.5,666667,3,4.0455,35,3.19703,1.78802,9.90047,117.897,0.584724,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,InsertionSort,128,2000,2,0,0.186047,4,250000,8,8.9535,33,2.88378,1.69817,8.06333,79.4729,0.561487,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,InsertionSort,256,2000,2,0,0.124138,9,111111,18,20.2965,76,12.785,3.57561,8.84815,92.6753,0.642268,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,InsertionSort,512,2000,2,0,0.0925553,23,43478.3,46,47.7135,135,8.70777,2.95089,17.2353,424.804,0.580672,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,InsertionSort,1024,2000,2,0,0.061614,56.5,17699.1,113,122.459,281,181.528,13.4732,5.50879,38.689,0.702021,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,InsertionSort,2048,2000,2,0,0.044352,160,6250,320,343.419,1097,1453.69,38.1273,7.47519,94.9702,0.614245,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,InsertionSort,4096,2000,2,0,0.0308444,511,1956.95,1022,1096.62,2287,13493.9,116.163,4.56136,24.6204,0.642346,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,QuickSort,64,2000,2,0,0.153846,1,1e+06,2,2.707,24,1.92111,1.38604,9.99569,121.501,0.510086,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,QuickSort,128,2000,2,0,0.116279,2.5,400000,5,6.5185,127,22.679,4.76225,12.6619,242.207,0.318862,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,QuickSort,256,2000,2,0,0.0758621,5.5,181818,11,13.391,44,10.8485,3.29371,5.97373,39.729,0.725929,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,QuickSort,512,2000,2,0,0.054326,13.5,74074.1,27,28.759,68,6.08096,2.46596,8.39068,86.394,0.713312,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,QuickSort,1024,2000,2,0,0.0316249,29,34482.8,58,63.3975,173,158.95,12.6075,4.68878,24.3046,0.428117,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,QuickSort,2048,2000,2,0,0.0174636,63,15873,126,133.461,432,197.08,14.0385,11.1413,188.347,0.531431,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,QuickSort,4096,2000,2,0,0.00802801,133,7518.8,266,283.615,712,1298.55,36.0354,5.49854,37.6957,0.488825,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,stdSort,64,2000,2,0,0.0769231,0.5,2e+06,1,2.099,11,0.260329,0.510225,4.56389,61.291,2.15395,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,stdSort,128,2000,2,0,0.0930233,2,500000,4,4.6665,16,0.436496,0.660678,4.7649,73.3782,1.00881,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,stdSort,256,2000,2,0,0.062069,4.5,222222,9,10.2385,22,0.999117,0.999559,6.39067,60.557,1.23905,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,stdSort,512,2000,2,0,0.0422535,10.5,95238.1,21,22.289,43,3.45621,1.85909,6.25736,46.5365,0.693351,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,stdSort,1024,2000,2,0,0.0250818,23,43478.3,46,51.6485,126,140.764,11.8644,2.89854,7.49573,0.476088,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,stdSort,2048,2000,2,0,0.0135828,49,20408.2,98,104.369,282,180.859,13.4484,6.58622,56.2908,0.473551,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
SortRandInts,stdSort,4096,2000,2,0,0.00636808,105.5,9478.67,211,221.056,468,392.122,19.8021,6.47829,52.0453,0.507826,1159168,1.15917e+06,1159168,0,0,-nan(ind),0,0,
```

Note that in this data, there are `T` statistics and there are `R` statistics.  `T` represents _Time_ and `R` represents _RAM_.

The point here is not that `std::sort` is better than more elementary sorting methods, but how easily measurable results can be obtained. In making such measurements more accessible and easier to code, they can become part of the way we code just as automated testing has become.

Test early and test often!

### Notes

-   Because I like explicitness as much as the next programmer, I want to note that the actual sorting algorithm used by `std::sort` is not defined in the standard, but references cite Introsort as a likely contender for how an STL implementation would approach `std::sort`. [Wikipedia](http://en.wikipedia.org/wiki/Introsort).
-   When choosing a sorting algorithm, start with `std::sort` and see if you can make improvements from there.
-   Don't just trust your experience, measure your code!

## FAQ

### Q: I asked for `N` iterations, but Celero ran `N+1` iterations.

The internal code will do one un-measured "warm-up" pass.  This helps account for caching which may otherwise influence measurements.

### Q: As my problem space increases in size, my runs take longer and longer.  How do I account for the increased complexity?

When defining a problem space, you set up a `celero::TestFixture::ExperimentValue`.  If the `Iterations` member in the class is greater than zero, that number will be used to control the amount of iterations for the corresponding `celero::TestFixture::ExperimentValue`.

```cpp
class MyFixture : public celero::TestFixture
{
public:
    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const override
    {
        std::vector<std::pair<int64_t, uint64_t>> problemSpaceValues;

        // We will run some total number of sets of tests together.
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

Example and demonstration code are provided under Celero's "experiments" folder.  There are two types of projects.  The first is "Demo" projects.  These are useful for illustrating techniques and ideas, but may not be interesting from a computer-science perspective.  Experiments, on the other hand, have been added which demonstrate real-world questions.  

The addition of real use cases of Celero is encouraged to be submitted to Celero's development branch for inclusion in the Demo and Experiment library.
