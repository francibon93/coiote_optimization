//
// Created by francesco on 24/11/16.
//

#ifndef COIOTE_PROJECT_EXCEPTIONMANAGER_H
#define COIOTE_PROJECT_EXCEPTIONMANAGER_H

#import <exception>


class ExceptionManager {
public:
    /*
     * TODO WRITE DESCRIPTION HERE
     */
    ExceptionManager(Exce);

    /*
     * TODO WRITE DESCRIPTION HERE
     */
    int loadMatrix(ifstream inputFile);
    // TODO check why it makes the build fail


private:
    /*
     * TODO WRITE DESCRIPTION HERE
     */
    int dim;

    /*
     * TODO WRITE DESCRIPTION HERE
     */
    int **costs;
};


#endif //COIOTE_PROJECT_EXCEPTIONMANAGER_H