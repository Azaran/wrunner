#ifndef SOCKET_H
#define	SOCKET_H

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "boinc_api.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "resolver.h"
#include "parser.h"
#include "control.h"

class Session {
public:
    Session(boost::asio::ioService& ioService, unsigned short sessionIndex);
    static void regular_handler(const boost::system::error_code& error);
    static void send_line(class Session * sessionx, std::string message);
    boost::asio::ip::tcp::socket& socket();
    
    void start();
    
    //bool connected;
    boost::asio::ip::tcp::socket * socket2;
    unsigned short sessionIndex;
    
private:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);
    
    enum { max_length = 1024 };
    char data_in[max_length];
    std::string data_out;
    boost::asio::ip::tcp::socket socket_;
    std::string remains;
    std::string message;
}; 

class Server {
public:
    Server(boost::asio::ioService& ioService);
    unsigned short listeningPort;
    
    void start_accept();

private:
    void handle_accept(Session* newSession, const boost::system::error_code& error);
    
    boost::asio::ioService& ioSession;
    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif /* SOCKET_H */
