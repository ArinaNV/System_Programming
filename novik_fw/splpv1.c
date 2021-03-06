/* 
 * SPLPv1.c
 * The file is part of practical task for System programming course. 
 * This file contains validation of SPLPv1 protocol. 
 */


// Specify your name and group
/*
 Yaroslav 13 group
*/



/*
---------------------------------------------------------------------------------------------------------------------------
# |      STATE      |         DESCRIPTION       |           ALLOWED MESSAGES            | NEW STATE | EXAMPLE
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
1 | INIT            | initial state             | A->B     CONNECT                      |     2     |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
2 | CONNECTING      | client is waiting for con-| A<-B     CONNECT_OK                   |     3     |
  |                 | nection approval from srv |                                       |           |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
3 | CONNECTED       | Connection is established | A->B     GET_VER                      |     4     |
  |                 |                           |        -------------------------------+-----------+----------------------
  |                 |                           |          One of the following:        |     5     |
  |                 |                           |          - GET_DATA                   |           |
  |                 |                           |          - GET_FILE                   |           |
  |                 |                           |          - GET_COMMAND                |           |
  |                 |                           |        -------------------------------+-----------+----------------------
  |                 |                           |          GET_B64                      |     6     |
  |                 |                           |        ------------------------------------------------------------------
  |                 |                           |          DISCONNECT                   |     7     |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
4 | WAITING_VER     | Client is waiting for     | A<-B     VERSION ver                  |     3     | VERSION 2
  |                 | server to provide version |          Where ver is an integer (>0) |           |
  |                 | information               |          value. Only a single space   |           |
  |                 |                           |          is allowed in the message    |           |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
5 | WAITING_DATA    | Client is waiting for a   | A<-B     CMD data CMD                 |     3     | GET_DATA a GET_DATA
  |                 | response from server      |                                       |           |
  |                 |                           |          CMD - command sent by the    |           |
  |                 |                           |           client in previous message  |           |
  |                 |                           |          data - string which contains |           |
  |                 |                           |           the following allowed cha-  |           |
  |                 |                           |           racters: small latin letter,|           |
  |                 |                           |           digits and '.'              |           |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
6 | WAITING_B64_DATA| Client is waiting for a   | A<-B     B64: data                    |     3     | B64: SGVsbG8=
  |                 | response from server.     |          where data is a base64 string|           |
  |                 |                           |          only 1 space is allowed      |           |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
7 | DISCONNECTING   | Client is waiting for     | A<-B     DISCONNECT_OK                |     1     |
  |                 | server to close the       |                                       |           |
  |                 | connection                |                                       |           |
---------------------------------------------------------------------------------------------------------------------------

IN CASE OF INVALID MESSAGE THE STATE SHOULD BE RESET TO 1 (INIT)

*/

#include "splpv1.h"
#include<string.h>




/* FUNCTION:  validate_message
 *
 * PURPOSE:
 *    This function is called for each SPLPv1 message between client
 *    and server
 *
 * PARAMETERS:
 *    msg - pointer to a structure which stores information about
 *    message
 *
 * RETURN VALUE:
 *    MESSAGE_VALID if the message is correct
 *    MESSAGE_INVALID if the message is incorrect or out of protocol
 *    state
 */

const char* Commands[] = { "GET_DATA", "GET_COMMAND", "GET_FILE" };

const char symbols[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };

const char symbolsBase64[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                               0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };

int state = 1;
int currentCommand = 0;
enum test_status validate_message( struct Message *msg )
{
    if (msg->direction == A_TO_B)
    {
        if (state == 1 && strcmp(msg->text_message, "CONNECT") == 0)
        {
            state = 2;
            return MESSAGE_VALID;
        }
        if (state == 3)
        {
            if (strcmp(msg->text_message, "GET_VER") == 0)
            {
                state = 4;
                return MESSAGE_VALID;
            }
            if (strcmp(msg->text_message, Commands[0]) == 0)
            {
                currentCommand = 1;
                state = 5;
                return MESSAGE_VALID;
            }
            if (strcmp(msg->text_message, Commands[1]) == 0)
            {
                currentCommand = 2;
                state = 5;
                return MESSAGE_VALID;
            }
            if (strcmp(msg->text_message, Commands[2]) == 0)
            {
                currentCommand = 3;
                state = 5;
                return MESSAGE_VALID;
            }
            if (strcmp(msg->text_message, "GET_B64") == 0)
            {
                state = 6;
                return MESSAGE_VALID;
            }
            if (strcmp(msg->text_message, "DISCONNECT") == 0)
            {
                state = 7;
                return MESSAGE_VALID;
            }
        }
    }
    else
    {
        if (state == 2 && strcmp(msg->text_message, "CONNECT_OK") == 0)
        {
            state = 3;
            return MESSAGE_VALID;
        }
        if (state == 7 && strcmp(msg->text_message, "DISCONNECT_OK") == 0)
        {
            state = 1;
            return MESSAGE_VALID;
        }
        if (state == 4)
        {
            int i = 7;
            if (strncmp(msg->text_message, "VERSION", i) == 0)
            {
                if (msg->text_message[i] == ' ')
                    i++;
                else
                {
                    state = 1;
                    return MESSAGE_INVALID;
                }
                while (msg->text_message[i] != '\0')
                {
                    if (symbols[(int)msg->text_message[i]] == 2)
                        i++;
                    else
                    {
                        state = 1;
                        return MESSAGE_INVALID;
                    }
                }
                state = 3;
                return MESSAGE_VALID;
            }
        }
        if (state == 5)
        {
            int comm_length = strlen(Commands[currentCommand - 1]);
            if (strncmp(msg->text_message, Commands[currentCommand - 1], comm_length) == 0)
            {
                int tmp = comm_length;
                char* msge = msg->text_message;
                if (msge[tmp] == ' ')
                {
                    tmp++;
                    while (symbols[(int)msge[tmp]] > 0)
                    {
                        tmp++;
                    }
                    if (msge[tmp] == ' ' && strncmp((msge + tmp + 1), Commands[currentCommand - 1], comm_length) == 0)
                    {
                        state = 3;
                        return MESSAGE_VALID;
                    }
                }
            }
        }
        if (state == 6)
        {
            char* msge = msg->text_message;
            if (strncmp(msge, "B64: ", 5) == 0)
            {
                int tmp = 0;
                tmp += 5;
                int beg = tmp;
                while (symbolsBase64[(int)msge[tmp]] == 1)
                    tmp++;
                int cnt = 0;
                while (msge[tmp] != '\0' && msge[tmp] == '=')
                {
                    cnt++;
                    tmp++;
                    if (cnt > 2)
                    {
                        state = 1;
                        return MESSAGE_INVALID;
                    }
                }
                if (msge[tmp] == '\0' && (tmp - beg) % 4 == 0)
                {
                    state = 3;
                    return MESSAGE_VALID;
                }
            }
        }

    }
    state = 1;
    return MESSAGE_INVALID;
}