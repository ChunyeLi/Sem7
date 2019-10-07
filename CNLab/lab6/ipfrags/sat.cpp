#include <bits/stdc++.h>
#include <dirent.h>
using namespace std;

/*
 * struct iphdr
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ihl:4;
    unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int version:4;
    unsigned int ihl:4;
#else
# error    "Please fix <bits/endian.h>"
#endif
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
    // options
};
 */

const string dir_name = "/home/sourabh/Documents/Sem7/CNLab/lab6/ipfrags";

uint16_t calculate_checksum (uint8_t *buffer, int length) {
    int header_size = buffer[0] & 0xf;
    header_size <<= 2;

    uint32_t checksum = 0;
    for (int i = 0; i < header_size; i += 2) {
        checksum = checksum + ((uint32_t) buffer[i] << 8) + buffer[i + 1];
        if (checksum & (1 << 16)) {
            checksum ^= (1 << 16);
            checksum++;
        }
    }
    uint16_t val = checksum;
    return ~val;
}

string get_data (uint8_t *buffer, int length) {
    int header_size = buffer[0] & 0xf;
    header_size <<= 2;
    string ans;
    for (int i = header_size; i <length; i++) {
        ans += buffer[i];
    }
    return ans;
}

uint32_t get_id (uint8_t *buffer, int length) {
    // id << 16 + offset;
    return ((uint32_t) buffer[4] << 24) + ((uint32_t)buffer[5] << 16) +
    ((uint32_t)buffer[6] << 8) + ((uint32_t)buffer[7]);
}

pair <uint32_t, string> decode_message (string file_name) {
    file_name = dir_name + file_name;
    ifstream file (file_name, ios::binary);
    if (!file.is_open()) {
        perror ("Could not open file.");
        exit (0);
    }
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    uint8_t *buffer = new uint8_t [length];
    file.read ((char*) buffer, length);

    uint16_t checksum = calculate_checksum (buffer, length);
    if (checksum == 0) {
        string data = get_data (buffer, length);
        uint32_t id = get_id (buffer, length);
        return make_pair (id, data);
    } else {
        return make_pair (-1, "");
    }
}

int main() {
    freopen ("out.txt", "w", stdout);
    DIR *dir;
    struct dirent *ent;
    vector <pair <uint32_t, string>> ans;

    if ((dir = opendir("/home/sourabh/Documents/Sem7/CNLab/lab6/ipfrags")) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            string file_name = ent->d_name;
            if (file_name.find ("ip") == string::npos) {
                continue;
            }
            pair <uint32_t, string> current_message = decode_message(file_name);
            if (current_message.first != -1) {
                ans.push_back (current_message);
            }
        }
        closedir (dir);
    } else {
        perror ("Path does not exist.");
        return 1;
    }
    sort (ans.begin(), ans.end());
    string final;
    for (auto el: ans) {
        final += el.second;
    }

    cout << final << "\n";

    return 0;
}