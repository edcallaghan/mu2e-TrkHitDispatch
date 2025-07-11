// Ed Callaghan
// Dispatch reduced hit information over a network socket
// July 2025

// system
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stddef.h>
#include <time.h>

// art
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

// cetlib_except
#include "cetlib_except/exception.h"

// fhiclcpp
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"

// mu2e
#include "Offline/RecoDataProducts/inc/StrawDigi.hh"
#include "TrkHitDispatch/inc/RobustDispatcher.hh"

namespace mu2e{
  class TrkHitDispatch: public art::EDFilter{
    public:
      struct Config{
        fhicl::Atom<std::string> host{
          fhicl::Name("host"),
          fhicl::Comment("Remote host to connect to")
        };
        fhicl::Atom<unsigned int> port{
          fhicl::Name("port"),
          fhicl::Comment("Remote port to connect to")
        };
        fhicl::Atom<art::InputTag> tag{
          fhicl::Name("StrawDigiCollection"),
          fhicl::Comment("StrawDigiCollection to dispatch over network")
        };
      };

      using Parameters = art::EDFilter::Table<Config>;
      TrkHitDispatch(const Parameters&);

      virtual void beginJob();
      virtual void endJob();
      virtual bool filter(art::Event&);

    protected:
      RobustDispatcher _dispatcher;
      art::InputTag _tag;

      void dispatch_hit(const StrawDigi&);

    private:
      /**/
  };

  TrkHitDispatch::TrkHitDispatch(const Parameters& config):
      art::EDFilter(config),
      _dispatcher(config().host(), config().port(), 100),
      _tag(config().tag()){
    /**/
  }

  void TrkHitDispatch::beginJob(){
    /**/
  }

  void TrkHitDispatch::endJob(){
    /**/
  }

  bool TrkHitDispatch::filter(art::Event& event){
    auto handle = event.getValidHandle<StrawDigiCollection>(_tag);
    for (const auto& digi: *handle){
      this->dispatch_hit(digi);
    }

//  struct timespec interval;
//  interval.tv_sec = 0;
//  interval.tv_nsec = 1000;
//  nanosleep(&interval, NULL);

    return true;
  }

  void TrkHitDispatch::dispatch_hit(const StrawDigi& digi){
    uint64_t buff[4];
    buff[0] = 0;
    buff[1] = static_cast<uint64_t>(digi.strawId().asUint16());
    buff[2] = static_cast<uint64_t>(digi.TDC(StrawEnd::cal));
    buff[3] = static_cast<uint64_t>(digi.PMP());
    ssize_t tbw = 4 * sizeof(uint64_t);
    _dispatcher.Dispatch(reinterpret_cast<char*>(&buff), tbw);
  }
} // namespace mu2e

DEFINE_ART_MODULE(mu2e::TrkHitDispatch)
