#ifndef CLICK_SIMPLENAT_HH
#define CLICK_SIMPLENAT_HH
#include <click/element.hh>
#include <click/ipaddress.hh>
#include <click/vector.hh>
#include <click/glue.hh>
#include <arpa/inet.h>
#include <unordered_map>
#include <map>
#include <boost/bimap.hpp>

CLICK_DECLS

// typedef std::unordered_map<in_addr_t,in_addr_t> ip_map;
typedef boost::bimap<in_addr_t,in_addr_t> bi_ip_map;


class SimpleNat : public Element { public:

    SimpleNat() CLICK_COLD;
    ~SimpleNat() CLICK_COLD;
    const char *class_name() const		{ return "SimpleNat"; }
    const char *port_count() const		{ return "2/2"; }
    const char *processing() const		{ return PUSH; }
    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    bool can_live_reconfigure() const		{ return true; }
    // WritablePacket *simple_action(Packet *); 
    void push(int i, Packet *);
  private:
    bi_ip_map bi_mapping;
    int add(bi_ip_map &mapping, const String &arg, ErrorHandler *errh) const;
};

CLICK_ENDDECLS
#endif
