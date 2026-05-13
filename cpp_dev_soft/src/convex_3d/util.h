#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <mutex>
#include <random>
#include "myhash.h"

// Global variables
extern const std::vector<mytuple_3d> sixdirs;
extern const double PI;
extern double DISTANCE_POWER;
extern double initemperature;
extern double finaltemperature;
extern bool hard_boundary;

// Enums
enum ShapeType
{
    SPHERE,
    OCTAHEDRON, 
    CUBE,
    RECTANGULAR_PRISM_1_2_3,
    PYRAMID,
    CYLINDER
};

// define shapes names for file output mapping from ShapeType to std::string
extern const std::vector<std::string> shapeNames;


enum Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
    INWARD,
    OUTWARD
};

// Structures
struct WalkContext
{
    std::mt19937_64 rng;
    WalkContext() = default;   // ❌ no seeding here
    // last cover count recorded by the most recent call to `walk` using this context
    std::size_t last_cover_count = 0;

};

// Function declarations
void freeHashmap(node **hashmap, int HashSize);
bool inBoundary(int x, int y, int z, double index, ShapeType shape);
int getPointsInShape(double index, ShapeType shape, bool ****_3DMask, int *xMax, int *yMax, int *zMax, int *xMin, int *yMin, int *zMin);
void initialize(mytuple_3d &machine, int &uniquePoints, int &totalPoints, ShapeType shape, double index, WalkContext &ctx);
short getDistance(int hashsize, mytuple_3d position, double index, ShapeType shape);
mytuple_3d getNextPosition(mytuple_3d position, int pointsOUT, double index, ShapeType shape, WalkContext &ctx);
int walk(double index, ShapeType shape, WalkContext &ctx, std::vector<mytuple_3d> *out_path = nullptr);
bool resultExists(const std::string &filename, double index, int numThreads);
mytuple_3d findCenter(double index, ShapeType shape);

int getCoveringNumber();
void getShapeBoundaries(ShapeType shape, double index, std::vector<double> &xCorners, std::vector<double> &yCorners);

#endif // UTIL_H
