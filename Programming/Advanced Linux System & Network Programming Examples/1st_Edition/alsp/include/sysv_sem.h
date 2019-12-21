/* vim: set ts=4 sw=4: */
#ifndef SYSV_SEM_H
#define SYSV_SEM_H

#if defined(__GNU_LIBRARY__) && !(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
	int val;                    /* SETVALÀ» À§ÇÑ°ª */
	struct semid_ds *buf;       /* IPC_STAT, IPC_SETÀ» À§ÇÑ ¹öÆÛ */
	unsigned short int *array;  /* GETALL, SETALLÀ» À§ÇÑ ¹è¿­ */
	struct seminfo *__buf;      /* IPC_INFOÀ» À§ÇÑ ¹öÆÛ */
};
#endif

#include <sys/ipc.h>
extern int sysv_semget(char *tok, key_t sem_fixkey, int n_sem, int sem_value, int user_mode);
extern int sysv_semrm(int sem_id);
extern int sysv_semwait(int sem_id, int sem_idx);
extern int sysv_semzwait(int sem_id, int sem_idx);
extern int sysv_sempost(int sem_id, int sem_idx);
extern int sysv_semval(int sem_id, int sem_idx);

#endif
