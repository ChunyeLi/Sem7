* **arp**: gives us arp entries. Like we know this ip has this hardware address on this interface. Can as well use `arp -i eth1` to get entries only for `eth1` interface. 
* **ifconfig**: we know.
* **route**: shows routing table, **gateway** tells which router to send for that **destination**. 
* **host**: just like **nslookup**, `host google.com` gives its ip, `host thatIP` gives name of the server. Note `host -t mx google.com`, `host -t ns google.com`, etc. gives mail servers name, name servers name, etc.
* **tcpdump**: as used, to dump traffic on a network.
* `netstat -tu` will give only tcp/udp connections.
4. ? 
5. easy
6. `netstat -l`.
7. `nslookup IP`.