

#ifndef CONTROL_H
#define	CONTROL_H

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <boost/algorithm/string/find.hpp>

#include "parser.h"
#include "socket.h"
#include "simulation.h"

extern pid_t pid;
extern boost::asio::io_service gio_service;

void control_main(class Session * session);
void control_read(std::string message);
void create_output_file(bool boinc);
void parse_message(std::string message, std::string * part1, std::string * part2, std::string * part3);
void parse_message_simple(std::string message, std::string * part1, std::string * part2);

#endif /* CONTROL_H */
