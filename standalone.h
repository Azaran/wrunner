#ifndef STANDALONE_H
#define	STANDALONE_H

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "control.h"

/**
 * @brief  Reads ARGV and set variables with value from parameters. 
 * @param argc
 * @param argv
 * @param xmlFile
 */
void standalone(int argc, char **argv, std::string * xmlFile);

#endif /* STANDALONE_H */
