//
// Created by Administrator on 2017/4/20 0020.
//

#ifndef INC_4OVER6_V4PACKET_H
#define INC_4OVER6_V4PACKET_H

struct v4Head {
   short VIT;
   short totLength;
   short Identification;
   short flag_fragOffset;
   char time2Live;
   char protocol;
   char headChecksum;
   int srcAddr;
   int dstAddr;
   int option_padding;
};

struct v4Packet {
    v4Head head;
    char data[4096];
};



#endif //INC_4OVER6_V4PACKET_H
