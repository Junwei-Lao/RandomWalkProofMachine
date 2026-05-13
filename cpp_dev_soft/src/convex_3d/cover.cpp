#include "cover.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <random>
#include <unordered_set>

static short INDEX = 100;

DiskCover::DiskCover(double radius)
    : r(radius), r2(radius * radius)
{
}

bool DiskCover::isCovered(int x, int y) const
{
    return covered.find({x, y}) != covered.end();
}

bool DiskCover::coverIfNeededRandom(int x, int y, std::mt19937_64 &rng)
{
    if (isCovered(x, y))
    {
        return false;
    }

    // uniform disk sampling
    std::uniform_real_distribution<double> uni01(0.0, 1.0);
    std::uniform_real_distribution<double> uniAngle(0.0, 2.0 * M_PI);

    double theta = uniAngle(rng);
    double rho = r * std::sqrt(uni01(rng));

    double cx = static_cast<double>(x) + rho * std::cos(theta);
    double cy = static_cast<double>(y) + rho * std::sin(theta);

    centers.push_back({cx, cy});
    insertDisk(cx, cy);

    return true;
}

void DiskCover::insertDisk(double cx, double cy)
{
    // bounding box for lattice enumeration
    int xmin = static_cast<int>(std::floor(cx - r));
    int xmax = static_cast<int>(std::ceil(cx + r));
    int ymin = static_cast<int>(std::floor(cy - r));
    int ymax = static_cast<int>(std::ceil(cy + r));

    for (int x = xmin; x <= xmax; ++x)
    {
        for (int y = ymin; y <= ymax; ++y)
        {
            double dx = static_cast<double>(x) - cx;
            double dy = static_cast<double>(y) - cy;
            if (dx * dx + dy * dy <= r2)
            {
                covered.insert({x, y});
            }
        }
    }
}

std::size_t DiskCover::diskCount() const
{
    return centers.size();
}

void DiskCover::writeDisks(const std::string &baseName, const std::string &shapeName) const
{
    std::ostringstream fname;
    fname << baseName << "_" << shapeName << "_" << INDEX << ".txt";

    std::ofstream out(fname.str());
    if (!out)
        return;

    out << std::fixed << std::setprecision(3);
    out << "# radius = " << r << "\n";
    out << "# index = " << INDEX << "\n";
    out << "# x y\n";

    for (const auto &c : centers)
    {
        out << c.x << " " << c.y << "\n";
    }

    INDEX++;
}
