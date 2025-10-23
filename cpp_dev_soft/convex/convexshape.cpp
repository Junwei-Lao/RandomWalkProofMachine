#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <thread>
#include <random>
#include <utility>
#include <stdexcept>
#include "myhash.h"

const int temperature = 1;

const std::vector<tuple> fourdirs = {tuple(0, 1),tuple(1, 0), tuple(0, -1), tuple(-1, 0)};
const double PI = std::acos(-1);
enum ShapeType {
    CIRCLE,
    RECTANGLE,
    SQUARE,
    TRIANGLE,
    TRAPEZOID
};
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};


tuple getNextPosition(tuple position, tuple** initialHashmap, int index, ShapeType shape) {
    //get weight for each direction
    std::vector<double> weights;
    std::vector<short> distances;
    for (Direction dirIndex = UP; dirIndex <= LEFT; dirIndex = static_cast<Direction>(dirIndex + 1)) {
        tuple NextPos(position.x + fourdirs[dirIndex].x, position.y + fourdirs[dirIndex].y);
        short distance;
        try{
            distance = getDistance(1024, NextPos, dirIndex, index, shape);
        } catch (std::runtime_error& e) {
            std::cerr << "Error in getDistance: " << e.what() << std::endl;
            throw;
        }
        distances.push_back(distance);
    }

    std::cout << "Distances: " << distances[0] << ", " << distances[1] << ", " << distances[2] << ", " << distances[3] << std::endl;

    for (short dist : distances) {
        double w = std::exp(-static_cast<double>(dist) / temperature);
        weights.push_back(w);
    }
    
    //choose direction based on weights
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());

    size_t chosenIndex = dist(rng);
    tuple dir = fourdirs[chosenIndex];

    std::cout << "Chosen direction: " << dir.x << ", " << dir.y << std::endl;

    return tuple(position.x + dir.x, position.y + dir.y);
}


//get the distannce four four next candidate directions
short getDistance(int hashsize, tuple position, Direction dirIndex, int index, ShapeType shape) {
    short distance = 0;
    std::queue<std::pair<tuple, short>> bfsQueue;
    node** hashmap = new node*[hashsize]();

    if (!Find_And_Insert_Node(hashmap, position, hashsize)) {
        throw std::runtime_error("Initial position already in hashmap in getDistance!");
    }

    if (inBoundary(position.x, position.y, index, shape)) {
        return distance;
    }
    bfsQueue.push(std::make_pair(position, distance));

    while(!bfsQueue.empty()) {
        tuple currentPos = bfsQueue.front().first;
        short distance = bfsQueue.front().second;
        bfsQueue.pop();
        for (const tuple& dir : fourdirs) {
            tuple newPos(currentPos.x + dir.x, currentPos.y + dir.y);
            tuple machineTuple(newPos.x, newPos.y);
            if (inBoundary(newPos.x, newPos.y, index, shape)) {
                delete[] hashmap;
                return distance+1;
            }
            if (Find_And_Insert_Node(hashmap, machineTuple, hashsize)) {
                bfsQueue.push(std::make_pair(newPos, distance+1));
            }
        }
    }

    delete[] hashmap;
    throw std::runtime_error("BFS exhausted all options without finding a boundary!");
}



int getPointsInShape(int index, ShapeType shape) {
    int points = 0;
    double xLength;
    switch (shape) {
        case CIRCLE:
            xLength = 2*index;
            break;
        case RECTANGLE:
            xLength = 3*std::sqrt(PI / 3)*index;
            break;
        case SQUARE:
            xLength = std::sqrt(PI)*index;
            break;
        case TRIANGLE:
            xLength = std::sqrt(4*PI / std::sqrt(3))*index;
            break;
        case TRAPEZOID:
            xLength = 3*std::sqrt(PI / 2)*index;
            break;
        default:
            std::cerr << "Invalid shape type!" << std::endl;
            throw std::invalid_argument("Invalid shape type from getPointsInShape");
    }

    for (int i = 0; i <= xLength; i++) {
        int y = -2;
        while (!inBoundary(i, y, index, shape)) {
            y += 1;
        }
        while (inBoundary(i, y, index, shape)) {
            points += 1;
            y += 1;
        }
    }

    return points;
}

void initialize(tuple& machine, int& uniquePoints, int& totalPoints, ShapeType shape, int index) {
    std::random_device rd;
    std::mt19937 gen(rd());
    switch (shape) {
        case CIRCLE:
            do {
                std::uniform_int_distribution<> distX(0, index*index + 1);
                std::uniform_int_distribution<> distY(0, index*index + 1);
                machine.x = distX(gen);
                machine.y = distY(gen);
            } while (!inBoundary(machine.x, machine.y, index, CIRCLE));
            break;
        case RECTANGLE:
            do {
                std::uniform_int_distribution<> distX(0, 3*std::sqrt(PI / 3)*index + 1);
                std::uniform_int_distribution<> distY(0, std::sqrt(PI / 3)*index + 1);
                machine.x = distX(gen);
                machine.y = distY(gen);
            } while (!inBoundary(machine.x, machine.y, index, RECTANGLE));
            break;
        case SQUARE:
            do {
                std::uniform_int_distribution<> distX(0, std::sqrt(PI)*index + 1);
                std::uniform_int_distribution<> distY(0, std::sqrt(PI)*index + 1);
                machine.x = distX(gen);
                machine.y = distY(gen);
            } while (!inBoundary(machine.x, machine.y, index, SQUARE));
            break;
        case TRIANGLE:
            do {
                std::uniform_int_distribution<> distX(0, std::sqrt(4*PI / std::sqrt(3))*index + 1);
                std::uniform_int_distribution<> distY(0, std::sqrt(3)*std::sqrt(4*PI / std::sqrt(3))*index / 2 + 1);
                machine.x = distX(gen);
                machine.y = distY(gen);
            } while (!inBoundary(machine.x, machine.y, index, TRIANGLE));
            break;
        case TRAPEZOID:
            do {
                std::uniform_int_distribution<> distX(0, 3*std::sqrt(PI / 2)*index + 1);
                std::uniform_int_distribution<> distY(0, std::sqrt(PI / 2)*index + 1);
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

bool inBoundary(int x, int y, int index, ShapeType shape) {
    switch (shape) {
        case CIRCLE:
            return ((x*x - index*index) + (y*y - index*index)) <= index*index; // Unit circle
        case RECTANGLE:
            double _constant = std::sqrt(PI / 3);
            return (x >= 0 && x <= 3*_constant*index && y >= 0 && y <= _constant*index); 
        case SQUARE:
            double _constant = std::sqrt(PI);
            return (x >= 0 && x <= _constant*index && y >= 0 && y <= _constant*index); // Unit square
        case TRIANGLE:
            double _constant = std::sqrt(4*PI / std::sqrt(3));
            if (x >= 0 && x < _constant*index/2) {
                return (y >= 0 && y <= std::sqrt(3)*x);
            } else if (x >= _constant*index/2 && x <= _constant*index) {
                return (y >= 0 && y <= -std::sqrt(3)*x + std::sqrt(3)*_constant*index);
            }
            return false;
        case TRAPEZOID:
            double _constant = std::sqrt(PI / 2);
            if (x >= 0 && x < _constant*index) {
                return (y >= 0 && y <= x);
            } else if (x >= _constant*index && x < 2*_constant*index) {
                return (y >= 0 && y <= _constant*index);
            } else if (x >= 2*_constant*index && x <= 3*_constant*index) {
                return (y >= 0 && y <= -x + 3*_constant*index);
            }
            return false;
        default:
            std::cerr << "Invalid shape type!" << std::endl;
            throw std::invalid_argument("Invalid shape type from inBoundary");
    }
    return true;
}

int walk(int index, ShapeType shape) {
    tuple machine(0, 0);
    int uniquePoints = 0;
    int totalPoints = 0;
    initialize(machine, uniquePoints, totalPoints, shape, index);

    int shapePoints = getPointsInShape(index, shape);
    int hashSize = shapePoints / 2;
    node** hashmap = new node*[hashSize]();

    int totalHalfPoints = shapePoints / 2;

    while (totalPoints < totalHalfPoints) {
        machine = getNextPosition(machine, nullptr, index, shape);
        totalPoints += 1;
        tuple machineTuple(machine.x, machine.y);
        if (Find_And_Insert_Node(hashmap, machineTuple, hashSize)) {
            uniquePoints += 1;
        }
    }
    
    return totalPoints;
}

int main() {
    std::cout << "Hello, Convex Shape!" << std::endl;
    return 0;
}