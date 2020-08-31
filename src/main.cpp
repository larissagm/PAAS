#include "Modelo.hpp"

using namespace operations_research;

int main(int argc, char** argv){
    
    Data data(argv[1]);
    Modelo modelo(data);
    modelo.solve();
    modelo.printSolution();
    
    return 0;
}