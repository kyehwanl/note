/* vim: set ts=4 sw=4: */
/* Filename    : lib_sysv_sem.c
 * Description : Critical section testing, 
 *               It just use specify one kind of IPC, SysV semamphore.(Not POSIX SEMAPHORE)
 * Author      : Steven Kim <sunyzero@yahoo.com>
 * Notes       : Critical section means only one process attaching/processing at same time.
 */
#include "sysv_sem.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define MAX_SEM	16

int sysv_semget(char *tok, key_t sem_fixkey, int n_sem, int sem_value, int user_mode);
int sysv_semrm(int sem_id);
int sysv_semwait(int sem_id, int sem_idx);
int sysv_semzwait(int sem_id, int sem_idx);
int sysv_sempost(int sem_id, int sem_idx);
int sysv_semval(int sem_id, int sem_idx);

/* Name : sysv_sem_init
 * Desc : init semaphore (getting or creation)
 * Argv : long key_sem, int n_sem, user_mode(octal notation)
 * Ret  : semaphore ID (if success), -1 (if fail)
 */
int sysv_semget(char *tok, key_t sem_fixkey, int n_sem, int sem_value, int user_mode)
{
	int	sem_id, i;
	union semun semun;
	unsigned short int *arr_semval = NULL;
	key_t	sem_key;

	/* if tok is not NULL, we make IPC key for semaphore */
	if (tok != NULL) {
		if ((sem_key = ftok(tok, 1234)) == -1) {
			return -1;
		}
	} else {
		sem_key = sem_fixkey;
	}
	if ((sem_id = semget(sem_key, n_sem, IPC_CREAT|IPC_EXCL|user_mode)) == -1) {
		if (errno == EEXIST) {
			sem_id = semget(sem_key, n_sem, 0);
			return sem_id;
		}
	}
	if (sem_id == -1) {
		fprintf(stderr,"FAIL: semget [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* init semaphore value */
	if ((arr_semval = (unsigned short int *)malloc(sizeof(unsigned short int) * n_sem)) == NULL) {
		fprintf(stderr, "FAIL: malloc [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	for(i=0; i<n_sem; i++) arr_semval[i] = sem_value;
	semun.array = arr_semval;
	if (semctl(sem_id, 0, SETALL, semun) == -1) {
		fprintf(stderr, "FAIL: semctl [%s:%d]\n", __FUNCTION__, __LINE__);
		if (arr_semval != NULL) free(arr_semval);
		return -1;
	}
	if (arr_semval != NULL) free(arr_semval);
	return sem_id;
}

/* Name : sysv_semrm
 * Desc : remove specified semaphore with sem_id
 * Argv : int sem_id
 * Ret  : 0 (if success), -1 (if fail)
 */
int sysv_semrm(int sem_id)
{
	if (semctl(sem_id, 0, IPC_RMID) == -1) {
		fprintf(stderr, "FAIL: semctl() 'IPC_RMID' [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/* Name : sysv_semzwait
 * Desc : wait-for-zero operation about specified SysV semaphore
 * Argv : int sem_id, int sem_idx
 * Ret  : 0 (if success), -1 (if fail)
 */
int sysv_semzwait(int sem_id, int sem_idx)
{
	struct sembuf	sem_buf;

	sem_buf.sem_num = sem_idx;
	sem_buf.sem_flg = SEM_UNDO;
	sem_buf.sem_op = 0;
	if (semop(sem_id, &sem_buf, 1) == -1) {
		fprintf(stderr, "FAIL: semop() 'P' operation [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/* Name : sysv_semwait
 * Desc : P operation about specified SysV semaphore
 * Argv : int sem_id, int sem_idx
 * Ret  : 0 (if success), -1 (if fail)
 */
int sysv_semwait(int sem_id, int sem_idx)
{
	struct sembuf	sem_buf;

	sem_buf.sem_num = sem_idx;
	sem_buf.sem_flg = SEM_UNDO;
	sem_buf.sem_op = -1;
	if (semop(sem_id, &sem_buf, 1) == -1) {
		fprintf(stderr, "FAIL: semop() 'P' operation [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/* Name : sysv_sempost
 * Desc : V operation about specified SysV semaphore
 * Argv : int sem_id, int sem_idx
 * Ret  : 0 (if success), -1 (if fail)
 */
int sysv_sempost(int sem_id, int sem_idx)
{
	struct sembuf	sem_buf;
	sem_buf.sem_num = sem_idx;
	sem_buf.sem_flg = SEM_UNDO;
	sem_buf.sem_op = 1;
	if (semop(sem_id, &sem_buf, 1) == -1) {
		fprintf(stderr, "FAIL: semop() 'P' operation [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/* Name : sysv_semval
 * Desc : get specified semaphore value 
 * Argv : int sem_id, int sem_idx
 * Ret  : semaphore value (if success), -1 (if fail)
 */
int sysv_semval(int sem_id, int sem_idx)
{
	int	semval;
	if ((semval = semctl(sem_id, sem_idx, GETVAL)) == -1) {
		fprintf(stderr, "FAIL: semctl() 'GETVAL' [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return semval;
}

