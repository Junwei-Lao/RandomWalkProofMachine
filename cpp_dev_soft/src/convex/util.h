#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <mutex>
#include <random>
#include "myhash.h"

// Global variables
extern const std::vector<mytuple> fourdirs;
extern const double PI;
extern double DISTANCE_POWER;
extern double initemperature;
extern double finaltemperature;
extern bool hard_boundary;

// Enums
enum ShapeType
{
    CIRCLE,
    RECTANGLE,
    RECTANGLE_4_1,
    SQUARE,
    TRIANGLE,
    SHARP_TRIANGLE, // top will be 30 degrees and two bottom corners 75 degrees
    FLAT_TRIANGLE,  // top will be 100 degrees and two bottom corners 40 degrees
    HEXAGON,
    TRAPEZOID,
    POLYGON8,
    POLYGON10,
    POLYGON12,
    POLYGON5,
    POLYGON9,
    POLYGON15,
    ROTATED_SQUARE,
    STRETCHED_ROTATED_SQUARE,
    BOWTIE,
    DOUBLE_BOWTIE,
    SLOTTED_RECT_30x10,
    V_NOTCH_RECT,
    SNOWFLAKE
};

// define shapes names for file output mapping from ShapeType to std::string
extern const std::vector<std::string> shapeNames;


enum Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
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
bool inBoundary(int x, int y, int index, ShapeType shape);
int getPointsInShape(int index, ShapeType shape, bool ***_2DMask = nullptr, int *xMax = nullptr, int *yMax = nullptr);
void initialize(mytuple &machine, int &uniquePoints, int &totalPoints, ShapeType shape, int index, WalkContext &ctx);
short getDistance(int hashsize, mytuple position, int index, ShapeType shape);
mytuple getNextPosition(mytuple position, int pointsOUT, int index, ShapeType shape, WalkContext &ctx);
int walk(int index, ShapeType shape, WalkContext &ctx, std::vector<mytuple> *out_path = nullptr);
bool resultExists(const std::string &filename, int index, int numThreads);
mytuple findCenter(int index, ShapeType shape);

int getCoveringNumber();
void getShapeBoundaries(ShapeType shape, int index, std::vector<double> &xCorners, std::vector<double> &yCorners);

#endif // UTIL_H
