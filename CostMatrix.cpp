/*
 * CostMatrix.cpp
 */

/*
 * TODO [DAVIDE]:
 *                Dando per scontato che la CostMatrix abbia i parametri in quest'ordine:
 *                [j][i][m][t]. Per la funzione getMinimumCost vorremmo che in fase di
 *                lettura per ogni j venisse salvata la media di ogni costo <i,m,t>.
 *                La classe CostMatrix deve avere un array di size j in cui ogni elemento
 *                ci sia la media di tutti i costi <i,m,t>.
 *                Sembra un impiccio ma porta ad un'ottimizzazione sostanziale (vedi getMinimumCost)
 *
 *                Per il calcolo della media di un enorme numero di elementi letti volta per volta
 *                abbiamo scritto la funzione welford in util.cpp
 *
 *
*/

#include "CostMatrix.hpp"
#include "util.hpp"

/*
 * This constructor initializes the costs matrix and the two average and stnd dev arrays (for welford)
 */
CostMatrix::CostMatrix(int cellsNumber, int peopleTypes, int timePeriods) {
    this->cellsNumber = cellsNumber;
    this->peopleTypes = peopleTypes;
    this->timePeriods = timePeriods;
    costs = new int ***[cellsNumber]();
    for (int j = 0; j < cellsNumber; j++) {
        costs[j] = new int **[cellsNumber]();
        for (int i = 0; i < cellsNumber; i++) {
            costs[j][i] = new int *[peopleTypes]();
            for (int m = 0; m < peopleTypes; m++) {
                costs[j][i][m] = new int[timePeriods]();
                for (int t = 0; t < timePeriods; t++) {
                    costs[j][i][m][t] = 0;
                }
            }
        }
    }
    averageCostsPerTask = new double[cellsNumber]();
    stdvCostsPerTask = new double[cellsNumber]();
    for (int i = 0; i < cellsNumber; i++) {
        averageCostsPerTask[i] = 0;
        stdvCostsPerTask[i] = 0;
    }
}

/*
 * todo comments
 */
int CostMatrix::loadMatrix(istream inputFile) {
    return 0;
}


//void CostMatrix::getMinimumCost(int j, int *i, int *m, int *t, int ***people, int tSize, int mSize, int iSize) {
//    /*
//     *      Visto che Costmatrix::averageCosts[j] ha la media dei costi per
//     *      j, posso evitare di cercare ogni volta tutta la matrice prendendo il primo
//     *      costo che sia sostanzialmente più piccolo della media per j.
//     *      Se sai qual è il costo medio cAvg, ad ogni iterazione prendi il primo
//     *      elemento elem di costMatrix <= (0.5 * cAvg)
//     */
//    // TODO: Tenere traccia di tutti i "minimi" richiesti leggendo una sola volta
//    double maxDistanceFromMin = 0.5;
//    for (int t_ = 0; t_ < tSize; t_++) {
//        for (int m_ = 0; m_ < mSize; m_++) {
//            for (int i_ = 0; i_ < iSize; i_++) {
//                if (people[t_][m_][i_] > 0) {
//                    if (this->costs[j][i_][m_][t_] <= (maxDistanceFromMin * averageCosts[j])) {
//                        *i = i_;
//                        *m = m_;
//                        *t = t_;
//                    }
//                }
//            }
//        }
//    }
//
//}

/*
 * TODO: Description
 * Note: Remember to delete the returned vector, which is allocated inside
 */
vector<CostMatrix::CostCoordinates> *
CostMatrix::getMinimumTaskCost(int j, int remainingTasksForJ, PeopleMatrix *people) {
    /*
     *      Visto che Costmatrix::averageCosts[j] ha la media dei costi per
     *      j, posso evitare di cercare ogni volta tutta la matrice prendendo il primo
     *      costo che sia sostanzialmente più piccolo della media per j.
     *      Se sai qual è il costo medio cAvg, ad ogni iterazione prendi il primo
     *      elemento elem di costMatrix <= (0.5 * cAvg)
     */
    // TODO: Tenere traccia di tutti i "minimi" richiesti leggendo una sola volta

    vector<CostMatrix::CostCoordinates> *minsVector = new vector<CostMatrix::CostCoordinates>;
    double maxDistanceFromMin = 0.5;
    int peopleTemp;
    for (int t_ = 0; t_ < timePeriods; t_++) {
        for (int i_ = 0; i_ < cellsNumber; i_++) {
            if (i_ == j) continue;
            for (int m_ = 0; m_ < peopleTypes; m_++) {
                if ((peopleTemp = people->getPeople(t_, m_, i_)) > 0) {
                    if (costs[j][i_][m_][t_] / (m_ + 1) <= (maxDistanceFromMin * averageCostsPerTask[j])) {
                        CostMatrix::CostCoordinates *currentCoordinates = new CostMatrix::CostCoordinates();
                        currentCoordinates->j = j;
                        currentCoordinates->i = i_;
                        currentCoordinates->m = m_;
                        currentCoordinates->t = t_;
                        minsVector->push_back(*currentCoordinates);
                        if ((remainingTasksForJ -= (m_ + 1) * peopleTemp) <= 0) {
                            /*
                             * Solution 1: Guarantees no waste of resources (people) at the
                             * computational cost of extending the reasarch at the end (last
                             * cells)
                             */

                            /*
                            if (remainingTasksForJ != 0) {
                                minsVector->pop_back();
                                remainingTasksForJ += (m_ + 1) * peopleTemp;
                            } else {
                                return minsVector;
                            }
                             */

                            /*
                             * Solution 2: here the remainingTasks might be negative, in the
                             * sense that a person of type 2 [1] or 3 [2] is being used for
                             * less than the max jobs that could accomplish. The advantage is
                             * that the research is the minimum for a given distanceFromAvg
                             */
                            return minsVector;
                        }
                    }
                }
            }
        }
    }
}

/*
 * TODO: Description
 * Note: Remember to delete the returned vector, which is allocated inside
 */
vector<CostMatrix::CostCoordinates> *CostMatrix::getMinimumTaskCostDiversified(int j, int remainingTasksForJ,
                                                                               PeopleMatrix *people) {
    /*
     *      Visto che Costmatrix::averageCosts[j] ha la media dei costi per
     *      j, posso evitare di cercare ogni volta tutta la matrice prendendo il primo
     *      costo che sia sostanzialmente più piccolo della media per j.
     *      Se sai qual è il costo medio cAvg, ad ogni iterazione prendi il primo
     *      elemento elem di costMatrix <= (0.5 * cAvg)
     */
    // TODO: Tenere traccia di tutti i "minimi" richiesti leggendo una sola volta

    vector<CostMatrix::CostCoordinates> *minsVector = new vector<CostMatrix::CostCoordinates>;

    double maxDistanceFromMin = 0.5;
    int peopleTemp;

    for (int t_ = 0; t_ < timePeriods; t_++) {
        for (int i_ = 0; i_ < cellsNumber; i_++) {
            for (int m_ = 0; m_ < peopleTypes; m_++) {
                if ((peopleTemp = people->getPeople(t_, m_, i_)) > 0) {
                    if (this->costs[j][i_][m_][t_] / (m_ + 1) <= (maxDistanceFromMin * averageCostsPerTask[j])) {
                        CostCoordinates *currentCoordinates = new CostCoordinates();
                        currentCoordinates->j = j;
                        currentCoordinates->i = i_;
                        currentCoordinates->m = t_;
                        currentCoordinates->t = m_;
                        minsVector->push_back(*currentCoordinates);
                        if ((remainingTasksForJ -= (m_ + 1)) <= 0) {
                            /*
                             * Solution 1: Guarantees no waste of resources (people) at the
                             * computational cost of extending the reasarch at the end (last
                             * cells)
                             */
                            if (remainingTasksForJ != 0) {
                                minsVector->pop_back();
                                remainingTasksForJ += (m_ + 1);
                            } else {
                                return minsVector;
                            }
                            /*
                             * Solution 2: here the remainingTasks might be negative, in the
                             * sense that a person of type 2 [1] or 3 [2] is being used for
                             * less than the max jobs that could accomplish. The advantage is
                             * that the research is the minimum for a given distanceFromAvg
                             */
//                           return minsVector;
                        }
                    }
                }
            }
        }
    }
}

int CostMatrix::getCost(int j, int i, int m, int t) {
    return costs[j][i][m][t];
}

/*
 * This method returns the average cost per task for the given destination cell "j".
 */
int CostMatrix::getAvgCostsPerTask(int j) {
    return averageCostsPerTask[j];
}

/*
 * It returns the internal representation of the cost matrix. Should be used with caution.
 */
int ****CostMatrix::getMatrix() {
    return costs;
}

/*
 * It sets the value of the cost matrix corrisponding to cell "i" "j", person type "m", time period "t".
 */
void CostMatrix::setValue(int j, int i, int m, int t, int value) {
    costs[j][i][m][t] = value;
}

/*
 * Updates the avg and the stdv of costs-per-task. The param "index" is the number of the value so that the first
 * value passed (independently from the j) will have index 0, the second value passed will have index 1 and so on.
 * The last value to be inserted will have "index" = N_of_cells - 1.
 * Remember to divide the cost by the number of tasks that the person of that type can perform. In this way
 * "newValue" will hold the costPerTask (Example: if for a person of type m = 2 the cost is 30, then the cost per
 * task will be 10 because that person can fullfill 3 tasks for that single bigger cost)
 */
void CostMatrix::updateAvgCostsPerTask(int j, int newValue, long index) {
    welford(newValue, averageCostsPerTask + j, stdvCostsPerTask + j, index);
}

//int CostMatrix::getSize() {
//    return size;
//}


void CostMatrix::print() {
    for (int t = 0; t < timePeriods; t++) {
        for (int m = 0; m < peopleTypes; m++) {
            std::cout << t << " " << m << std::endl;
            for (int j = 0; j < cellsNumber; j++) {
                for (int i = 0; i < cellsNumber; i++) {
                    std::cout << costs[j][i][m][t] << " ";
                }
                std::cout << std::endl;
            }
        }
    }
}

