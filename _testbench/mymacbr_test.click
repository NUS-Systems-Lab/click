
d1 :: InfiniteSource(DATA \<
  // Ethernet header
  00 00 c0 ae 67 ef  00 00 c0 ab 81 de  08 00
  // IP header
  45 00 00 44  00 00 00 00  40 11 81 3b  c0 a8 bc 05  c0 a8 bc 17
  // UDP header
  13 69 13 69  00 14 d6 41
  // UDP payload
  55 44 50 20  70 61 63 6b  65 74 21 0a  04 00 00 00  01 00 00 00  
  01 00 00 00  00 00 00 00  00 80 04 08  00 80 04 08  53 53 00 00
>, LIMIT 5, STOP true);

d2 :: InfiniteSource(DATA \<
  // Ethernet header
  00 00 c0 ab 81 de  00 00 c0 ae 67 ef  08 00
  // IP header
  45 00 00 44  00 00 00 00  40 11 81 3b  c0 a8 bc 17  c0 a8 bc 05
  // UDP header
  13 69 13 69  00 14 d6 41
  // UDP payload
  55 44 50 20  70 61 63 6b  65 74 21 0a  04 00 00 00  01 00 00 00  
  01 00 00 00  00 00 00 00  00 80 04 08  00 80 04 08  53 53 00 00
>, LIMIT 5, STOP true);

macbr :: MyMACBridge

d1 -> Print(1) -> [0] macbr;
d2 -> Print(2) -> [1] macbr;

macbr [0] -> Print(3) -> Discard;
macbr [1] -> Print(4) -> Discard;