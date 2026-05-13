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
    if (initemperature == 0.0)
    {
        initemperature = 1e-6;
    }

    finaltemperature = std::atof(argv[3]);
    if (finaltemperature == 0.0)
    {
        finaltemperature = 1e-6;
    }

    std::cout << "Starting shape analysis..." << std::endl;

    const std::vector<ShapeType> shapes = {SPHERE, OCTAHEDRON, CUBE, RECTANGULAR_PRISM_1_2_3, CYLINDER}; // HEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    const unsigned num_runs = 1000;

    std::vector<int> results;
    WalkContext mainCtx;

    std::random_device rd;

    // WalkContext mainCtx;
    std::seed_seq main_seq{rd(), rd(), rd(), rd()};
    mainCtx.rng.seed(main_seq);

    // Determine worker count and prepare contexts
    unsigned worker_count = std::thread::hardware_concurrency();
    if (worker_count == 0)
        worker_count = 4;
    worker_count = std::min<unsigned>(worker_count, 8); //std::min<unsigned>(worker_count, 8)
    std::vector<WalkContext> contexts(worker_count);

    // std::random_device rd;

    for (unsigned t = 0; t < worker_count; ++t)
    {
        std::seed_seq seq{
            rd(), rd(), rd(), rd(),
            static_cast<unsigned>(t)};
        contexts[t].rng.seed(seq);
    }

    for (ShapeType shape : shapes)
    {
        std::string shapeName;
        switch (shape)
        {
            case SPHERE:
                shapeName = "SPHERE";
                break;
            case OCTAHEDRON:
                shapeName = "OCTAHEDRON";
                break;
            case CUBE:
                shapeName = "CUBE";
                break;
            case RECTANGULAR_PRISM_1_2_3:
                shapeName = "RECTANGULAR_PRISM_1_2_3";
                break;
            case CYLINDER:
                shapeName = "CYLINDER";
                break;
            case PYRAMID:
                shapeName = "PYRAMID";
                break;
        }

        for (int index = 20; index <= 70; index++)
        {
            std::string filename = "example_walk_results_" + shapeName + '_' + std::to_string(DISTANCE_POWER) + '_' + std::to_string(initemperature) + '_' + std::to_string(finaltemperature) + '_' + ".csv";
            std::string distributionName = "distribution_" + shapeName + "_" + std::to_string(index) + ".csv";
            if (resultExists(filename, index, num_runs))
            {
                std::cout << "Skipping existing index " << index << std::endl;
                continue;
            }

            double index1 = 0.5*static_cast<double>(index); 

            // Preallocate results array
            results.assign(num_runs, 0);

            // pick a random run to record a path (use mainCtx)
            std::uniform_int_distribution<unsigned> runDist(0, num_runs - 1);
            unsigned target_run = runDist(mainCtx.rng);

            std::vector<mytuple_3d> chosen_path;
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
                                                std::vector<mytuple_3d> local_path;
                                                res = walk(index1, shape, local_ctx, &local_path);
                                                if (!local_path.empty())
                                                {
                                                    std::lock_guard<std::mutex> lock(chosen_path_mutex);
                                                    if (chosen_path.empty())
                                                        chosen_path = std::move(local_path);
                                                }
                                             }
                                             else
                                             {
                                                res = walk(index1, shape, local_ctx, nullptr);
                                             }
                                             results[runIndex] = res;
                                            // record cover disk count reported by walk()
                                            // cover_results[runIndex] = local_ctx.last_cover_count;
                                         } });
            }

            for (auto &th : threads)
                th.join();

            std::ofstream distFile(distributionName);
            if (!distFile.tellp())
            {
                distFile << "Steps\n";
            }

            // Compute mean and stddev from results and cover_results vectors
            double sum = 0.0;
            // double cover_sum = 0.0;
            for (unsigned i = 0; i < num_runs; ++i)
            {
                distFile << results[i] << "\n";
                sum += static_cast<double>(results[i]);
                // cover_sum += static_cast<double>(cover_results[i]);
            }
            double mean = sum / static_cast<double>(num_runs);
            //double cover_mean = cover_sum / static_cast<double>(num_runs);
            distFile.close();

            double sq_sum = 0.0;
            //double cover_sq_sum = 0.0;
            for (unsigned i = 0; i < num_runs; ++i)
            {
                double d = static_cast<double>(results[i]) - mean;
                sq_sum += d * d;
                //double cd = static_cast<double>(cover_results[i]) - cover_mean;
                //cover_sq_sum += cd * cd;
            }
            double std_dev = std::sqrt(sq_sum / static_cast<double>(num_runs));
            //double cover_std_dev = std::sqrt(cover_sq_sum / static_cast<double>(num_runs));

            // Write CSV
            std::ofstream outFile(filename, std::ios::app);
            if (!outFile.tellp())
            {
                outFile << "Index,Mean,StdDev\n";
            }
            outFile << std::fixed << std::setprecision(6) << index1 << "," << mean << "," << std_dev << "\n";
            outFile.close();

            // Write chosen path if recorded
            if (!chosen_path.empty())
            {
                std::ostringstream pfn;
                pfn << "path_" << shapeName << "_" << index1 << ".txt";
                std::ofstream pfile(pfn.str());
                if (pfile)
                {
                    // Write path points
                    for (const auto &pt : chosen_path)
                        pfile << pt.x << " " << pt.y << " " << pt.z << "\n";
                }
                else
                {
                    std::cerr << "Warning: failed to open path file: " << pfn.str() << "\n";
                }
            }

            std::cout << "Completed index " << index1 << " with mean: " << mean
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
