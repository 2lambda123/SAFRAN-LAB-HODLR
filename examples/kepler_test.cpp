#include <cmath>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <Eigen/Dense>

#include "get_Matrix.hpp"
#include "HODLR_Tree.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::getline;
using Eigen::VectorXd;
using Eigen::MatrixXd;

vector<double> _theta, _time, _flux, _ferr;

double get_Matrix_Entry(const unsigned i, const unsigned j) {
    double d = _time[i] - _time[j];
    return _theta[0] * exp(-0.5 * d * d / (_theta[0]));
}

int main ()
{
    _theta.push_back(1e-3*1e-3);
    _theta.push_back(1.5*1.5);

    cout << "Loading the data..." << endl;
    std::string tmp;
    while (getline(cin, tmp, ',')) {
        _time.push_back(atof(tmp.c_str()));
        getline(cin, tmp, ',');
        _flux.push_back(atof(tmp.c_str()));
        getline(cin, tmp);
        _ferr.push_back(atof(tmp.c_str()));
    }

    // Settings for the solver.
    unsigned N       = _time.size();
    unsigned nLeaf   = 50;
    double tolerance = 1e-14;

    cout << N << " data points" << endl;

    // Build the RHS matrix.
    MatrixXd b(N, 1), x;
    VectorXd yvar(N);
    for (int i = 0; i < N; ++i) {
        b(i, 0) = _flux[i];
        yvar(i) = _ferr[i] * _ferr[i];
    }

    clock_t start, end;
    cout << endl << "Setting things up..." << endl;
    start = clock();
    HODLR_Tree * A = new HODLR_Tree(N, nLeaf);
    end = clock();
    cout << "Time taken is: " << double(end-start)/double(CLOCKS_PER_SEC)<< endl;

    cout << endl << "Assembling the matrix in HODLR form..." << endl;
    start = clock();
    A->assemble_Matrix(yvar, tolerance);
    end = clock();
    cout << "Time taken is: " << double(end-start)/double(CLOCKS_PER_SEC)<< endl;

    cout << endl << "Factoring the matrix..." << endl;
    start = clock();
    A->compute_Factor();
    end = clock();
    cout << "Time taken is: " << double(end-start)/double(CLOCKS_PER_SEC)<< endl;

    cout << endl << "Solving the system..." << endl;
    start = clock();
    A->solve(b, x);
    end = clock();
    cout << "Time taken is: " << double(end-start)/double(CLOCKS_PER_SEC)<< endl;

    double determinant;
    cout << endl << "Computing the determinant..." << endl;
    start = clock();
    A->compute_Determinant(determinant);
    end = clock();
    cout << "Time taken is: " << double(end-start)/double(CLOCKS_PER_SEC)<< endl;

    cout << endl << "Fast determinant is: " << std::setprecision(16) << determinant << endl;

    return 0;
}