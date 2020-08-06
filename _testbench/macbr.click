
macbr :: MyMACBridge

FromDevice(s3-eth1, SNIFFER false) -> [0] macbr [0] -> Queue(8) -> ToDevice(s3-eth1);
FromDevice(s3-eth2, SNIFFER false) -> [1] macbr [1] -> Queue(8) -> ToDevice(s3-eth2);