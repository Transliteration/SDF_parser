#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "Eigen/Core"


#include <iostream>

#include "Atom.h"
#include "Bond.h"

using namespace Eigen;

class Visualizer
{
private:
    Matrix3Xd points;
    Matrix3Xd origPoints;
    std::vector<Atom> atoms;
    std::vector<iBond> bonds;

    std::vector<double> used_angles;

    template <typename Derived>
    void rotate_around_bond(int bond_index, bool forward, double angle, MatrixBase<Derived> const &points_to_rotate);
    template <typename Derived>
    void rotate_points(const AngleAxis<double> &rot, MatrixBase<Derived> const &points_to_rotate, Vector3d &tr);
    double compute_square_error(const Matrix3Xd &compare_to);

public:
    Visualizer(std::vector<std::shared_ptr<Atom>> atoms, std::vector<std::shared_ptr<Bond>> bonds);
    ~Visualizer() = default;

    void rotate_all_by_random_angles();
    void find_transformation_with_low_error(double thresh, size_t rot_count = 5);
    template <typename Derived>
    void show(MatrixBase<Derived> const &points_to_rotate);
};
