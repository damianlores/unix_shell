#ifndef MEM_MANAGEMENT_H
#define MEM_MANAGEMENT_H


#include <sys/shm.h>
#include <sys/types.h>

void do_mmap(char *args[], tListF *OFList, tListM *memList);
void *obtainMemoryShmget(tListM *memList, key_t key, size_t size);
void freeMalloc(tListM *memList, tPosM pos);
void freeMMap(tListM *memList, tPosM pos);
void freeShared(tListM *memList, tPosM pos);
void freeBlocks(tListM *memList);
void do_deleteKeyShared (key_t key);
void doSharedAttach(char *args[], tListM *memList);
void recursive(int n);
void *stringToPointer(char *s);
ssize_t readFile (char *f, void *p, size_t cont);
ssize_t writeFile (char *f, void *p, size_t cont); // Function created to follow the same structure as readfile
void do_pmap (void);
void fillMem (void *p, size_t cont, unsigned char byte);


#endif
