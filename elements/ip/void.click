define($HOST1 s1-eth1, $HOST2 s1-eth2)
nat::SimpleNat(10.0.0.1/192.168.1.10,
               10.0.0.2/192.168.1.11,
               10.0.0.3/192.168.1.12);
FromDevice($HOST1)->[0]nat[0]->Queue(8)->ToDevice($HOST2);
FromDevice($HOST2)->[1]nat[1]->Queue(8)->ToDevice($HOST1);