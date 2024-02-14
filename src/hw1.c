#include "hw1.h"
#include <math.h>

int getSourceAddress(unsigned char packet[]) {
    int sourceAddress = (packet[0] << 20) | (packet[1] << 12) | (packet[2] << 4) | (packet[3] >> 4);
    return sourceAddress;
}
int getDestAddress(unsigned char packet[]) {
    int destAddress = ((packet[3] & 0x0f) << 24) | (packet[4] << 16) | (packet[5] << 8) | packet[6];
    return destAddress;
}
char getSourcePort(unsigned char packet[]) {
    char sourcePort = packet[7] >> 4;
    return sourcePort;
}
char getDestPort(unsigned char packet[]) {
    char destPort = packet[7] & 0x0f;
    return destPort;
}
int getFragmentOffset(unsigned char packet[]) {
    int fragmentOffset = (packet[8] << 6) | (packet[9] >> 2);
    return fragmentOffset;
}
int getPacketLength(unsigned char packet[]) {
    int packetLength = ((packet[9] & 0x03) << 12) | (packet[10] << 4) | (packet[11] >> 4);
    return packetLength;
}
char getMaxHopCount(unsigned char packet[]) {
    char maxHopCount = ((packet[11] & 0x0f) << 1) | (packet[12] >> 7);
    return maxHopCount;
}
int getChecksum(unsigned char packet[]) {
    int checksum = ((packet[12] & 0x7f) << 16) | (packet[13] << 8) | packet[14];
    return checksum;
}
char getCompScheme(unsigned char packet[]) {
    char compScheme = packet[15] >> 6;
    return compScheme;
}
char getTrafficClass(unsigned char packet[]) {
    char trafficClass = packet[15] & 0x3f;
    return trafficClass;
}

void print_packet_sf(unsigned char packet[])
{
    int packetSize = getPacketLength(packet);
    signed int currentPayload;
    printf("Source Address: %d\nDestination Address: %d\nSource Port: %d\nDestination Port: %d\nFragment Offset: %d\nPacket Length: %d\nMaximum Hop Count: %d\nChecksum: %d\nCompression Scheme: %d\nTraffic Class: %d\nPayload:",
            getSourceAddress(packet),getDestAddress(packet),getSourcePort(packet),getDestPort(packet),getFragmentOffset(packet),getPacketLength(packet),getMaxHopCount(packet),getChecksum(packet),getCompScheme(packet),getTrafficClass(packet));
    for (int i = 16; i < packetSize; i = i + 4) {
        currentPayload = (packet[i] << 24) | (packet[i+1] << 16) | (packet[i+2] << 8) | packet[i+3];
        printf(" %d", currentPayload);
    }
    printf("\n");
}

unsigned int compute_checksum_sf(unsigned char packet[])
{
    unsigned int sum = getSourceAddress(packet)+getDestAddress(packet)+getSourcePort(packet)+getDestPort(packet)+getFragmentOffset(packet)+getPacketLength(packet)+getMaxHopCount(packet)+getCompScheme(packet)+getTrafficClass(packet);
    for (int i = 16; i < getPacketLength(packet); i = i + 4) {
        sum = sum + abs((packet[i] << 24) | (packet[i+1] << 16) | (packet[i+2] << 8) | packet[i+3]);
    }
    int checksum = sum % 0x7fffff;
    return checksum;
}

unsigned int reconstruct_array_sf(unsigned char *packets[], unsigned int packets_len, int *array, unsigned int array_len) {
    int success = 0;
    for (int i = 0; i < packets_len; i++) {
        unsigned char **currentPacket = (packets + i);
        int packetSize = getPacketLength(*currentPacket);
        int startIndex = getFragmentOffset(*currentPacket)/4;

        for (int j = 16; j < packetSize; j = j + 4) {
            if (compute_checksum_sf(*currentPacket) == getChecksum(*currentPacket)) {
                unsigned int payload = (*((*currentPacket) + j) << 24) | (*((*currentPacket) + j + 1) << 16) | (*((*currentPacket) + j + 2) << 8) | *((*currentPacket) + j + 3);

                if (startIndex + (j - 16)/4 < array_len) {
                    *(array + startIndex + (j - 16)/4) = payload;
                    success++;
                }
            }
        }
    }
    return success;
}

unsigned int packetize_array_sf(int *array, unsigned int array_len, unsigned char *packets[], unsigned int packets_len,
                          unsigned int max_payload, unsigned int src_addr, unsigned int dest_addr,
                          unsigned int src_port, unsigned int dest_port, unsigned int maximum_hop_count,
                          unsigned int compression_scheme, unsigned int traffic_class)
{
    (void)array;
    (void)array_len;
    (void)packets;
    (void)packets_len;
    (void)max_payload;
    (void)src_addr;
    (void)dest_addr;
    (void)src_port;
    (void)dest_port;
    (void)maximum_hop_count;
    (void)compression_scheme;
    (void)traffic_class;
    return -1;
}
