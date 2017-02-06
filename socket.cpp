#include "socket.h"

using namespace std;
using std::string;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::tcp;

boost::asio::deadline_timer * gTimer;
std::vector<class Session *> sessions;
unsigned int secondsElapsed = 0;

Session::Session(io_service& io_service, unsigned short sessionIndex) : socket_(io_service) {
    //cerr << "socket.cpp:" << __LINE__ << endl;

    this->sessionIndex = sessionIndex;
    gTimer = new deadline_timer(io_service, posix_time::seconds(1));
    //cerr << "socket.cpp:" << __LINE__ << endl;
    gTimer->async_wait(Session::regular_handler);
    //cerr << "socket.cpp:" << __LINE__ << endl;
    remains = "";
    message = "";
}

void Session::regular_handler(const system::error_code& error)
{
    //cerr << "socket.cpp:" << __LINE__ << endl;
    if(!error) {
        //cerr << "socket.cpp:" << __LINE__ << endl;
        bool atLeastOneConnected = false;
        for(class Session * session : sessions)
        {
            //cerr << "socket.cpp:" << __LINE__ << endl;
            if(session != NULL) {
                control_main(session);
                atLeastOneConnected = true;
            }
        }
        if(atLeastOneConnected == false && secondsElapsed >= 5) {
            //cerr << "socket.cpp:" << __LINE__ << endl;
            gio_service.stop();
        }
        gTimer->expires_from_now(posix_time::seconds(1));
        gTimer->async_wait(regular_handler);
    }
    else if (error == error::eof) {
        //cerr << "socket.cpp:" << __LINE__ << endl;
    }
    else if (error == error::connection_reset) {
        //cerr << "socket.cpp:" << __LINE__ << endl;
    }
    secondsElapsed++;
}

void Session::send_line(class Session * session, string message)
{
    //cerr << "socket.cpp:" << __LINE__ << endl;
    if(message.length() == 0 || message.at(message.length()-1) != '\n')
        message += '\n';
    async_write(*(session->socket2), buffer(message, message.length()), bind(&Session::handle_write, session, boost::asio::placeholders::error));
    //cerr << "socket.cpp:" << __LINE__ << endl;
}

tcp::socket& Session::socket() {
    return socket_;
}

void Session::start() {
    //cerr << "socket.cpp:" << __LINE__ << endl;
    sessions.push_back(this);
    //cerr << "socket.cpp:" << __LINE__ << endl;
    socket2 = &socket_;
    socket_.async_read_some(boost::asio::buffer(data_in, max_length), boost::bind(&Session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    //cerr << "socket.cpp:" << __LINE__ << endl;
}

void Session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    /*
    cerr << "socket.cpp:" << __LINE__ << " error:" << error << ",bytes_transferred:" << bytes_transferred << endl;
    if(error == error::eof)
        cerr << "socket.cpp:" << __LINE__ << endl;
    if(error == error::connection_reset)
        cerr << "socket.cpp:" << __LINE__ << endl;
    */
    if (!error || error == error::eof || error == error::connection_reset ) {
        //cerr << "socket.cpp:" << __LINE__ << " " << bytes_transferred << endl;
        remains.append(data_in, bytes_transferred);
        //cerr << "socket.cpp:" << __LINE__ << " " << data_in << endl;
        //cerr << "socket.cpp:" << __LINE__ << " " << remains << endl;
        
        int index = remains.find('\n');
        //cerr << "socket.cpp:" << __LINE__ << " " << index << endl;
        while(index < string::npos) {
            //cerr << "socket.cpp:" << __LINE__ << endl;
            message = remains.substr(0, index);
            //cerr << "socket.cpp:" << __LINE__ << " " << message << endl;
            if(message.at(message.length() - 1) == '\r')
                message = remains.substr(0, message.length() - 1);
            remains = remains.substr(index + 1, remains.length() - (index + 1));
            
            //cerr << "socket.cpp:" << __LINE__ << " " << message << endl;
            control_read(message);
            index = remains.find('\n');
        }
        //cerr << "socket.cpp:" << __LINE__ << endl;
    }
    //cerr << "socket.cpp:" << __LINE__ << endl;
    if(error) {
        //cerr << "socket.cpp:" << __LINE__ << endl;
        if(sessions[sessionIndex] != NULL) {
            //cerr << "socket.cpp:" << __LINE__ << endl;
            sessions[sessionIndex] = NULL;
            //delete this;
        }
        //cerr << "socket.cpp:" << __LINE__ << endl;
    }
    else {
        //cerr << "socket.cpp:" << __LINE__ << endl;
        socket_.async_read_some(boost::asio::buffer(data_in, max_length), boost::bind(&Session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        //cerr << "socket.cpp:" << __LINE__ << endl;
    }
    //cerr << "socket.cpp:" << __LINE__ << endl;
}

void Session::handle_write(const boost::system::error_code& error) {
    /*cerr << "socket.cpp:" << __LINE__ << " " << error << endl;
    if(error == error::eof)
        cerr << "socket.cpp:" << __LINE__ << endl;
    if(error == error::connection_reset)
        cerr << "socket.cpp:" << __LINE__ << endl;
    */
    if (!error) {
        //cerr << "socket.cpp:" << __LINE__ << endl;
        socket_.async_read_some(boost::asio::buffer(data_in, max_length), boost::bind(&Session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        //cerr << "socket.cpp:" << __LINE__ << endl;
    }
    else {
        //cerr << "socket.cpp:" << __LINE__ << endl;
        if(sessions[sessionIndex] != NULL) {
            //cerr << "socket.cpp:" << __LINE__ << endl;
            sessions[sessionIndex] = NULL;
            //delete this;
        }
        //cerr << "socket.cpp:" << __LINE__ << endl;
    }
    //cerr << "socket.cpp:" << __LINE__ << endl;
}


Server::Server(boost::asio::io_service& io_service) : io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), 0)) {
    tcp::endpoint le = acceptor_.local_endpoint();
    //cerr << "socket.cpp:" << __LINE__ << endl; 
    this->listeningPort = le.port();
    //cerr << "socket.cpp:" << __LINE__ << endl;
    //start_accept();
    //cerr << "socket.cpp:" << __LINE__ << endl;
}

void Server::start_accept() {
    Session* new_session = new Session(io_service_, sessions.size());
    //cerr << "socket.cpp:" << __LINE__ << endl;
    acceptor_.async_accept(new_session->socket(), boost::bind(&Server::handle_accept, this, new_session, boost::asio::placeholders::error));
    //cerr << "socket.cpp:" << __LINE__ << endl;
    io_service_.run();
}

void Server::handle_accept(Session* new_session, const boost::system::error_code& error) {
    /*cerr << "socket.cpp:" << __LINE__ << " " << error << endl;
    if(error == error::eof)
        cerr << "socket.cpp:" << __LINE__ << endl;
    if(error == error::connection_reset)
        cerr << "socket.cpp:" << __LINE__ << endl;
    */
    if (!error) {
        //cerr << "socket.cpp:" << __LINE__ << endl;
        new_session->start();
        //cerr << "socket.cpp:" << __LINE__ << endl;	
    }
    else {
        //cerr << "socket.cpp:" << __LINE__ << endl;
        if(sessions[new_session->sessionIndex] != NULL) {
            //cerr << "socket.cpp:" << __LINE__ << endl;
            sessions[new_session->sessionIndex] = NULL;
            //delete new_session;
        }
        //cerr << "socket.cpp:" << __LINE__ << endl;
    }
    //cerr << "socket.cpp:" << __LINE__ << endl;
    start_accept();
    //cerr << "socket.cpp:" << __LINE__ << endl;
}