#ifndef DATA_HPP
#define DATA_HPP

#include <fstream>
#include <string>
#include <vector>

class Data {

    private:
        std::string nome_arq;
        int n_turmas, n_salas;
        std::vector<int> cap_sala, dem_turma, n_aulas;
        std::vector<std::vector<int>> horarios;
        std::vector<std::vector<std::pair<int,int>>> choque;

    public:
        Data(std::string file);
        int getNTurmas();
        int getNSalas();
        std::string getNomeArq();
        std::vector<int> getNAulas();
        std::vector<int> getCapSala();
        std::vector<int> getDemTurma();
        std::vector<std::vector<std::pair<int,int>>> getChoque();
};
#endif