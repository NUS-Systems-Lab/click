#include <click/config.h>
#include "mymacbridge.hh"
#include <clicknet/ether.h>
#include <click/etheraddress.hh>
#include <click/glue.hh>
#include <click/args.hh>
#include <click/straccum.hh>
#include <click/error.hh>
CLICK_DECLS

MyMACBridge::MyMACBridge() 
  : _fib(-1) {}

MyMACBridge::~MyMACBridge() {_fib.clear();}

int MyMACBridge::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return Args(conf, this, errh)
	// .read("TIMEOUT", SecondsArg(), _timeout)
	.complete();
}

void
MyMACBridge::push(int source, Packet *p)
{
  click_ether* e = (click_ether*) p->data();

  EtherAddress srcMAC(e->ether_shost), dstMAC(e->ether_dhost);
  _fib.set(srcMAC, source);

  // For a two-port bridge, the frame should be forwarded when it is
  //  a) group-cast; b) to a non-recorded source; 
  //  c) to the MAC associated with the other port.
  // In other words, the frame should be discarded only when it is unicast,
  // found in the FIB that its destination is same as its source, otherwise forwarded.
  
  // click_chatter("Received frame from %s to %s", srcMAC.s().c_str(), dstMAC.s().c_str());
  if (!dstMAC.is_group()) { // unicast
    auto itr = _fib.find(dstMAC);
    if (itr != _fib.end()) {
      // click_chatter("Destination found in FIB");
      if (itr.value() == source) {
        click_chatter("Frame is dropped");
        p->kill();
        return;
      }
    }
  }

  click_chatter("Forward frame from %s to %s", srcMAC.s().c_str(), dstMAC.s().c_str());
  output(source == 0 ? 1 : 0).push(p);
}

EXPORT_ELEMENT(MyMACBridge)
CLICK_ENDDECLS