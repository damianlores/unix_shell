#ifndef DYNAMIC_LIST_H
#define DYNAMIC_LIST_H

#include <stdbool.h>
#include <sys/types.h>

#include "types.h"

#define LNULL NULL

// tListH type is declared in types.h
typedef struct tNodeH* tPosH;
typedef struct tNodeH {
    tHistoricItem data;
    tPosH next;
} tNodeH;
// tListF type is declared in types.h
typedef struct tNodeF* tPosF;
typedef struct tNodeF {
    tOFilesItem data;
    tPosF next;
} tNodeF;
// tListM type is declared in types.h
typedef struct tNodeM* tPosM;
typedef struct tNodeM {
    tItemM data;
    tPosM next;
} tNodeM;

// HISTORIC LIST
bool createNodeH (tListH *pos);
void createEmptyListH (tListH *L);
void clearListH (tListH *L);
bool insertItemH (tListH *L, tHistoricItem item);
size_t countH(tListH L);
tPosH findItemH (tListH L, int id);
tHistoricItem getItemH (tListH L,tPosH pos);
tPosH firstH(tListH L);
tPosH nextH(tListH L, tPosH pos);

// OPEN FILES LIST
bool createNodeF (tPosF *pos);
void createEmptyListF (tListF *L);
void clearListF (tListF *L);
bool insertItemF (tListF *L, tOFilesItem item);
void deleteAtPosF (tListF *L, tPosF pos);
tPosF findItemF (tListF L, int fd);
tOFilesItem getItemF (tListF L, tPosF pos);
tPosF firstF(tListF L);
tPosF nextF (tListF L, tPosF pos);

// MEMORY BLOCKS LIST
bool createNodeM(tPosM *p);
void createEmptyListM(tListM *L);
bool insertItemM(tListM *L, tItemM item);
tItemM getItemM(tListM L, tPosM pos);
tPosM findItemM(tListM L, void *addr);
void deleteAtPosM(tListM *L, tPosM pos);
tPosM findMallocItemM(tListM L, size_t size);
tPosM findMMapItemM(tListM L, char *filename);
tPosM findSharedItemM(tListM L, key_t key);
tPosM firstM(tListM L);
tPosM nextM (tListM L, tPosM pos);

#endif
