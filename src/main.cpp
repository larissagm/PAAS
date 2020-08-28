#include <ortools/linear_solver/linear_solver.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Data.hpp"

using namespace std;

namespace operations_research{
void solve(Data &data){

    MPSolver solver("paas", MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);
    //solver.set_time_limit(120000);
    char name[100];

    //Variáveis

    //X_ijk = 1 se a aula j da turma i ocorrer na sala k
    vector<vector<vector<MPVariable*>>> x(data.getNTurmas());

    for (int i=0; i<data.getNTurmas(); i++){
        x[i] = vector<vector<MPVariable*>>(data.getNAulas(i));
        for (int j=0; j<data.getNAulas(i); j++){
            x[i][j] = vector<MPVariable*>(data.getNSalas());
            for (int k=0; k<data.getNSalas(); k++){
                sprintf(name, "X(%d,%d,%d)", i, j, k);
                x[i][j][k] = solver.MakeBoolVar(name);
            }    
        }
    }

    //Y_t = 1 se as aulas da turma t ocorrerem em salas diferentes
    vector<MPVariable*> y(data.getNTurmas());

    for (int t=0; t<data.getNTurmas(); t++){
        sprintf(name, "Y(%d)", t);
        y[t] = solver.MakeBoolVar(name);
    }

    //Função objetivo
    MPObjective* obj = solver.MutableObjective();

    for (int t=0; t<data.getNTurmas(); t++){
        obj->SetCoefficient(y[t],1);
    }
    obj->SetMinimization();

    //Restrições

    //Todas as aulas devem ser alocadas a uma sala
    for (int i=0,r=0; i<data.getNTurmas(); i++){
        for (int j=0; j<data.getNAulas(i); j++,r++){

            sprintf(name, "R1(%d)", r);
            MPConstraint* ct = solver.MakeRowConstraint(1,1,name);

            // sum_{k} X[i][j][k] = 1, para todo i e j
            for (int k=0; k<data.getNSalas(); k++){
                ct->SetCoefficient(x[i][j][k],1);
            }
        }
    }

    //Choque de horário
    for (int i=0,j=0,r=0; i<data.getNTurmas(); i++){
        for (int a=0; a<data.getNAulas(i); a++,j++){
            for (int k=0; k<data.getNChoque(j); k++){
                for (int l=0; l<data.getNSalas(); l++,r++){
                    
                    sprintf(name, "R2(%d)", r);
                    MPConstraint* ct = solver.MakeRowConstraint(-solver.infinity(),1,name);

                    // X[i][a][l] + X[b][c][l] <= 1, para todas as salas l e 
                    // todas as aulas que tenham choque de horário
                    ct->SetCoefficient(x[i][a][l],1);
                    ct->SetCoefficient(x[data.getChoque(j,k,1)][data.getChoque(j,k,2)][l],1);
                }
            }
        }
    }
    

    //Demanda e capacidade das salas
    for (int i=0,r=0; i<data.getNTurmas(); i++){
        for (int j=0; j<data.getNAulas(i); j++){
            for (int k=0; k<data.getNSalas(); k++,r++){

                sprintf(name,"R3(%d)",r);
                MPConstraint* ct = solver.MakeRowConstraint(-solver.infinity(),data.getCapSala(k),name);

                // demTurma(i) * X[i][j][k] <= capSala(k), para todo i, j, k
                ct->SetCoefficient(x[i][j][k],data.getDemTurma(i));
            }
        }
    }

    //Aulas de uma mesma turma alocadas a salas diferentes -> Y[t] = 1
    // Y[t] >= abs (X[i][j][l] - X[i][k][l])
    for (int i=0,r=0;i<data.getNTurmas();i++){
        for (int j=0;j<data.getNAulas(i)-1;j++){
            for (int k=j+1;k<data.getNAulas(i);k++){
                for (int l=0;l<data.getNSalas();l++,r++){

                    //Variáveis auxiliares para tornar a restrição linear
                    sprintf(name,"A(%d)",r);
                    MPVariable* a = solver.MakeBoolVar(name);
                    sprintf(name,"B(%d)",r);
                    MPVariable* b = solver.MakeBoolVar(name);
                    
                    sprintf(name,"R4(%d)",r);
                    MPConstraint* ct = solver.MakeRowConstraint(0,solver.infinity(),name); 

                    // Y[i] >= a + b
                    ct->SetCoefficient(y[i],1);
                    ct->SetCoefficient(a,-1);
                    ct->SetCoefficient(b,-1);

                    sprintf(name,"R5(%d)",r);
                    MPConstraint* ct2 = solver.MakeRowConstraint(0,0,name);

                    // a - b = X[i][j][l] - X[i][k][l]
                    ct2->SetCoefficient(a,1);
                    ct2->SetCoefficient(b,-1);
                    ct2->SetCoefficient(x[i][j][l],-1);
                    ct2->SetCoefficient(x[i][k][l],1);
                }
            }
        }
    }

    string file = data.getNomeArq();
    file = file.substr(8,file.size()-13);

    ofstream arq("modelo-"+ file + ".lp");
    string modelo;

    solver.ExportModelAsLpFormat(false, &modelo);
    arq << modelo;
    arq.close();


    //Resolve o modelo
    const MPSolver::ResultStatus result = solver.Solve();

    if (result == MPSolver::NOT_SOLVED) cout << "Not solved\n\n";
    else if (result == MPSolver::INFEASIBLE) cout << "Infeasible\n\n";
    else if (result == MPSolver::OPTIMAL) {
        cout << "Optimal\n\n";

        //Solução

        string nome = "resultado-" + file + ".txt";
        ofstream arq(nome);

        arq << "Valor objetivo: " << obj->Value() << "\n\n";
        
        for (int i=0; i<data.getNTurmas(); i++){
            arq << "Turma " << i << "\n";
            for (int j=0; j<data.getNAulas(i); j++){
                for (int k=0; k<data.getNSalas(); k++){
                    if (x[i][j][k]->solution_value() > 0.9) {
                        arq << "Aula " << j << " na sala " << k << "\n";
                    }
                }
            }
            arq<<"\n";
        }
        arq.close();
    }
}
}

int main(int argc, char** argv){
    
    Data data(argv[1]);
    operations_research::solve(data);
    return 0;
}