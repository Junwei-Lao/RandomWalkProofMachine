// random_walk_fibonacci_standard_shape.cpp
//
// Requires CGAL.
//
// This program:
// 1. Builds ONE base convex hull from Fibonacci sphere points
// 2. Optionally perturbs radii slightly
// 3. Reuses that same base hull for every target volume by scaling copies
// 4. Extracts integer grid points inside/on the hull
// 5. Exports all points and shell points
// 6. Runs repeated random walks until half the unique grid points are visited
// 7. Writes mean/stddev results to a CSV
//
// Boundary rule:
// - If the walker proposes a move outside the domain, it stays in place.
// - That still counts as one step.
//
// Notes:
// - "Same shape" here means same continuous polyhedron up to scaling.
// - The integer-grid realization will still change discretely with volume.
//
// Example compile command (adjust for your system):
// g++ -std=c++20 -O2 random_walk_fibonacci_standard_shape.cpp -o sim \
//     -lCGAL -lgmp -lmpfr

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Side_of_triangle_mesh.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/boost/graph/copy_face_graph.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

const double PI = std::acos(-1);
enum ShapeType
{
    OCTAHEDRON, 
    CUBE,
    RECTANGULAR_PRISM_1_2_3,
    PYRAMID,
};

namespace PMP = CGAL::Polygon_mesh_processing;

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point3 = Kernel::Point_3;
using Mesh   = CGAL::Surface_mesh<Point3>;

struct GridPoint {
    int x{};
    int y{};
    int z{};

    bool operator==(const GridPoint& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct GridPointHash {
    std::size_t operator()(const GridPoint& p) const noexcept {
        std::size_t h1 = std::hash<int>{}(p.x);
        std::size_t h2 = std::hash<int>{}(p.y);
        std::size_t h3 = std::hash<int>{}(p.z);
        return h1 ^ (h2 << 1) ^ (h3 << 7);
    }
};

struct Stats {
    double mean{};
    double sample_stddev{};
};

struct ExperimentRow {
    int vertex_count{};
    double target_volume{};
    double actual_volume{};
    double base_area{};
    double actual_area{};
    std::size_t grid_point_count{};
    std::size_t shell_point_count{};
    int trials{};
    double coverage_fraction{};
    double mean_steps{};
    double stddev_steps{};
};

class ConvexHullDomain {
public:
    bool generate_base_hull_regular(ShapeType shape) {
        std::vector<Point3> pts;

        switch (shape)
        {
        case OCTAHEDRON:
            // Regular octahedron inscribed in a 1x1x1 box
            pts = {
                Point3(0.5, 0.5, 0.0),
                Point3(0.5, 0.5, 1.0),
                Point3(0.5, 0.0, 0.5),
                Point3(0.5, 1.0, 0.5),
                Point3(0.0, 0.5, 0.5),
                Point3(1.0, 0.5, 0.5)
            };
            break;

        case CUBE:
            // Unit cube: 1 x 1 x 1
            pts = {
                Point3(0.0, 0.0, 0.0),
                Point3(1.0, 0.0, 0.0),
                Point3(1.0, 1.0, 0.0),
                Point3(0.0, 1.0, 0.0),
                Point3(0.0, 0.0, 1.0),
                Point3(1.0, 0.0, 1.0),
                Point3(1.0, 1.0, 1.0),
                Point3(0.0, 1.0, 1.0)
            };
            break;

        case RECTANGULAR_PRISM_1_2_3:
            // Rectangular prism: 1 x 2 x 3
            pts = {
                Point3(0.0, 0.0, 0.0),
                Point3(1.0, 0.0, 0.0),
                Point3(1.0, 2.0, 0.0),
                Point3(0.0, 2.0, 0.0),
                Point3(0.0, 0.0, 3.0),
                Point3(1.0, 0.0, 3.0),
                Point3(1.0, 2.0, 3.0),
                Point3(0.0, 2.0, 3.0)
            };
            break;

        case PYRAMID:
            // Square pyramid with base 1 x 1 and height 1
            pts = {
                Point3(0.0, 0.0, 0.0),
                Point3(1.0, 0.0, 0.0),
                Point3(1.0, 1.0, 0.0),
                Point3(0.0, 1.0, 0.0),
                Point3(0.5, 0.5, 1.0)
            };
            break;

        default:
            return false;
        }

        Mesh temp;
        CGAL::convex_hull_3(pts.begin(), pts.end(), temp);

        if (!CGAL::is_closed(temp)) {
            return false;
        }

        double vol = std::abs(PMP::volume(temp));
        if (!(vol > 0.0)) {
            return false;
        }

        double area = PMP::area(temp);
        if (!(area > 0.0)) {
            return false;
        }

        mesh_ = std::move(temp);
        vertex_count_ = static_cast<int>(mesh_.number_of_vertices());
        base_volume_ = vol;
        base_area_ = area;
        target_volume_ = base_volume_;
        actual_volume_ = base_volume_;
        actual_area_ = base_area_;

        return true;
    }




    bool generate_base_hull_random(int target_vertices,
                                double coord_limit,
                                std::mt19937_64& rng,
                                int max_attempts = 1000) {
        if (target_vertices < 4) {
            throw std::invalid_argument("target_vertices must be at least 4.");
        }
        if (coord_limit <= 0.0) {
            throw std::invalid_argument("coord_limit must be positive.");
        }
        

        for (int attempt = 0; attempt < max_attempts; ++attempt) {
            std::vector<Point3> pts = generate_random_points(target_vertices, coord_limit, rng);

            Mesh temp;
            CGAL::convex_hull_3(pts.begin(), pts.end(), temp);

            if (!CGAL::is_closed(temp)) {
                continue;
            }

            if (static_cast<int>(temp.number_of_vertices()) != target_vertices) {
                continue;
            }

            double vol = std::abs(PMP::volume(temp));
            if (!(vol > 0.0)) {
                continue;
            }

            double area = PMP::area(temp);
            if (!(area > 0.0)) {
                continue;
            }


            mesh_ = std::move(temp);
            vertex_count_ = target_vertices;
            base_volume_ = vol;
            base_area_ = area;
            return true;
        }

        return false;
    }
    // Build the ONE base hull from Fibonacci sphere points.
    bool generate_base_hull_from_fibonacci(int target_vertices,
                                           double base_radius,
                                           double perturbation_fraction,
                                           std::mt19937_64& rng,
                                           int max_attempts = 50) {
        if (target_vertices < 4) {
            throw std::invalid_argument("target_vertices must be at least 4.");
        }
        if (base_radius <= 0.0) {
            throw std::invalid_argument("base_radius must be positive.");
        }
        if (perturbation_fraction < 0.0 || perturbation_fraction >= 1.0) {
            throw std::invalid_argument("perturbation_fraction must be in [0, 1).");
        }

        for (int attempt = 0; attempt < max_attempts; ++attempt) {
            std::vector<Point3> pts = generate_fibonacci_sphere_points(
                target_vertices, base_radius, perturbation_fraction, rng
            );

            Mesh temp;
            CGAL::convex_hull_3(pts.begin(), pts.end(), temp);

            if (!CGAL::is_closed(temp)) {
                continue;
            }

            if (static_cast<int>(temp.number_of_vertices()) != target_vertices) {
                continue;
            }

            double vol = std::abs(PMP::volume(temp));
            if (!(vol > 0.0)) {
                continue;
            }

            double area = PMP::area(temp);
            if (!(area > 0.0)) {
                continue;
            }

            mesh_ = std::move(temp);
            vertex_count_ = target_vertices;
            base_volume_ = vol;
            base_area_ = area;

            return true;
        }

        return false;
    }

    // Create a scaled copy of the SAME base shape for a target volume.
    ConvexHullDomain scaled_copy_for_volume(double target_volume) const {
        if (mesh_.number_of_vertices() == 0) {
            throw std::runtime_error("Base mesh is empty. Generate the base hull first.");
        }
        if (target_volume <= 0.0) {
            throw std::invalid_argument("target_volume must be positive.");
        }
        if (!(base_volume_ > 0.0)) {
            throw std::runtime_error("Base volume is invalid.");
        }

        ConvexHullDomain out;
        out.vertex_count_ = vertex_count_;

        // Deep copy mesh
        CGAL::copy_face_graph(mesh_, out.mesh_);

        double scale = std::cbrt(target_volume / base_volume_);
        scale_mesh(out.mesh_, scale);

        out.base_volume_ = base_volume_;
        out.actual_volume_ = std::abs(PMP::volume(out.mesh_));
        out.target_volume_ = target_volume;
        out.base_area_ = base_area_;
        out.actual_area_ = std::abs(PMP::area(out.mesh_));

        return out;
    }

    void build_integer_grid_points() {
        inside_points_.clear();
        inside_set_.clear();

        if (mesh_.number_of_vertices() == 0) {
            throw std::runtime_error("Mesh is empty.");
        }

        CGAL::Bbox_3 bbox = CGAL::bbox_3(mesh_.points().begin(), mesh_.points().end());

        int xmin = static_cast<int>(std::ceil(bbox.xmin()));
        int xmax = static_cast<int>(std::floor(bbox.xmax()));
        int ymin = static_cast<int>(std::ceil(bbox.ymin()));
        int ymax = static_cast<int>(std::floor(bbox.ymax()));
        int zmin = static_cast<int>(std::ceil(bbox.zmin()));
        int zmax = static_cast<int>(std::floor(bbox.zmax()));

        CGAL::Side_of_triangle_mesh<Mesh, Kernel> inside_tester(mesh_);

        for (int x = xmin; x <= xmax; ++x) {
            for (int y = ymin; y <= ymax; ++y) {
                for (int z = zmin; z <= zmax; ++z) {
                    Point3 q(static_cast<double>(x),
                             static_cast<double>(y),
                             static_cast<double>(z));

                    auto side = inside_tester(q);
                    if (side == CGAL::ON_BOUNDED_SIDE || side == CGAL::ON_BOUNDARY) {
                        GridPoint p{x, y, z};
                        inside_points_.push_back(p);
                        inside_set_.insert(p);
                    }
                }
            }
        }

        if (inside_points_.empty()) {
            throw std::runtime_error("No integer grid points found inside the hull.");
        }
    }

    bool contains(const GridPoint& p) const {
        return inside_set_.find(p) != inside_set_.end();
    }

    const std::vector<GridPoint>& points() const {
        return inside_points_;
    }

    std::size_t grid_point_count() const {
        return inside_points_.size();
    }

    int vertex_count() const {
        return vertex_count_;
    }

    double base_volume() const {
        return base_volume_;
    }

    double target_volume() const {
        return target_volume_;
    }

    double actual_volume() const {
        return actual_volume_;
    }

    double base_area() const {
        return base_area_;
    }

    double actual_area() const {
        return actual_area_;
    }

private:
    static std::vector<Point3> generate_random_points(int n,
                                                    double coord_limit,
                                                    std::mt19937_64& rng) {
        std::vector<Point3> pts;
        pts.reserve(n);

        std::uniform_real_distribution<double> dist(-coord_limit, coord_limit);

        for (int i = 0; i < n; ++i) {
            pts.emplace_back(dist(rng), dist(rng), dist(rng));
        }

        return pts;
    }

    static std::vector<Point3> generate_fibonacci_sphere_points(int n,
                                                                double base_radius,
                                                                double perturbation_fraction,
                                                                std::mt19937_64& rng) {
        std::vector<Point3> pts;
        pts.reserve(n);

        std::uniform_real_distribution<double> noise_dist(
            -perturbation_fraction, perturbation_fraction
        );

        // Golden angle
        const double golden_angle = M_PI * (3.0 - std::sqrt(5.0));

        for (int i = 0; i < n; ++i) {
            // z in (-1, 1)
            double z = 1.0 - 2.0 * (static_cast<double>(i) + 0.5) / static_cast<double>(n);
            double r_xy = std::sqrt(std::max(0.0, 1.0 - z * z));
            double theta = golden_angle * static_cast<double>(i);

            double x = r_xy * std::cos(theta);
            double y = r_xy * std::sin(theta);

            double radius = base_radius;
            if (perturbation_fraction > 0.0) {
                radius *= (1.0 + noise_dist(rng));
            }

            pts.emplace_back(radius * x, radius * y, radius * z);
        }

        return pts;
    }

    static void scale_mesh(Mesh& mesh, double scale) {
        for (auto v : mesh.vertices()) {
            const Point3& p = mesh.point(v);
            mesh.point(v) = Point3(scale * p.x(), scale * p.y(), scale * p.z());
        }
    }

private:
    Mesh mesh_;
    std::vector<GridPoint> inside_points_;
    std::unordered_set<GridPoint, GridPointHash> inside_set_;

    int vertex_count_{0};
    double base_volume_{0.0};
    double base_area_{0.0};
    double target_volume_{0.0};
    double actual_volume_{0.0};
    double actual_area_{0.0};
};

class RandomWalkSimulator {
public:
    explicit RandomWalkSimulator(std::uint64_t seed)
        : rng_(seed) {}

    std::uint64_t run_one_trial(const ConvexHullDomain& domain, double coverage_fraction) {
        const auto& pts = domain.points();
        const std::size_t total_points = pts.size();

        if (total_points == 0) {
            throw std::runtime_error("Domain has no integer points.");
        }
        if (!(coverage_fraction > 0.0 && coverage_fraction <= 1.0)) {
            throw std::invalid_argument("coverage_fraction must be in (0, 1].");
        }

        const std::size_t target_unique =
            static_cast<std::size_t>(std::ceil(coverage_fraction * static_cast<double>(total_points)));

        std::uniform_int_distribution<std::size_t> start_dist(0, total_points - 1);
        std::uniform_int_distribution<int> dir_dist(0, 5);

        static const std::array<GridPoint, 6> dirs{{
            {+1,  0,  0},
            {-1,  0,  0},
            { 0, +1,  0},
            { 0, -1,  0},
            { 0,  0, +1},
            { 0,  0, -1}
        }};

        GridPoint current = pts[start_dist(rng_)];
        std::unordered_set<GridPoint, GridPointHash> visited;
        visited.reserve(target_unique * 2 + 16);
        visited.insert(current);

        std::uint64_t steps = 0;

        while (visited.size() < target_unique) {
            const GridPoint d = dirs[dir_dist(rng_)];
            GridPoint next{current.x + d.x, current.y + d.y, current.z + d.z};

            if (domain.contains(next)) {
                current = next;
            }

            visited.insert(current);
            ++steps;
        }

        return steps;
    }

    Stats run_many_trials(const ConvexHullDomain& domain,
                          double coverage_fraction,
                          int trials) {
        if (trials < 2) {
            throw std::invalid_argument("trials must be at least 2.");
        }

        std::vector<double> values;
        values.reserve(static_cast<std::size_t>(trials));

        for (int i = 0; i < trials; ++i) {
            values.push_back(static_cast<double>(run_one_trial(domain, coverage_fraction)));
        }

        return compute_stats(values);
    }

private:
    static Stats compute_stats(const std::vector<double>& values) {
        const std::size_t n = values.size();

        double mean = std::accumulate(values.begin(), values.end(), 0.0)
                    / static_cast<double>(n);

        double accum = 0.0;
        for (double x : values) {
            double diff = x - mean;
            accum += diff * diff;
        }

        double variance = accum / static_cast<double>(n - 1);
        return Stats{mean, std::sqrt(variance)};
    }

private:
    std::mt19937_64 rng_;
};

static bool is_shell_point(const ConvexHullDomain& domain, const GridPoint& p) {
    static const std::array<GridPoint, 6> dirs{{
        {+1,  0,  0},
        {-1,  0,  0},
        { 0, +1,  0},
        { 0, -1,  0},
        { 0,  0, +1},
        { 0,  0, -1}
    }};

    for (const auto& d : dirs) {
        GridPoint q{p.x + d.x, p.y + d.y, p.z + d.z};
        if (!domain.contains(q)) {
            return true;
        }
    }
    return false;
}

static std::size_t write_shell_points_txt(const ConvexHullDomain& domain,
                                          const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Failed to open shell file: " + filename);
    }

    std::size_t count = 0;
    for (const auto& p : domain.points()) {
        if (is_shell_point(domain, p)) {
            out << p.x << " " << p.y << " " << p.z << "\n";
            ++count;
        }
    }
    return count;
}

static void write_all_points_txt(const ConvexHullDomain& domain,
                                 const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Failed to open full-points file: " + filename);
    }

    for (const auto& p : domain.points()) {
        out << p.x << " " << p.y << " " << p.z << "\n";
    }
}

static void write_base_vertices_txt(const Mesh& mesh, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Failed to open base-vertex file: " + filename);
    }

    for (auto v : mesh.vertices()) {
        const Point3& p = mesh.point(v);
        out << std::setprecision(17)
            << p.x() << " " << p.y() << " " << p.z() << "\n";
    }
}

static void write_csv_header_if_needed(const std::string& filename) {
    std::ifstream fin(filename);
    if (fin.good()) {
        return;
    }

    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Failed to create CSV file: " + filename);
    }

    out << "vertex_count,"
        << "target_volume,"
        << "actual_volume,"
        << "base_area,"
        << "actual_area,"
        << "grid_point_count,"
        << "shell_point_count,"
        << "trials,"
        << "coverage_fraction,"
        << "mean_steps,"
        << "stddev_steps\n";
}

static void append_csv_row(const std::string& filename, const ExperimentRow& row) {
    std::ofstream out(filename, std::ios::app);
    if (!out) {
        throw std::runtime_error("Failed to append CSV file: " + filename);
    }

    out << row.vertex_count << ","
        << std::fixed << std::setprecision(6) << row.target_volume << ","
        << std::fixed << std::setprecision(6) << row.actual_volume << ","
        << std::fixed << std::setprecision(6) << row.base_area << ","
        << std::fixed << std::setprecision(6) << row.actual_area << ","
        << row.grid_point_count << ","
        << row.shell_point_count << ","
        << row.trials << ","
        << std::fixed << std::setprecision(6) << row.coverage_fraction << ","
        << std::fixed << std::setprecision(6) << row.mean_steps << ","
        << std::fixed << std::setprecision(6) << row.stddev_steps
        << "\n";
}

int main() {
    try {
        // =========================
        // User settings
        // =========================
        const double base_radius = 10.0;
        // 0.0 => no perturbation
        // 0.05 => each radius perturbed by up to +/-5%
        const double perturbation_fraction = 0.05;
        const int trials_per_volume = 1000;
        const double coverage_fraction = 0.75;

        const bool export_all_points = false;
        const bool export_shell_points = false;

        const std::string csv_filename = "random_walk_results.csv";

        const std::uint64_t hull_seed = 123456789ULL;
        const std::uint64_t walk_seed_base = 987654321ULL;

        int vertex_count = 0;


        for (ShapeType shape : {OCTAHEDRON, CUBE, RECTANGULAR_PRISM_1_2_3, PYRAMID}) {
            vertex_count++;

            std::vector<double> target_volumes;
            for (double i = 20.0; i <= 70.0; i += 1.0) {
                target_volumes.push_back(4.0/3.0 * PI * std::pow(i/2.0, 3));
            }

            // =========================
            // Build ONE base hull
            // =========================
            //std::mt19937_64 hull_rng(hull_seed + static_cast<std::uint64_t>(vertex_count));

            ConvexHullDomain base_domain;
            const double coord_limit = 10.0;
            bool ok = base_domain.generate_base_hull_regular(shape);
            /*
            bool ok = base_domain.generate_base_hull_from_fibonacci(
                vertex_count,
                base_radius,
                perturbation_fraction,
                hull_rng,
                1000
            );
            */
            if (!ok) {
                std::cerr << "Failed to generate the base hull with "
                        << vertex_count << " vertices.\n";
                return 1;
            }

            std::cout << "Generated ONE base hull.\n";
            std::cout << "Base hull vertex count: " << base_domain.vertex_count() << "\n";
            std::cout << "Base hull volume: " << base_domain.base_volume() << "\n";

            write_csv_header_if_needed(csv_filename);

            // =========================
            // Loop over target volumes
            // =========================
            for (std::size_t i = 0; i < target_volumes.size(); ++i) {
                const double target_volume = target_volumes[i];

                std::cout << "========================================\n";
                std::cout << "Target volume = " << target_volume << "\n";

                // Same base shape, scaled copy only
                ConvexHullDomain domain = base_domain.scaled_copy_for_volume(target_volume);

                std::cout << "Scaled from same base hull.\n";
                std::cout << "Actual scaled volume: " << domain.actual_volume() << "\n";

                domain.build_integer_grid_points();

                std::cout << "Interior/on-boundary integer grid points: "
                        << domain.grid_point_count() << "\n";

                std::size_t shell_count = 0;

                if (export_all_points) {
                    std::ostringstream name;
                    name << "all_points_v" << vertex_count
                        << "_vol" << static_cast<long long>(target_volume)
                        << ".txt";
                    write_all_points_txt(domain, name.str());
                    std::cout << "Wrote all points to " << name.str() << "\n";
                }

                if (export_shell_points) {
                    std::ostringstream name;
                    name << "shell_points_v" << vertex_count
                        << "_vol" << static_cast<long long>(target_volume)
                        << ".txt";
                    shell_count = write_shell_points_txt(domain, name.str());
                    std::cout << "Wrote shell points to " << name.str() << "\n";
                    std::cout << "Shell point count: " << shell_count << "\n";
                }

                if (domain.grid_point_count() < 2) {
                    std::cerr << "Too few grid points for meaningful walk. Skipping.\n";
                    continue;
                }

                std::uint64_t walk_seed = walk_seed_base + 1000ULL * static_cast<std::uint64_t>(vertex_count) + static_cast<std::uint64_t>(i);
                RandomWalkSimulator simulator(walk_seed);
                Stats stats = simulator.run_many_trials(
                    domain,
                    coverage_fraction,
                    trials_per_volume
                );

                std::cout << "Mean steps: " << stats.mean << "\n";
                std::cout << "Stddev:     " << stats.sample_stddev << "\n";

                ExperimentRow row;
                row.vertex_count = vertex_count;
                row.target_volume = target_volume;
                row.actual_volume = domain.actual_volume();
                row.base_area = domain.base_area();
                row.actual_area = domain.actual_area();
                row.grid_point_count = domain.grid_point_count();
                row.shell_point_count = shell_count;
                row.trials = trials_per_volume;
                row.coverage_fraction = coverage_fraction;
                row.mean_steps = stats.mean;
                row.stddev_steps = stats.sample_stddev;

                append_csv_row(csv_filename, row);

                std::cout << "Appended result to " << csv_filename << "\n";
            }
        }

        std::cout << "========================================\n";
        std::cout << "All experiments finished.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}