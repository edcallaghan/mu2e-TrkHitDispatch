// Ed Callaghan
// Maintain a possibly-faulty socket connection, and forward data when possible
// July 2025

#ifndef TrkDiag_RobustDispatcher_hh
#define TrkDiag_RobustDispatcher_hh

// system
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

// stl
#include <chrono>
#include <mutex>
#include <string>
#include <thread>

// cetlib_except
#include "cetlib_except/exception.h"

namespace mu2e{
  class RobustDispatcher{
    public:
      RobustDispatcher(std::string, unsigned int, unsigned int);

      void Dispatch(const char*, ssize_t);

    protected:
      std::string _host;
      unsigned int _port;
      int _fd;
      std::chrono::milliseconds _interval;
      std::mutex _mutex;

      void socket_connect();
      void socket_disconnect();

    private:
      /**/
  };
} // namespace mu2ee

#endif
