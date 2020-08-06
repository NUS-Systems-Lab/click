define($PA s1-eth1, $PB s1-eth2)

macbr :: MyMACBridge

elementclass TransChecksumFix {
    input -> arpcls :: Classifier(12/0806, -);
    arpcls [0] -> output;
    arpcls [1] -> CheckIPHeader(OFFSET 14) -> ipcls :: IPClassifier(ip proto udp, ip proto tcp, -);
    ipcls [0] -> SetUDPChecksum -> output;
    ipcls [1] -> SetTCPChecksum -> output;
    ipcls [2] -> output;
}

FromDevice($PA, SNIFFER false) -> [0] macbr [0] -> TransChecksumFix -> Queue(8) -> ToDevice($PA);
FromDevice($PB, SNIFFER false) -> [1] macbr [1] -> TransChecksumFix -> Queue(8) -> ToDevice($PB);