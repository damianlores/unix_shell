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
// tListP type is declared in types.h
typedef struct tNodeP* tPosP;
typedef struct tNodeP {
    tItemP data;
    tPosP next;
} tNodeP;


// HISTORIC LIST
void createEmptyListH (tListH *L);
void clearListH (tListH *L);
bool insertItemH (tListH *L, tHistoricItem item);
size_t countH(tListH L);
tPosH findItemH (tListH L, int id);
tHistoricItem getItemH (tListH L,tPosH pos);
tPosH firstH(tListH L);
tPosH nextH(tListH L, tPosH pos);

// OPEN FILES LIST
void createEmptyListF (tListF *L);
void clearListF (tListF *L);
bool insertItemF (tListF *L, tOFilesItem item);
void deleteAtPosF (tListF *L, tPosF pos);
tPosF findItemF (tListF L, int fd);
tOFilesItem getItemF (tListF L, tPosF pos);
tPosF firstF(tListF L);
tPosF nextF (tListF L, tPosF pos);

// MEMORY BLOCKS LIST
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

// 	PROCESSES LIST
void createEmptyListP(tListP *L);
bool insertItemP(tListP *L, tItemP item);
tItemP getItemP(tListP L, tPosP pos);
// tPosP findItemP(tListP L, );
void deleteAtPosP(tListP *L, tPosP pos);
// tPosM findItemItemP(tListP L, );
tPosM firstP(tListP L);
tPosM nextP(tListP L, tPosP pos);

#endif
