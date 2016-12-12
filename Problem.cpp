/*
 * Problem.cpp
 */

#include "Dummy.hpp"
#include "Problem.hpp"
//#include "Exceptions/Headers/CoioteException.hpp"

using namespace std;

/*
 * constructor: gets inputPath and outputPath from command line then call private method load() to load attributes
 */
Problem::Problem(string inputPath, string outputPath) {
    this->inputPath = inputPath;
    this->outputPath = outputPath;

    // following attributes are loaded by load()
    cellsNumber = 0;
    timePeriods = 0;
    peopleTypes = 0;
    tasksUserCanDo = nullptr;
    costs = nullptr;
    totalTasks = nullptr;
    people = nullptr;

    this->load();

    //printInputFileRead();
}

/*
 * load the attributes for the problem reading the input file
 */
void Problem::load() {
    string line;    // contains a line of the Input file
    string word;    // contains a single word (into a line)
    int m;          // index for peopleTypes
    int i;          // index for cellsNumber

    // open input file
    ifstream inputFileStream(inputPath.c_str());

    if (!inputFileStream.is_open()) {
        // todo x francesco: lancia eccezione
        //throw new CoioteException("Error: unable to open " + Problem::inputPath);

        cout << "Error: unable to open " + inputPath + "\n";
        return;
    }

    /*
     * LOADING cellsNumber, timePeriods, peopleTypes
     */
    getline(inputFileStream, line);
    istringstream inputStringStream1(line);

    inputStringStream1 >> word;
    cellsNumber = atoi(word.c_str());
    inputStringStream1 >> word;
    timePeriods = atoi(word.c_str());
    inputStringStream1 >> word;
    peopleTypes = atoi(word.c_str());

    // jump empty line
    getline(inputFileStream, line);

    /*
     * LOADING tasksUserCanDo
     */
    getline(inputFileStream, line);
    istringstream inputStringStream2(line);
    tasksUserCanDo = new int[peopleTypes];

    for (m = 0; m < peopleTypes; m++) {
        inputStringStream2 >> word;
        tasksUserCanDo[m] = atoi(word.c_str());
    }

    // jump empty line
    getline(inputFileStream, line);

    /*
     * LOADING costs
     */
    costs = new CostMatrix(cellsNumber, peopleTypes, timePeriods);
    if (!costs->load(&inputFileStream)) {
        // todo x francesco: lancia eccezione
        //throw new CoioteException("Error: unable to load cost matrix");

        cout << "Error: unable to load cost matrix\n";
        return;
    }

    // jump the empty line
    getline(inputFileStream, line);

    /*
      * LOADING totalTasks
      */
    getline(inputFileStream, line);
    istringstream inputStringStream3(line);
    totalTasks = new int[cellsNumber];
    for (i = 0; i < cellsNumber; i++) {
        inputStringStream3 >> word;
        totalTasks[i] = atoi(word.c_str());
    }

    /*
     * LOADING people
     */
    this->people = new PeopleMatrix(timePeriods, peopleTypes, cellsNumber);
    if (!people->load(&inputFileStream)) {
        // todo x francesco: lancia eccezione
        //throw new CoioteException("Error: unable to load cost matrix");

        cout << "Error: unable to load cost matrix\n";
        return;
    }

    return;
}

/*
 * test function for printing input file read into a new file
 */
void Problem::printInputFileRead () {
    ofstream outputFileStream("/home/riccardo/ClionProjects/coiote_optimization/Resources/Output/test.txt");
    int m, i, j, t, ****costMatrix, ***peopleMatrix;

    outputFileStream << cellsNumber << " " << timePeriods << " " << peopleTypes << "\n\n";

    for (m = 0; m < peopleTypes; m++) {
        outputFileStream << tasksUserCanDo[m] << " ";
    }
    outputFileStream << "\n\n";

    costMatrix = costs->getMatrix();
    for (m = 0; m < peopleTypes; m++) {
        for (t = 0; t < timePeriods; t++) {
            outputFileStream << m << " " << t << "\n";
            for (i = 0; i < cellsNumber; i++) {
                for (j = 0; j < cellsNumber; j++) {
                    outputFileStream << costMatrix[j][i][m][t] << " ";
                }
            }
            outputFileStream << "\n";
        }
    }
    outputFileStream << "\n";

    for (i = 0; i < cellsNumber; i++) {
        outputFileStream << totalTasks[i] << " ";
    }
    outputFileStream << "\n\n";

    peopleMatrix = people->getPeopleMatrix();
    for (m = 0; m < peopleTypes; m++) {
        for (t = 0; t < timePeriods; t++) {
            outputFileStream << m << " " << t << "\n";
            for (i = 0; i < cellsNumber; i++) {
                outputFileStream << peopleMatrix[t][m][i] << " ";
            }
            outputFileStream << "\n";
        }
    }
    outputFileStream << "\n";
    outputFileStream.close();
}

/*
 * The following constructor is temporary, it is used for testing the program before the loader implemmentation,
 * where the parameters are hard-coded and the data is generated by the Dummy class functions.
 */
Problem::Problem(int cellsNumber, int peopleTypes, int timePeriods) {
    this->cellsNumber = cellsNumber;
    this->peopleTypes = peopleTypes;
    this->timePeriods = timePeriods;
    costs = new CostMatrix(cellsNumber, peopleTypes, timePeriods);
    people = new PeopleMatrix(timePeriods, peopleTypes, cellsNumber);
    dummyLoad();
}

/*
 * Just a temporary method that fills the problem data with randomly generated data
 */
void Problem::dummyLoad() {
    cout << "dummyLoad() called\n";
    int MAX_TASKS = 50;
    Dummy::fillCostMatrix(costs, cellsNumber, timePeriods, peopleTypes);

    Dummy::fillPeopleMatrix(people, cellsNumber, timePeriods, peopleTypes, MAX_TASKS, MAX_TASKS * 2);
    totalTasks = Dummy::allocateAndFillTasksArray(cellsNumber, MAX_TASKS);
}

/*
 * generate initial solution and solve the problem with population-based simulated annealing
 * Parameters meaning explained in-line
 */
int Problem::solve(int populationDimension, int eliteDimension) {
    cout << "solve() called, attempting to solve the problem\n";

    Solution *initialSolution = new Solution(people);
    vector<int> eliteIndexes;                       //The elite is an vector of the indexes of the best solution
    bool flag;                                      //Flag used to distinguish solutions in the elite
    int invariantCellsOfNeighborhoodRatio = 4;      //The invariant cells number is given by (sizeOfPreviousSolution)/(invariantCellsOfNeighborhoodRatio)
    int stepsPerWave = 2;                           //A step means a decrement of temperature
    int stableWaves = 0;                            //Parameter necessary to stop the algorithm
    int maxStableWaves = 20;                        //Parameter necessary to stop the algorithm

    initialSolution->generateInitialSolution(totalTasks, cellsNumber, people, costs, cellsNumber);
    currentBestSolution = initialSolution;          //at the beginning the best solution is the initial solution
    /*
     * below a population of instances of standard Simulation Annealing is declared, each one with his current solution stored inside,
     * provided in the first wave of the algorithm by making a copy (with copy-constructor) of the initial solution generated above;
     */
    simAnnealings = new SimulatedAnnealing[populationDimension]();

    //First wave is performed separatly to be able to assign each instance of Simulated Annealing his own starting solution
    for (int i = 0; i < populationDimension; i++) {
        simAnnealings[i].setInitialSolution(new Solution(*initialSolution));
        simAnnealings[i].getCurrSolution()->setSolutionPeople(*people);
        simAnnealings[i].resetTemperature();                                                //temperature set to max
        simAnnealings[i].run(invariantCellsOfNeighborhoodRatio, totalTasks, cellsNumber, people, costs, cellsNumber,
                             stepsPerWave);
    }
    for (int i = 0; i < populationDimension; i++) {
        cout << simAnnealings[i].getCurrSolution()->evaluate() << endl;
    }

    updateElite(simAnnealings, populationDimension, &eliteIndexes, eliteDimension);
    //end of first wave, the elite contains the best solutions up to now
    //successive waves, the cycle is stopped when the best solution has not been updated for "maxStableWaves" consecutive waves
    int wave = 0;
    while (stableWaves < maxStableWaves) {
        //cout << "wave: " << wave++ << endl;
        for (int i = 0; i < populationDimension; i++) {
            flag = false;
            for (int j = 0; j < eliteDimension; j++)
                if (i == eliteIndexes[j]) flag = 1;       //checking if this solution is part of the elite

            if (flag) {     //if the Solution belongs to the elite the Simulated Annealing simply resume from the point it left
                simAnnealings[i].run(invariantCellsOfNeighborhoodRatio, totalTasks, cellsNumber,
                                     people, costs, cellsNumber, stepsPerWave);
            } else {          //if the Solution doesn't belongs to the elite it restart at max temperatur with currentBestSolution as starting Solution
                simAnnealings[i].setInitialSolution(new Solution(*currentBestSolution));
                simAnnealings[i].resetTemperature();
                simAnnealings[i].run(invariantCellsOfNeighborhoodRatio, totalTasks, cellsNumber,
                                     people, costs, cellsNumber, stepsPerWave);
            }
        }//end of wave;
        if (updateElite(simAnnealings, populationDimension, &eliteIndexes, eliteDimension)) {
            stableWaves = 0;
        } else stableWaves++;
    }
    return 0;
}

/*
 *      This method updates the elite with the current best solutions found after a new wave of the algorithm.
 *      It eventually also update the general current Best Solution of the Problem
 *
 *      Parameters:
 *      simAnnealingInstances -> this parameter is required to have access to all current solutions in the population and their evaluations
 *      elite -> this is a vector storing the indexes in simAnnealingInstances of the members of the elite.
 *      using std::vector for a better implementation of insertion sort
 *
 *      This method return true if the best solution of the problem has been updated. False otherwise
 */
bool Problem::updateElite(SimulatedAnnealing *simAnnealingInstances, int simAnnealingDimension, vector<int> *elite,
                          int eliteDim) {
    vector<int>::iterator it;
    bool flag = 0;
    for (int i = 0; i < simAnnealingDimension; i++) {
        it = elite->begin(); //if elite is empty elite.begin() == elite.end()
        if (elite->size() == 0) it = elite->end();
        while ((it != elite->end()) && (simAnnealings[i].getCurrSolution()->evaluate() >
                                        simAnnealings[*it].getCurrSolution()->evaluate()))
            it++;
        //con && se l'iteratore esce dalla dimensine massima di elite, non viene effettuato il secondo controllo
        elite->insert(it, i);
        if (elite->size() > eliteDim)
            elite->pop_back();           //riporta la dimensione dell'elite a quella corretta
    }
    //checking if current best solution has to be updated.
    if (simAnnealings[elite->at(0)].getCurrSolution()->evaluate() < currentBestSolution->evaluate()) {
        currentBestSolution = simAnnealings[elite->at(0)].getCurrSolution();
        cout << "bestSolution Updated! cost now: " << currentBestSolution->evaluate() << endl;
        flag = true;
    }
    return flag;
}

Solution *Problem::getBestSolution() {
    return currentBestSolution;
}
