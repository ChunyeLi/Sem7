#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// consider the unsigned hexadecimal number 0x1234, which requires at least two bytes to represent. In a big-endian ordering they would be [ 0x12, 0x34 ], while in a little-endian ordering, the bytes would be arranged [ 0x34, 0x12 ]
// __BYTE_ORDER is __LITTLE_ENDIAN
// struct iphdr
//   {
// #if __BYTE_ORDER == __LITTLE_ENDIAN
//     unsigned int ihl:4;
//     unsigned int version:4;
// #elif __BYTE_ORDER == __BIG_ENDIAN
//     unsigned int version:4;
//     unsigned int ihl:4;
// #else
// # error	"Please fix <bits/endian.h>"
// #endif
//     uint8_t tos;
//     uint16_t tot_len;
//     uint16_t id;
//     uint16_t frag_off;
//     uint8_t ttl;
//     uint8_t protocol;
//     uint16_t check;
//     uint32_t saddr;
//     uint32_t daddr;
//     /*The options start here. */
//   };


struct ipPacket {
  unsigned int id;
  unsigned char *data;
} pkts[3][54];

int cnt[3] = {0, 0, 0};

void sort() {
  for (int k = 0; k < 3; k++) {
    for (int i = 0; i < cnt[k]; i++) {
      for (int j = i + 1; j < cnt[k]; j++) {
        struct ipPacket temp;
        if (pkts[k][j].id < pkts[k][i].id) {
          temp = pkts[k][j];
          pkts[k][j] = pkts[k][i];
          pkts[k][i] = temp;
        }
      }
    }

  }
}

int main () {
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (!d) {
    printf("Error in opening the directory!\n");
    return 0;
  }
  bool srcDone = false;
  while ((dir = readdir(d)) != NULL) {
    char fileInterest[] = "ip_";
    if (strstr(dir->d_name, fileInterest) == NULL) {
      continue;
    }
    FILE *fptr = fopen(dir->d_name, "rb");
    fseek(fptr, 0, SEEK_END);
    unsigned int len = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    unsigned char buffer[len];
    fread(buffer, sizeof(unsigned char), len, fptr);
    fclose(fptr);
    unsigned int headerSize = (unsigned int)buffer[0] & 0xf;
    headerSize <<= 2;
    // printf("Header size: %d\n", headerSize);
    unsigned int sum = 0;
    for (unsigned int i = 0; i < headerSize; i += 2) {
        sum += (((unsigned int)buffer[i]) << 8) | (unsigned int)buffer[i + 1];
        if (sum & (1 << 16)) {
            sum ^= (1 << 16);
            sum++;
        }
    }
    __uint16_t val = sum;
    val = ~val;
    // printf("%d ", val);
    if (val == 0) {
      if (!srcDone) {
        printf("Source IP: ");
        for (int k = 12; k < 16; k++) {
          printf("%u%c", buffer[k], ".\n"[k == 15]);
        }
        srcDone = true;
        printf("Destination IP: ");
        for (int k = 16; k < 20; k++) {
          printf("%u%c", buffer[k], ".\n"[k == 19]);
        }
      }
      unsigned int id = ((unsigned int) buffer[4] << 8) | ((unsigned int)buffer[5]);  // big endian, most significant byte is placed first.
      if (id == 14926) {
        pkts[0][cnt[0]].id = ((unsigned int)buffer[6] << 8) | ((unsigned int)buffer[7]);
        pkts[0][cnt[0]].data = (unsigned char *)malloc((len - headerSize + 1) * sizeof(unsigned char));
        for (unsigned int i = headerSize; i < len; i++) {
          pkts[0][cnt[0]].data[i - headerSize] = buffer[i];
        }
        pkts[0][cnt[0]].data[len - headerSize] = '\0';
        cnt[0]++;
      } else if (id == 26437) {
        pkts[1][cnt[1]].id = ((unsigned int)buffer[6] << 8) | ((unsigned int)buffer[7]);
        pkts[1][cnt[1]].data = (unsigned char *)malloc((len - headerSize + 1) * sizeof(unsigned char));
        for (unsigned int i = headerSize; i < len; i++) {
          pkts[1][cnt[1]].data[i - headerSize] = buffer[i];
        }
        pkts[1][cnt[1]].data[len - headerSize] = '\0';
        cnt[1]++;
      } else {
        pkts[2][cnt[2]].id = ((unsigned int)buffer[6] << 8) | ((unsigned int)buffer[7]);
        pkts[2][cnt[2]].data = (unsigned char *)malloc((len - headerSize + 1) * sizeof(unsigned char));
        for (unsigned int i = headerSize; i < len; i++) {
          pkts[2][cnt[2]].data[i - headerSize] = buffer[i];
        }
        pkts[2][cnt[2]].data[len - headerSize] = '\0';
        cnt[2]++;
      }
    }
  }
  sort();
  printf("Total %d correct ipv4 fragments\n", cnt[0] + cnt[1] + cnt[2]);
  for (int k = 0; k < 3; k++) {
    printf("Message %d: ", k);
    for (int i = 0; i < cnt[k]; i++) {
      printf("%s", pkts[k][i].data);
    }
    printf("\n");
  }
	return 0;
}
