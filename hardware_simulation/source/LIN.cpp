#include "LIN.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>

#define LIN_MODE_MASK       0b001111111111111
#define LIN_PORT_FOLLOWER_START      8080
#define LIN_PORT_HEADER_START        9080

typedef union {
    struct {
        uint8_t baud_rate: 8;
        uint8_t parity: 2;
        uint8_t stop_bits: 2;
        uint8_t data_bits: 2;
        uint8_t reserved: 2;
        uint8_t flow_control: 2;
    } bits;
    uint32_t reg;
} LIN_CR;

#define ISR_FRAME_ISR   0b01
#define ISR_FRAME_RESP  0b10
typedef struct {
    LIN_CR reg;
    uint8_t isr;
    uint8_t id;
    uint8_t data[8];
    void (*isr_cb)(uint8_t id);
    void (*isr_cb_resp)(uint8_t id, uint8_t *data, uint8_t len);
    uint16_t port;
} LIN_t;

enum LIN_MSG_TYPE {
    LIN_RESPONSE = 0,
    LIN_HEADER = 1
};
typedef struct {
    uint8_t id;
    uint8_t data[8];
    LIN_CR bus_reg;
    uint8_t len;
    LIN_MSG_TYPE msg_type;
} LIN_FRAME_t;

static LIN_t g_lin;

static void lin_isr_thread() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Problem with LIN emulation socket creation\n");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(g_lin.port); // Port

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("Problem with LIN emulation socket binding\n");
        exit(EXIT_FAILURE);
    }

    while(true) {
        int len = sizeof(cliaddr); // len is value/result
        LIN_FRAME_t frame;

        recvfrom(sockfd, &frame, (size_t)sizeof(frame), MSG_WAITALL, (struct sockaddr *)&cliaddr, (socklen_t *)&len);

        if((frame.bus_reg.reg & LIN_MODE_MASK) ==
           (g_lin.reg.reg & LIN_MODE_MASK)) {
            if(frame.msg_type == LIN_HEADER && g_lin.isr_cb != nullptr) {
                g_lin.isr |= ISR_FRAME_ISR;
                while (g_lin.isr & ISR_FRAME_ISR) {
                    g_lin.isr_cb(frame.id);
                }
            } else if(frame.msg_type == LIN_RESPONSE && g_lin.isr_cb_resp != nullptr) {
                g_lin.isr |= ISR_FRAME_RESP;
                while (g_lin.isr & ISR_FRAME_RESP) {
                    g_lin.isr_cb_resp(frame.id, frame.data, frame.len);
                }
            }
        }
    }
}

int lin_write_config(uint32_t mem_mapped_addr, uint32_t value) {
    // Write the configuration value to the LIN bus
    if(mem_mapped_addr == 0xFF000040) {
        g_lin.reg.reg = value;
    } else {
        g_lin.reg.reg = 0;
    }

    if((g_lin.reg.reg & LIN_MODE_FOLLOWER) == LIN_MODE_FOLLOWER) {
        g_lin.port = LIN_PORT_FOLLOWER_START;
    } else {
        g_lin.port = LIN_PORT_HEADER_START;
    }
    return 0;
}

int lin_add_frame_resp_interrupt(void (*callback)(uint8_t id, uint8_t *data, uint8_t len)) {
    // Add a callback function to the LIN bus
    g_lin.isr_cb_resp = callback;
    g_lin.isr = 0;
    std::thread(lin_isr_thread).detach();
    return 0;
}

int lin_add_rx_frame_header_interrupt(void (*callback)(uint8_t id)) {
    // Add a callback function to the LIN bus
    g_lin.isr_cb = callback;
    g_lin.isr = 0;
    std::thread(lin_isr_thread).detach();
    return 0;
}

void lin_clear_frame_resp_interrupt() {
    // Clear the callback function from the LIN bus
    g_lin.isr -= ISR_FRAME_RESP;
}

void lin_clear_rx_frame_interrupt() {
    // Clear the callback function from the LIN bus
    g_lin.isr -= ISR_FRAME_ISR;
}

static int lin_setup_bus_socket(struct sockaddr_in *servaddr, uint16_t port) {
    int sockfd;

    // Creating socket fd
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Problem with LIN emulation socket creation\n");
        exit(EXIT_FAILURE);
    }

    memset(servaddr, 0, sizeof(*servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_port = htons(port);
    servaddr->sin_addr.s_addr = INADDR_ANY;
    return sockfd;
}

int lin_write_response_data(uint8_t id, uint8_t *data, uint8_t len) {
    // Write data to the LIN bus
    LIN_FRAME_t frame;
    struct sockaddr_in servaddr;
    frame.id = id;
    frame.bus_reg.reg = g_lin.reg.reg;
    frame.len = len;
    frame.msg_type = LIN_RESPONSE;
    memcpy(frame.data, data, len);

    for(int i = 0; i < 10; i++) {
        if (LIN_PORT_HEADER_START + i == g_lin.port) {
            continue;
        }
        int sockfd = lin_setup_bus_socket(&servaddr, LIN_PORT_HEADER_START + i);
        sendto(sockfd, &frame, sizeof(frame), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        close(sockfd);
    }
    return 0;
}

void lin_write_frame_header(uint8_t id) {
    LIN_FRAME_t frame;
    frame.id = id;
    frame.bus_reg.reg = g_lin.reg.reg;
    frame.msg_type = LIN_HEADER;
    struct sockaddr_in servaddr;


    for (int i = 0; i < 10; i++) {
        if (LIN_PORT_FOLLOWER_START + i == g_lin.port) {
            continue;
        }

        int sockfd = lin_setup_bus_socket(&servaddr, LIN_PORT_FOLLOWER_START + i);
        sendto(sockfd, &frame, sizeof(frame), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        close(sockfd);
    }
}