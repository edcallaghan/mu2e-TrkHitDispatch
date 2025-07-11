// Ed Callaghan
// Maintain a possibly-faulty socket connection, and forward data when possible
// July 2025

#include "TrkHitDispatch/inc/RobustDispatcher.hh"

namespace mu2e{
  RobustDispatcher::RobustDispatcher(std::string host, unsigned int port,
                                     unsigned int interval):
      _host(host), _port(port), _fd(-1), _interval(interval){
    signal(SIGPIPE, SIG_IGN);
    auto f = [this] () {
      while (true){
        if (_fd < 0){
          std::lock_guard<std::mutex> lock(_mutex);
          this->socket_connect();
        }
        std::this_thread::sleep_for(_interval);
      }
    };
    std::thread reconnect(f);
    reconnect.detach();
  }

  void RobustDispatcher::Dispatch(const char* buf, ssize_t count){
    if (-1 < _fd){
      std::lock_guard<std::mutex> lock(_mutex);
      if (write(_fd, buf, count) < count){
        this->socket_disconnect();
        if (errno == EINTR){
          std::string msg = "received EINTR";
          throw cet::exception("TrkHitDispatch") << msg << std::endl;
        }
        /*
        if (errno == EPIPE){
          std::string msg = "received EPIPE";
          throw cet::exception("TrkHitDispatch") << msg << std::endl;
        }
        */
      }
    }
  }

  void RobustDispatcher::socket_connect(){
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0){
      std::string msg = "failed to reserve socket";
      throw cet::exception("RobustDispatcher") << msg;
    }

    fcntl(_fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    struct hostent* entry = gethostbyname(_host.c_str());
    memcpy(entry->h_addr, &(address.sin_addr), entry->h_length);
    address.sin_port = htons(_port);
    if (connect(_fd, (struct sockaddr*) &address, sizeof(address)) < 0){
      std::string errmsg = "no error";
      // if socket is connecting, then poll until writeable
      if (errno == EINPROGRESS){
        fd_set set;
        FD_ZERO(&set);
        FD_SET(_fd, &set);
        struct timeval timeout{1, 0};
        int selected;
        while ((selected = select(_fd + 1, NULL, &set, NULL, &timeout)) < 1){
          /**/
        }
        if (!(FD_ISSET(_fd, &set))){
          std::string msg = "inconsistent select() return";
          throw cet::exception("RobustDispatcher") << msg << std::endl;
        }
      }
      // but if connection is plain refused, then just ignore this attempt
      else if (errno == ECONNREFUSED){
        _fd = -1;
      }
      // otherwise we have an unexpected error, and should abort
      else{
        if (errno == EADDRINUSE)    errmsg = "EADDRINUSE";
        if (errno == EADDRNOTAVAIL) errmsg = "EADDRNOTAVAIL";
        if (errno == EAGAIN)        errmsg = "EAGAIN";
        if (errno == EINTR)         errmsg = "EINTR";
        if (errno == EISCONN)       errmsg = "EISCONN";
        if (errno == ENETUNREACH)   errmsg = "ENETUNREACH";
        if (errno == ETIMEDOUT)     errmsg = "ETIMEDOUT";
        std::string msg = "unable to connect to "
                        + _host + ":" + std::to_string(_port)
                        + " (" + errmsg + ")";
        throw cet::exception("RobustDispatcher") << msg;
      }
    }
  }

  void RobustDispatcher::socket_disconnect(){
    if (-1 < _fd){
      close(_fd);
    }
    _fd = -1;
  }
} // namespace mu2e
