// -*- c-basic-offset: 4 -*-
/*
 * arpresponder.{cc,hh} -- element that responds to ARP queries
 * Robert Morris
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

#include<iostream>
#include <clicknet/ip.h>
#include <click/config.h>
#include "simplenat.hh"
#include <clicknet/ether.h>
#include <click/ipaddress.hh>
#include <click/args.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/straccum.hh>
#include <click/packet_anno.hh>
#include <arpa/inet.h>
#include <unordered_map>


CLICK_DECLS

SimpleNat::SimpleNat()
{
}

SimpleNat::~SimpleNat()
{
}

int
SimpleNat::add(bi_ip_map &mapping, const String &arg, ErrorHandler *errh) const
{
    int old_vsize = mapping.size();
    Vector<String> words;
    cp_spacevec(arg, words);

    for (int i = 0; i < words.size(); ++i) {
        IPAddress internal,external;
        if (IPPrefixArg(true).parse(words[i], internal, external, this)) {
        mapping.insert({internal.addr(),external.addr()});  
        }
    }
    return 0;   
}

int
SimpleNat::configure(Vector<String> &conf, ErrorHandler *errh)
{
    for (int i = 0; i < conf.size(); i++) {
	PrefixErrorHandler perrh(errh, "argument " + String(i) + ": ");
	add(bi_mapping, conf[i], &perrh);
    }
    if (!errh->nerrors()) {
	// normalize(v, true, errh);
	return 0;
    } else
	return -1;
}

// WritablePacket *
// SimpleNat::simple_action(Packet *p_in)
// {
//     p_in->set_mac_header(p_in->data() + 0,14);
//     WritablePacket *p = p_in->uniqueify();
//     click_ip *ip = p->ip_header();
//     const uint16_t old_hw1 = (reinterpret_cast<uint16_t *>(ip))[6];
//     std::cout<<""<<std::endl;
//     const uint16_t old_hw2 = (reinterpret_cast<uint16_t *>(ip))[7];
//     std::cout<<""<<std::endl;
//     if(bi_mapping.find((ip->ip_src).s_addr)!=_mapping.end()){
//        (ip->ip_src).s_addr=_mapping.at((ip->ip_src).s_addr);
//        const uint16_t new_hw1 = (reinterpret_cast<uint16_t *>(ip)[6]);
//        const uint16_t new_hw2 = (reinterpret_cast<uint16_t *>(ip)[7]);
//        click_update_in_cksum(&ip->ip_sum,old_hw1,new_hw1);
//        click_update_in_cksum(&ip->ip_sum,old_hw2,new_hw2);
//        p_in->kill();
//        return p;
//     }
//     else{
//         checked_output_push(1, p);
//         WritablePacket *a=0;
//         return a;
//     }
    
// }

void 
SimpleNat::push(int i, Packet *p_in)
{
    p_in->set_mac_header(p_in->data() + 0,14);
    WritablePacket *p = p_in->uniqueify();
    click_ip *ip = p->ip_header(); 
    if (i==0){
       if(bi_mapping.left.find((ip->ip_src).s_addr)!=bi_mapping.left.end()){
       const uint16_t old_hw1 = (reinterpret_cast<uint16_t *>(ip))[6];
       std::cout<<""<<std::endl;
       const uint16_t old_hw2 = (reinterpret_cast<uint16_t *>(ip))[7];
       std::cout<<""<<std::endl;
       (ip->ip_src).s_addr=bi_mapping.left.at((ip->ip_src).s_addr);
       const uint16_t new_hw1 = (reinterpret_cast<uint16_t *>(ip)[6]);
       const uint16_t new_hw2 = (reinterpret_cast<uint16_t *>(ip)[7]);
       click_update_in_cksum(&ip->ip_sum,old_hw1,new_hw1);
       click_update_in_cksum(&ip->ip_sum,old_hw2,new_hw2);
       p_in->kill();       
       }
       output(0).push(p);
    }
    else{
       if(bi_mapping.right.find((ip->ip_dst).s_addr)!=bi_mapping.right.end()){
       const uint16_t old_hw1 = (reinterpret_cast<uint16_t *>(ip))[8];
       std::cout<<""<<std::endl;
       const uint16_t old_hw2 = (reinterpret_cast<uint16_t *>(ip))[9];
       std::cout<<""<<std::endl;
       (ip->ip_dst).s_addr=bi_mapping.right.at((ip->ip_dst).s_addr);
       const uint16_t new_hw1 = (reinterpret_cast<uint16_t *>(ip)[8]);
       const uint16_t new_hw2 = (reinterpret_cast<uint16_t *>(ip)[9]);
       click_update_in_cksum(&ip->ip_sum,old_hw1,new_hw1);
       click_update_in_cksum(&ip->ip_sum,old_hw2,new_hw2);
       p_in->kill();       
       }
       output(1).push(p);
    }
}





EXPORT_ELEMENT(SimpleNat)
ELEMENT_MT_SAFE(SimpleNat)
CLICK_ENDDECLS
