/*
 * $Id: demo.c,v 1.4 2005/12/07 20:55:52 dplonka Exp $
 *
 * This is based on "demo.c" provided with MRT-2.2.2a.
 */

#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */
#include <uthash.h>
#include <string.h>

struct my_struct {
    int id;                    /* key */
    char name[10];
    UT_hash_handle hh;         /* makes this structure hashable */
};

struct my_struct *users = NULL;

void add_user(int user_id, char *name) {
    struct my_struct *s;

    HASH_FIND_INT(users, &user_id, s);  /* id already in the hash? */
    if (s==NULL) {
      s = (struct my_struct*)malloc(sizeof(struct my_struct));
      s->id = user_id;
      HASH_ADD_INT( users, id, s );  /* id: name of key field */
    }
    strcpy(s->name, name);
}


struct my_struct *find_user(int user_id) {
    struct my_struct *s;

    HASH_FIND_INT( users, &user_id, s );  /* s: output pointer */
    return s;
}

void delete_user(struct my_struct *user) {
    HASH_DEL( users, user);  /* user: pointer to deletee */
    free(user);
}

void delete_all() {
  struct my_struct *current_user, *tmp;

  HASH_ITER(hh, users, current_user, tmp) {
    HASH_DEL(users,current_user);  /* delete it (users advances to next) */
    free(current_user);            /* free it */
  }
}

void print_users() {
    struct my_struct *s;

    for(s=users; s != NULL; s=(struct my_struct*)(s->hh.next)) {
        printf("user id %d: name %s\n", s->id, s->name);
    }
}

int name_sort(struct my_struct *a, struct my_struct *b) {
    return strcmp(a->name,b->name);
}

int id_sort(struct my_struct *a, struct my_struct *b) {
    return (a->id - b->id);
}

void sort_by_name() {
    HASH_SORT(users, name_sort);
}

void sort_by_id() {
    HASH_SORT(users, id_sort);
}


#if 1
int
main(int argc, char *argv[])
{
    const max_int = 100000;
    //const max_int = 65536;
   
    char in[10];
    int id=0, running=1;
    struct my_struct *s;
    unsigned num_users;


    char *find = NULL;
    if (argv[1] != NULL)
    {
        find = argv[1];
    }
    else 
    {
        find = "100";
    }
    int find_val = atoi(find);


#if defined (__TIME_MEASURE__)
#include "libtm_rdtsc.h"
    static unsigned int valCount=0;
    const unsigned int g_measureCount = 10;
    unsigned long long val_end_clock;

    tm_rdtsc_init(); // initialize time measure library
#endif /* __TIME_MEASURE__ */
    
    char buf[10];
    for (int i=0; i < max_int; i++)             
    {
        sprintf(buf, "%d", id);
        add_user(id++, buf);
    }
    num_users=HASH_COUNT(users);
    printf("there are %u users\n", num_users);


    while(id !=0)
    {
        printf("id? for testing perf\n");
        gets(in); 
        id = atoi(in);

#if defined (__TIME_MEASURE__)
        if(valCount == 0 && g_measureCount != 0)
            start_clock = rdtsc();
#endif /* __TIME_MEASURE__ */



        s = find_user(id);


#if defined (__TIME_MEASURE__)
        //valCount++;
        //if(valCount >= g_measureCount && g_measureCount != 0)
        {
            end_clock = rdtsc();
            //printf("count reached %ld and terminate \n", g_measureCount);
            //valCount =0;

            print_clock_time(end_clock, start_clock,      "Elapsed time");
            printf("\n");
        }
#endif /* __TIME_MEASURE__ */

        printf("user: %s\n", s ? s->name : "unknown");
    }

    printf("Work Done \n");
    exit(0);
}

#endif

#if _COMMENT_

it took average 1.6 us to find a single node in case the number of 65536 data were saved in UTHash

in vmware-005 server,  Intel(R) Xeon(R) CPU E5-2699 v3 @ 2.30GHz, 8 GB memory

#endif


#if 0
int main(int argc, char *argv[]) {
    char in[10];
    int id=1, running=1;
    struct my_struct *s;
    unsigned num_users;

    while (running) {
        printf(" 1. add user\n");
        printf(" 2. add/rename user by id\n");
        printf(" 3. find user\n");
        printf(" 4. delete user\n");
        printf(" 5. delete all users\n");
        printf(" 6. sort items by name\n");
        printf(" 7. sort items by id\n");
        printf(" 8. print users\n");
        printf(" 9. count users\n");
        printf("10. quit\n");
        gets(in);
        switch(atoi(in)) {
            case 1:
                printf("name?\n");
                add_user(id++, gets(in));
                break;
            case 2:
                printf("id?\n");
                gets(in); id = atoi(in);
                printf("name?\n");
                add_user(id, gets(in));
                break;
            case 3:
                printf("id?\n");
                s = find_user(atoi(gets(in)));
                printf("user: %s\n", s ? s->name : "unknown");
                break;
            case 4:
                printf("id?\n");
                s = find_user(atoi(gets(in)));
                if (s) delete_user(s);
                else printf("id unknown\n");
                break;
            case 5:
                delete_all();
                break;
            case 6:
                sort_by_name();
                break;
            case 7:
                sort_by_id();
                break;
            case 8:
                print_users();
                break;
            case 9:
                num_users=HASH_COUNT(users);
                printf("there are %u users\n", num_users);
                break;
            case 10:
                running=0;
                break;
        }
    }

    delete_all();  /* free any structures */
    return 0;
}


#endif
