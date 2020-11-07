/*
 * $Id: demo.c,v 1.4 2005/12/07 20:55:52 dplonka Exp $
 *
 * This is based on "demo.c" provided with MRT-2.2.2a.
 */

#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */
#include <string.h>

 
// The number of children for each node
// We will construct a N-ary tree and make it
// a Trie
// Since we have 26 english letters, we need
// 26 children per node
#define N 10
 
typedef struct TrieNode TrieNode;
 
struct TrieNode {
    // The Trie Node Structure
    // Each node has N children, starting from the root
    // and a flag to check if it's a leaf node
    char data; // Storing for printing purposes only
    TrieNode* children[N];
    int is_leaf;
};
 
TrieNode* make_trienode(char data) {
    // Allocate memory for a TrieNode
    TrieNode* node = (TrieNode*) calloc (1, sizeof(TrieNode));
    for (int i=0; i<N; i++)
        node->children[i] = NULL;
    node->is_leaf = 0;
    node->data = data;
    return node;
}
 
void free_trienode(TrieNode* node) {
    // Free the trienode sequence
    for(int i=0; i<N; i++) {
        if (node->children[i] != NULL) {
            free_trienode(node->children[i]);
        }
        else {
            continue;
        }
    }
    free(node);
}
 
TrieNode* insert_trie(TrieNode* root, char* word) {
    // Inserts the word onto the Trie
    // ASSUMPTION: The word only has lower case characters
    TrieNode* temp = root;
 
    for (int i=0; word[i] != '\0'; i++) {
        // Get the relative position in the alphabet list
        int idx = (int) word[i] - '0';
        //printf("index: %02x(%d), word[%d]: %c  \n", idx, idx, i, word[i]);
        if (temp->children[idx] == NULL) {
            // If the corresponding child doesn't exist,
            // simply create that child!
            temp->children[idx] = make_trienode(word[i]);
        }
        else {
            // Do nothing. The node already exists
        }
        // Go down a level, to the child referenced by idx
        // since we have a prefix match
        temp = temp->children[idx];
    }
    // At the end of the word, mark this node as the leaf node
    temp->is_leaf = 1;
    return root;
}
 
int search_trie(TrieNode* root, char* word)
{
    // Searches for word in the Trie
    TrieNode* temp = root;
 
    for(int i=0; word[i]!='\0'; i++)
    {
        int position = word[i] - '0';
        if (temp->children[position] == NULL)
            return 0;
        temp = temp->children[position];
    }
    if (temp != NULL && temp->is_leaf == 1)
        return 1;
    return 0;
}
 
void print_trie(TrieNode* root) {
    // Prints the nodes of the trie
    if (!root)
        return;
    TrieNode* temp = root;
    printf("%c -> ", temp->data);
    for (int i=0; i<N; i++) {
        print_trie(temp->children[i]); 
    }
}
 
void print_search(TrieNode* root, char* word) {
    printf("Searching for %s: ", word);
    if (search_trie(root, word) == 0)
        printf("Not Found\n");
    else
        printf("Found!\n");
}

#if 1
int
main(int argc, char *argv[])
{
   
    //const max_int = 65536;
    const max_int = 100000;

    char *cnt = NULL;
    if (argv[1] != NULL)
        cnt = argv[1];
    else 
        cnt = "10";

    unsigned int cnt_val = atoi(cnt);

    if (cnt_val > max_int)
        cnt_val = max_int;


#if defined (__TIME_MEASURE__)
#include "libtm_rdtsc.h"
    static unsigned int valCount=0;
    const unsigned int g_measureCount = cnt_val;
    unsigned long long val_end_clock;

    tm_rdtsc_init(); // initialize time measure library
#endif /* __TIME_MEASURE__ */
    
    TrieNode* root = make_trienode('\0');
    char buffer[10];
    
    for (int i=0; i< max_int; i++)             
    {
        //sprintf(buffer, "%d", 61500);
        sprintf(buffer, "%d", i);
        root = insert_trie(root, buffer);
    }

    unsigned int id = 1;
    char in[10];
    while(id != 0)
    {

        printf("AS number? for testing perf\n");
        gets(in); 
        id = atoi(in);

#if defined (__TIME_MEASURE__)
        if(valCount == 0 && g_measureCount != 0)
            start_clock = rdtsc();
#endif /* __TIME_MEASURE__ */



        //int result = search_trie(root, "61501"); 
        //int result = search_trie(root, "61501"); 
        int result = search_trie(root, in); 


#if defined (__TIME_MEASURE__)
        //valCount++;
        //if(valCount >= g_measureCount && g_measureCount != 0)
        {
            end_clock = rdtsc();
            //printf("count reached %ld and terminate \n", g_measureCount);
            printf(" result : %s\n", result==0? "Not Found": "Found");
            valCount =0;

            print_clock_time(end_clock, start_clock,      "Elapsed time");
            printf("\n");
        }
        print_search(root, in);
#endif /* __TIME_MEASURE__ */
    }


    printf("Work Done \n");
    exit(0);
}

#endif

#if _COMMENT_


#endif


#if 0
int main() {
    // Driver program for the Trie Data Structure Operations
    TrieNode* root = make_trienode('\0');
    root = insert_trie(root, "61500");
    root = insert_trie(root, "61301");
    //root = insert_trie(root, "hello");
    //root = insert_trie(root, "hi");
    //root = insert_trie(root, "teabag");
    //root = insert_trie(root, "teacan");

    /*
    print_search(root, "tea");
    print_search(root, "teabag");
    print_search(root, "teacan");
    print_search(root, "hi");
    print_search(root, "hey");
    print_search(root, "hello");
    */
    printf(" --- searching --- \n");
    print_search(root, "61500");
    print_search(root, "61300");
    print_search(root, "60000");

    print_trie(root);
    free_trienode(root);
    printf("\n");
    return 0;
}
#endif









