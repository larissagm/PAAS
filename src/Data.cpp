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

        arq >> dem_turma[i];

        //Horários da turma i
        std::string horario;
        arq >> horario;

        //Conversão do horário (string) para inteiros,
        //armazenados em horarios[i] para cada turma i
        for(int j=0, f=0; j<horario.size(); j++){
			if (isalpha(horario[j])){		
				int c=0;
				for (int k=f; k<j; k++){
					for (int l=j+1; l<horario.size(); l++){
                        //Fim da leitura atual
						if (horario[l]=='-') {
							c=l;
							break;
						}
                        
						//Cálculo do horário
						int x = 15*horario[k]+horario[l]-798;
						if (horario[j]=='T') x+=5;
						else if (horario[j]=='N') x+=11;

						horarios[i].push_back(x);
					}
				}
                //Continua a leitura do próximo horário
				if (c!=0) {
					f=c+1;
					j=c;
				}
				else break;
			}
		}
        n_aulas[i] = horarios[i].size();
        total_aulas+=n_aulas[i];
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

std::vector<int> Data::getNAulas(){
    return n_aulas;
}

std::vector<int> Data::getCapSala(){
    return cap_sala;
}

std::vector<int> Data::getDemTurma(){
    return dem_turma;
}

std::vector<std::vector<std::pair<int,int>>> Data::getChoque(){
    return choque;
}