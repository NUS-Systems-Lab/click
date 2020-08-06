#ifndef CLICK_MYMACBRIDGE_HH
#define CLICK_MYMACBRIDGE_HH
#include <click/element.hh>
#include <click/etheraddress.hh>
#include <click/hashtable.hh>
CLICK_DECLS

class MyMACBridge : public Element { public:

  MyMACBridge() CLICK_COLD;
  ~MyMACBridge() CLICK_COLD;

  const char *class_name() const		{ return "MyMACBridge"; }
  const char *port_count() const		{ return "2/2"; }
  const char *processing() const		{ return PUSH; }

  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
//   void add_handlers() CLICK_COLD;

  void push(int port, Packet* p);

 private:
  HashTable<EtherAddress, int> _fib;

};

CLICK_ENDDECLS
#endif