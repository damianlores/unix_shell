#ifndef DYNAMIC_LIST_H
#define DYNAMIC_LIST_H

#include <stdbool.h>
#include <sys/types.h>

#include "types.h"

#define LNULL NULL


typedef struct tNodeH* tPosH;

typedef struct tNodeH {
    tHistoricItem data;
    tPosH next;
} tNodeH;

// tListH type is declared in types.h

bool createNodeH (tListH *pos);
void createEmptyListH (tListH *L);
void clearListH (tListH *L);
bool insertItemH (tListH *L, tHistoricItem item);
void printLastNH(tListH L,int n);
size_t countH(tListH L);
tPosH findItemH (tListH L, int id);
tPosH firstH(tListH L);
tHistoricItem getItemH (tListH L,tPosH pos);

typedef struct tNodeF *tPosF;

typedef struct tNodeF {
    tOFilesItem data;
    tPosF next;
} tNodeF;

// tListF type is declared in types.h

bool createNodeF (tPosF *pos);
void createEmptyListF (tListF *L);
void clearListF (tListF *L);
bool insertItemF (tListF *L, tOFilesItem item);
void deleteAtPosF (tListF *L, tPosF pos);
tPosF findItemF (tListF L, int fd);
tOFilesItem getItemF (tListF L, tPosF pos);
void printListF(tListF L);
tPosF firstF(tListF L);
tPosF nextF (tListF L, tPosF pos);




typedef struct tNodeM *tPosM;

typedef struct tNodeM {
    tItemM data;
    struct tNodeM *next;
} tNodeM;

// tListM type is declared in types.h

bool createNodeM(tPosM *p);
void createEmptyListM(tListM *L);
bool insertItemM(tListM *L, tItemM item);
tItemM getItemM(tListM L, tPosM pos);
tPosM findItemM(tListM L, void *addr);
tPosM findMallocItemM(tListM L, size_t size);
tPosM findMMapItemM(tListM L, char *filename);
tPosM findSharedItemM(tListM L, key_t key);
tPosM firstM(tListM L);
tPosM nextM (tListM L, tPosM pos);
void deleteAtPosM(tListM *L, tPosM pos);
void printListM(tListM L);
void printListMallocM(tListM L);
void printListMMapM(tListM L);
void printListSharedM(tListM L);

#endif
