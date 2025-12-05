#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "types.h"
#include "dynamic_list.h"



// ======================== HISTORIC LIST IMPLEMENTATION ========================s



bool createNodeH(tPosH *pos) {
    *pos = malloc(sizeof(tNodeH));
    return (*pos != LNULL);
}
void createEmptyListH(tListH *L) {
    *L = LNULL;
}
bool isEmptyList(tListH L) {
    return (L == LNULL);
}
void clearListH (tListH *L) {
    tPosH temp;
    while (*L != LNULL) {
        temp = *L;
        *L = (*L) -> next;
        free(temp);
    }
}
bool insertItemH (tListH *L, tHistoricItem item) {
    // Only 2 cases, in which one of them is a specific case of the other:
    //      1: Insert item at the end.
    //      2: Insert item with empty list.
    tPosH temp,i;
    if (!createNodeH(&temp))
        return false;

    temp -> data = item;
    temp -> next = LNULL;
    if(*L == LNULL) {
        temp -> data.id=0;
        *L = temp;
    }
    else {
        temp -> data.id=1;
        for (i = *L; i -> next != LNULL; i = i -> next)
            temp -> data.id++;
        i -> next = temp;
    }
    return true;
}
size_t countH (tListH L) {
    tPosH pos=L;
    unsigned int cnt = 0;
    while (pos != LNULL) {
        cnt++;
        pos = pos -> next;
    }
    return cnt;
}
tPosH findItemH (tListH L, int id) {
    tPosH pos=L;
    if (L == LNULL)	return LNULL;
    while (pos != LNULL && pos->data.id != id) {
        pos = pos->next;
    }
    return pos;
}
tHistoricItem getItemH(tListH L, tPosH pos) {
    return pos -> data;
}
tPosH firstH(tListH L) {
	return L;
}
tPosH nextH(tListH L, tPosH pos) {
	return pos->next;
}



// ======================== OPEN FILES LIST IMPLEMENTATION ========================



bool createNodeF(tPosF *pos) {
    *pos = malloc(sizeof(tNodeF));
    return (*pos != LNULL);
}
void createEmptyListF(tListF *L) {
    *L=LNULL;
}
void clearListF (tListF *L) {
    tPosF temp;
    while (*L != LNULL) {
        temp = *L;
        *L = (*L) -> next;
        free(temp);
    }
}
bool insertItemF (tListF *L, tOFilesItem item) {
   tPosF temp;
    if (!createNodeF(&temp))
        return false;
        
    temp -> data = item;
    temp -> next = LNULL;
    if (*L == NULL) {
        *L = temp;
    } else {
        tPosF current = *L;
        tPosF prev = NULL;
        while (current && current->data.fd < item.fd) {
            prev = current;
            current = current->next;
        }
        if (!prev) {
            // Insert at head
            temp->next = *L;
            *L = temp;
        } else {
            temp->next = current;
            prev->next = temp;
        }
    }
    return true;
}
void deleteItemF(tListF* L, int fd) {
    if (L==LNULL) return;

    tPosF current = *L;
    tPosF prev = LNULL;
    // Traverse the list looking for the node with fd
    while (current != LNULL) {
        if (current->data.fd == fd) {
            if (prev == LNULL) {
                *L = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Error: no open file with such fd.\n");
}
void deleteAtPosF(tListF* L, tPosF pos) {
    // PRECD: if (L==LNULL) return;
    tPosF current = *L;
    tPosF prev = LNULL;
    while (current != LNULL) {
        if (current == pos) {
            if (prev == LNULL) {
                *L = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}
// PreCD: fd is a valid descriptor of a file
tPosF findItemF(tListF L, int fd) {
    tPosF pos;
    for (pos = L ; fd != pos->data.fd ; pos = pos -> next);
    return pos;
}
tOFilesItem getItemF(tListF L, tPosF pos) {
    return pos->data;
}
tPosF firstF(tListF L) {
	return L;
}
tPosF nextF (tListF L, tPosF pos) {
	return pos->next;
}



// ======================== MEM BLOCKS LIST IMPLEMENTATION ========================



bool createNodeM(tPosM *pos) {
    *pos = malloc(sizeof(tNodeM));
    return (*pos != LNULL);
}
void createEmptyListM(tListM *L) {
    *L = NULL;
}
bool insertItemM(tListM *L, tItemM item) {
    struct tNodeM *temp;

    if (!createNodeM(&temp)) return false;

    temp->data = item;
    temp->next = *L;
    *L = temp;

    return true;
}
void deleteAtPosM(tListM *L, tPosM pos) {
    // PRECD: if (L==LNULL) return;
    tPosM current = *L;
    tPosM prev = LNULL;

    while (current != LNULL) {
        if (current == pos) {
            if (prev == LNULL) {
                *L = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}
tItemM getItemM(tListM L, tPosM pos) {
    return pos->data;
}
tPosM findItemM(tListM L, void *addr) {
	tPosM pos = L;
    if (L == LNULL)	return LNULL;
    while (pos != LNULL && pos->data.addr != addr) {
        pos = pos->next;
    }
    return pos;
}
tPosM findMallocItemM(tListM L, size_t size) {
	tPosM pos = L;
	tItemM item;
    while (pos != LNULL) {
		item = getItemM(L, pos);
        if (item.size == size && item.alloc_mode == MEM_MALLOC) {
            return pos;
        }
        pos = pos->next;
    }
    return LNULL;
}
tPosM findMMapItemM(tListM L, char *filename) {
	tPosM pos = L;
    if (L == LNULL)	return LNULL;
    while ((pos != LNULL) && (strcmp(pos->data.file, filename) == 0) && (pos->data.alloc_mode == MEM_MMAP)) {
        pos = pos->next;
    }
    return pos;
}
tPosM findSharedItemM(tListM L, key_t key) {
	tPosM pos = L;
    if (L == LNULL)	return LNULL;
    while ((pos != LNULL) && (pos->data.key != key) && (pos->data.alloc_mode == MEM_SHARED)) {
        pos = pos->next;
    }
    return pos;
}
tPosM firstM(tListM L) {
	return L;
}
tPosM nextM(tListM L, tPosM pos) {
    return pos -> next;
}



// ======================== PROCESSES LIST IMPLEMENTATION ========================



bool createNodeP(tPosP *pos) {
    *pos = malloc(sizeof(tNodeP));
    return (*pos != LNULL);
}
void createEmptyListP(tListP *L) {
    *L = NULL;
}
















