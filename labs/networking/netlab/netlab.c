#include <stdio.h>
#include "netsim.h"

int expected_seq = 1;
int total_packets = 0;
int timeout_id = 0;
char message_num;
int finished = 0;

void send_request(char *type, char seq) {
    char data[5];

    data[1] = type[0]; data[2] = type[1]; data[3] = type[2]; data[4] = seq;
    data[0] = data[1] ^ data[2] ^ data[3] ^ data[4];

    send(5, data);
}

void resend(void *arg) {
    if (finished) return;

    if (expected_seq == 1) {
        send_request("GET", message_num);
    } else {
        send_request("ACK", expected_seq - 1);
    }

    timeout_id = setTimeout(resend, 2000, NULL);
}

void recvd(size_t len, void* _data) {
    char *data = _data;

    int seq = data[1];
    total_packets = data[2];

    if (seq != expected_seq) return;

    clearTimeout(timeout_id);

    fwrite(data+3,1,len-3,stdout);
    fflush(stdout);

    expected_seq += 1;

    if (expected_seq > total_packets) {
        finished = 1;
        return;
    }

    send_request("ACK", seq);
    timeout_id = setTimeout(resend, 2000, NULL);
}

int main(int argc, char *argv[]) {
    // this code should work without modification
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;
    }
    char data[5];
    data[1] = 'G'; data[2] = 'E'; data[3] = 'T'; data[4] = argv[1][0];
    // end of working code

    //fixes
    message_num = argv[1][0];
    
    data[0] = data[1] ^ data[2] ^ data[3] ^ data[4];
    send(5, data);

    timeout_id = setTimeout(resend, 2000, NULL);

    waitForAllTimeoutsAndMessagesThenExit();
}
