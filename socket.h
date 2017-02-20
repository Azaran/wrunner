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
    /**
     * @brief   Session constructor.
     * @param io_service&ioService
     * @param sessionIndex
     */
    Session(boost::asio::io_service& ioService, unsigned short sessionIndex);

    /**
     * @brief	Periodically checks whether the session is still active and end
     *		tool is running and is responding.
     * @param error
     */
    static void regular_handler(const boost::system::error_code& error);

    /**
     * @brief   Sends message to the session.
     * @param sessionx
     * @param message
     */
    static void send_line(class Session * sessionx, std::string message);
    
    /**
     * @brief   Starts the session.
     */
    void start();
    
    boost::asio::ip::tcp::socket& socket();
    //bool connected;
    boost::asio::ip::tcp::socket * socket2;
    unsigned short sessionIndex;
    
private:
    /**
     * @brief   Handles reading from message properly in safe way. 
     * @param error
     * @param bytesTransfered
     */
    void handle_read(const boost::system::error_code& error, size_t bytesTransfered);

    /**
     * @brief   Handles writing into message properly.
     * @param error
     */
    void handle_write(const boost::system::error_code& error);
    
    enum { maxLength = 1024 };
    char dataIn[maxLength];
    std::string dataOut;
    boost::asio::ip::tcp::socket socket_;
    std::string remains;
    std::string message;
};

class Server {
public:
    /**
     * @brief   Constructor of Server.
     * @param io_service&ioService
     */
    Server(boost::asio::io_service& ioService);
    unsigned short listeningPort;
    
    /**
     * @brief   Makes new session and starts accepting asynchronously.
     */
    void start_accept();

private:
    /**
     * @brief   Handles start of server's acceptance. Starts session and server
     *		asynchronous accept.
     * @param newSession
     * @param error
     */
    void handle_accept(Session* newSession, const boost::system::error_code& error);
    
    boost::asio::io_service& ioSession;
    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif /* SOCKET_H */
