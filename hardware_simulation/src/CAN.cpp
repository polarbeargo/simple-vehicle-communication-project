#include "CAN.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <iostream>
#include <cstdio>

#define MAX_FILTERS 5

typedef struct {
    uint32_t reg;
    uint8_t data[8];
    void (*isr_cb)(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len);
    uint8_t isr;
    uint16_t port;
    uint32_t last_sent_uid;
    uint16_t masks[MAX_FILTERS];
    uint16_t filters[MAX_FILTERS];
    uint8_t filter_count;
} CAN_t;

typedef struct {
    uint32_t id;
    CAN_FRAME_TYPES type;
    uint8_t data[8];
    uint8_t len;
    uint32_t reg;
    uint32_t last_sent_uid;
} CAN_FRAME_t;

#define CAN_PORT_START 9000

static CAN_t g_can;

int can_add_filter(uint8_t idx, uint16_t mask, uint16_t filter) {
    if(idx >= MAX_FILTERS) {
        return -1;
    }

    g_can.masks[idx] = mask;
    g_can.filters[idx] = filter;
    g_can.filter_count++;

    return 0;
}

void can_write_config(uint32_t reg, uint32_t value) {
    // Write the configuration value to the CAN bus
    if(reg == CAN_HARDWARE_REGISTER) {
        g_can.reg = reg;
    } else {
        g_can.reg = 0;
    }

    // start with a random UID
    g_can.last_sent_uid = rand() % 100;
}

static void can_isr_thread() {
    int socfd;
    struct sockaddr_in cliaddr, serveraddr;

    // Create a socket
    if((socfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Problem with CAN emulation socket creation\n");
        exit(EXIT_FAILURE);
    }
    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(g_can.port);

    int i = 0;
    for(; i < 10; i++) {
        if(bind(socfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
            g_can.port++;
            serveraddr.sin_port = htons(g_can.port);
        } else {
            break;
        }
    }
    if(i == 10) {
        printf("Problem with CAN emulation socket bind\n");
        exit(EXIT_FAILURE);
    }

    printf("CAN ISR thread started on port %d\n", g_can.port);

    while (true) {
        int len = sizeof(cliaddr);
        CAN_FRAME_t frame;
        if(recvfrom(socfd, &frame, sizeof(frame), MSG_WAITALL, (struct sockaddr *)&cliaddr, (socklen_t *)&len) < 0) {
            printf("Problem with CAN emulation socket receive\n");
            exit(EXIT_FAILURE);
        }

        // We sent this frame to ourselves, ignore it
        if(frame.last_sent_uid == g_can.last_sent_uid - 1) {
            continue;
        }

        printf("CAN: g_can.reg=0x%X, frame.reg=0x%X\n", g_can.reg, frame.reg);

        if(g_can.isr_cb != nullptr && frame.reg == g_can.reg) {
            g_can.isr = 1;
            while(g_can.isr) {
                for (int idx = 0; idx < g_can.filter_count; idx++) {
                    if ((frame.id & g_can.masks[idx]) == g_can.filters[idx]) {
                        g_can.isr_cb(frame.id, frame.type, frame.data, frame.len);
                    }
                }
            }
        }
    }
}

void can_clear_rx_packet_interrupt() {
    g_can.isr = 0;
}

int can_send_new_packet(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len) {
    int socfd;
    struct sockaddr_in cliaddr;
    CAN_FRAME_t frame;
    frame.id = id;
    frame.type = type;
    frame.reg = g_can.reg;
    frame.len = len;
    frame.last_sent_uid = g_can.last_sent_uid;
    g_can.last_sent_uid++;

    if(data != nullptr) {
        memcpy(frame.data, data, len);
    }

    if((socfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Problem with CAN emulation socket creation\n");
        exit(EXIT_FAILURE);
    }
    memset(&cliaddr, 0, sizeof(cliaddr));

    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = INADDR_ANY;
    for(int i = 0; i < 10; i++) {
        if(CAN_PORT_START + i == g_can.port) {
            continue;
        }
        cliaddr.sin_port = htons(CAN_PORT_START + i);

        if (sendto(socfd, &frame, sizeof(frame), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0) {
            printf("Problem with CAN emulation socket send\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("CAN SEND: id=0x%X reg=0x%X type=%d len=%d\n", id, g_can.reg, type, len);

    return 0;
}

void can_add_rx_packet_interrupt(void (*callback)(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len)) {
    g_can.isr_cb = callback;
    std::thread(can_isr_thread).detach();
}

void can_set_port(uint16_t port) {
    g_can.port = port;
}