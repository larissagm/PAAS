#ifndef DATA_HPP
#define DATA_HPP

#include <fstream>
#include <string>
#include <vector>

class Data {

    private:
        std::string nome_arq;
        int n_turmas, n_salas;
        std::pair<int,int> par;
        std::vector<int> cap_sala, dem_turma, n_aulas;
        std::vector<std::vector<int>> horarios;
        std::vector<std::vector<std::pair<int,int>>> choque;

    public:
        Data(std::string file);
        std::string getNomeArq();
        int getNTurmas();
        int getNSalas();
        int getNAulas(int x);
        int getCapSala(int x);
        int getDemTurma(int x);
        int getNChoque(int x);
        int getChoque(int x,int y,int z);
};
#endif