* `wget domain:port/dir` 
* `wget --server-response --spider domain:port` - to get HTTP protocol
* `lftp tc@h2.virtnet.com` - from inside VM.
  * use `get` (`mget` for multiple files, `mget *.xls`), `put` (`mput`) to download/upload resp. Use `lcd dir` to specify download dir.
* `scp -P 14505 tc@localhost:my_capture.pcapng ~/Desktop`.
* DNS happens before ICMP.
* `plt.plot(x_data, y_data)`.
* `plt.show()`.
* `np.linspace(0, 1, 100)`.
* `sudo hping3 -1 -y -d 396 192.168.101.2`.
* ```c
  DIR *d;  // see ipv4.c
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
  ```

* ```c
  ofstream o("arrivals_shape.txt");
  ifstream f("arrivals.txt");  // now use as cout, cin resp.
  ```
* `execvp("./add", args)`. 
* `man atoi`.
* ```c
  strncpy((char *)ifr.ifr_name, "eth1", IFNAMSIZ);
  if (ioctl(sock, SIOCGIFINDEX, &ifr) == -1)
  sll.sll_protocol = htons(ETH_P_IP);
  sll.sll_family = AF_PACKET
  sll.sll_ifindex = ifr.ifr_ifindex;
  bind(sock, (struct sockaddr *)&sll, sizeof (sll))
  ```