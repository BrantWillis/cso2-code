#include "lab.h"
#include <string.h>
#include <stdio.h>

/* forward function for "attack 0" case.
   change this code to implement your attack
 */
void forward_attack_0(struct message *message) {
    if(message->from == 'A' && message->to == 'B') {
        strcpy(message->data, "PAY $5000719 TO M");
        message->data_size = strlen("PAY $5000719 TO M");
    }
    send_message(message);
}

/* forward function for "attack 1" case.
   change this code to implement your attack
 */
void forward_attack_1(struct message *message) {
    if(message->from == 'A' && message->to == 'B') {
        struct message *new = new_message(
            'A',
            'B',
            "PAY $5000719 TO M",   // NULL-terminated string
            true,  // encrypt the message
            false      // sign the message --- only supported if 'from' is 'M'
        );
        send_message(new);
    }
    else {
        send_message(message);
    }
}

/* forward function for "attack 2" case.
   change this code to implement your attack
 */
void forward_attack_2(struct message *message) {
    if(message->from == 'A' && message->to == 'B') {
        send_message(message);
        send_message(message);
    }
}

/* forward function for "attack 3" case.
   change this code to implement your attack
 */
void forward_attack_3(struct message *message) {
    static int req_count = 0;

    if(message->from == 'A' && message->to == 'B') {
        req_count += 1;
        send_message(message);

        //duplicate second message (asking for $1000)
        if(req_count == 2) {
            send_message(message);
        }
    }
    else {
        send_message(message);
    }
}

/* forward function for "attack 4" case.
   change this code to implement your attack */
void forward_attack_4(struct message *message) {
    send_message(message);
}

/* forward function for "attack 5" case.
   I did not intend this one to be possible. */
void forward_attack_5(struct message *message) {
    send_message(message);
}
