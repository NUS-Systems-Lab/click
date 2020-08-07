/*
 * etherswitch.{cc,hh} -- learning, forwarding Ethernet bridge
 * John Jannotti
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include "myetherswitch.hh"
#include <clicknet/ether.h>
#include <click/etheraddress.hh>
#include <click/glue.hh>
#include <click/args.hh>
#include <click/straccum.hh>
#include <click/error.hh>
#include <sstream>
CLICK_DECLS

MyEtherSwitch::MyEtherSwitch()
    : _table(AddrInfo(-1, Timestamp())), _timeout(300)
{
    // NOTE: In the constructor function ninputs() and noutputs() do not work correctly.
}

MyEtherSwitch::~MyEtherSwitch()
{
    _table.clear();
}

int
MyEtherSwitch::configure(Vector<String> &conf, ErrorHandler *errh)
{
    int ret;
    String disablePorts = "";

    ret = Args(conf, this, errh)
	.read("TIMEOUT", SecondsArg(), _timeout)
    .read("DISABLE", disablePorts)
	.complete();

    // click_chatter("CONF: %u, %s\n", _timeout, disablePorts);
    // click_chatter("in: %d, out: %d\n", ninputs(), noutputs());
    _portfwd.clear();
    for (int i = 0; i < ninputs(); i++)
        _portfwd.push_back(true);
    
    if (disablePorts != "") {
        std::stringstream ss;
        ss << disablePorts;
        while (!ss.eof()) {
            int pn;
            ss >> pn;   // Assume user input are all numbers
            if (pn >= ninputs())
                errh->error("Specified port %d is not connected.", pn);
            else
                _portfwd[pn] = false;
        }
    }

    for (int i = 0; i < _portfwd.size(); i++)
        click_chatter("port %d: %s\n", i, _portfwd[i] ? "enabled" : "disabled");

    return ret;
}

void
MyEtherSwitch::broadcast(int source, Packet *p)
{
  int n = noutputs();
  assert((unsigned) source < (unsigned) n);
  int sent = 0, totalCount = n - 1;

  for (int i = n - 1; i >=0 ; i--)
    if (i != source && _portfwd[i] == false)
      totalCount--;

  for (int i = n - 1; i >=0 ; i--)
    if (i != source && _portfwd[i] == true) {
      Packet *pp = (sent < totalCount - 1 ? p->clone() : p);
      output(i).push(pp);
      sent++;
    }
  assert(sent == totalCount);
}

void
MyEtherSwitch::push(int source, Packet *p)
{
    click_ether* e = (click_ether*) p->data();
    int outport = -1;		// Broadcast

    if (_portfwd[source] == false) {
      p->kill();
      return;
    }

    // 0 timeout means dumb switch
    if (_timeout != 0) {
	_table.set(EtherAddress(e->ether_shost), AddrInfo(source, p->timestamp_anno()));

	// Set outport if dst is unicast, we have info about it, and the
	// info is still valid.
	EtherAddress dst(e->ether_dhost);
	if (!dst.is_group()) {
	    if (Table::iterator dst_info = _table.find(dst)) {
		if (p->timestamp_anno() < dst_info.value().stamp + Timestamp(_timeout, 0))
		    outport = dst_info.value().port;
		else
		    _table.erase(dst_info);
	    }
	}
    }

  if (outport < 0)
    broadcast(source, p);
  else if (outport == source || _portfwd[outport] == false)	// Don't send back out on same interface
    p->kill();
  else				// forward
    output(outport).push(p);
}

String
MyEtherSwitch::reader(Element* f, void *thunk)
{
    MyEtherSwitch* sw = (MyEtherSwitch*)f;
    switch ((intptr_t) thunk) {
    case 0: {
	StringAccum sa;
	for (Table::iterator iter = sw->_table.begin(); iter.live(); iter++)
	    sa << iter.key() << ' ' << iter.value().port << '\n';
	return sa.take_string();
    }
    case 1:
	return String(sw->_timeout);
    default:
	return String();
    }
}

int
MyEtherSwitch::writer(const String &s, Element *e, void *, ErrorHandler *errh)
{
    MyEtherSwitch *sw = (MyEtherSwitch *) e;
    if (!SecondsArg().parse_saturating(s, sw->_timeout))
	return errh->error("expected timeout (integer)");
    return 0;
}

void
MyEtherSwitch::add_handlers()
{
    add_read_handler("table", reader, 0);
    add_read_handler("timeout", reader, 1);
    add_write_handler("timeout", writer, 0);
}

EXPORT_ELEMENT(MyEtherSwitch)
CLICK_ENDDECLS
