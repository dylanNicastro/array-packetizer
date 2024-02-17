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
    int packets_created = 0;
    int k = 0;
    double packets_needed = 0.0;
    double exact_needed = array_len/(max_payload/4)+1;
    while (packets_needed < exact_needed && packets_needed < packets_len) {
        packets_needed++;
    }
    for (int i = 0; i < packets_needed; i++) {
            int packet_payload_size;
            if ((i == packets_len - 1) && (array_len < packets_len*(max_payload/4))) {
                packet_payload_size = array_len*4 - (packets_len-1)*(max_payload);
            }
            else {
                packet_payload_size = max_payload;
            }
            int packet_length = 16+packet_payload_size;
            int fragment_offset = i*max_payload;
            packets[i] = malloc(packet_length);
            packets[i][0] = src_addr >> 20;
            packets[i][1] = (src_addr >> 12) & 0xff;
            packets[i][2] = (src_addr >> 4) & 0xff;
            packets[i][3] = ((src_addr & 0x0f) << 4) | (dest_addr >> 24);
            packets[i][4] = (dest_addr >> 16) & 0xff;
            packets[i][5] = (dest_addr >> 8) & 0xff;
            packets[i][6] = dest_addr & 0xff;
            packets[i][7] = (src_port << 4) | dest_port;
            packets[i][8] = fragment_offset >> 6;
            packets[i][9] = ((fragment_offset & 0x3f) << 2) | (packet_length >> 12);
            packets[i][10] = (packet_length >> 4) & 0xff;
            packets[i][11] = ((packet_length & 0x0f) << 4) | (maximum_hop_count >> 1);
            packets[i][12] = ((maximum_hop_count & 0x01) << 7);
            packets[i][15] = ((compression_scheme & 0x03) << 6) | traffic_class;
            for (int j = 16; j < packet_length; j = j + 4) {
                packets[i][j] = array[k] >> 24;
                packets[i][j+1] = (array[k] >> 16) & 0xff;
                packets[i][j+2] = (array[k] >> 8) & 0xff;
                packets[i][j+3] = array[k] & 0xff;
                k++;
            }
            packets_created++;
            unsigned int checksum = compute_checksum_sf(packets[i]);
            packets[i][12] = packets[i][12] | (checksum >> 16);
            packets[i][13] = (checksum >> 8) & 0xff;
            packets[i][14] = checksum & 0xff;
    }
    return packets_created;
}
