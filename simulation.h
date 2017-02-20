#ifndef SIMULATION_H
#define	SIMULATION_H

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief   Sends control messages to BOINC client. Works like basic
 *	    substitution of BOINC manager and allows to send control commands at
 *	    given times.
 * @param index
 * @param counter
 * @return result 
 */
std::string simulation(int index, unsigned long long int counter);

#endif /* SIMULATION_H */
