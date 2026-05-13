#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <sstream>

#include "util.h"

bool isShellVoxel(bool*** mask, int i, int j, int k, int sx, int sy, int sz)
{
    if (!mask[i][j][k]) return false;

    const int dirs[6][3] = {
        { 1, 0, 0}, {-1, 0, 0},
        { 0, 1, 0}, { 0,-1, 0},
        { 0, 0, 1}, { 0, 0,-1}
    };

    for (int d = 0; d < 6; d++)
    {
        int ni = i + dirs[d][0];
        int nj = j + dirs[d][1];
        int nk = k + dirs[d][2];

        // If neighbor is outside the array, current voxel is on shell
        if (ni < 0 || ni >= sx || nj < 0 || nj >= sy || nk < 0 || nk >= sz)
            return true;

        // If neighbor is outside the shape, current voxel is on shell
        if (!mask[ni][nj][nk])
            return true;
    }

    return false;
}


int main(int argc, char *argv[])
{
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
    int index = 20;

    bool ***_mask = nullptr;

    int xMax, yMax, zMax, xMin, yMin, zMin;

    int points = getPointsInShape(index, CYLINDER, &_mask, &xMax, &yMax, &zMax, &xMin, &yMin, &zMin);

    std::ostringstream fname;
    fname << "mask_points_" << index << ".txt";
    std::ofstream out(fname.str());
    if (!out)
    {
        std::cerr << "Failed to open output file: " << fname.str() << "\n";
        return 1;
    }

    int sx = xMax - xMin;
    int sy = yMax - yMin;
    int sz = zMax - zMin;

    try
    {
        for (int x = xMin; x < xMax; x++)
        {
            for (int y = yMin; y < yMax; y++)
            {
                for (int z = zMin; z < zMax; z++)
                {
                    int i = x - xMin;
                    int j = y - yMin;
                    int k = z - zMin;

                    if (isShellVoxel(_mask, i, j, k, sx, sy, sz))
                    {
                        out << x << " " << y << " " << z << "\n";
                    }
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "In generateDots traverse" << '\n';
    }

    out.close();
    std::cout << "Wrote mask points to " << fname.str() << " (index=" << index << ")\n";

    return 0;
}