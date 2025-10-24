#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <queue>
#include <algorithm> // for std::min/max
#include <iomanip>   // for std::setprecision
#include <thread>
#include <random>
#include <utility>
#include <mutex>
#include <stdexcept>
#include "myhash.h"

#include <cassert>

const int temperature = 1;

const std::vector<mytuple> fourdirs = {mytuple(0, 1), mytuple(1, 0), mytuple(0, -1), mytuple(-1, 0)};
const double PI = std::acos(-1);

unsigned short num_threads = 1500;
enum ShapeType
{
    CIRCLE,
    RECTANGLE,
    SQUARE,
    TRIANGLE,
    TRAPEZOID
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

bool inBoundary(int x, int y, int index, ShapeType shape)
{
    double _constant;
    switch (shape)
    {
    case CIRCLE:
        return ((x - index)*(x - index) + (y - index)*(y - index)) <= index * index;
    case RECTANGLE:
        _constant = std::sqrt(PI / 3);
        return (x >= 0 && x <= 3 * _constant * index && y >= 0 && y <= _constant * index);
    case SQUARE:
        _constant = std::sqrt(PI);
        return (x >= 0 && x <= _constant * index && y >= 0 && y <= _constant * index); // Unit square
    case TRIANGLE:
        _constant = std::sqrt(4 * PI / std::sqrt(3));
        if (x >= 0 && x < _constant * index / 2)
        {
            return (y >= 0 && y <= std::sqrt(3) * x);
        }
        else if (x >= _constant * index / 2 && x <= _constant * index)
        {
            return (y >= 0 && y <= -std::sqrt(3) * x + std::sqrt(3) * _constant * index);
        }
        return false;
    case TRAPEZOID:
        _constant = std::sqrt(PI / 2);
        if (x >= 0 && x < _constant * index)
        {
            return (y >= 0 && y <= x);
        }
        else if (x >= _constant * index && x < 2 * _constant * index)
        {
            return (y >= 0 && y <= _constant * index);
        }
        else if (x >= 2 * _constant * index && x <= 3 * _constant * index)
        {
            return (y >= 0 && y <= -x + 3 * _constant * index);
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
    std::cout << std::setprecision(20) << PI << std::endl;
    int points = 0;
    double xLength;
    double yLength;
    switch (shape)
    {
    case CIRCLE:
        xLength = 2 * index;
        yLength = 2 * index;
        break;
    case RECTANGLE:
        yLength = std::sqrt(PI / 3) * index;
        xLength = 3 * yLength;
        break;
    case SQUARE:
        xLength = std::sqrt(PI) * index;
        yLength = xLength;
        break;
    case TRIANGLE:
        xLength = std::sqrt(4 * PI / std::sqrt(3)) * index;
        yLength = std::sqrt(3) * xLength / 2;
        break;
    case TRAPEZOID:
        xLength = 3 * std::sqrt(PI / 2) * index;
        yLength = std::sqrt(PI / 2) * index;
        break;
    default:
        std::cerr << "Invalid shape type!" << std::endl;
        throw std::invalid_argument("Invalid shape type from getPointsInShape");
    }

    xLength += 1;
    yLength += 1;

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

void initialize(mytuple &machine, int &uniquePoints, int &totalPoints, ShapeType shape, int index, WalkContext &ctx)
{
    // Use the RNG from the context
    auto &gen = ctx.rng;
    switch (shape)
    {
    case CIRCLE:
        do
        {
            std::uniform_int_distribution<> distX(0, index * index + 1);
            std::uniform_int_distribution<> distY(0, index * index + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, CIRCLE));
        break;
    case RECTANGLE:
        do
        {
            std::uniform_int_distribution<> distX(0, 3 * std::sqrt(PI / 3) * index + 1);
            std::uniform_int_distribution<> distY(0, std::sqrt(PI / 3) * index + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, RECTANGLE));
        break;
    case SQUARE:
        do
        {
            std::uniform_int_distribution<> distX(0, std::sqrt(PI) * index + 1);
            std::uniform_int_distribution<> distY(0, std::sqrt(PI) * index + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, SQUARE));
        break;
    case TRIANGLE:
        do
        {
            std::uniform_int_distribution<> distX(0, std::sqrt(4 * PI / std::sqrt(3)) * index + 1);
            std::uniform_int_distribution<> distY(0, std::sqrt(3) * std::sqrt(4 * PI / std::sqrt(3)) * index / 2 + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, TRIANGLE));
        break;
    case TRAPEZOID:
        do
        {
            std::uniform_int_distribution<> distX(0, 3 * std::sqrt(PI / 2) * index + 1);
            std::uniform_int_distribution<> distY(0, std::sqrt(PI / 2) * index + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, TRAPEZOID));
        break;
    default:
        std::cerr << "Invalid shape type!" << std::endl;
        throw std::invalid_argument("Invalid shape type from initialize");
    }
    uniquePoints = 1;
    totalPoints = 1;
}

short getDistance(int hashsize, mytuple position, int index, ShapeType shape)
{
    short distance = 0;
    std::queue<std::pair<mytuple, short>> bfsQueue;
    node **hashmap = new node *[hashsize]();

    if (!Find_And_Insert_Node(hashmap, position, hashsize))
    {
        throw std::runtime_error("Initial position already in hashmap in getDistance!");
    }

    if (inBoundary(position.x, position.y, index, shape))
    {
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
                delete[] hashmap;
                return distance + 1;
            }
            if (Find_And_Insert_Node(hashmap, machineTuple, hashsize))
            {
                bfsQueue.push(std::make_pair(newPos, distance + 1));
            }
        }
    }

    delete[] hashmap;
    throw std::runtime_error("BFS exhausted all options without finding a boundary!");
}

mytuple getNextPosition(mytuple position, int index, ShapeType shape, WalkContext &ctx)
{
    // get weight for each direction
    std::vector<double> weights;
    std::vector<short> distances;
    for (Direction dirIndex = UP; dirIndex <= LEFT; dirIndex = static_cast<Direction>(dirIndex + 1))
    {
        mytuple NextPos(position.x + fourdirs[dirIndex].x, position.y + fourdirs[dirIndex].y);
        short distance;
        try
        {
            distance = getDistance(1024, NextPos, index, shape);
        }
        catch (std::runtime_error &e)
        {
            std::cerr << "Error in getDistance: " << e.what() << std::endl;
            throw;
        }
        distances.push_back(distance);
    }

    std::cout << "Distances: " << distances[0] << ", " << distances[1] << ", " << distances[2] << ", " << distances[3] << std::endl;

    for (short dist : distances)
    {
        double w = std::exp(-static_cast<double>(dist) / temperature);
        weights.push_back(w);
    }

    // choose direction based on weights using context's RNG
    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());

    size_t chosenIndex = dist(ctx.rng);
    mytuple dir = fourdirs[chosenIndex];

    std::cout << "Chosen direction: " << dir.x << ", " << dir.y << std::endl;

    return mytuple(position.x + dir.x, position.y + dir.y);
}

int walk(int index, ShapeType shape, WalkContext &ctx)
{
    mytuple machine(0, 0);
    int uniquePoints = 0;
    int totalPoints = 0;

    // Pass WalkContext to initialize
    initialize(machine, uniquePoints, totalPoints, shape, index, ctx);

    int shapePoints = getPointsInShape(index, shape);
    int hashSize = shapePoints / 2;
    node **hashmap = new node *[hashSize]();

    try
    {
        int totalHalfPoints = shapePoints / 2;
        while (totalPoints < totalHalfPoints)
        {
            // Pass WalkContext to getNextPosition
            machine = getNextPosition(machine, index, shape, ctx);
            totalPoints += 1;
            mytuple machineTuple(machine.x, machine.y);
            if (Find_And_Insert_Node(hashmap, machineTuple, hashSize))
            {
                uniquePoints += 1;
            }
        }

        delete[] hashmap;
        return uniquePoints;
    }
    catch (...)
    {
        delete[] hashmap;
        throw;
    }
}

void container()
{
    std::cout << "Starting shape analysis..." << std::endl;

    const std::vector<ShapeType> shapes = {CIRCLE, RECTANGLE, SQUARE, TRIANGLE, TRAPEZOID};
    const std::vector<int> indices = {100, 200, 300}; // Add more indices if needed

    std::vector<std::thread> threads;
    std::vector<int> results;
    std::mutex results_mutex;
    std::vector<WalkContext> contexts(num_threads);

    for (ShapeType shape : shapes)
    {
        for (int index = 0; index < 300; index++)
        {
            std::cout << "\nProcessing shape " << static_cast<int>(shape)
                      << " with index " << index << std::endl;

            results.clear();
            results.reserve(num_threads);

            for (unsigned short i = 0; i < num_threads; ++i)
            {
                threads.emplace_back([&results, &results_mutex, &contexts, i, index, shape]()
                                     {
            int result = walk(index, shape, contexts[i]);
            {
                std::lock_guard<std::mutex> lock(results_mutex);
                results.push_back(result);
            } });
            }

            // Wait for all threads to complete
            for (auto &thread : threads)
            {
                thread.join();
            }

            // Now analyze the results
            std::cout << "\nAnalyzing results from " << num_threads << " walks:\n";

            // Calculate statistics
            double sum = 0;
            int min_val = results[0];
            int max_val = results[0];

            for (int result : results)
            {
                sum += result;
                min_val = std::min(min_val, result);
                max_val = std::max(max_val, result);
            }

            double mean = sum / results.size();

            // Calculate standard deviation
            double sq_sum = 0;
            for (int result : results)
            {
                sq_sum += (result - mean) * (result - mean);
            }
            double std_dev = std::sqrt(sq_sum / results.size());

            // Get shape name
            std::string shapeName;
            switch (shape)
            {
            case CIRCLE:
                shapeName = "Circle";
                break;
            case RECTANGLE:
                shapeName = "Rectangle";
                break;
            case SQUARE:
                shapeName = "Square";
                break;
            case TRIANGLE:
                shapeName = "Triangle";
                break;
            case TRAPEZOID:
                shapeName = "Trapezoid";
                break;
            }

            // Create filename with shape name
            std::string filename = "walk_results_" + shapeName + ".csv";

            // Open CSV file for writing (append mode)
            std::ofstream outFile(filename, std::ios::app);

            // If file is empty, write header
            if (outFile.tellp() == 0)
            {
                outFile << "Index,NumThreads,Mean,StdDev,Min,Max\n";
            }

            // Write results with fixed precision
            outFile << std::fixed << std::setprecision(5)
                    << index << ","
                    << num_threads << ","
                    << mean << ","
                    << std_dev << ","
                    << min_val << ","
                    << max_val << "\n";

            outFile.close();

            // Also print to console
            std::cout << "\nResults written to walk_results.csv\n";
            std::cout << "Shape: " << static_cast<int>(shape)
                      << ", Index: " << index
                      << ", Threads: " << num_threads << "\n";
            std::cout << "Mean: " << mean
                      << ", StdDev: " << std_dev
                      << ", Min: " << min_val
                      << ", Max: " << max_val << "\n";

            // return 0;
        }
    }
}

/**/
int main() {

    int hashsize = 1024;
    int index = 50;
    //mytuple pos_boundary(18, 18);
    
    //assert(getPointsInShape(index, CIRCLE) == 2821);

    std::cout << getPointsInShape(index, CIRCLE) << std::endl;
    return 0;
}
