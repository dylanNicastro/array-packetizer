# Array Packetizer
`functions.c` is a set of functions to convert an array of 32-bit signed integers into network packets and back with bitwise operations. 

> [!IMPORTANT]
> `functions.c` does not do anything on its own, as this file is intended for use within a larger project.

The functions depend on a fictional packet structure, detailed below. 

## Packet structure:
![Image of the packet structure](assets/image.png)
There always exists a 16-byte header that contains:
- 28-bit Source Address
- 28-bit Destination Address
- 4-bit Source Port
- 4-bit Destination Port
- 14-bit Fragment Offset
- 14-bit Packet Length
- 5-bit Maimum Hop Count
- 23-bit Checksum
- 2-bit Compression Scheme
- 6-bit Traffic Class

After the header comes the payload, containing anywhere from 0 to 16.368 kilobytes of array data.

> [!NOTE]
> The functions written will depacketize correctly even if the packets arrive out of order, and it will check for corruption by computing the checksum!

## Functions
### `void print_packet_sf(unsigned char packet[])`
Given a packet, prints the fields in the order shown in the [Packet Structure](#packet-structure)
- `unsigned char packet[]`: The packet to be printed, given as an array of unsigned chars (its bit values are used to determine each field in the header)

---

### `unsigned int reconstruct_array_sf(unsigned char *packets[], unsigned int packets_len, int *array, unsigned int array_len)`
Given an array of network packets, this function reconstructs the payloads of those packets into the original array.
- `unsigned char *packets[]`: The array of packets. Each packet is an array of unsigned chars (its bit values are used to determine each field in the header)
- `unsigned int packets_len`: The amount of packets given in the `packets` array.
- `int *array`: An uninitialized region of memory where the reconstructed array should be stored.
- `unsigned int array_len`: The amount of 32-bit integers that `array` can hold. If this number is smaller than the amount of integers in the packet data, the function will stop populating the array after `array_len` is reached.

**Return value**: The number of integers that were written into `array`.

> [!WARNING]
> Any corrupted packets will be discarded, and their payloads will not be part of the reconstructed array.

---

### `packetize_array_sf(int *array, unsigned int array_len, unsigned char *packets[], unsigned int packets_len, unsigned int max_payload, unsigned int src_addr, unsigned int dest_addr, unsigned int src_port, unsigned int dest_port, unsigned int maximum_hop_count, unsigned int compression_scheme, unsigned int traffic_class)`
Takes an array and information about the packets, and converts them into packets.
- `int *array`: The array to be packetized.
- `unsigned int array_len`: The length of `array`.
- `unsigned char *packets[]`: An empty array where all packets should be stored sequentially as `char*` pointers. `packets[0]` should point to the first packet, `packets[1]` should point to the second, and so on.
- `packets_len`: The number of pointers in `packets`.
- `unsigned int max_payload`: The maximum payload size of any packet.
- `src_addr, unsigned int dest_addr, unsigned int src_port, unsigned int dest_port, unsigned int maximum_hop_count, unsigned int compression_scheme, unsigned int traffic_class`: Various elements of the packet structure to be stored in the header of the packet.

> [!NOTE]
> Each packet is dynamically allocated to be able to store all of its information.
>
> Additionally, this function automatically calculates the checksum for each packet and includes it in the header, so that the packet can be checked for corruption when being received.