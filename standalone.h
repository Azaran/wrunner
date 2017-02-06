#ifndef STANDALONE_H
#define	STANDALONE_H

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "control.h"

void standalone(int argc, char **argv, std::string * xmlFile);

#endif /* STANDALONE_H */
