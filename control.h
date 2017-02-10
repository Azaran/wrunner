#ifndef CONTROL_H
#define	CONTROL_H

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <boost/algorithm/string/find.hpp>

#include "parser.h"
#include "socket.h"
#include "simulation.h"

#define COUNTERMAX 100000000
#define HEARTBEATMAX 160000000000 //16
#define BENCHMARK_TIMEOUT 60
#define DIVIDER ';'

extern pid_t pid;
extern boost::asio::io_service gio_service;

/** 
 * @brief   Prepares report for the server. 
 * @return  result
 */
std::string generate_output_content() ;

/**
 * @brief   Makes file 'out' with the report which is sent the server and also prints the
 * report on the STDERR.
 * @param boinc
 */
void create_output_file(bool boinc);

/**
 * @brief   Counts time since last 'heartbeat'.
 * @return  counter - lastHeartbeat
 *	    COUNTERMAX - lastHeartbeat + counter
 */
int heartbeat_diff();

/**
 * @brief  Communicates with BOINC manager and controls the task's status.
 * @param session
 */
void control_main(class Session * session);

/**
 * @brief   Parses Session message containing up to 3 divided values.
 * @param message
 * @param part1
 * @param part2
 * @param part3
 */
void parse_message(std::string message, std::string * part1, std::string * part2, std::string * part3);

//void parse_message2(std::string message, std::string * part1, std::string * part2, std::string * part3);

/**
 * @brief   Parses line of parameters from the task parameters. 
 * @param message
 * @param part1
 * @param part2
 */
void parse_message_simple(std::string message, std::string * part1, std::string * part2);

/**
 * @brief  Reads Session message and executes action based on was in the
 * message. 
 * @param message
 */
void control_read(std::string message);

#endif /* CONTROL_H */
