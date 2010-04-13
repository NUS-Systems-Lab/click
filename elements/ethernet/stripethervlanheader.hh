#ifndef CLICK_STRIPETHERVLANHEADER_HH
#define CLICK_STRIPETHERVLANHEADER_HH
#include <click/element.hh>
#include <clicknet/ether.h>
CLICK_DECLS

/*
=c

StripEtherVlanHeader([NATIVE_VLAN_ID])

=s ethernet

strips 802.1Q VLAN Ethernet header

=d

Strips each packet's 802.1Q header.

Checks the packet's outermost Ethernet type.  If that type is 0x8100 (the
802.1Q type), then sets the packet's VLAN annotation to the corresponding
802.1Q field and removes the 18-byte 802.1Q header.  Otherwise, the packet is
not VLAN encapsulated.  The element sets packet's VLAN annotation to the
NATIVE_VLAN_ID value and removes the 14-byte Ethernet header.

If NATIVE_VLAN_ID is not supplied, then non-802.1Q packets are dropped (or
emitted from output 1).

=a

EtherVlanEncap */

class StripEtherVlanHeader : public Element { public:

    StripEtherVlanHeader();
    ~StripEtherVlanHeader();

    const char *class_name() const	{ return "StripEtherVlanHeader"; }
    const char *port_count() const	{ return PORTS_1_1X2; }
    const char *processing() const	{ return PROCESSING_A_AH; }

    int configure(Vector<String> &conf, ErrorHandler *errh);
    bool can_live_reconfigure() const	{ return true; }

    Packet *simple_action(Packet *p);

  private:

    bool _has_native_vlan;
    uint16_t _native_vlan;

};

CLICK_ENDDECLS
#endif
