#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// struct iphdr {
//     __uint8_t version : 4;
//     __uint8_t ihl : 4;
//     __uint8_t tos;
//     __uint16_t len;
//     __uint16_t id;
//     __uint16_t flags : 3;
//     __uint16_t frag_offset : 13;
//     __uint8_t ttl;
//     __uint8_t proto;
//     __uint16_t csum;
//     __uint32_t saddr;
//     __uint32_t daddr;
//     __uint32_t op1;
//     __uint32_t op2;
// } __attribute__((packed));

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
          printf("%u%c", buffer[k], ":\n"[k == 15]);
        }
        srcDone = true;
        printf("Destination IP: ");
        for (int k = 16; k < 20; k++) {
          printf("%u%c", buffer[k], ":\n"[k == 19]);
        }
      }
      unsigned int id = ((unsigned int) buffer[4] << 8) | ((unsigned int)buffer[5]);
      if (id == 14926) {
        pkts[0][cnt[0]].id = ((unsigned int)buffer[6] << 8) | ((unsigned int)buffer[7]);
        pkts[0][cnt[0]].data = (unsigned char *)malloc(len - headerSize + 1 * sizeof(unsigned char));
        for (unsigned int i = headerSize; i < len; i++) {
          pkts[0][cnt[0]].data[i - headerSize] = buffer[i];
        }
        pkts[0][cnt[0]].data[len - headerSize] = '\0';
        cnt[0]++;
      } else if (id == 26437) {
        pkts[1][cnt[1]].id = ((unsigned int)buffer[6] << 8) | ((unsigned int)buffer[7]);
        pkts[1][cnt[1]].data = (unsigned char *)malloc(len - headerSize + 1 * sizeof(unsigned char));
        for (unsigned int i = headerSize; i < len; i++) {
          pkts[1][cnt[1]].data[i - headerSize] = buffer[i];
        }
        pkts[1][cnt[1]].data[len - headerSize] = '\0';
        cnt[1]++;
      } else {
        pkts[2][cnt[2]].id = ((unsigned int)buffer[6] << 8) | ((unsigned int)buffer[7]);
        pkts[2][cnt[2]].data = (unsigned char *)malloc(len - headerSize + 1 * sizeof(unsigned char));
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
