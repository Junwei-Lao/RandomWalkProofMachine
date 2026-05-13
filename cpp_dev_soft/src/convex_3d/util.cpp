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

#define isCrossBoundary true

const std::vector<mytuple_3d> sixdirs = {mytuple_3d(-1, 0, 0), mytuple_3d(1, 0, 0), mytuple_3d(0, -1, 0), mytuple_3d(0, 1, 0), mytuple_3d(0, 0, -1), mytuple_3d(0, 0, 1)};
const double PI = std::acos(-1);

const std::vector<std::string> shapeNames = {
    "SPHERE",
    "OCTAHEDRON"
    "CUBE",
    "RECTANGULAR_PRISM_3_1",
    "PYRAMID",
    "CYLINDER"
};




double DISTANCE_POWER;
double initemperature;
double finaltemperature;
bool hard_boundary = true;


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



bool inBoundary(int x, int y, int z, double index, ShapeType shape)
{
    switch (shape)
    {
    case SPHERE:
        return (x * x + y * y + z * z) <= (index * index);
    case OCTAHEDRON:
    {
        double side = index * pow(pow(8, 0.5)*PI, 1.0/3.0); // Calculate side length for given index
        return (std::abs(x) + std::abs(y) + std::abs(z)) <= std::abs(side/pow(2, 0.5));
    }
    case CUBE:
    {
        double constant = pow(4.0*PI/3.0, 1.0/3.0) * index; // Calculate constant for given index
        double half_side = constant / 2.0;
        return (std::abs(x) <= half_side) && (std::abs(y) <= half_side) && (std::abs(z) <= half_side);
    }
    case RECTANGULAR_PRISM_1_2_3:
    {
        double constant = pow(2.0*PI/9.0, 1.0/3.0) * index; // Calculate constant for given index
        double half_x = constant / 2.0;
        double half_y = constant;
        double half_z = 1.5 * constant;
        return (std::abs(x) <= half_x) && (std::abs(y) <= half_y) && (std::abs(z) <= half_z);
    }
    case PYRAMID:
    {
        double height = index * pow(pow(3, 0.5)*PI, 1.0/3.0) / pow(2, 0.5); // Calculate height for given index
        double base_half = height / pow(2, 0.5); // Base half-length for given height
        return (z >= 0) && (z <= height) && (std::abs(x) <= base_half * (1 - z / height)) && (std::abs(y) <= base_half * (1 - z / height));
    }
    case CYLINDER:
    {
        double radius = index;
        double height = (4.0 / 3.0) * radius; // Calculate height for given index
        return (x * x + y * y) <= (radius * radius) && (z >= 0) && (z <= height);
    }

    default:
        std::cerr << "Invalid shape type!" << std::endl;
        throw std::invalid_argument("Invalid shape type from inBoundary");
    }
    return false;
}


int getPointsInShape(double index, ShapeType shape, bool ****_3DMask = nullptr, int *xMax = nullptr, int *yMax = nullptr, int *zMax = nullptr, int *xMin = nullptr, int *yMin = nullptr, int *zMin = nullptr)
{
    int points = 0;
    
    int xfloor;
    int yfloor;
    int zfloor;

    int xcell;
    int ycell;
    int zcell;

    double constant;

    constexpr double MARGIN = 20.0;

    switch (shape)
    {
    case OCTAHEDRON:
        constant = index * pow(pow(8, 0.5)*PI, 1.0/3.0) / pow(2, 0.5);

        xfloor = -1*static_cast<int>(std::floor(constant)) - 1;
        yfloor = -1*static_cast<int>(std::floor(constant)) - 1;
        zfloor = -1*static_cast<int>(std::floor(constant)) - 1;

        xcell = static_cast<int>(std::ceil(constant)) + 1;
        ycell = static_cast<int>(std::ceil(constant)) + 1;
        zcell = static_cast<int>(std::ceil(constant)) + 1;
        break;
    case SPHERE:
        constant = index;

        xfloor = -1*static_cast<int>(std::floor(constant)) - 1;
        yfloor = -1*static_cast<int>(std::floor(constant)) - 1;
        zfloor = -1*static_cast<int>(std::floor(constant)) - 1;

        xcell = static_cast<int>(std::ceil(constant)) + 1;
        ycell = static_cast<int>(std::ceil(constant)) + 1;
        zcell = static_cast<int>(std::ceil(constant)) + 1;
        break;
    case CUBE:
        constant = pow(4.0*PI/3.0, 1.0/3.0) * index;
        xfloor = -1*static_cast<int>(std::floor(constant / 2.0)) - 1;
        yfloor = -1*static_cast<int>(std::floor(constant / 2.0)) - 1;
        zfloor = -1*static_cast<int>(std::floor(constant / 2.0)) - 1;

        xcell = static_cast<int>(std::ceil(constant / 2.0)) + 1;
        ycell = static_cast<int>(std::ceil(constant / 2.0)) + 1;
        zcell = static_cast<int>(std::ceil(constant / 2.0)) + 1;
        break;
    case RECTANGULAR_PRISM_1_2_3:
        constant = pow(2.0*PI/9.0, 1.0/3.0) * index;
        xfloor = -1*static_cast<int>(std::floor(constant / 2.0)) - 1;
        yfloor = -1*static_cast<int>(std::floor(constant)) - 1;
        zfloor = -1*static_cast<int>(std::floor(1.5 * constant)) - 1;
        xcell = static_cast<int>(std::ceil(constant / 2.0)) + 1;
        ycell = static_cast<int>(std::ceil(constant)) + 1;
        zcell = static_cast<int>(std::ceil(1.5 * constant)) + 1;
        break;
    case CYLINDER:
        constant = index;
        xfloor = -1*static_cast<int>(std::floor(constant)) - 1;
        yfloor = -1*static_cast<int>(std::floor(constant)) - 1;
        zfloor = -1;

        xcell = static_cast<int>(std::ceil(constant)) + 1;
        ycell = static_cast<int>(std::ceil(constant)) + 1;
        zcell = static_cast<int>(std::ceil((4.0 / 3.0) * constant)) + 1;
        break;
    default:
        throw std::invalid_argument("Invalid shape type in getPointsInShape");
    }

    if (_3DMask)
    {
        if (xMax) *xMax = xcell;
        if (yMax) *yMax = ycell;
        if (zMax) *zMax = zcell;
        if (xMin) *xMin = xfloor;
        if (yMin) *yMin = yfloor;
        if (zMin) *zMin = zfloor;

        if (*_3DMask != nullptr)
        {
            throw std::invalid_argument(
                "getPointsInShape requires *_3DMask == nullptr");
        }

        *_3DMask = new bool**[xcell - xfloor];
        for (int i = 0; i < xcell - xfloor; ++i)
        {
            (*_3DMask)[i] = new bool*[ycell - yfloor];
            for (int j = 0; j < ycell - yfloor; ++j)
            {
                (*_3DMask)[i][j] = new bool[zcell - zfloor];
                for (int k = 0; k < zcell - zfloor; ++k)
                {
                    (*_3DMask)[i][j][k] = inBoundary(xfloor + i, yfloor + j, zfloor + k, index, shape);
                }
            }
        }
    }



    for (int i = xfloor; i < xcell; ++i)
    {
        for (int j = yfloor; j < ycell; ++j)
        {
            for (int k = zfloor; k < zcell; ++k)
            {
                if (inBoundary(i, j, k, index, shape))
                {
                    ++points;
                }
            }
        }
    }

    return points;


}


void initialize(mytuple_3d &machine,
                int &uniquePoints,
                int &totalPoints,
                ShapeType shape,
                double index,
                WalkContext &ctx)
{
    auto &gen = ctx.rng;

    constexpr double MARGIN = 1.0;

    // helper: sample integer point in [xfloor, xceil) × [yfloor, yceil) × [zfloor, zceil)
    auto sampleInBox = [&](double xfloor, double yfloor, double zfloor, double xceil, double yceil, double zceil)
    {
        std::uniform_int_distribution<> distX(
            static_cast<int>(std::floor(xfloor)), static_cast<int>(std::ceil(xceil)));
        std::uniform_int_distribution<> distY(
            static_cast<int>(std::floor(yfloor)), static_cast<int>(std::ceil(yceil)));
        std::uniform_int_distribution<> distZ(
            static_cast<int>(std::floor(zfloor)), static_cast<int>(std::ceil(zceil)));
        machine.x = distX(gen);
        machine.y = distY(gen);
        machine.z = distZ(gen);
    };

    switch (shape)
    {
        case OCTAHEDRON:
        {
            double constant = index * pow(pow(8, 0.5)*PI, 1.0/3.0) / pow(2, 0.5);
            double xfloor = -1*constant - MARGIN;
            double yfloor = -1*constant - MARGIN;
            double zfloor = -1*constant - MARGIN;
            double xceil = constant + MARGIN;
            double yceil = constant + MARGIN;
            double zceil = constant + MARGIN;
            do {
                sampleInBox(xfloor, yfloor, zfloor, xceil, yceil, zceil);
            } while (!inBoundary(machine.x, machine.y, machine.z, index, OCTAHEDRON));
        }
        break;

        case SPHERE:
        {
            double constant = index;
            double xfloor = -1*constant - MARGIN;
            double yfloor = -1*constant - MARGIN;
            double zfloor = -1*constant - MARGIN;
            double xceil = constant + MARGIN;
            double yceil = constant + MARGIN;
            double zceil = constant + MARGIN;
            do {
                sampleInBox(xfloor, yfloor, zfloor, xceil, yceil, zceil);
            } while (!inBoundary(machine.x, machine.y, machine.z, index, SPHERE));
        }
        break;

        case CUBE:
        {
            double constant = pow(4.0*PI/3.0, 1.0/3.0) * index;
            double half_side = constant / 2.0;
            double xfloor = -1*half_side - MARGIN;
            double yfloor = -1*half_side - MARGIN;
            double zfloor = -1*half_side - MARGIN;
            double xceil = half_side + MARGIN;
            double yceil = half_side + MARGIN;
            double zceil = half_side + MARGIN;
            do {
                sampleInBox(xfloor, yfloor, zfloor, xceil, yceil, zceil);
            } while (!inBoundary(machine.x, machine.y, machine.z, index, CUBE));
        }
        break;

        case RECTANGULAR_PRISM_1_2_3:
        {
            double constant = pow(2.0*PI/9.0, 1.0/3.0) * index;
            double half_x = constant / 2.0;
            double half_y = constant;
            double half_z = 1.5 * constant;
            double xfloor = -1*half_x - MARGIN;
            double yfloor = -1*half_y - MARGIN;
            double zfloor = -1*half_z - MARGIN;
            double xceil = half_x + MARGIN;
            double yceil = half_y + MARGIN;
            double zceil = half_z + MARGIN;
            do {
                sampleInBox(xfloor, yfloor, zfloor, xceil, yceil, zceil);
            } while (!inBoundary(machine.x, machine.y, machine.z, index, RECTANGULAR_PRISM_1_2_3));
        }
        break;

        case CYLINDER:
        {
            double constant = index;
            double radius = constant;
            double height = (4.0 / 3.0) * radius;
            double xfloor = -1*radius - MARGIN;
            double yfloor = -1*radius - MARGIN;
            double zfloor = -1*MARGIN;
            double xceil = radius + MARGIN;
            double yceil = radius + MARGIN;
            double zceil = height + MARGIN;
            do {
                sampleInBox(xfloor, yfloor, zfloor, xceil, yceil, zceil);
            } while (!inBoundary(machine.x, machine.y, machine.z, index, CYLINDER));
        }
        break;

        default:
            throw std::invalid_argument("Invalid shape type in initialize");
    }

    uniquePoints = 1;
    totalPoints  = 1;
}


short getDistance(int hashsize, mytuple_3d position, double index, ShapeType shape)
{
    // Reduce hashmap size
    const int REDUCED_HASHSIZE = hashsize; // Use smaller fixed size
    short distance = 0;
    std::queue<std::pair<mytuple_3d, short>> bfsQueue;
    node **hashmap = new node *[REDUCED_HASHSIZE]();

    if (!Find_And_Insert_Node(hashmap, position, REDUCED_HASHSIZE))
    {
        freeHashmap(hashmap, REDUCED_HASHSIZE); // Clean up before throwing
        throw std::runtime_error("Initial position already in hashmap in getDistance!");
    }

    if (inBoundary(position.x, position.y, position.z, index, shape))
    {
        freeHashmap(hashmap, REDUCED_HASHSIZE);
        return distance;
    }
    bfsQueue.push(std::make_pair(position, distance));

    while (!bfsQueue.empty())
    {
        mytuple_3d currentPos = bfsQueue.front().first;
        short distance = bfsQueue.front().second;
        bfsQueue.pop();
        for (const mytuple_3d &dir : sixdirs)
        {
            mytuple_3d newPos(currentPos.x + dir.x, currentPos.y + dir.y, currentPos.z + dir.z);
            mytuple_3d machineTuple(newPos.x, newPos.y, newPos.z);
            if (inBoundary(newPos.x, newPos.y, newPos.z, index, shape))
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

mytuple_3d getNextPosition(mytuple_3d position, int pointsOUT, double index, ShapeType shape, WalkContext &ctx)
{
    // If hard boundary is enabled, restrict moves to neighbors that are inside the shape.
    if (hard_boundary)
    {
        std::vector<size_t> allowed;
        for (size_t dirIndex = 0; dirIndex < sixdirs.size(); ++dirIndex)
        {
            mytuple_3d nextPos(position.x + sixdirs[dirIndex].x, position.y + sixdirs[dirIndex].y, position.z + sixdirs[dirIndex].z);
            if (inBoundary(nextPos.x, nextPos.y, nextPos.z, index, shape))
                allowed.push_back(dirIndex);
        }

        if (!allowed.empty())
        {
            // choose uniformly among allowed neighbors
            std::uniform_int_distribution<size_t> uniformDist(0, allowed.size() - 1);
            size_t idx = allowed[uniformDist(ctx.rng)];
            return mytuple_3d(position.x + sixdirs[idx].x, position.y + sixdirs[idx].y, position.z + sixdirs[idx].z);
        }
        else
        {
            // No allowed neighbor — stay in place to avoid leaving boundary
            std::cerr << "Warning: No allowed moves from position (" << position.x << ", " << position.y << ", " << position.z << "). Staying in place." << std::endl;
            return position;
        }
    } 

    // get distances for each direction
    std::vector<short> distances;
    std::vector<double> weights;

    bool allInside = true; // Track if all next positions are inside
    double temperature = initemperature + (finaltemperature - initemperature) * std::exp(-0.05 * static_cast<double>(pointsOUT));

    // First check all possible next positions and compute their distances
    for (Direction dirIndex = UP; dirIndex <= OUTWARD; dirIndex = static_cast<Direction>(dirIndex + 1))
    {
        mytuple_3d NextPos(position.x + sixdirs[dirIndex].x, position.y + sixdirs[dirIndex].y, position.z + sixdirs[dirIndex].z);
        short distance;
        try
        {
            distance = getDistance(512, NextPos, index, shape);
            if (distance != 0)
            { // If any position is outside
                allInside = false;
            }
        }
        catch (std::runtime_error &e)
        {
            std::cerr << "Error in getDistance: " << e.what() << std::endl;
            throw;
        }
        distances.push_back(distance);
    }

    // If all next positions are inside, use uniform random choice
    if (allInside)
    {
        std::uniform_int_distribution<size_t> uniformDist(0, 5);
        size_t randomIndex = uniformDist(ctx.rng);
        return mytuple_3d(position.x + sixdirs[randomIndex].x,
                    position.y + sixdirs[randomIndex].y, position.z + sixdirs[randomIndex].z);
    }

    // Otherwise use weighted choice based on distances
    for (short dist : distances)
    {
        double w = std::exp(-1 * std::pow(static_cast<double>(dist), DISTANCE_POWER) / temperature);
        weights.push_back(w);
    }
    
    // if cross boundary is allowed, use the third way of walking
    // currently this walking is only for square shape, the weight will be 0.5+
    

    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
    size_t chosenIndex = dist(ctx.rng);
    return mytuple_3d(position.x + sixdirs[chosenIndex].x, position.y + sixdirs[chosenIndex].y, position.z + sixdirs[chosenIndex].z);
}

// walk now optionally records the path taken in out_path (if non-null)
int walk(double index, ShapeType shape, WalkContext &ctx, std::vector<mytuple_3d> *out_path)
{

    mytuple_3d machine(0, 0, 0);
    int uniquePoints = 0;
    int totalPoints = 0;
    int pointOUT = 0;

    bool **_mask = nullptr;
    int xMax, yMax, zMax;

    initialize(machine, uniquePoints, totalPoints, shape, index, ctx);

    int shapePoints = getPointsInShape(index, shape);
    int hashSize = shapePoints / 2;
    int expected_steps = std::min<int>(shapePoints / 2 + 5, 100000);
    std::vector<mytuple_3d> path;
    path.reserve(std::max(expected_steps, 100000)); // Reserve more space to handle cross boundary cases
    path.push_back(machine);
    node **hashmap = new node *[hashSize]();

    // Insert initial position into hashmap
    if (inBoundary(machine.x, machine.y, machine.z, index, shape))
    {
        Find_And_Insert_Node(hashmap, machine, hashSize);
    }

    try
    {
        int totalHalfPoints = shapePoints * 3 / 4;

        while (uniquePoints < totalHalfPoints)
        {
            machine = getNextPosition(machine, totalPoints, index, shape, ctx);
            if (!inBoundary(machine.x, machine.y, machine.z, index, shape))
            {
                std::cerr << "Moved outside boundary to (" << machine.x << ", " << machine.y << ", " << machine.z << "). Total points: " << totalPoints << ", Unique points: " << uniquePoints << ", Points OUT: " << pointOUT << std::endl;
            }
            path.emplace_back(machine.x, machine.y, machine.z);
            totalPoints += 1;

            if (inBoundary(machine.x, machine.y, machine.z, index, shape))
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

        // delete 2D mask
        if (_mask)
        {
            for (int i = 0; i < xMax; i++)
            {
                delete[] _mask[i];
            }
            delete[] _mask;
        }

        return totalPoints;
    }
    catch (...)
    {
        freeHashmap(hashmap, hashSize);
        throw;
    }
}



// Add this function before main()
bool resultExists(const std::string &filename, double index, int numThreads)
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

