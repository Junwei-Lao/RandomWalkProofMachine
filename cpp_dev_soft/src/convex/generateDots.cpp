#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <sstream>


#include "util.h"

int main(int argc, char *argv[])
{
    // minimal arg parsing, keep existing three args for compatibility
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " DISTANCE_POWER initemperature finaltemperature [index]\n";
        return 1;
    }

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

    // optional index argument (size parameter), default to 10
    int index = 100;

    // Compute bounding extents for HEXAGON (same formulas as util.cpp)
    double xLength, yLength;
    
    double _constant = std::sqrt(PI / (5*std::sin(36*PI / 180)));
    double longside = 2 * _constant * index * std::cos(18 * PI / 180);


    xLength = std::sqrt(4 * PI / std::tan(40 * PI / 180)) * index;
    yLength = std::tan(40 * PI / 180) * xLength / 2;

    // choose integer bounds similar to getPointsInShape usage
    int xMax = static_cast<int>(std::ceil(xLength)) + 1;
    int yMax = static_cast<int>(std::ceil(yLength)) + 1;

    std::ostringstream fname;
    fname << "flattir_points_" << index << ".txt";
    std::ofstream out(fname.str());
    if (!out)
    {
        std::cerr << "Failed to open output file: " << fname.str() << "\n";
        return 1;
    }

    // header (optional)
    //out << "x y\n";

    // loop over bounding box and test inBoundary for HEXAGON
    for (int x = -10; x <= xMax; ++x)
    {
        for (int y = -10; y <= yMax; ++y)
        {
            if (inBoundary(x, y, index, FLAT_TRIANGLE))
            {
                out << x << " " << y << "\n";
            }
        }
    }

    out.close();
    std::cout << "Wrote sharptriangle points to " << fname.str() << " (index=" << index << ")\n";
    return 0;
}