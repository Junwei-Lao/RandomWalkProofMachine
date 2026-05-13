#ifndef COVER_H
#define COVER_H

#include <unordered_set>
#include <vector>
#include <utility>
#include <string>
#include <cstdint>
#include <random>

// integer lattice point
struct IntPoint
{
    int x;
    int y;

    bool operator==(const IntPoint &other) const
    {
        return x == other.x && y == other.y;
    }
};

// hash for IntPoint
struct IntPointHash
{
    std::size_t operator()(const IntPoint &p) const noexcept
    {
        // 64-bit mix
        return (static_cast<std::size_t>(static_cast<uint32_t>(p.x)) << 32) ^ static_cast<std::size_t>(static_cast<uint32_t>(p.y));
    }
};

// disk center (double precision)
struct DiskCenter
{
    double x;
    double y;
};

class DiskCover
{
public:
    explicit DiskCover(double radius = 1.0);

    // returns true if (x,y) is already covered
    bool isCovered(int x, int y) const;

    // ensure coverage for (x,y); creates a disk if needed
    // returns true if a new disk was added
    bool coverIfNeededRandom(int x, int y, std::mt19937_64 &rng);

    // number of disks
    std::size_t diskCount() const;

    // print disk centers to file
    void writeDisks(const std::string &baseName, const std::string &shapeName) const;

    double radius() const { return r; }

private:
    double r;
    double r2;

    // covered lattice points
    std::unordered_set<IntPoint, IntPointHash> covered;

    // disk centers
    std::vector<DiskCenter> centers;

    // insert lattice points covered by disk centered at (cx, cy)
    void insertDisk(double cx, double cy);
};

#endif // COVER_H
