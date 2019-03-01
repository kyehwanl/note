/* vim: set ts=4 sw=4: */
#ifndef SYSV_SHM_H
#define SYSV_SHM_H
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>

extern int sysv_shmget(char *tok, key_t shm_fixkey, int size, int user_mode);
extern int sysv_shmrm(int shm_id);

#endif
