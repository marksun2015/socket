http://www.pdbuchan.com/rawsock/rawsock.html


IPv4
Three combinations of the Domain, Type, and Protocol arguments are shown here. There are other possible combinations you could try. The packet parameters that can be modified are determined by which combination you choose.

In the Table 1 examples below, we tell the kernal the IP header is included (by us) by using setsockopt() and the IP_HDRINCL flag, and we can modify all values within the packet, but the kernal fills out the layer 2 (data link) information (source and next-hop MAC addresses) for us.
Table 1: 	sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW); The kernel fills out layer 2 (data link) information (MAC addresses) for us.
  tcp4.c 	Send SYN packet (an example with no TCP data).
  get4.c 	Send HTTP GET (an example with TCP data) (note).
  icmp4.c 	Send ICMP Echo Request with data.
  udp4.c 	Send UDP packet with data.

In the Table 2 examples, we fill out all values, including the layer 2 (data link) information (source and next-hop MAC addresses). To do this, we must know the MAC address of the router/host the frames will be routed to next (more explanation), as well as the MAC address of the network interface ("network card") we're sending the packet from.
Table 2: 	sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL));We provide layer 2 (data link) information. i.e., we specify ethernet frame header with MAC addresses.
  tcp4_ll.c 	Send SYN packet (an example with no TCP data).
  get4_ll.c 	Send HTTP GET (an example with TCP data) (note).
  icmp4_ll.c 	Send ICMP Echo Request with data.
  ping4_ll.c 	Send ICMP Echo Request with data and receive reply. i.e., ping
  udp4_ll.c 	Send UDP packet with data.

In the Table 3 examples, we fill out all values, but only including the destination (i.e., next-hop) layer 2 (data link) information (not source MAC address). This is called a "cooked packet." To do this, we must know the MAC address of the router/host the frames will be routed to next (more explanation).
Table 3: 	sd = socket (PF_PACKET, SOCK_DGRAM, htons (ETH_P_ALL));We provide a "cooked" packet with destination MAC address in struct sockaddr_ll.
  tcp4_cooked.c 	Send SYN packet (an example with no TCP data).
  get4_cooked.c 	Send HTTP GET (an example with TCP data) (note).
  icmp4_cooked.c 	Send ICMP Echo Request with data.
  udp4_cooked.c 	Send UDP packet with data.

To learn the next-hop's MAC address for use in the Table 2 and 3 examples above, you must use the Address Resolution Protocol (ARP). I have included an example which sends an ARP request ethernet frame as well as an example that receives an ARP reply ethernet frame. Additionally, I have included some router solicitation and advertisement routines.
Table 4: 	Miscellaneous
  arp.c 	Send an ARP request ethernet frame.
  receive_arp.c 	Receive an ARP reply ethernet frame.
  rs4.c 	Send a router solicitation.
  ra4.c 	Send a router advertisement.
  receive_ra4.c 	Receive a router advertisement.
  tr4_ll.c 	TCP/ICMP/UDP traceroute

Table 5 below provides some examples of packet fragmentation. The first file, called "data", contains a list of numbers. The following three routines use it as data for the upper layer protocols. Feel free to provide to the routines your own data in any manner you prefer.
Table 5: 	Fragmentation
data 	12390-byte file to use as upper layer protocol data
  tcp4_frag.c 	Send TCP packet with enough data to require fragmentation.
  icmp4_frag.c 	Send ICMP packet with enough data to require fragmentation.
  udp4_frag.c 	Send UDP packet with enough data to require fragmentation.

Table 6 below presents examples of packets with IP and TCP options.
Table 6: 	IP and TCP Options
  tcp4_maxseg.c 	Send TCP packet with a TCP option which sets maximum segment size.
  tcp4_maxseg_tsopt.c 	Send TCP packet with two TCP options: set maximum segment size, and provide time stamp.
  tcp4_maxseg-timestamp.c 	Send TCP packet with IP option to send time stamp, and TCP option to set maximum segment size.
  tcp4_maxseg-security.c 	Send TCP packet with security IP option and TCP option to set maximum segment size.
  tcp4_2ip-opts_2tcp_opts.c 	Send TCP packet with two IP options and two TCP options.
