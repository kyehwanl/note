/* vim: set ts=4 sw=4: */
/* Filename    : lib_sysv_shm.c
 * Description : Shared Memory is the fastest among IPCs.
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : SHM = Shared Memory
 */
#include "sysv_shm.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

int sysv_shmget(char *tok, key_t shm_fixkey, int size, int user_mode);
int sysv_shmrm(int shm_id);

/* Name : shm_init
 * Desc : init shared memory (getting or creation)
 * Argv : long key_shm, int n_size (byte), mode(octal notation)
 * Ret  : shm ID (if success), -1 (if fail)
 */
int sysv_shmget(char *tok, key_t shm_fixkey, int size, int user_mode)
{
	key_t	shm_key;
	int	shm_id;

	if (tok != NULL) {
		if ((shm_key = ftok(tok, 1234)) == -1) {
			return -1;
		}
	} else {
		shm_key = shm_fixkey;
	}

	if ((shm_id = shmget(shm_key, size, IPC_CREAT|IPC_EXCL|user_mode)) == -1) {
		if (errno == EEXIST) { /* already exist shm, just get ipc id */
			shm_id = shmget(shm_key, 0, 0);
		}
	}
	if (shm_id == -1)
		fprintf(stderr,"FAIL: shmget():%s [%s:%d]\n", strerror(errno), __FUNCTION__, __LINE__);
	return shm_id;
}

/* Name : sysv_shmrm
 * Desc : remove shared memory
 * Argv : int shm_id or key
 * Ret  : 0 (if success), -1 (fail)
 */
int sysv_shmrm(int shm_id)
{
	int		ret;
	if ((ret = shmctl(shm_id, IPC_RMID, NULL)) == -1) {
		fprintf(stderr,"FAIL: shmctl():%s [%s:%d]\n", 
			strerror(errno), __FUNCTION__, __LINE__);
		return -1;
	}
	return ret;
}
