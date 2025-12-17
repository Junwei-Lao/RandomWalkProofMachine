#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <queue>
#include <algorithm> // for std::min/max
#include <iomanip>   // for std::setprecision
#include <random>
#include <utility>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include "myhash.h"
#include <cassert>
#include <climits>
#include <thread> // added for multithreading
#include <string>

#include "util.h"


int main(int argc, char *argv[])
{
    DISTANCE_POWER = std::atof(argv[1]);

    initemperature = std::atof(argv[2]);
    if (initemperature == 0.0) {initemperature = 1e-6;}

    finaltemperature = std::atof(argv[3]);
    if (finaltemperature == 0.0) {finaltemperature = 1e-6;}



    std::cout << "Starting shape analysis..." << std::endl;

    const std::vector<ShapeType> shapes = {CIRCLE, RECTANGLE, RECTANGLE_4_1, SQUARE, TRIANGLE, SHARP_TRIANGLE, FLAT_TRIANGLE, HEXAGON};
    const unsigned num_runs = 1000;

    std::vector<int> results;
    WalkContext mainCtx;

    // Determine worker count and prepare contexts
    unsigned worker_count = std::thread::hardware_concurrency();
    if (worker_count == 0)
        worker_count = 4;
    worker_count = std::min<unsigned>(worker_count, 8);
    std::vector<WalkContext> contexts(worker_count);

    for (ShapeType shape : shapes)
    {
        std::string shapeName;
        switch (shape)
        {
        case CIRCLE:
            shapeName = "Circle";
            break;
        case RECTANGLE:
            shapeName = "Rectangle";
            break;
        case RECTANGLE_4_1:
            shapeName = "Rectangle_4_1";
            break;
        case SQUARE:
            shapeName = "Square";
            break;
        case TRIANGLE:
            shapeName = "Triangle";
            break;
        case SHARP_TRIANGLE:
            shapeName = "Sharp_Triangle";
            break;
        case FLAT_TRIANGLE:
            shapeName = "Flat_Triangle";
            break;
        case TRAPEZOID:
            shapeName = "Trapezoid";
            break;
        case HEXAGON:
            shapeName = "Hexagon";
            break;
        }

        for (int index = 100; index <= 200; index++)
        {
            std::string filename = "walk_results_" + shapeName + '_' + std::to_string(DISTANCE_POWER) + '_' + std::to_string(initemperature) + '_' + std::to_string(finaltemperature) + '_' + ".csv";
            std::string distributionName = "distribution_" + shapeName + "_" + std::to_string(index) + ".csv";
            if (resultExists(filename, index, num_runs))
            {
                std::cout << "Skipping existing index " << index << std::endl;
                continue;
            }

            // Preallocate results array
            results.assign(num_runs, 0);

            // pick a random run to record a path (use mainCtx)
            std::uniform_int_distribution<unsigned> runDist(0, num_runs - 1);
            unsigned target_run = runDist(mainCtx.rng);

            std::vector<mytuple> chosen_path;
            std::mutex chosen_path_mutex;

            // Launch workers
            std::vector<std::thread> threads;
            threads.reserve(worker_count);
            size_t runs_per_thread = (num_runs + worker_count - 1) / worker_count;

            for (unsigned t = 0; t < worker_count; ++t)
            {
                size_t start = t * runs_per_thread;
                size_t end = std::min<size_t>(start + runs_per_thread, num_runs);
                if (start >= end)
                    continue;

                threads.emplace_back([&, t, start, end]()
                                     {
                                         WalkContext &local_ctx = contexts[t];
                                         for (size_t runIndex = start; runIndex < end; ++runIndex)
                                         {
                                             int res;
                                             if (runIndex == target_run)
                                             {
                                                 std::vector<mytuple> local_path;
                                                 res = walk(index, shape, local_ctx, &local_path);
                                                 if (!local_path.empty())
                                                 {
                                                     std::lock_guard<std::mutex> lock(chosen_path_mutex);
                                                     if (chosen_path.empty())
                                                         chosen_path = std::move(local_path);
                                                 }
                                             }
                                             else
                                             {
                                                 res = walk(index, shape, local_ctx, nullptr);
                                             }
                                             results[runIndex] = res;
                                         } });
            }

            for (auto &th : threads)
                th.join();


            std::ofstream distFile(distributionName);
            if (!distFile.tellp()) {
                distFile << "Steps\n";
            } 

            // Compute mean and stddev from results vector
            double sum = 0.0;
            for (unsigned i = 0; i < num_runs; ++i) {
                distFile << results[i] << "\n";
                sum += static_cast<double>(results[i]);
            }
            double mean = sum / static_cast<double>(num_runs);
            distFile.close();

            double sq_sum = 0.0;
            for (unsigned i = 0; i < num_runs; ++i)
            {
                double d = static_cast<double>(results[i]) - mean;
                sq_sum += d * d;
            }
            double std_dev = std::sqrt(sq_sum / static_cast<double>(num_runs));

            // Write CSV
            std::ofstream outFile(filename, std::ios::app);
            if (!outFile.tellp())
            {
                outFile << "Index,Mean,StdDev\n";
            }
            outFile << std::fixed << std::setprecision(6) << index << "," << mean << "," << std_dev << "\n";
            outFile.close();

            // Write chosen path if recorded
            if (!chosen_path.empty())
            {
                std::ostringstream pfn;
                pfn << "path_" << shapeName << "_" << index << ".txt";
                std::ofstream pfile(pfn.str());
                if (pfile)
                {
                    for (const auto &pt : chosen_path)
                        pfile << pt.x << " " << pt.y << "\n";
                }
                else
                {
                    std::cerr << "Warning: failed to open path file: " << pfn.str() << "\n";
                }
            }

            std::cout << "Completed index " << index << " with mean: " << mean
                      << ", std_dev: " << std_dev << std::endl;

            // Cleanup
            results.clear();
            results.shrink_to_fit();
            chosen_path.clear();
            chosen_path.shrink_to_fit();
            std::cout.flush();
        }
    }
    return 0;
}
