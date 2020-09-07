#ifndef MODELO_HPP
#define MODELO_HPP

#include <ortools/linear_solver/linear_solver.h>
#include "Data.hpp"

namespace operations_research{
class Modelo{

    private:
        std::string status, file;
        MPSolver *solver;
        std::vector<std::vector<std::vector<MPVariable*>>> x;
        std::vector<MPVariable*> y;
        MPObjective* obj;

    public:
        Modelo(const Data &data);
        void solve();
        void printSolution();
};
}
#endif