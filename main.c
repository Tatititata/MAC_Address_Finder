#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

unsigned short checksum(void *packet, int len) {
  unsigned short *ptr = packet;
  unsigned int sum = 0;
  for (; len > 1; len -= 2)
    sum += *ptr++;
  if (len == 1)
    sum += *(unsigned char *)ptr;
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}

int receive_icmp_reply(int sock, int pid, struct sockaddr_in *address) {
  int result = 0;
  char buf[1024];
  ssize_t len = recv(sock, buf, sizeof(buf), 0);
  if (len <= 0) {
    perror("recv");
  } else {
    const struct iphdr *ip = (struct iphdr *)buf;
    size_t iphdrlen = ip->ihl * 4;
    if ((size_t)len < iphdrlen + sizeof(struct icmphdr)) {
      fprintf(stderr, "Packet too short\n");
    } else {
      const struct icmphdr *icmp_reply = (struct icmphdr *)(buf + iphdrlen);
      if (icmp_reply->type == ICMP_ECHOREPLY && icmp_reply->un.echo.id == pid) {
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address->sin_addr), ip_str, sizeof(ip_str));
        printf("Received reply from %s\n", ip_str);
        result = 1;
      }
    }
  }
  return result;
}

int ping(int sock, struct sockaddr_in *address) {
  int result = 0;
  char packet[64];
  struct icmphdr *icmp = (struct icmphdr *)packet;

  int pid = getpid() & 0xFFFF;

  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.id = pid;
  icmp->un.echo.sequence = 1;
  icmp->checksum = 0;
  memset(packet + sizeof(struct icmphdr), 0xAA,
         sizeof(packet) - sizeof(struct icmphdr));
  icmp->checksum = checksum(packet, sizeof(packet));

  if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)address,
             sizeof(*address)) <= 0) {
    perror("sendto");
  } else {
    fd_set fds;
    struct timeval tv = {1, 0};
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    int ret = select(sock + 1, &fds, NULL, NULL, &tv);
    if (ret == 0) {
      printf("Timeout waiting for reply\n");
    } else if (ret < 0) {
      perror("select");
    } else {
        result = receive_icmp_reply(sock, pid, address);
    }
  }
  return result;
}

void print_mac_from_arp(const char *ip) {
  FILE *arp = fopen("/proc/net/arp", "r");
  if (!arp) {
    perror("fopen /proc/net/arp");
  } else {
    char line[256];
    fgets(line, sizeof(line), arp);
    int result = 0;
    while (fgets(line, sizeof(line), arp) && !result) {
      char ip_addr[16], hw_type[10], flags[10], mac[18], mask[20], device[20];
      sscanf(line, "%15s %9s %9s %17s %19s %19s", ip_addr, hw_type, flags, mac,
             mask, device);
      if (strcmp(ip_addr, ip) == 0) {
        printf("MAC address for %s is %s\n", ip, mac);
        result = 1;
      }
    }
    if (!result)
      printf("MAC address for %s not found in ARP cache\n", ip);
    fclose(arp);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: sudo %s <IPv4 address>\n", argv[0]);
  } else {

    const char *target_ip = argv[1];
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
      perror("socket");
    } else {

      struct sockaddr_in address = {0};
      address.sin_family = AF_INET;
      if (inet_pton(AF_INET, target_ip, &address.sin_addr) != 1) {
        fprintf(stderr, "Invalid IP address: %s\n", target_ip);
        close(sock);
      } else {
        printf("Pinging %s...\n", target_ip);
        if (ping(sock, &address)) {
          print_mac_from_arp(target_ip);
        } else {
          printf("No reply from %s\n", target_ip);
        }
        close(sock);
      }
    }
  }
  return 0;
}
