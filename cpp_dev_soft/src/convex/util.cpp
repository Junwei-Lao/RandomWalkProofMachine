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
#include "cover.h"

#define isCrossBoundary true

const std::vector<mytuple> fourdirs = {mytuple(-1, 0), mytuple(1, 0), mytuple(0, -1), mytuple(0, 1)};
const double PI = std::acos(-1);

const std::vector<std::string> shapeNames = {
    "CIRCLE",
    "RECTANGLE",
    "RECTANGLE_4_1",
    "SQUARE",
    "TRIANGLE",
    "SHARP_TRIANGLE", // top will be 30 degrees and two bottom corners 75 degrees
    "FLAT_TRIANGLE",  // top will be 100 degrees and two bottom corners 40 degrees
    "HEXAGON",
    "TRAPEZOID",
    "POLYGON8",
    "POLYGON10",
    "POLYGON12",
    "POLYGON5",
    "POLYGON9",
    "POLYGON15",
    "ROTATED_SQUARE",
    "STRETCHED_ROTATED_SQUARE",
    "BOWTIE",
    "DOUBLE_BOWTIE",
    "SLOTTED_RECT_30x10",
    "V_NOTCH_RECT",
    "SNOWFLAKE"
};




double DISTANCE_POWER;
double initemperature;
double finaltemperature;
bool hard_boundary = true;
bool make_cover = false;
const double reference_radius = 0.007; //0.007，0.008，0.009，0.01，0.02, 0.03

#ifdef isCrossBoundary
    const double drafting_constant = 10000.0;
#endif


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

static bool inRegularPolygon(int x, int y, int index, int N)
{
    // circumradius from area normalization
    double R = index * std::sqrt((2.0 * PI) /
                                 (N * std::sin(2.0 * PI / N)));

    // apothem (distance to each edge)
    double a = R * std::cos(PI / N);

    // translate grid so center is at origin
    constexpr double MARGIN = 20.0;
    double cx = R + MARGIN;
    double cy = R + MARGIN;
    double dx = x - cx;
    double dy = y - cy;

    double r = std::sqrt(dx * dx + dy * dy);
    if (r > R) return false;   // quick reject

    double theta = std::atan2(dy, dx);

    // fold angle into symmetric sector
    double sector = 2.0 * PI / N;
    theta = std::fmod(theta + PI, sector);
    if (theta < 0) theta += sector;
    theta -= sector / 2.0;

    // half-plane test against polygon edge
    return r * std::cos(theta) <= a;
}

static double slottedRect30x10_a(int index)
{
    // Outer: 30a x 10a
    // Slot: thickness 0.1a, length 29a, starts at x = 1a (so ends at x = 30a)
    constexpr double WU = 60.0;
    constexpr double HU = 10.0;
    constexpr double slot_len_u = 59.0;
    constexpr double slot_t_u   = 0.5;

    const double outer_area_u = WU * HU;                 // 1200
    const double slot_area_u  = slot_len_u * slot_t_u;   // 29.5
    const double area_u       = outer_area_u - slot_area_u; // 1170.5
    // (area_u * a^2) = PI * index^2
    return static_cast<double>(index) * std::sqrt(PI / area_u);
}

static void slottedRect30x10_dims(int index,
                                 double &W, double &H,
                                 double &sx0, double &sx1,
                                 double &sy0, double &sy1)
{
    constexpr double WU = 60.0;
    constexpr double HU = 10.0;
    constexpr double slot_left_gap_u = 1.0; // 1a
    constexpr double slot_len_u      = 59.0;
    constexpr double slot_t_u        = 0.5;

    const double a = slottedRect30x10_a(index);

    W = WU * a;
    H = HU * a;

    sx0 = slot_left_gap_u * a;
    sx1 = (slot_left_gap_u + slot_len_u) * a;

    const double t = slot_t_u * a;
    const double cy = H / 2.0;
    sy0 = cy - t / 2.0;
    sy1 = cy + t / 2.0;
}

mytuple findCenter(int index, ShapeType shape)
{
    double cx, cy;
    switch (shape)
    {
    case CIRCLE:
        cx = index;
        cy = index;
        break;
    case SQUARE:
        cx = 0.5 * std::sqrt(PI) * index;
        cy = 0.5 * std::sqrt(PI) * index;
        break;
    default:
        // For other shapes, we can use the bounding box center as an approximation.
        // This is not exact for all shapes but should be close enough for our purposes.
        cx = index; // Placeholder, can be refined based on shape geometry
        cy = index; // Placeholder, can be refined based on shape geometry
        break;
    }
    return mytuple(cx, cy);
}


bool inBoundary(int x, int y, int index, ShapeType shape)
{
    double _constant;
    double longside;

    double rotatedX, rotatedY;

    // For stretched rotated square
    double xlength, ylength;

    // For bowtie and double bowtie
    double width, length, hyposide, Height, Long;

    switch (shape)
    {
    case CIRCLE:
        return ((x - index) * (x - index) + (y - index) * (y - index)) < index * index;
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
            return (y >= 0 && y <= _constant * index / std::tan(15 * PI / 180) - std::tan(75 * PI / 180) * x);
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
            return (y >= 0 && y <= _constant * index * std::tan(40 * PI / 180) - std::tan(40 * PI / 180) * x);
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

    case SLOTTED_RECT_30x10:
    {
        double W, H, sx0, sx1, sy0, sy1;
        slottedRect30x10_dims(index, W, H, sx0, sx1, sy0, sy1);

        // inside outer rectangle
        if (!(x >= 0 && x <= W && y >= 0 && y <= H)) return false;

        // hole (slot) removed
        const bool inSlot = (x >= sx0 && x <= sx1 && y >= sy0 && y <= sy1);
        return !inSlot;
    }

    case HEXAGON:
        return inRegularPolygon(x, y, index, 6);

    case POLYGON5:
        return inRegularPolygon(x, y, index, 5);

    case POLYGON8:
        return inRegularPolygon(x, y, index, 8);

    case POLYGON9:
        return inRegularPolygon(x, y, index, 9);

    case POLYGON10:
        return inRegularPolygon(x, y, index, 10);

    case POLYGON12:
        return inRegularPolygon(x, y, index, 12);

    case POLYGON15:
        return inRegularPolygon(x, y, index, 15);

    case ROTATED_SQUARE:
        // 45 degree rotated square
        // rotation center at (0, original_longside)
        _constant = std::sqrt(PI);
        longside = _constant * index;
        rotatedX = (x) * std::cos(-45.0 * PI / 180.0) - (y - longside) * std::sin(-45.0 * PI / 180.0);
        rotatedY = (x) * std::sin(-45.0 * PI / 180.0) + (y - longside) * std::cos(-45.0 * PI / 180.0) + longside;
        return (rotatedX >= 0 && rotatedX <= longside && rotatedY >= 0 && rotatedY <= longside);
    case STRETCHED_ROTATED_SQUARE:
        // this shape was originally designed to be stretched the rotated square, but using two triangles to implement will be more efficient
        // first triangle: vertices at (0,ylength/2), (xLength/2,0), (xLength/2,yLength/2)
        // second triangle: (xLength/2,0), (xLength/2,yLength/2), (xlength, yLength/2)
        // xlength = 3*ylength
        _constant = std::sqrt(2*PI/3);
        ylength = _constant * index;
        xlength = 3*ylength;
        if (x >= 0 && x <= xlength/2) {
            if ((y >= (ylength/2.0 - x/3.0)) && (y <= (ylength/2.0 + x/3.0))) {
                return true;
            }
        } 
        else if (x > xlength/2 && x <= xlength) {
            if (y >= (ylength/2.0) - ((xlength-x)/3.0) && y <= (ylength/2.0) + ((xlength-x)/3.0)) {
                return true;
            }
        }
        return false;
    case BOWTIE:
        width = std::sqrt(PI / 45) * index;
        length = 5 * width;
        hyposide = 4 * width;
        Height = width + 2 * hyposide;
        Long = length + 2 * hyposide;
        if (x >= 0 && x <= hyposide)
        {
            if (y >= x and y <= Height - x)
            {
                return true;
            }
        }
        else if (x > hyposide and x <= hyposide + length)
        {
            if (y >= hyposide and y <= hyposide + width)
            {
                return true;
            }
        }
        else if (x > hyposide + length and x <= Long)
        {
            if (y >= Long - x and y <= x + Height - Long)
            {
                return true;
            }
        }
        return false;
    case V_NOTCH_RECT:
    {
        // Area normalization consistent with your other shapes:
        // Area = (4a * 6a) - (1/2 * 4a * 5a) = 24a^2 - 10a^2 = 14a^2
        // Set 14a^2 = PI * index^2  =>  a = index * sqrt(PI / 14)
        const double a = static_cast<double>(index) * std::sqrt(PI / 14.0);

        const double W = 4.0 * a;
        const double H = 6.0 * a;

        // Must be inside the outer rectangle first
        if (x < 0 || x > W || y < 0 || y > H) return false;

        // V-notch boundary lines:
        // Left edge: through (0, 6a) and (2a, a):
        //   slope = (a - 6a) / (2a - 0) = -5/2
        //   y_left(x) = 6a - (5/2) x
        //
        // Right edge: through (4a, 6a) and (2a, a):
        //   slope = (a - 6a) / (2a - 4a) = +5/2
        //   y_right(x) = (5/2) x - 4a
        //
        // The removed region is ABOVE these lines (up to y=6a).
        // So we keep points that are BELOW the V boundary.

        const double xApex = 2.0 * a;

        if (x <= xApex)
        {
            const double y_boundary = 6.0 * a - 2.5 * x;
            return (y <= y_boundary);
        }
        else
        {
            const double y_boundary = 2.5 * x - 4.0 * a;
            return (y <= y_boundary);
        }
    }
    case DOUBLE_BOWTIE:
    {
        // Geometry:
        // Union of:
        //   - horizontal bowtie (same as BOWTIE) shifted upward by 2a
        //   - vertical bowtie (same equations with x/y swapped) shifted rightward by 2a
        //
        // Dimensions in terms of a:
        //   bowtie width (central thickness) = a
        //   central length = 5a
        //   triangle length = 4a
        //
        // Bounding box becomes [0, 13a] x [0, 13a] with this placement.

        const double a = std::sqrt(PI / 89.0) * index;   // area-normalized
        const double w = a;
        const double h = 4.0 * a;     // triangle length
        const double L = 5.0 * a;     // central rectangle length
        const double Height = w + 2.0 * h;   // = 9a
        const double Long   = L + 2.0 * h;   // = 13a

        // local helper (kept inside the case as you requested: no external helper functions)
        auto inBowtieH = [&](double px, double py) -> bool
        {
            // Horizontal bowtie in local coords:
            // px in [0, Long], py in [0, Height]
            if (px < 0.0 || px > Long || py < 0.0 || py > Height) return false;

            if (px <= h)
            {
                // left wedge: y in [x, Height - x]
                return (py >= px && py <= Height - px);
            }
            else if (px <= h + L)
            {
                // center rectangle: y in [h, h + w]
                return (py >= h && py <= h + w);
            }
            else
            {
                // right wedge: y in [Long - x, x + Height - Long]
                return (py >= (Long - px) && py <= (px + Height - Long));
            }
        };

        // Place the horizontal bowtie inside the 13a x 13a box by shifting it up by 2a:
        // HB local coords: (x, y - 2a) in [0,13a] x [0,9a]
        const bool inHB = inBowtieH(static_cast<double>(x), static_cast<double>(y) - 2.0 * a);

        // Place the vertical bowtie by swapping axes and shifting right by 2a:
        // VB local coords: (y, (x - 2a)) in [0,13a] x [0,9a]
        const bool inVB = inBowtieH(static_cast<double>(y), static_cast<double>(x) - 2.0 * a);

        return inHB || inVB;
    }
    case SNOWFLAKE:
    {
        // Snowflake = two hub crosses connected by a short bar.
        // At each OUTER endpoint, attach a recursive "+" cross fractal.
        //
        // Key constraint:
        //   At every recursive cross, we ONLY draw/recurse into directions that move AWAY
        //   from the *parent cross center*, NOT "away from the global hub".
        // This prevents "opposite-facing" growth that overlaps the mother branch.

        const double a = std::sqrt(PI / 140.0) * index;

        // -------------------------
        // Geometry knobs (tune here)
        // -------------------------
        const double hubThk      = 1.0 * a;      // hub thickness
        const double hubArm      = 2.0 * a;      // hub arm length
        const double connectorLen = 1.6 * a;     // hubs closer if smaller

        const int    depth    = 3;              // fractal levels
        const double scaleLen = 0.55;           // arm length shrink per level
        const double scaleThk = 0.50;           // thickness shrink per level

        const double endArm0 = 1.9 * a;         // endpoint cross arm length (level 0)
        const double endThk0 = 0.55 * a;        // endpoint cross thickness (level 0)

        // -------------------------
        // Put everything positive
        // -------------------------
        const double W  = 80.0 * a;
        const double cy = 40.0 * a;
        const double cxL = 40.0 * a - (connectorLen / 2.0 + hubArm);
        const double cxR = 40.0 * a + (connectorLen / 2.0 + hubArm);

        const double X = static_cast<double>(x);
        const double Y = static_cast<double>(y);

        // quick reject
        if (X < 0.0 || X > W || Y < 0.0 || Y > W) return false;

        auto inRect = [&](double px, double py,
                        double x0, double x1,
                        double y0, double y1) -> bool
        {
            return (px >= x0 && px <= x1 && py >= y0 && py <= y1);
        };

        // Full "+" cross (used for hubs only)
        auto inCrossFull = [&](double px, double py,
                            double cx, double cy,
                            double arm, double thk) -> bool
        {
            bool ok = inRect(px, py,
                            cx - arm, cx + arm,
                            cy - thk / 2.0, cy + thk / 2.0);
            ok = ok || inRect(px, py,
                            cx - thk / 2.0, cx + thk / 2.0,
                            cy - arm, cy + arm);
            return ok;
        };

        // Outward-only cross relative to PARENT center (px0,py0).
        // Only draw half-arms whose endpoints are further from the parent than the cross center.
        auto inCrossOutward = [&](double px, double py,
                                double cx, double cy,
                                double arm, double thk,
                                double px0, double py0) -> bool
        {
            const double d0 = (cx - px0) * (cx - px0) + (cy - py0) * (cy - py0);
            bool ok = false;

            auto outwardEndpoint = [&](double ex, double ey) -> bool
            {
                const double d1 = (ex - px0) * (ex - px0) + (ey - py0) * (ey - py0);
                return d1 > d0 + 1e-9;
            };

            // +X half-arm
            if (outwardEndpoint(cx + arm, cy))
                ok = ok || inRect(px, py,
                                cx, cx + arm,
                                cy - thk / 2.0, cy + thk / 2.0);

            // -X half-arm
            if (outwardEndpoint(cx - arm, cy))
                ok = ok || inRect(px, py,
                                cx - arm, cx,
                                cy - thk / 2.0, cy + thk / 2.0);

            // +Y half-arm
            if (outwardEndpoint(cx, cy + arm))
                ok = ok || inRect(px, py,
                                cx - thk / 2.0, cx + thk / 2.0,
                                cy, cy + arm);

            // -Y half-arm
            if (outwardEndpoint(cx, cy - arm))
                ok = ok || inRect(px, py,
                                cx - thk / 2.0, cx + thk / 2.0,
                                cy - arm, cy);

            return ok;
        };

        // Recursive outward-only cross fractal.
        // parentx/parenty is the cross center we came from.
        auto inCrossFractalOutward = [&](auto&& self,
                                        double px, double py,
                                        double cx, double cy,
                                        double arm, double thk,
                                        double parentx, double parenty,
                                        int d) -> bool
        {
            if (arm <= 0.0 || thk <= 0.0) return false;

            bool ok = inCrossOutward(px, py, cx, cy, arm, thk, parentx, parenty);
            if (d <= 1) return ok;

            const double d0 = (cx - parentx) * (cx - parentx) + (cy - parenty) * (cy - parenty);

            auto tryRecurse = [&](double nx, double ny)
            {
                const double d1 = (nx - parentx) * (nx - parentx) + (ny - parenty) * (ny - parenty);
                if (d1 > d0 + 1e-9)
                {
                    const double nextArm = arm * scaleLen;
                    const double nextThk = thk * scaleThk;

                    // parent becomes current center (cx,cy)
                    ok = ok || self(self, px, py, nx, ny, nextArm, nextThk, cx, cy, d - 1);
                }
            };

            // recurse from outward endpoints only (relative to parent)
            tryRecurse(cx + arm, cy);
            tryRecurse(cx - arm, cy);
            tryRecurse(cx, cy + arm);
            tryRecurse(cx, cy - arm);

            return ok;
        };

        bool inside = false;

        // connector bar
        inside = inside || inRect(X, Y,
                                cxL + hubArm, cxR - hubArm,
                                cy - hubThk / 2.0, cy + hubThk / 2.0);

        // hub crosses (full)
        inside = inside || inCrossFull(X, Y, cxL, cy, hubArm, hubThk);
        inside = inside || inCrossFull(X, Y, cxR, cy, hubArm, hubThk);

        // attach outward-only snowflakes at OUTER endpoints only
        // Left hub endpoints: left, up, down
        inside = inside || inCrossFractalOutward(inCrossFractalOutward,
                                                X, Y,
                                                cxL - hubArm, cy,
                                                endArm0, endThk0,
                                                cxL, cy,
                                                depth);

        inside = inside || inCrossFractalOutward(inCrossFractalOutward,
                                                X, Y,
                                                cxL, cy + hubArm,
                                                endArm0, endThk0,
                                                cxL, cy,
                                                depth);

        inside = inside || inCrossFractalOutward(inCrossFractalOutward,
                                                X, Y,
                                                cxL, cy - hubArm,
                                                endArm0, endThk0,
                                                cxL, cy,
                                                depth);

        // Right hub endpoints: right, up, down
        inside = inside || inCrossFractalOutward(inCrossFractalOutward,
                                                X, Y,
                                                cxR + hubArm, cy,
                                                endArm0, endThk0,
                                                cxR, cy,
                                                depth);

        inside = inside || inCrossFractalOutward(inCrossFractalOutward,
                                                X, Y,
                                                cxR, cy + hubArm,
                                                endArm0, endThk0,
                                                cxR, cy,
                                                depth);

        inside = inside || inCrossFractalOutward(inCrossFractalOutward,
                                                X, Y,
                                                cxR, cy - hubArm,
                                                endArm0, endThk0,
                                                cxR, cy,
                                                depth);

        return inside;
    }

    default:
        std::cerr << "Invalid shape type!" << std::endl;
        throw std::invalid_argument("Invalid shape type from inBoundary");
    }
    return false;
}

static double regularPolygonCircumradius(int index, int N)
{
    return index * std::sqrt((2.0 * PI) / (N * std::sin(2.0 * PI / N)));
}

int getPointsInShape(int index, ShapeType shape,
                     bool ***_2DMask, int *xMax, int *yMax)
{
    int points = 0;
    double xLength = 0.0;
    double yLength = 0.0;

    constexpr double MARGIN = 20.0;

    switch (shape)
    {
    case CIRCLE:
        xLength = 2.0 * index;
        yLength = 2.0 * index;
        break;

    case RECTANGLE:
    {
        double h = std::sqrt(PI / 3.0) * index;
        xLength = 3.0 * h;
        yLength = h;
    }
    break;

    case RECTANGLE_4_1:
    {
        double h = std::sqrt(PI / 4.0) * index;
        xLength = 4.0 * h;
        yLength = h;
    }
    break;

    case SQUARE:
    {
        double s = std::sqrt(PI) * index;
        xLength = s;
        yLength = s;
    }
    break;

    case TRIANGLE:
    {
        double s = std::sqrt(4.0 * PI / std::sqrt(3.0)) * index;
        xLength = s;
        yLength = std::sqrt(3.0) * s / 2.0;
    }
    break;

    case SHARP_TRIANGLE:
    {
        double s = std::sqrt(4.0 * PI / std::tan(75.0 * PI / 180.0)) * index;
        xLength = s;
        yLength = std::tan(75.0 * PI / 180.0) * s / 2.0;
    }
    break;

    case FLAT_TRIANGLE:
    {
        double s = std::sqrt(4.0 * PI / std::tan(40.0 * PI / 180.0)) * index;
        xLength = s;
        yLength = std::tan(40.0 * PI / 180.0) * s / 2.0;
    }
    break;

    case TRAPEZOID:
    {
        double h = std::sqrt(PI / 2.0) * index;
        xLength = 3.0 * h;
        yLength = h;
    }
    break;

    case SLOTTED_RECT_30x10:
    {
        double W, H, sx0, sx1, sy0, sy1;
        slottedRect30x10_dims(index, W, H, sx0, sx1, sy0, sy1);
        xLength = W;
        yLength = H;
    }
    break;


    case HEXAGON:
    {
        double R = regularPolygonCircumradius(index, 6);
        xLength = 2.0 * R + 2.0 * MARGIN;
        yLength = 2.0 * R + 2.0 * MARGIN;
    }
    break;

    case POLYGON5:
    {
        double R = regularPolygonCircumradius(index, 5);
        xLength = 2.0 * R + 2.0 * MARGIN;
        yLength = 2.0 * R + 2.0 * MARGIN;
    }
    break;

    case POLYGON8:
    {
        double R = regularPolygonCircumradius(index, 8);
        xLength = 2.0 * R + 2.0 * MARGIN;
        yLength = 2.0 * R + 2.0 * MARGIN;
    }
    break;

    case POLYGON9:
    {
        double R = regularPolygonCircumradius(index, 9);
        xLength = 2.0 * R + 2.0 * MARGIN;
        yLength = 2.0 * R + 2.0 * MARGIN;
    }
    break;

    case POLYGON10:
    {
        double R = regularPolygonCircumradius(index, 10);
        xLength = 2.0 * R + 2.0 * MARGIN;
        yLength = 2.0 * R + 2.0 * MARGIN;
    }
    break;

    case POLYGON12:
    {
        double R = regularPolygonCircumradius(index, 12);
        xLength = 2.0 * R + 2.0 * MARGIN;
        yLength = 2.0 * R + 2.0 * MARGIN;
    }
    break;

    case POLYGON15:
    {
        double R = regularPolygonCircumradius(index, 15);
        xLength = 2.0 * R + 2.0 * MARGIN;
        yLength = 2.0 * R + 2.0 * MARGIN;
    }
    break;

    case ROTATED_SQUARE:
    {
        double s = std::sqrt(PI) * index;
        xLength = s * std::sqrt(2.0);
        yLength = s * std::sqrt(2.0);
    }
    break;

    case STRETCHED_ROTATED_SQUARE:
    {
        double h = std::sqrt(2.0 * PI / 3.0) * index;
        xLength = 3.0 * h;
        yLength = h;
    }
    break;

    case BOWTIE:
    {
        double w = std::sqrt(PI / 45.0) * index;
        double h = 2.0 * (4.0 * w) + w;
        double l = 5.0 * w + 2.0 * (4.0 * w);
        xLength = l;
        yLength = h;
    }
    break;

    case DOUBLE_BOWTIE:
    {
        // Bounding box is 13a by 13a with our placement.
        const double a = std::sqrt(PI / 89.0) * index;
        const double Long = 13.0 * a;
        xLength = Long;
        yLength = Long;
    }
    break;

    case V_NOTCH_RECT:
    {
        const double a = static_cast<double>(index) * std::sqrt(PI / 14.0);
        xLength = 4.0 * a;
        yLength = 6.0 * a;
    }
    break;

    case SNOWFLAKE:
    {
        const double a = std::sqrt(PI / 140.0) * index;
        xLength = 80.0 * a;
        yLength = 80.0 * a;
    }
    break;



    default:
        throw std::invalid_argument("Invalid shape type in getPointsInShape");
    }

    // robust integer bounds
    const int xLen = static_cast<int>(std::ceil(xLength)) + 1;
    const int yLen = static_cast<int>(std::ceil(yLength)) + 1;

    if (_2DMask)
    {
        if (xMax) *xMax = xLen;
        if (yMax) *yMax = yLen;

        if (*_2DMask != nullptr)
        {
            throw std::invalid_argument(
                "getPointsInShape requires *_2DMask == nullptr");
        }

        *_2DMask = new bool*[xLen];
        for (int i = 0; i < xLen; ++i)
        {
            (*_2DMask)[i] = new bool[yLen];
            for (int j = 0; j < yLen; ++j)
            {
                (*_2DMask)[i][j] = false;
            }
        }
    }

    for (int i = 0; i < xLen; ++i)
    {
        for (int j = 0; j < yLen; ++j)
        {
            if (inBoundary(i, j, index, shape))
            {
                ++points;
                if (_2DMask)
                {
                    (*_2DMask)[i][j] = true;
                }
            }
        }
    }

    return points;
}


void initialize(mytuple &machine,
                int &uniquePoints,
                int &totalPoints,
                ShapeType shape,
                int index,
                WalkContext &ctx)
{
    auto &gen = ctx.rng;

    constexpr double MARGIN = 20.0;

    // helper: sample integer point in [0, xLen] × [0, yLen]
    auto sampleInBox = [&](double xLen, double yLen)
    {
        std::uniform_int_distribution<> distX(
            0, static_cast<int>(std::ceil(xLen)));
        std::uniform_int_distribution<> distY(
            0, static_cast<int>(std::ceil(yLen)));
        machine.x = distX(gen);
        machine.y = distY(gen);
    };

    switch (shape)
    {
    case CIRCLE:
    {
        double box = 2.0 * index;
        do {
            sampleInBox(box, box);
        } while (!inBoundary(machine.x, machine.y, index, CIRCLE));
    }
    break;

    case RECTANGLE:
    {
        double h = std::sqrt(PI / 3.0) * index;
        double w = 3.0 * h;
        do {
            sampleInBox(w, h);
        } while (!inBoundary(machine.x, machine.y, index, RECTANGLE));
    }
    break;

    case RECTANGLE_4_1:
    {
        double h = std::sqrt(PI / 4.0) * index;
        double w = 4.0 * h;
        do {
            sampleInBox(w, h);
        } while (!inBoundary(machine.x, machine.y, index, RECTANGLE_4_1));
    }
    break;

    case SQUARE:
    {
        double s = std::sqrt(PI) * index;
        do {
            sampleInBox(s, s);
        } while (!inBoundary(machine.x, machine.y, index, SQUARE));
    }
    break;

    case TRIANGLE:
    {
        double s = std::sqrt(4.0 * PI / std::sqrt(3.0)) * index;
        double h = std::sqrt(3.0) * s / 2.0;
        do {
            sampleInBox(s, h);
        } while (!inBoundary(machine.x, machine.y, index, TRIANGLE));
    }
    break;

    case SHARP_TRIANGLE:
    {
        double s = std::sqrt(4.0 * PI / std::tan(75.0 * PI / 180.0)) * index;
        double h = std::tan(75.0 * PI / 180.0) * s / 2.0;
        do {
            sampleInBox(s, h);
        } while (!inBoundary(machine.x, machine.y, index, SHARP_TRIANGLE));
    }
    break;

    case FLAT_TRIANGLE:
    {
        double s = std::sqrt(4.0 * PI / std::tan(40.0 * PI / 180.0)) * index;
        double h = std::tan(40.0 * PI / 180.0) * s / 2.0;
        do {
            sampleInBox(s, h);
        } while (!inBoundary(machine.x, machine.y, index, FLAT_TRIANGLE));
    }
    break;

    case TRAPEZOID:
    {
        double h = std::sqrt(PI / 2.0) * index;
        double w = 3.0 * h;
        do {
            sampleInBox(w, h);
        } while (!inBoundary(machine.x, machine.y, index, TRAPEZOID));
    }
    break;

    case SLOTTED_RECT_30x10:
    {
        double W, H, sx0, sx1, sy0, sy1;
        slottedRect30x10_dims(index, W, H, sx0, sx1, sy0, sy1);

        do {
            sampleInBox(W, H);
        } while (!inBoundary(machine.x, machine.y, index, SLOTTED_RECT_30x10));
    }
    break;


    // -------------------------------------------------
    // Regular polygons (HEXAGON, POLYGON5/8/9/10/12/15)
    // -------------------------------------------------
    case HEXAGON:
    case POLYGON5:
    case POLYGON8:
    case POLYGON9:
    case POLYGON10:
    case POLYGON12:
    case POLYGON15:
    {
        int N = 0;
        switch (shape)
        {
        case HEXAGON:   N = 6;  break;
        case POLYGON5:  N = 5;  break;
        case POLYGON8:  N = 8;  break;
        case POLYGON9:  N = 9;  break;
        case POLYGON10: N = 10; break;
        case POLYGON12: N = 12; break;
        case POLYGON15: N = 15; break;
        default: break;
        }

        double R = regularPolygonCircumradius(index, N);
        double box = 2.0 * R + 2.0 * MARGIN;

        do {
            sampleInBox(box, box);
        } while (!inBoundary(machine.x, machine.y, index, shape));
    }
    break;

    case ROTATED_SQUARE:
    {
        double s = std::sqrt(PI) * index;
        double box = s * std::sqrt(2.0);
        do {
            sampleInBox(box, box);
        } while (!inBoundary(machine.x, machine.y, index, ROTATED_SQUARE));
    }
    break;

    case STRETCHED_ROTATED_SQUARE:
    {
        double h = std::sqrt(2.0 * PI / 3.0) * index;
        double w = 3.0 * h;
        do {
            sampleInBox(w, h);
        } while (!inBoundary(machine.x, machine.y, index, STRETCHED_ROTATED_SQUARE));
    }
    break;

    case BOWTIE:
    {
        double w = std::sqrt(PI / 45.0) * index;
        double h = w + 2.0 * (4.0 * w);
        double l = 5.0 * w + 2.0 * (4.0 * w);
        do {
            sampleInBox(l, h);
        } while (!inBoundary(machine.x, machine.y, index, BOWTIE));
    }
    break;

    case DOUBLE_BOWTIE:
    {
        // sample inside its bounding box [0, 13a] x [0, 13a]
        const double a = std::sqrt(PI / 89.0) * index;
        const double box = 13.0 * a;

        do {
            std::uniform_int_distribution<> distX(0, static_cast<int>(std::ceil(box)));
            std::uniform_int_distribution<> distY(0, static_cast<int>(std::ceil(box)));
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, DOUBLE_BOWTIE));
    }
    break;

    case V_NOTCH_RECT:
    {
        const double a = static_cast<double>(index) * std::sqrt(PI / 14.0);
        const double W = 4.0 * a;
        const double H = 6.0 * a;

        do {
            std::uniform_int_distribution<> distX(0, static_cast<int>(std::ceil(W)));
            std::uniform_int_distribution<> distY(0, static_cast<int>(std::ceil(H)));
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, V_NOTCH_RECT));
    }
    break;

    case SNOWFLAKE:
    {
        const double a = std::sqrt(PI / 140.0) * index;
        const double W = 80.0 * a;
        const double H = 80.0 * a;

        do
        {
            std::uniform_int_distribution<> distX(0, static_cast<int>(std::ceil(W)));
            std::uniform_int_distribution<> distY(0, static_cast<int>(std::ceil(H)));
            machine.x = distX(gen);
            machine.y = distY(gen);
        } while (!inBoundary(machine.x, machine.y, index, SNOWFLAKE));
    }
    break;


    default:
        throw std::invalid_argument("Invalid shape type in initialize");
    }

    uniquePoints = 1;
    totalPoints  = 1;
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
    if (hard_boundary && !isCrossBoundary)
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
    std::vector<double> weights;

    if (!isCrossBoundary) {
        bool allInside = true; // Track if all next positions are inside
        double temperature = initemperature + (finaltemperature - initemperature) * std::exp(-0.05 * static_cast<double>(pointsOUT));

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
        for (short dist : distances)
        {
            double w = std::exp(-1 * std::pow(static_cast<double>(dist), DISTANCE_POWER) / temperature);
            weights.push_back(w);
        }
        
        // if cross boundary is allowed, use the third way of walking
        // currently this walking is only for square shape, the weight will be 0.5+
        

        std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
        size_t chosenIndex = dist(ctx.rng);
        return mytuple(position.x + fourdirs[chosenIndex].x, position.y + fourdirs[chosenIndex].y);

    } else {
        // If cross boundary is allowed, bias towards right-up corner (right and up directions)
        // Directions: 0=left, 1=right, 2=down, 3=up
        std::vector<double> weights = {0.25, 0.25+drafting_constant, 0.25, 0.25+drafting_constant}; // High probability for right (1) and up (3)
        std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
        size_t chosenIndex = dist(ctx.rng);

        mytuple nextPos(position.x + fourdirs[chosenIndex].x, position.y + fourdirs[chosenIndex].y);

        // Wrap around boundaries to simulate toroidal conditions (for square shape)
        if (shape == SQUARE) {
            double _constant = std::sqrt(PI);
            double width = _constant * index;
            double height = _constant * index;
            nextPos.x = std::fmod(nextPos.x, width);
            if (nextPos.x < 0) nextPos.x += width;
            nextPos.y = std::fmod(nextPos.y, height);
            if (nextPos.y < 0) nextPos.y += height;
        } else {
            // For other shapes, fall back to reflection over center
            if (!inBoundary(nextPos.x, nextPos.y, index, shape)) {
                mytuple center = findCenter(index, shape);
                nextPos.x = 2 * center.x - nextPos.x;
                nextPos.y = 2 * center.y - nextPos.y;
            }
        }

        return nextPos;
    }
}

// walk now optionally records the path taken in out_path (if non-null)
int walk(int index, ShapeType shape, WalkContext &ctx, std::vector<mytuple> *out_path)
{

    DiskCover cover(reference_radius * index * std::sqrt(PI));

    mytuple machine(0, 0);
    int uniquePoints = 0;
    int totalPoints = 0;
    int pointOUT = 0;

    bool **_mask = nullptr;
    int xMax, yMax;

    initialize(machine, uniquePoints, totalPoints, shape, index, ctx);

    if (make_cover) {
        cover.coverIfNeededRandom(machine.x, machine.y, ctx.rng);
    }

    int shapePoints = getPointsInShape(index, shape, nullptr, &xMax, &yMax);
    int hashSize = shapePoints / 2;
    int expected_steps = std::min<int>(shapePoints / 2 + 5, 100000);
    std::vector<mytuple> path;
    path.reserve(std::max(expected_steps, 100000)); // Reserve more space to handle cross boundary cases
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
            if (!inBoundary(machine.x, machine.y, index, shape))
            {
                std::cerr << "Moved outside boundary to (" << machine.x << ", " << machine.y << "). Total points: " << totalPoints << ", Unique points: " << uniquePoints << ", Points OUT: " << pointOUT << std::endl;
            }
            path.emplace_back(machine.x, machine.y);
            totalPoints += 1;

            if (inBoundary(machine.x, machine.y, index, shape))
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

            if (make_cover) {
                cover.coverIfNeededRandom(machine.x, machine.y, ctx.rng);
            }

        }

        if (out_path)
        {
            *out_path = std::move(path);
            //print the covering disks if needed
            if (make_cover) {
                cover.writeDisks("cover_disks", shapeNames[shape]);
            }
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

        // report cover disk count back to caller via context
        ctx.last_cover_count = cover.diskCount();

        return totalPoints;
    }
    catch (...)
    {
        freeHashmap(hashmap, hashSize);
        throw;
    }
}

int walk_with_cross_index(int index, ShapeType shape, WalkContext &ctx, std::vector<mytuple> *out_path)
{
    // For now, just call the regular walk function. The cross-index logic can be implemented here if needed.
    return walk(index, shape, ctx, out_path);
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

