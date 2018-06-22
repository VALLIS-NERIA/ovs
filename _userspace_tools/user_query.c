#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX_BATCH 2048
typedef uint64_t elemtype;
unsigned short listen_port = 0x8888;
unsigned short response_port = 0x9999;
struct flow_key {
    uint32_t srcip;
    uint32_t dstip;
    uint16_t srcport;
    uint16_t dstport;
    uint16_t protocol;
};

struct countmax_entry {
    struct flow_key key;
    elemtype value;
};

const char* magic_finish = "finish";

void print_ip(uint32_t ip) {
    printf("%d.%d.%d.%d", (ip) % 0x100, (ip / 0x100) % 0x100, (ip / 0x10000) % 0x100, ip / 0x1000000);
}

int main() {
    const unsigned buf_size = MAX_BATCH * sizeof(struct flow_key);
    struct sockaddr_in s_addr;
    int ret = 0;

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(listen_port);
    inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);
    //printf("1\n");
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ret = connect(sockfd, (struct sockaddr*)&s_addr, sizeof(s_addr));

    char* send_buf = "lol233hhh";
    printf("%d\n", ret);
    write(sockfd, send_buf, strlen(send_buf) + 1);

    size_t recv_size = MAX_BATCH * sizeof(struct countmax_entry);
    struct countmax_entry* recv_buf = malloc(recv_size);
    bzero(recv_buf, recv_size);
    while (1) {
        int n = read(sockfd, recv_buf, recv_size);
        if (n) printf("get response! length: %d\n", n);
        sleep(1);
        if(strcmp((char*)recv_buf,magic_finish) == 0){
            printf("END.\n");
            break;
        }
        int i = 0;
        for (; i < n / sizeof(struct countmax_entry); ++i) {
            //if(recv_buf[i].key.srcip==0&&recv_buf[i].key.dstip==0) break;
            print_ip(recv_buf[i].key.srcip);
            printf("->");
            print_ip(recv_buf[i].key.dstip);

            printf("%llu \n", recv_buf[i].value);
        }
    }
    close(sockfd);
    return 0;
}
