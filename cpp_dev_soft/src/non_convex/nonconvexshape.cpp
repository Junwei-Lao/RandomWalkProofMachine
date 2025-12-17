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
#include <thread>

// Softmax temperature (must be positive). Use double to avoid integer truncation.
double initemperature = 5.0;
double finaltemperature = 1.0;

const std::vector<mytuple> fourdirs = {mytuple(0, 1), mytuple(1, 0), mytuple(0, -1), mytuple(-1, 0)};
const double PI = std::acos(-1);
double DISTANCE_POWER = 1.0;

// Default thread count: keep modest to avoid creating thousands of OS threads
unsigned short num_threads = 8;
enum ShapeType
{
    BOWTIE,
    DOUBLEBOWTIE
};
enum Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

struct WalkContext
{
    std::mt19937_64 rng;
    std::mutex cout_mutex;

    WalkContext() : rng(std::random_device{}()) {}
};

void freeHashmap(node **hashmap, int HashSize)
{
    for (int i = 0; i < HashSize; i++)
    {
        node *cur = hashmap[i];
        while (cur)
        {
            node *next = cur->nextNode;
            delete cur;
            cur = next;
        }
    }
    delete[] hashmap;
}

bool inBoundary(int x, int y, int index, ShapeType shape)
{
    double width = sqrt(PI / 45) * index;
    double length = 5 * width;
    double hyposide = 4 * width;
    double Height = width + 2 * hyposide;
    double Long = length + 2 * hyposide;
    bool indicator;
    int rotatedX, rotatedY;
    double fixedWidth = 40.0;

    switch (shape)
    {
    case BOWTIE:
        if (x <= hyposide)
        {
            if (y >= x and y <= Height - x)
            {
                return true;
            }
        }
        else if (x > hyposide and x <= hyposide + length)
        {
            if (y >= hyposide and y <= hyposide + width)
            {
                return true;
            }
        }
        else if (x > hyposide + length and x <= Long)
        {
            if (y >= Long - x and y <= x + Height - Long)
            {
                return true;
            }
        }
        return false;
    case DOUBLEBOWTIE:
        rotatedX = -1 * y + (hyposide + length / 2) + (hyposide + fixedWidth / 2);
        rotatedY = x + (hyposide + fixedWidth / 2) - (hyposide + length / 2);
        indicator = false;
        if (x <= hyposide)
        {
            if (y >= x && y <= Height - x)
            {
                return true;
            }
        }
        else if (x > hyposide && x <= hyposide + length)
        {
            if (y >= hyposide && y <= hyposide + fixedWidth)
            {
                return true;
            }
        }
        else if (x > hyposide + length && x <= Long)
        {
            if (y >= Long - x && y <= x + Height - Long)
            {
                return true;
            }
        }

        if (!indicator)
        {
            if (rotatedX >= 0 && rotatedX <= hyposide)
            {
                if (rotatedY >= rotatedX && rotatedY <= Height - rotatedX)
                {
                    return true;
                }
            }
            else if (rotatedX > hyposide && rotatedX <= (hyposide + length))
            {
                if (rotatedY >= hyposide && rotatedY <= (hyposide + fixedWidth))
                {
                    return true;
                }
            }
            else if (rotatedX > (hyposide + length) && rotatedX <= Long)
            {
                if (rotatedY >= (Long - rotatedX) && rotatedY <= (rotatedX + Height - Long))
                {
                    return true;
                }
            }
        }

        return false;
    default:
        std::cerr << "Invalid shape type!" << std::endl;
        throw std::invalid_argument("Invalid shape type from inBoundary");
    }
    return true;
}

int getPointsInShape(int index, ShapeType shape)
{
    // std::cout << std::setprecision(20) << PI << std::endl;
    int points = 0;
    double xLength;
    double yLength;
    switch (shape)
    {
    case BOWTIE:
    {
        xLength = 5 * std::sqrt(PI / 45) * index + 2 * 4 * std::sqrt(PI / 45) * index + 1;
        yLength = std::sqrt(PI / 45) * index + 2 * 4 * std::sqrt(PI / 45) * index + 1;
        for (int i = -1; i <= xLength; i++)
        {
            for (int j = -1; j <= yLength; j++)
            {
                if (inBoundary(i, j, index, shape))
                {
                    points += 1;
                }
            }
        }
        return points;
    }
    case DOUBLEBOWTIE:
    {
        double width = sqrt(PI / 45) * index;
        double length = 5 * width;

        xLength = 5 * std::sqrt(PI / 45) * index + 2 * 4 * std::sqrt(PI / 45) * index + 1;
        yLength = 5 * std::sqrt(PI / 45) * index + 2 * 4 * std::sqrt(PI / 45) * index + 1;
        for (int i = -1; i <= xLength; i++)
        {
            for (int j = ((40.0 - length) / 2) - 1; j <= yLength; j++)
            {
                if (inBoundary(i, j, index, shape))
                {
                    points += 1;
                }
            }
        }
        return points;
    }
    default:
        std::cerr << "Invalid shape type!" << std::endl;
        throw std::invalid_argument("Invalid shape type from getPointsInShape");
    }
}

void initialize(mytuple &machine, int &uniquePoints, int &totalPoints, ShapeType shape, int index, WalkContext &ctx)
{
    // Use the RNG from the context
    auto &gen = ctx.rng;
    switch (shape)
    {
    case BOWTIE:
    {
        std::uniform_int_distribution<> distX(0, 5 * std::sqrt(PI / 45) * index + 2 * 4 * std::sqrt(PI / 45) * index + 1);
        std::uniform_int_distribution<> distY(0, std::sqrt(PI / 45) * index + 2 * 4 * std::sqrt(PI / 45) * index + 1);
        do
        {
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, BOWTIE));
        break;
    }
    case DOUBLEBOWTIE:
    {
        std::uniform_int_distribution<> distX2(0, 5 * std::sqrt(PI / 45) * index + 2 * 4 * std::sqrt(PI / 45) * index + 1);
        std::uniform_int_distribution<> distY2(((40.0 - 5 * std::sqrt(PI / 45) * index) / 2) - 1, 5 * std::sqrt(PI / 45) * index + 2 * 4 * std::sqrt(PI / 45) * index + 1);
        do
        {
            machine.x = distX2(gen);
            machine.y = distY2(gen);
        } while (!inBoundary(machine.x, machine.y, index, DOUBLEBOWTIE));
        break;
    }
    default:
        std::cerr << "Invalid shape type!" << std::endl;
        throw std::invalid_argument("Invalid shape type from initialize");
    }
    uniquePoints = 1;
    totalPoints = 1;
}

short getDistance(int hashsize, mytuple position, int index, ShapeType shape)
{
    // Reduce hashmap size
    const int REDUCED_HASHSIZE = hashsize;
    short distance = 0;
    std::queue<std::pair<mytuple, short>> bfsQueue;
    node **hashmap = new node *[REDUCED_HASHSIZE]();

    if (!Find_And_Insert_Node(hashmap, position, REDUCED_HASHSIZE))
    {
        freeHashmap(hashmap, REDUCED_HASHSIZE); // Clean up before throwing
        throw std::runtime_error("Initial position already in hashmap in getDistance!");
    }

    if (inBoundary(position.x, position.y, index, shape))
    {
        freeHashmap(hashmap, REDUCED_HASHSIZE);
        return distance;
    }

    bfsQueue.push(std::make_pair(position, distance));
    while (!bfsQueue.empty())
    {
        mytuple currentPos = bfsQueue.front().first;
        short distance = bfsQueue.front().second;
        bfsQueue.pop();
        for (const mytuple &dir : fourdirs)
        {
            mytuple newPos(currentPos.x + dir.x, currentPos.y + dir.y);
            mytuple machineTuple(newPos.x, newPos.y);
            if (inBoundary(newPos.x, newPos.y, index, shape))
            {
                freeHashmap(hashmap, REDUCED_HASHSIZE);
                return distance + 1;
            }
            if (Find_And_Insert_Node(hashmap, machineTuple, REDUCED_HASHSIZE))
            {
                bfsQueue.push(std::make_pair(newPos, distance + 1));
            }
        }
    }

    freeHashmap(hashmap, REDUCED_HASHSIZE);
    throw std::runtime_error("BFS exhausted all options without finding a boundary!");
}

mytuple getNextPosition(mytuple position, int pointOUT, int index, ShapeType shape, WalkContext &ctx)
{
    // get weight for each direction
    std::vector<double> weights;
    std::vector<short> distances;
    bool isAllInBoundary = true;
    double temperature = initemperature + (finaltemperature - initemperature) * std::exp (-0.05*static_cast<double>(pointOUT));

    for (Direction dirIndex = UP; dirIndex <= LEFT; dirIndex = static_cast<Direction>(dirIndex + 1))
    {
        mytuple NextPos(position.x + fourdirs[dirIndex].x, position.y + fourdirs[dirIndex].y);
        short distance;
        try
        {
            distance = getDistance(1024, NextPos, index, shape);
            if (distance == 0)
            {
                isAllInBoundary = isAllInBoundary && true;
            }
            else
            {
                isAllInBoundary = isAllInBoundary && false;
            }
        }
        catch (std::runtime_error &e)
        {
            std::cerr << "Error in getDistance: " << e.what() << std::endl;
            throw;
        }
        distances.push_back(distance);
    }

    // std::cout << "Distances: " << distances[0] << ", " << distances[1] << ", " << distances[2] << ", " << distances[3] << std::endl;

    if (isAllInBoundary)
    {
        std::uniform_int_distribution<size_t> uniformDist(0, 3);
        size_t randomIndex = uniformDist(ctx.rng);
        mytuple dir = fourdirs[randomIndex];
        return mytuple(position.x + dir.x, position.y + dir.y);
    }
    else
    {
        for (short dist : distances)
        {
            double w = std::exp(-1*std::pow(static_cast<double>(dist), DISTANCE_POWER) / temperature);
            weights.push_back(w);
        }
        std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
        size_t chosenIndex = dist(ctx.rng);
        mytuple dir = fourdirs[chosenIndex];

        return mytuple(position.x + dir.x, position.y + dir.y);
    }
}

// walk now optionally records the path taken in out_path (if non-null)
int walk(int index, ShapeType shape, WalkContext &ctx, std::vector<mytuple> *out_path = nullptr)
{
    mytuple machine(0, 0);
    int uniquePoints = 0;
    int totalPoints = 0;
    int pointOUT = 0;

    initialize(machine, uniquePoints, totalPoints, shape, index, ctx);

    int shapePoints = getPointsInShape(index, shape);
    int hashSize = shapePoints / 2;
    int expected_steps = std::min<int>(shapePoints / 2 + 1, 100000);
    std::vector<mytuple> path;
    path.reserve(expected_steps);
    path.push_back(machine);
    node **hashmap = new node *[hashSize]();

    // Insert initial position into hashmap
    if (inBoundary(machine.x, machine.y, index, shape))
    {
        Find_And_Insert_Node(hashmap, machine, hashSize);
    }

    try
    {
        int totalHalfPoints = shapePoints / 2;

        while (uniquePoints < totalHalfPoints)
        {
            machine = getNextPosition(machine, totalPoints, index, shape, ctx);
            path.emplace_back(machine.x, machine.y);
            totalPoints += 1;

            if (inBoundary(machine.x, machine.y, index, shape))
            {
                if (Find_And_Insert_Node(hashmap, machine, hashSize))
                {
                    uniquePoints += 1;
                }
            }
            else
            {
                pointOUT += 1;
            }
        }

        if (out_path)
        {
            *out_path = std::move(path);
        }

        freeHashmap(hashmap, hashSize);
        return totalPoints;
    }
    catch (...)
    {
        freeHashmap(hashmap, hashSize);
        throw;
    }
}

// Add this function before main()
bool resultExists(const std::string &filename, int index, int numThreads)
{
    std::ifstream inFile(filename);
    if (!inFile)
        return false;

    std::string line;
    // Skip header
    std::getline(inFile, line);

    while (std::getline(inFile, line))
    {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> row;

        while (std::getline(ss, item, ','))
        {
            row.push_back(item);
        }

        if (row.size() >= 2)
        {
            try
            {
                int fileIndex = std::stoi(row[0]);
                int fileThreads = std::stoi(row[1]);
                if (fileIndex == index && fileThreads == numThreads)
                {
                    return true;
                }
            }
            catch (...)
            {
                continue;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    DISTANCE_POWER = std::atof(argv[1]);
    
    initemperature = std::atof(argv[2]);
    if (initemperature == 0.0) {initemperature = 1e-6;}

    finaltemperature = std::atof(argv[3]);
    if (finaltemperature == 0.0) {finaltemperature = 1e-6;}





    std::cout << "Starting shape analysis..." << std::endl;

    const std::vector<ShapeType> shapes = {BOWTIE};
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
        case BOWTIE:
            shapeName = "BOWTIE";
            break;
        case DOUBLEBOWTIE:
            shapeName = "DOUBLEBOWTIE";
            break;
        }

        for (int index = 10; index <= 75; index++)
        {
            std::string filename = "walk_results_" + shapeName + ".csv";
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

            // Write distribution data
            std::ofstream distFile(distributionName);
            if (!distFile.tellp())
            {
                distFile << "Steps\n";
            }

            // Compute statistics
            double sum = 0.0;
            for (unsigned i = 0; i < num_runs; ++i)
            {
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

            // Write results
            std::ofstream outFile(filename, std::ios::app);
            if (!outFile.tellp())
            {
                outFile << "Index,Mean,StdDev\n";
            }
            outFile << std::fixed << std::setprecision(6) << index << "," << mean << "," << std_dev << "\n";
            outFile.close();

            // Write path file
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
