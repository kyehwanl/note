/* vim: set ts=4 sw=4: */
#ifndef SYSV_MSG_H
#define SYSV_MSG_H
#include <sys/ipc.h>

extern int sysv_msgget(char *tok, key_t msg_fixkey, int user_mode);
extern int sysv_msgrm(int msg_id);

#endif
