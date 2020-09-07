#include "Modelo.hpp"

using namespace std;

namespace operations_research{
Modelo::Modelo(const Data &data){

    status = "Not solved";
    file = data.nome_arq;
    file = file.substr(8,file.size()-13);

    solver = solver->CreateSolver("paas", "CBC_MIXED_INTEGER_PROGRAMMING");
    char name[100];

    //Variáveis

    //X_ijk = 1 se a aula j da turma i ocorrer na sala k
    x = vector<vector<vector<MPVariable*>>>(data.n_turmas);

    for (int i=0; i<data.n_turmas; i++){
        x[i] = vector<vector<MPVariable*>>(data.n_aulas[i]);
        for (int j=0; j<data.n_aulas[i]; j++){
            x[i][j] = vector<MPVariable*>(data.n_salas);
            for (int k=0; k<data.n_salas; k++){
                sprintf(name, "X(%d,%d,%d)", i, j, k);
                x[i][j][k] = solver->MakeBoolVar(name);
            }    
        }
    }

    //Y_t = 1 se as aulas da turma t ocorrerem em salas diferentes
    y = vector<MPVariable*>(data.n_turmas);

    for (int t=0; t<data.n_turmas; t++){
        sprintf(name, "Y(%d)", t);
        y[t] = solver->MakeBoolVar(name);
    }

    //Função objetivo
    obj = solver->MutableObjective();

    for (int t=0; t<data.n_turmas; t++){
        obj->SetCoefficient(y[t],1);
    }
    obj->SetMinimization();

    //Restrições

    //Todas as aulas devem ser alocadas a uma sala
    for (int i=0,r=0; i<data.n_turmas; i++){
        for (int j=0; j<data.n_aulas[i]; j++,r++){

            sprintf(name, "R1(%d)", r);
            MPConstraint* ct = solver->MakeRowConstraint(1,1,name);

            // sum_{k} X[i][j][k] = 1, para todo i e j
            for (int k=0; k<data.n_salas; k++){
                ct->SetCoefficient(x[i][j][k],1);
            }
        }
    }

    //Choque de horário
    for (int i=0,j=0,r=0; i<data.n_turmas; i++){
        for (int a=0; a<data.n_aulas[i]; a++,j++){
            for (int k=0; k<data.choque[j].size(); k++){
                for (int l=0; l<data.n_salas; l++,r++){
                    
                    sprintf(name, "R2(%d)", r);
                    MPConstraint* ct = solver->MakeRowConstraint(-solver->infinity(),1,name);

                    // X[i][a][l] + X[b][c][l] <= 1, para todas as salas l e 
                    // todas as aulas que tenham choque de horário
                    ct->SetCoefficient(x[i][a][l],1);
                    ct->SetCoefficient(x[data.choque[j][k].first][data.choque[j][k].second][l],1);
                }
            }
        }
    }
    
    //Demanda e capacidade das salas
    for (int i=0,r=0; i<data.n_turmas; i++){
        for (int j=0; j<data.n_aulas[i]; j++){
            for (int k=0; k<data.n_salas; k++,r++){

                sprintf(name,"R3(%d)",r);
                MPConstraint* ct = solver->MakeRowConstraint(-solver->infinity(),data.cap_sala[k],name);

                // demTurma(i) * X[i][j][k] <= capSala(k), para todo i, j, k
                ct->SetCoefficient(x[i][j][k],data.dem_turma[i]);
            }
        }
    }

    //Aulas de uma mesma turma alocadas a salas diferentes -> Y[t] = 1
    // Y[t] >= abs (X[i][j][l] - X[i][k][l])
    for (int i=0,r=0; i<data.n_turmas; i++){
        for (int j=0; j<data.n_aulas[i]-1; j++){
            for (int k=j+1; k<data.n_aulas[i]; k++){
                for (int l=0; l<data.n_salas; l++,r++){

                    //Variáveis auxiliares para tornar a restrição linear
                    sprintf(name,"A(%d)",r);
                    MPVariable* a = solver->MakeBoolVar(name);
                    sprintf(name,"B(%d)",r);
                    MPVariable* b = solver->MakeBoolVar(name);
                    
                    sprintf(name,"R4(%d)",r);
                    MPConstraint* ct = solver->MakeRowConstraint(0,solver->infinity(),name); 

                    // Y[i] >= a + b
                    ct->SetCoefficient(y[i],1);
                    ct->SetCoefficient(a,-1);
                    ct->SetCoefficient(b,-1);

                    sprintf(name,"R5(%d)",r);
                    MPConstraint* ct2 = solver->MakeRowConstraint(0,0,name);

                    // a - b = X[i][j][l] - X[i][k][l]
                    ct2->SetCoefficient(a,1);
                    ct2->SetCoefficient(b,-1);
                    ct2->SetCoefficient(x[i][j][l],-1);
                    ct2->SetCoefficient(x[i][k][l],1);
                }
            }
        }
    }

    ofstream arq("modelo-"+ file + ".lp");
    string modelo;

    solver->ExportModelAsLpFormat(false, &modelo);
    arq << modelo;
    arq.close();

}

void Modelo::solve(){

    //solver->set_time_limit(120000);
    const MPSolver::ResultStatus result = solver->Solve();
    
    if (result == MPSolver::NOT_SOLVED) status = "Not solved";
    else if (result == MPSolver::INFEASIBLE) status = "Infeasible";
    else if (result == MPSolver::OPTIMAL) status = "Optimal";
}

void Modelo::printSolution(){

    string nome = "resultado-" + file + ".txt";
    ofstream arq(nome);

    arq << status << "\n\n";

    if (status == "Optimal"){

        arq << "Valor objetivo: " << obj->Value() << "\n\n";

        for (int i=0; i<x.size(); i++){
            arq << "Turma " << i << "\n";

            for (int j=0; j<x[i].size(); j++){
                for (int k=0; k<x[i][j].size(); k++){

                    if (x[i][j][k]->solution_value() > 0.9){
                        arq << "Aula " << j << " na sala " << k << "\n";
                    }
                }
            }
            arq<<"\n";
        }
    }
    arq.close();
}
}