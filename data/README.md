## Test Data


The "*.dat" files in the data/ folder are binary files. Each 13-byte string is a 5-tuple in the format of (srcIP, srcPort, dstIP, dstPort, protocol).

We use the trace from CAIDA. The original trace files are in '.pcap' format, and the first 4 bytes of these files are '\xd4\xc3\xb2\xa1', so the original files are big-endian. The "*.dat" files in the data/. folder are also in big-endian.

If you read a 13-byte string, for example, "\x00\x01\x02\x03\x05\x06\x07\x08\x09\x0a\x0b\x0c", the ASCII code should be:
- srcIP = "\x00\x01\x02\x03"
- srcPort = "\x04\x05"
- dstIP = "\x06\x07\x08\x09"
- dstPort = "\x0a\x0b"
- protocol = "\x0c".

So, you have to reverse it on little-endian systems.
