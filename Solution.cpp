/*
 * Solution.cpp
 */

#include "Solution.hpp"

/*
 * todo comments
 */
Solution::Solution() {

}

/*
 * initialSolution = eventuale soluzione iniziale di cui generare il vicinato. (null = genera nuova soluzione)
 * n = numero di celle da lasciare invariate nella generazione del vicinato
 * N = numero di celle in totale [parametro del problema]
 * null implica n = 0
 * populateSolution popola se stessa partendo da una initial solution di cui ne tiene n
 */
void Solution::populateSolution(Solution *initialSolution, int n, int *tasks, int sizeOfTasks, int ***people,
                                CostMatrix *costs, int N) {
    // People is a matrix Time x TypePerson x CellNumber
    // TODO: Add exception / full control of passed parameters
    int remainingTask[sizeOfTasks];
    // TODO: [available/pickable]People never used/initialized
    int *availablePeople;
    int pickablePeople;
    this->totalCost = 0;
    double keptCellProbability = ((double) n / (double) cells.size());
    for (int i = 0; i < sizeOfTasks; i++)
        remainingTask[i] = tasks[i];
    for (int i = 0; i < n; i++) {
        /*
         * Codice commentato poichè abbiamo pensato di ottimizzare
         * facendo scorrere un'unica volta la lista e controllando
         * volta per volta la probabilità che esca.
         * Inoltre, abbiamo modificato il tutto in modo da non
         * distruggere la initialSolution.
         * */
        /*int r = (int) ((rand() / RAND_MAX) * initialSolution->cells.size());     //choose which cells of previous solution has to be picked
        SolutionCell toAdd = initialSolution->cells[r]; //TODO remove cell[r]
        remainingTask[toAdd.getJ()] -= toAdd.getX();
        people[toAdd.getTime()][toAdd.getType()][toAdd.getI()] -= toAdd.getX();
        totalCost += costs->getCost(toAdd.getI(), toAdd.getJ(), toAdd.getTime(), toAdd.getType());
        addSolutionCell(toAdd);*/
        vector<SolutionCell>::iterator it;
        for (it = initialSolution->cells.begin(); it != initialSolution->cells.end(); it++) {
            double r = ((double) rand()) / RAND_MAX;
            if (r >= keptCellProbability) {
                // TODO: Valuta se usare addSolutionCell [ Meno efficiente. Probabilmente è già ordinata ]
                SolutionCell cellToAdd = *(it->clone());
                this->cells.insert(this->cells.begin(), cellToAdd);
                remainingTask[cellToAdd.getJ()] -= cellToAdd.getX();
                people[cellToAdd.getTime()][cellToAdd.getType()][cellToAdd.getI()] -= cellToAdd.getX();
                totalCost += costs->getCost(cellToAdd.getI(), cellToAdd.getJ(), cellToAdd.getType(), cellToAdd.getTime());
            }
        }
    }
    for (int j = 0; j < N; j++) {
        while (remainingTask[j] > 0) {
            int x = 0, i = 0, t = 0, m = 0;
            costs->getMinimumCost(j, &i, &m, &t, people);      //search best parameters
            switch (m) {
                //TODO fare in modo che persone di tipo 1 e 2 possano fare meno di 2 e 3 task rispettivamente
                case 0: {
                    if (remainingTask[j] >= people[t][m][i]) {
                        remainingTask -= people[t][m][i];
                        x = people[t][m][i];
                        people[t][m][i] = 0;
                    } else {
                        people[t][m][i] -= remainingTask[j];
                        remainingTask[j] = 0;
                    }
                    break;
                }
                case 1: {
                    if (remainingTask[j] >= 2 * people[t][m][i]) {
                        remainingTask -= 2 * people[t][m][i];
                        people[t][m][i] = 0;
                    } else {

                        people[t][m][i] -= remainingTask[j] / 2;
                        remainingTask[j] = remainingTask[j] % 2;
                    }
                    break;
                }
                case 2: {
                    if (remainingTask[j] >= 3 * people[t][m][i]) {
                        remainingTask -= 3 * people[t][m][i];
                        people[t][m][i] = 0;
                    } else {
                        people[t][m][i] -= remainingTask[j] / 3;
                        remainingTask[j] = remainingTask[j] % 3;
                    }
                    break;
                }
                default:;  // TODO throw exception
            }
            SolutionCell cellToAdd = SolutionCell(i, j, m, t, x);
            addSolutionCell(cellToAdd);
            // TODO: addSolutionCell must check for cases where <i,j,m,t> for the new entry equals those of an old entry
        }
    }
}

/*
 * todo comments
 */
void Solution::generateInitialSolution(int *remainingTask, int ***people, CostMatrix *costs, int N) {
    populateSolution(nullptr, 0, remainingTask, people, costs, N);
}


void Solution::addSolutionCell(SolutionCell toAdd) {
    // Adds a solution cell using an insertion sort
    vector<SolutionCell>::iterator it;
    for (it = cells.begin(); (toAdd.getI() > it->getI()) && (it != cells.end()); it++);
    if (it == cells.end()) {
        cells.push_back(toAdd);
        return;
    }
    for (; (toAdd.getI() == it->getI()) && (toAdd.getJ() > it->getJ()) && (it != cells.end()); it++);
    cells.insert(it, toAdd);
    return;
}