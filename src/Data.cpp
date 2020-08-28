#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Data.hpp"

Data::Data(std::string file){
    
    nome_arq = file;
    std::ifstream arq(file);
    arq >> n_turmas >> n_salas;

    cap_sala = std::vector<int>(n_salas);
    dem_turma = std::vector<int>(n_turmas);
    n_aulas = std::vector<int>(n_turmas);
    horarios = std::vector<std::vector<int> >(n_turmas);

    for (int i=0; i<n_salas; i++){
        arq >> cap_sala[i];
    }

    int total_aulas=0;

    for (int i=0; i<n_turmas; i++){

        arq >> dem_turma[i] >> n_aulas[i];
        total_aulas+= n_aulas[i];

        //Horários da turma i
        horarios[i]=std::vector<int>(n_aulas[i]);

        for (int j=0;j<n_aulas[i];j++){
            arq >> horarios[i][j];
        }
    }

    //Choques de horário

    /*  
        choque[x] contém todos os pares (turma, aula) que dão choque com a aula x.
        Nessa representação, as aulas vão de 0 a total_aulas-1, de modo que:
        Aulas da turma 0: 0 a n_aulas[0]-1
        Aulas da turma x: n_aulas[x-1] a n_aulas[x]-1 
    */

    choque = std::vector<std::vector<std::pair<int,int>>>(total_aulas);

    for (int i=0,j=0; i<n_turmas-1; i++){
        for (int a=0; a<n_aulas[i]; a++,j++){
            for (int k=i+1; k<n_turmas; k++){
                for (int l=0; l<n_aulas[k]; l++){
                    if (horarios[i][a] == horarios[k][l]){
                        choque[j].push_back({k,l});
                    }
                }
            }
        }
    }

    arq.close();
}

std::string Data::getNomeArq(){
    return nome_arq;
}

int Data::getNTurmas(){
    return n_turmas;
}

int Data::getNSalas(){
    return n_salas;
}

int Data::getNAulas(int x){
    return n_aulas[x];
}

int Data::getCapSala(int x){
    return cap_sala[x];
}

int Data::getDemTurma(int x){
    return dem_turma[x];
}

int Data::getNChoque(int x){
    return choque[x].size();
}

int Data::getChoque(int x, int y, int z){
    if (z==1) return choque[x][y].first;
    else return choque[x][y].second;
}
