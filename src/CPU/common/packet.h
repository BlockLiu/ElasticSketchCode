#ifndef __PACKET_H_
#define __PACKET_H_

class Packet
{
 public:
  uint32_t srcip;
  uint32_t dstip;
  uint32_t srcport;
  uint32_t dstport;
  uint8_t proto; 
  Packet()
    {
      srcip = 0;
      dstip = 0;
      srcport = 0;
      dstport = 0;
      proto = 0;
    };
};
#endif
