/*
 * prifleges.h
 *
 *  Created on 11 April 2014
 *      By: Tim Gerety
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

#ifndef CPRIVLEGES_H_
#define CPRIVLEGES_H_

class CPrivleges
{
public:
    // Constructors
    CPrivleges();
    CPrivleges(string file);
    //virtual ~CPrivleges();

    // Calls
    bool checkUsr(string usr);

private:
    // Config variables
    string privFile;
    vector<string> admins;

    bool loadFile();
};

#endif /* CPRIVLEGES_H_ */
