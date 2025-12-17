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


const std::vector<mytuple> fourdirs = {mytuple(0, 1), mytuple(1, 0), mytuple(0, -1), mytuple(-1, 0)};
const double PI = std::acos(-1);
double DISTANCE_POWER = 1.0;
double initemperature = 5.0;
double finaltemperature = 1.0;
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

bool inBoundary(int x, int y, int index, ShapeType shape)
{
    double _constant;
    double longside;

    // For POLYGON8
    double side;
    double refHeight;
    double refTopY;
    double rotatedX, rotatedY;


    switch (shape)
    {
    case CIRCLE:
        return ((x - index) * (x - index) + (y - index) * (y - index)) <= index * index;
    case RECTANGLE:
        _constant = std::sqrt(PI / 3);
        return (x >= 0 && x <= 3 * _constant * index && y >= 0 && y <= _constant * index);
    case RECTANGLE_4_1:
        _constant = std::sqrt(PI / 4);
        return (x >= 0 && x <= 4 * _constant * index && y >= 0 && y <= _constant * index);
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
    case SHARP_TRIANGLE:
        _constant = std::sqrt(4 * PI / std::tan(75 * PI / 180));
        if (x >= 0 && x < _constant * index / 2)
        {
            return (y >= 0 && y <= std::tan(75 * PI / 180) * x);
        }
        else if (x >= _constant * index / 2 && x <= _constant * index)
        {
            return (y >= 0 && y <= _constant*index/std::tan(15 * PI / 180) - std::tan(75 * PI / 180) * x);
        }
        return false;
    case FLAT_TRIANGLE:
        _constant = std::sqrt(4 * PI / std::tan(40 * PI / 180));
        if (x >= 0 && x < _constant * index / 2)
        {
            return (y >= 0 && y <= std::tan(40 * PI / 180) * x);
        }
        else if (x >= _constant * index / 2 && x <= _constant * index)
        {
            return (y >= 0 && y <= _constant*index*std::tan(40 * PI / 180) - std::tan(40 * PI / 180) * x);
        }
        return false;
    case HEXAGON:
        _constant = std::sqrt(2*PI / (3 * std::sqrt(3)));
        longside = 2*_constant*index;
        if (x >= 0 && x < longside/4)
        {
            return (y >= longside*std::sqrt(3)/4 - std::sqrt(3)*x && y <= longside*std::sqrt(3)/4 + std::sqrt(3)*x);
        }
        else if (x >= longside/4 && x < 3*longside/4)
        {
            return (y >= 0 && y <= longside*std::sqrt(3)/2);
        }
        else if (x >= 3*longside/4 && x <= longside)
        {
            return (y >= longside*std::sqrt(3)/4 - std::sqrt(3)*(longside - x) && y <= longside*std::sqrt(3)/4 + std::sqrt(3)*(longside - x));
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
    case POLYGON8:
        _constant = std::sqrt(PI / (4*std::sin(45*PI / 180)));
        longside = 2 * _constant * index * std::cos(22.5 * PI / 180);
        side = 2 * _constant * index * std::cos(67.5 * PI / 180); 
        // Instead of hardcoding the entire shape, I am planning to use a reference triangle and rotate points into it.
        // The reference triangle will have vertices at (0,side/sqrt(2)), (0, side/sqrt(2)+side), and (longside/2, side/sqrt(2)+side/2)
        // rotating center is (longside/2, side/sqrt(2)+side/2)
        refHeight = _constant * index * (1-std::cos(67.5 * PI / 180));
        refTopY = refHeight + side;

        for (int i = 0; i < 8; i++)
        {
            double angle = -i * 45.0 * PI / 180.0;
            rotatedX = (x - longside / 2) * std::cos(angle) - (y - (refHeight + side / 2)) * std::sin(angle) + longside / 2;
            rotatedY = (x - longside / 2) * std::sin(angle) + (y - (refHeight + side / 2)) * std::cos(angle) + (refHeight + side / 2);
            if (rotatedX >= 0 && rotatedX <= longside / 2)
            {
                double slope = std::tan(22.5 * PI / 180);
                double lineY_upper = refTopY - slope * rotatedX;
                double lineY_lower = refHeight + slope * rotatedX;
                if (rotatedY >= lineY_lower && rotatedY <= lineY_upper)
                {
                    return true;
                }
            }
        }

        /*
        for (int i = 0; i < 8; i++)
        {
            double angle = -i * 45.0 * PI / 180.0;
            rotatedX = x * std::cos(angle) - y * std::sin(angle);
            rotatedY = x * std::sin(angle) + y * std::cos(angle);
            if (rotatedX >= 0 && rotatedX <= longside / 2)
            {
                double slope = std::tan(22.5 * PI / 180);
                double lineY_upper = refTopY - slope * rotatedX;
                double lineY_lower = refHeight + slope * rotatedX;
                if (rotatedY >= lineY_lower && rotatedY <= lineY_upper)
                {
                    return true;
                }
            }
        }
        */
        return false;
    case POLYGON10:
        //same as polygon8 but 10 sides
        // reference triangle will have vertices at (0, _constant * index * (1-std::cos(72 * PI / 180))), (0, _constant * index * (1-std::cos(72 * PI / 180)) + side), and (longside/2, _constant * index * (1-std::cos(72 * PI / 180)) + side/2)
        _constant = std::sqrt(PI / (5*std::sin(36*PI / 180)));
        longside = 2 * _constant * index * std::cos(18 * PI / 180);
        side = 2 * _constant * index * std::cos(72 * PI / 180);
        refHeight = _constant * index * (1-std::cos(72 * PI / 180));
        refTopY = refHeight + side;
        for (int i = 0; i < 10; i++)
        {
            double angle = -i * 36.0 * PI / 180.0;
            rotatedX = (x - longside / 2) * std::cos(angle) - (y - (refHeight + side / 2)) * std::sin(angle) + longside / 2;
            rotatedY = (x - longside / 2) * std::sin(angle) + (y - (refHeight + side / 2)) * std::cos(angle) + (refHeight + side / 2);
            if (rotatedX >= 0 && rotatedX <= longside / 2)
            {
                double slope = std::tan(18.0 * PI / 180.0);
                double lineY_upper = refTopY - slope * rotatedX;
                double lineY_lower = refHeight + slope * rotatedX;
                if (rotatedY >= lineY_lower && rotatedY <= lineY_upper)
                {
                    return true;
                }
            }
        }
        return false;
    case POLYGON12:
        //same as polygon8 but 12 sides
        // reference triangle will have vertices at (0, side/sqrt(3)), (0, side/sqrt(3)+side), and (longside/2, side/sqrt(3)+side/2)
        _constant = std::sqrt(PI / (6*std::sin(30*PI / 180)));
        longside = 2 * _constant * index * std::cos(15 * PI / 180);
        side = 2 * _constant * index * std::cos(75 * PI / 180);
        refHeight = _constant * index * (1-std::cos(75 * PI / 180));
        refTopY = refHeight + side;
        for (int i = 0; i < 12; i++)
        {
            double angle = -i * 30.0 * PI / 180.0;
            rotatedX = (x - longside / 2) * std::cos(angle) - (y - (refHeight + side / 2)) * std::sin(angle) + longside / 2;
            rotatedY = (x - longside / 2) * std::sin(angle) + (y - (refHeight + side / 2)) * std::cos(angle) + (refHeight + side / 2);
            if (rotatedX >= 0 && rotatedX <= longside / 2)
            {
                double slope = std::tan(15.0 * PI / 180.0);
                double lineY_upper = refTopY - slope * rotatedX;
                double lineY_lower = refHeight + slope * rotatedX;
                if (rotatedY >= lineY_lower && rotatedY <= lineY_upper)
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
    case CIRCLE:
        xLength = 2 * index;
        yLength = 2 * index;
        break;
    case RECTANGLE:
        yLength = std::sqrt(PI / 3) * index;
        xLength = 3 * yLength;
        break;
    case RECTANGLE_4_1:
        yLength = std::sqrt(PI / 4) * index;
        xLength = 4 * yLength;
        break;
    case SQUARE:
        xLength = std::sqrt(PI) * index;
        yLength = xLength;
        break;
    case TRIANGLE:
        xLength = std::sqrt(4 * PI / std::sqrt(3)) * index;
        yLength = std::sqrt(3) * xLength / 2;
        break;
    case SHARP_TRIANGLE:
        xLength = std::sqrt(4 * PI / std::tan(75 * PI / 180)) * index;
        yLength = std::tan(75 * PI / 180) * xLength / 2;
        break;
    case FLAT_TRIANGLE:
        xLength = std::sqrt(4 * PI / std::tan(40 * PI / 180)) * index;
        yLength = std::tan(40 * PI / 180) * xLength / 2;
        break;
    case TRAPEZOID:
        xLength = 2 * std::sqrt(PI / 2) * index;
        yLength = std::sqrt(PI / 2) * index;
        break;
    case HEXAGON:
        xLength = 3 * std::sqrt(2*PI / (3 * std::sqrt(3))) * index;
        yLength = std::sqrt(3) * xLength / 2;
        break;
    case POLYGON8:
        {
            double _constant = std::sqrt(PI / (4*std::sin(45*PI / 180)));
            double longside = 2 * _constant * index * std::cos(22.5 * PI / 180);
            xLength = longside;
            yLength = longside;
        }
        break;
    case POLYGON10:
        {
            double _constant = std::sqrt(PI / (5*std::sin(36*PI / 180)));
            double longside = 2 * _constant * index * std::cos(18 * PI / 180);
            xLength = longside;
            yLength = 2 * _constant * index;
        }
        break;
    case POLYGON12:
        {
            double _constant = std::sqrt(PI / (6*std::sin(30*PI / 180)));
            double longside = 2 * _constant * index * std::cos(15 * PI / 180);
            xLength = longside;
            yLength = longside;
        }
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
    case RECTANGLE_4_1:
        do
        {
            std::uniform_int_distribution<> distX(0, 4 * std::sqrt(PI / 4) * index + 1);
            std::uniform_int_distribution<> distY(0, std::sqrt(PI / 4) * index + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, RECTANGLE_4_1));
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
    case SHARP_TRIANGLE:
        do
        {
            std::uniform_int_distribution<> distX(0, std::sqrt(4 * PI / std::tan(75 * PI / 180)) * index + 1);
            std::uniform_int_distribution<> distY(0, std::tan(75 * PI / 180) * std::sqrt(4 * PI / std::tan(75 * PI / 180)) * index / 2 + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, SHARP_TRIANGLE));
        break;
    case FLAT_TRIANGLE:
        do 
        {
            std::uniform_int_distribution<> distX(0, std::sqrt(4 * PI / std::tan(40 * PI / 180)) * index + 1);
            std::uniform_int_distribution<> distY(0, std::tan(40 * PI / 180) * std::sqrt(4 * PI / std::tan(40 * PI / 180)) * index / 2 + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, FLAT_TRIANGLE));
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
    case HEXAGON:
        do
        {
            double _longside = 2 * std::sqrt(2*PI / (3 * std::sqrt(3))) * index;
            std::uniform_int_distribution<> distX(0, _longside + 1);
            std::uniform_int_distribution<> distY(0, std::sqrt(3) * _longside / 2 + 1);
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, HEXAGON));
        break;
    case POLYGON8:
        {
            double _constant = std::sqrt(std::sqrt(2) * PI / 4);
            double longside = 2 * _constant * index * std::cos(22.5 * PI / 180);
            do
            {
                std::uniform_int_distribution<> distX(0, longside + 1);
                std::uniform_int_distribution<> distY(0, longside + 1);
                machine.x = distX(gen);
                machine.y = distY(gen);
            } while (!inBoundary(machine.x, machine.y, index, POLYGON8));
        }
        break;
    case POLYGON10:
        {
            double _constant = std::sqrt(PI / (5*std::sin(36*PI / 180)));
            double longside = 2 * _constant * index * std::cos(18 * PI / 180);
            double height = 2 * _constant * index;
            do
            {
                std::uniform_int_distribution<> distX(0, longside + 1);
                std::uniform_int_distribution<> distY(0, height + 1);
                machine.x = distX(gen);
                machine.y = distY(gen);
            } while (!inBoundary(machine.x, machine.y, index, POLYGON10));
        }
        break;
    case POLYGON12:
        {
            double _constant = std::sqrt(PI / (6*std::sin(30*PI / 180)));
            double longside = 2 * _constant * index * std::cos(15 * PI / 180);
            do
            {
                std::uniform_int_distribution<> distX(0, longside + 1);
                std::uniform_int_distribution<> distY(0, longside + 1);
                machine.x = distX(gen);
                machine.y = distY(gen);
            } while (!inBoundary(machine.x, machine.y, index, POLYGON12));
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
    const int REDUCED_HASHSIZE = hashsize; // Use smaller fixed size
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

mytuple getNextPosition(mytuple position, int pointsOUT, int index, ShapeType shape, WalkContext &ctx)
{
    // If hard boundary is enabled, restrict moves to neighbors that are inside the shape.
    if (hard_boundary)
    {
        std::vector<size_t> allowed;
        for (size_t dirIndex = 0; dirIndex < fourdirs.size(); ++dirIndex)
        {
            mytuple nextPos(position.x + fourdirs[dirIndex].x, position.y + fourdirs[dirIndex].y);
            if (inBoundary(nextPos.x, nextPos.y, index, shape))
                allowed.push_back(dirIndex);
        }

        if (!allowed.empty())
        {
            // choose uniformly among allowed neighbors
            std::uniform_int_distribution<size_t> uniformDist(0, allowed.size() - 1);
            size_t idx = allowed[uniformDist(ctx.rng)];
            return mytuple(position.x + fourdirs[idx].x, position.y + fourdirs[idx].y);
        }
        else
        {
            // No allowed neighbor — stay in place to avoid leaving boundary
            std::cerr << "Warning: No allowed moves from position (" << position.x << ", " << position.y << "). Staying in place." << std::endl;
            return position;
        }
    }

    // get distances for each direction
    std::vector<short> distances;
    bool allInside = true; // Track if all next positions are inside

    double temperature = initemperature + (finaltemperature - initemperature) * std::exp(-0.05*static_cast<double>(pointsOUT));

    // First check all possible next positions and compute their distances
    for (Direction dirIndex = UP; dirIndex <= LEFT; dirIndex = static_cast<Direction>(dirIndex + 1))
    {
        mytuple NextPos(position.x + fourdirs[dirIndex].x, position.y + fourdirs[dirIndex].y);
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
        std::uniform_int_distribution<size_t> uniformDist(0, 3);
        size_t randomIndex = uniformDist(ctx.rng);
        return mytuple(position.x + fourdirs[randomIndex].x,
                       position.y + fourdirs[randomIndex].y);
    }

    // Otherwise use weighted choice based on distances
    std::vector<double> weights;
    for (short dist : distances)
    {
        double w = std::exp(-1*std::pow(static_cast<double>(dist), DISTANCE_POWER) / temperature);
        weights.push_back(w);
    }

    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
    size_t chosenIndex = dist(ctx.rng);
    return mytuple(position.x + fourdirs[chosenIndex].x,
                   position.y + fourdirs[chosenIndex].y);
}

// walk now optionally records the path taken in out_path (if non-null)
int walk(int index, ShapeType shape, WalkContext &ctx, std::vector<mytuple> *out_path)
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
            } else {
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
