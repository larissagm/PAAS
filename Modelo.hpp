#ifndef MODELO_HPP
#define MODELO_HPP

#include <ortools/linear_solver/linear_solver.h>
#include "Data.hpp"

namespace operations_research{
class Modelo{

    private:
        std::string file, status;
        int n_turmas, n_salas, objetivo;
        std::vector<int> cap_sala, dem_turma, n_aulas;
        std::vector<std::vector<std::pair<int,int>>> choque;
        std::vector<std::vector<int>> solucao;

    public:
        Modelo(Data data);
        void solve();
        void printSolution();
};
}
#endif