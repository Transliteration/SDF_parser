#include "Visualizer.h"

// Drawing shapes
#include <opencv2/imgproc.hpp>

#include "Eigen/Geometry"

#include <cstdlib>
#include <ctime>
#include <random>

double SERROR = 0, wanted_error = 0;

double factorial(const int n)
{
    double f = 1;
    for (int i=1; i<=n; ++i)
        f *= i;
    return f;
}

/*
* Function return all possible combinations of k elements from N-size inputVector.
* The result is returned as a vector of k-long vectors containing all combinations.
https://rosettacode.org/wiki/Combinations_with_repetitions#C.2B.2B
*/
template<typename T> 
std::vector<std::vector<T>> getAllCombinations(const std::vector<T>& inputVector, int k)
{
    std::vector<std::vector<T>> combinations;
    std::vector<int> selector(inputVector.size());
    std::fill(selector.begin(), selector.begin() + k, 1);

    do {
        std::vector<int> selectedIds;
        std::vector<T> selectedVectorElements;
        for (size_t i = 0; i < inputVector.size(); i++) {
            if (selector[i]) {
                selectedIds.push_back(i);
            }
        }
        for (auto& id : selectedIds) {
            selectedVectorElements.push_back(inputVector[id]);
        }
        combinations.push_back(selectedVectorElements);
    } while (std::prev_permutation(selector.begin(), selector.end()));

    return combinations;
}

Visualizer::Visualizer(std::vector<std::shared_ptr<Atom>> atoms, std::vector<std::shared_ptr<Bond>> bonds)
: points(3, atoms.size())
, origPoints(3, atoms.size())
{
    size_t pIndex = 0;
    for (auto &&atom : atoms)
    {
        this->atoms.push_back(*atom);
        points(0, pIndex) = atom->x;
        points(1, pIndex) = atom->y;
        points(2, pIndex) = atom->z;
        pIndex++;
    }
    
    for (auto &&bond : bonds)
    {
        auto a1 = std::find(atoms.begin(), atoms.end(), bond->from.lock());
        auto a2 = std::find(atoms.begin(), atoms.end(), bond->to.lock());
        
        int index1 = a1 - atoms.begin();
        int index2 = a2 - atoms.begin();
        
        this->bonds.push_back(iBond(index1, index2));
    }
    
    // std::cout << points;
    origPoints = points;
}

// assume that matrix coords of SDF atoms arranged like in matrix coords in rotated molecule
double Visualizer::compute_square_error(const Matrix3Xd &compare_to)
{
    double error = 0;
    for (int i = 0; i < points.cols(); ++i)
    {
        error += (compare_to.col(i) - origPoints.col(i)).norm();
    }

    return error;

}

template <typename Derived>
void Visualizer::rotate_around_bond(int bond_index, bool forward, double angle,  MatrixBase<Derived> const &points_to_rotate)
{
    Vector3d p1 = origPoints.col(bonds[bond_index].in1);
    Vector3d p2 = origPoints.col(bonds[bond_index].in2);
    
    Vector3d axis;
    if (forward) axis = (p2-p1).normalized();
    else axis = (p1-p2).normalized();
    AngleAxisd t(angle, axis);

    MatrixBase<Derived>& C = const_cast< MatrixBase<Derived>& >(points_to_rotate);

    // for (int i = 0; i < C.cols(); ++i)
    // {
    //     C.col(i) = t * (C.col(i) - p2) + p2;
    // }
    C = t.toRotationMatrix()*C;
    
    // std::cout << Quaternion<double>::FromTwoVectors(p2, p1).normalized() << std::endl;
}
template <typename Derived>
void Visualizer::rotate_points(const AngleAxis<double> &rot, MatrixBase<Derived> const &points_to_rotate, Vector3d &tr)
{
    MatrixBase<Derived>& C = const_cast< MatrixBase<Derived>& >(points_to_rotate);

    for (int i = 0; i < C.cols(); ++i)
    {
        C.col(i) = rot * (C.col(i) - tr) + tr;
    }
}

void Visualizer::find_transformation_with_low_error(double thresh, size_t rot_count)
{
    Matrix3Xd points_after_transform;
    points_after_transform.resize(points.rows(), points.cols());  
    points_after_transform = points;  

    const size_t bonds_num = bonds.size();
    // std::cout << "Bond num=" << bonds_num << '\n';


    // size_t rot_count = 5;
    std::vector<double> angles;
    double angle = 360./rot_count;
    for (size_t i = 0; i < rot_count; i++)
    {
        angles.push_back(angle*i);
    }

    auto all_comb = getAllCombinations(angles, bonds_num);


    std::vector<size_t> permutation(bonds_num);
    for (size_t i = 0; i < permutation.size(); i++)
    {
        permutation[i] = i;
    }
    // auto best_rot = rot_combinations[0];
    auto best_rot = all_comb[0];
    auto best_perm = permutation;
    bool foundNextApprox = false;
    
    for (auto &&comb : all_comb)
    {
        if (SERROR < wanted_error) break;
        do {
            if (SERROR < wanted_error) break;
            bool isSearchedAngles = true;
            points_after_transform = points;
            for (size_t i = 0; i < permutation.size(); i++)
            {
                if ( std::abs(used_angles[permutation[i]] - comb[i]/M_PI) > 0.001)
                    isSearchedAngles = false;
                rotate_around_bond(permutation[i], false, comb[i]/M_PI, points_after_transform);
            }
            

            
            
            double error = compute_square_error(points_after_transform);
            if (isSearchedAngles && error < 0.1)
            {
                std::cout << "EXACT ANGLES ERROR: " << error << '\n';
                for (size_t i = 0; i < comb.size(); i++)
                {
                    std::cout << "Bond#" << permutation[i] << ", angle=" << comb[i]<< '\n';
                }
                
                std::cout <<'\n';
            }
            if (error < SERROR)
            {
                SERROR = error;
                best_rot = comb;
                best_perm = permutation;
                foundNextApprox = true;
        }
        } while (next_permutation(permutation.begin(), permutation.end()));
    }

    std::cout << "ERROR BELOW THRESH: " << SERROR << '\n';
    points_after_transform = points;
    for (size_t i = 0; i < best_perm.size(); i++)
    {
        rotate_around_bond(best_perm[i], false, best_rot[i]/M_PI, points_after_transform);
    }
    std::cout << "Real ERROR: " << compute_square_error(points_after_transform) << '\n';
    for (size_t i = 0; i < best_rot.size(); i++)
    {
        std::cout << "Bond#" << best_perm[i] << ", angle=" << best_rot[i] << '\n';
    }

    if (foundNextApprox)
        show(points_after_transform);   
}

void Visualizer::rotate_all_by_random_angles()
{
    // restore original values
    points = origPoints;
    // clear previous angles
    used_angles.clear();

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, 359);

    
    for (size_t i = 0; i < bonds.size(); i++)
    {
        double rand_num = (distrib(gen))/M_PI;
        used_angles.push_back(rand_num);
        rotate_around_bond(i, true, rand_num, points);
        std::cout << "Angle: " <<  rand_num*M_PI << ", bond num: " << i << '\n';
        // show();
    }


    SERROR = compute_square_error(points);
    wanted_error = SERROR*1/100;


    std::cout << "Def error: " << compute_square_error(points) << '\n';

    show(points);
    find_transformation_with_low_error(SERROR, 10);
    size_t rot_count = 0;
    while (SERROR > wanted_error && rot_count < 361)
    {
        std::cout << rot_count << "=rotcount\n";
        find_transformation_with_low_error(SERROR, rot_count+=10);
    }
    
    std::cout << rot_count << '\n';

}

template <typename Derived>
void Visualizer::show(MatrixBase<Derived> const &points_to_rotate)
{
    // return;
    constexpr size_t w = 600, h = 600;
    constexpr double cw = w/10, ch = h/10/* , cd = (w+h)/10/6 */;
    cv::Mat img(h, w, CV_8UC3);
    // static int current_bond = 0;

    auto bColor = cv::Scalar(125, 255, 255);
    auto aColor = cv::Scalar(255, 125, 125);
    auto orig_bColor = cv::Scalar(170, 170, 170);
    auto orig_aColor = cv::Scalar(120, 120, 120);

    Matrix3Xd mp, orig_mp;
    mp.resize(points_to_rotate.rows(), points_to_rotate.cols());
    orig_mp.resize(origPoints.rows(), origPoints.cols());

    // std::cout << '\n' <<  points.cols() << points.rows() << '\n';

    Affine3d projectTo2d = Scaling(1.,1.,3.) * Translation3d(w/2, h/2, 3) * Scaling(cw,ch,-1.);


    mp = projectTo2d*points_to_rotate;
    orig_mp = projectTo2d*origPoints;


    for (size_t i = 0; i < bonds.size(); ++i)
    {
        int in1 = bonds[i].in1;
        int in2 = bonds[i].in2;

        // orig structure
        cv::line(img, cv::Point(orig_mp(0, in1), orig_mp(1, in1)), cv::Point(orig_mp(0, in2), orig_mp(1, in2)), orig_bColor, 4, 8);
        
        cv::circle(img, cv::Point(orig_mp(0, in1), orig_mp(1, in1)), /* orig_mp(2, in1) */5, orig_aColor, -1);
        cv::circle(img, cv::Point(orig_mp(0, in2), orig_mp(1, in2)), /* orig_mp(2, in2) */5, orig_aColor, -1);
        // current structure
        cv::line(img, cv::Point(mp(0, in1), mp(1, in1)), cv::Point(mp(0, in2), mp(1, in2)), bColor, 4, 8);
        
        cv::circle(img, cv::Point(mp(0, in1), mp(1, in1)), /* mp(2, in1) */5, aColor, -1);
        cv::circle(img, cv::Point(mp(0, in2), mp(1, in2)), /* mp(2, in2) */5, aColor, -1);
        
    }
    
    cv::imshow("Visualizer",img);
    std::cout << "Error=" << compute_square_error(points_to_rotate) << '\n';
    char button = cv::waitKey(0);
    cv::destroyWindow("Visualizer");
    std::cout << (int)button << " pressed" << std::endl;
    switch (button)
    {
    case 27:
        return;
        break;
    // case -83:
    //     std::cout << "- pressed" << std::endl;
    //     rotate_around_bond(current_bond, false);
    //     break;
    // case -85:
    //     std::cout << "+ pressed" << std::endl;
    //     rotate_around_bond(current_bond, true);
    //     break;
    // case 49:
    //     std::cout << "- pressed" << std::endl;
    //     current_bond = (current_bond+1) % bonds.size();
    //     break;
    // case 50:
    //     std::cout << "+ pressed" << std::endl;
    //     current_bond = (current_bond-1) % bonds.size();
    //     break;
    case 114:
        std::cout << "Reset pressed" << std::endl;
        // points_to_rotate = origPoints;
        // current_bond = 0;
        break;
    default:
        break;
    }
    // show();
}

