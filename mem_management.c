#include "types.h"
#include "dynamic_list.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>


#define SIZE 1024



void *obtainMemoryShmget(tListM *memList, key_t key, size_t size) {
    void *p;
    int aux, id, flags=0777; // least 9 significant bits of flags: permissions
    struct shmid_ds s;

    if (size)     // size != 0 means create
        flags=flags | IPC_CREAT | IPC_EXCL; // When not creating we pass size 0
    if (key==IPC_PRIVATE)  { // Invalid key
    	errno = EINVAL;
    	return NULL;
	}
    if ((id = shmget(key, size, flags)) == -1)
        return (NULL);
    if ((p = shmat(id, NULL, 0)) == (void*) -1) {
        aux=errno;
        if (size)
        	shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id, IPC_STAT, &s); // if not create, we need size, which is s.shm_segsz
    
	time_t raw_time = time(NULL);
    struct tm *tm_info;
    char time_buffer[TIME_BUFFER_MAX];
    tm_info = localtime(&raw_time); 
    strftime(time_buffer, sizeof(time_buffer), "%a %b %d %H:%M:%S %Y", tm_info);
    
    // Update list
    tItemM item = DEFAULT_ITEM_M; // Unchanged values aren't usable as type of allocation is not a mapped file (filename, fd)
    item.addr = p;
    item.size = s.shm_segsz;
    strcpy(item.time, time_buffer);
    item.alloc_mode = MEM_SHARED;
    item.key = key;

    insertItemM(memList, item);
    
    return (p);
}

void doSharedAttach(char *args[], tListM *memList) {
	key_t key;
    int id;
    void *addr;
    struct shmid_ds s;

    key = (key_t) strtoul(args[1], NULL, 10);
    if (key == 0) { printf("Invalid key.\n"); return; }

    // passing 0 as size means search for existing
    // 0 flag means don't create, just find
    id = shmget(key, 0, 0);
    
    if (id == -1) { perror("Cannot find shared memory key"); return; }

    // attach memory
    addr = shmat(id, NULL, 0);
    // (void *)-1) is the value returned by shmat in error
    if (addr == (void *)-1) { perror("Cannot attach shared memory"); return; }


    if (shmctl(id, IPC_STAT, &s) == -1) {
        perror("Cannot get shared memory info");
        shmdt(addr); 
        return;
    }
	time_t raw_time = time(NULL);
    struct tm *tm_info;
    char time_buffer[TIME_BUFFER_MAX];
    tm_info = localtime(&raw_time); 
    strftime(time_buffer, sizeof(time_buffer), "%a %b %d %H:%M:%S %Y", tm_info);

    tItemM item = DEFAULT_ITEM_M;
    item.addr = addr;
    item.size = s.shm_segsz;
    strcpy(item.time, time_buffer);
    item.alloc_mode = MEM_SHARED;
    item.key = key;

    insertItemM(memList, item);
    
    printf("Shared memory attached at %p (key %d, size %lu)\n", addr, key, s.shm_segsz);
}



void *mapFile(char *file, int protection, tListF *OFList, tListM *memList) {
    int fd, map = MAP_PRIVATE, mode = O_RDONLY;
    struct stat st;
    void *p;

    if (protection & PROT_WRITE)
    	mode = O_RDWR;
    if (stat(file, &st) == -1 || (fd = open(file, mode)) == -1)
        return NULL;
    if ((p = mmap(NULL, st.st_size, protection, map, fd, 0)) == MAP_FAILED)
        return NULL;
    
    tOFilesItem itemF;
    itemF.fd = fd;
    itemF.dup_of=-1;
    strcpy(itemF.name, file);
    if (mode |= O_RDWR)
    	strcpy(itemF.mode, "rw");
    else 
    	strcpy(itemF.mode, "ro");
    insertItemF(OFList, itemF);
    
	time_t raw_time = time(NULL);
    struct tm *tm_info;
    char time_buffer[TIME_BUFFER_MAX];
    tm_info = localtime(&raw_time); 
    strftime(time_buffer, sizeof(time_buffer), "%a %b %d %H:%M:%S %Y", tm_info);
    
    tItemM item = DEFAULT_ITEM_M;
    item.addr = p;    
    item.size = st.st_size;
    strcpy(item.time, time_buffer);
    item.alloc_mode = MEM_MMAP;
    item.fd = fd;
    strcpy(item.file, file);
    
    insertItemM(memList, item);
/* Guardar en la lista    InsertarNodoMmap (&L,p, s.st_size,df,fichero); */
/* Gurdas en la lista de descriptores usados df, fichero*/
    return p;
}



void do_mmap(char *args[], tListF *OFList, tListM *memList) { 
    char *perm;
	void *p;
    int protection=0;
     
	if (args[0] == NULL) {
    	printListMMapM(*memList); 
    	return;
    }
    if ((perm = args[1]) != NULL && strlen(perm) < 4) {
    	if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
		if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
		if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p = mapFile(args[1], protection, OFList, memList)) == NULL)
		perror("Impossible to map file");
    else
		printf ("File %s mapped at %p\n", args[0], p);
}


void freeMalloc(tListM *memList, tPosM pos) {
	free(getItemM(*memList, pos).addr);
    deleteAtPosM(memList, pos); 	// Delete node in list
}
void freeMMap(tListM *memList, tPosM pos) {
	tItemM block = getItemM(*memList, pos);
	
    munmap(block.addr, block.size); // unmap file
    close(block.fd);				// close file
    
	deleteAtPosM(memList, pos); // delete from list
}
void freeShared(tListM *memList, tPosM pos) {
    if (shmdt(getItemM(*memList, pos).addr) == -1) {	// Detach shared memory
    	perror("Unable to detach shared block");
    	return;
    }
    deleteAtPosM(memList, pos); 	// Delete node in list
}

void freeBlocks(tListM *memList) {
	tPosM pos;
	tItemM item;
	while((pos = firstM(*memList)) != LNULL) {
		item = getItemM(*memList, pos);
		switch (item.alloc_mode) {
			case MEM_MALLOC:
				freeMalloc(memList, pos);
				break;
			case MEM_MMAP:
				freeMMap(memList, pos);
				break;
			case MEM_SHARED:
				freeShared(memList, pos);	
				break;
			default:
				break;
		}
	}
}

void do_deleteKeyShared (key_t key) {
    int id;
	if (key == IPC_PRIVATE) {
		printf ("Delete key needs valid key\n");
        return;
    }
    if ((id = shmget(key, 0, 0666)) == -1){
        perror ("shmget: Impossible to access shared memory");
        return;
    }
    if (shmctl(id, IPC_RMID, NULL) == -1)
		perror ("shmctl: Impossible to delete shared memory");
}

void recursive(int n) {
	char automatic[SIZE];
	static char unmutable[SIZE];

	printf ("parameter: %3d (%p) array %p, static array %p\n",n,&n,automatic, unmutable);

	if (n > 0)
    recursive(n-1);
}

void *stringToPointer (char *s) {
	void *p;
	sscanf(s, "%p", &p);
	if (p == NULL)
		errno=EFAULT;
	return p;
}

ssize_t readFile(char *f, void *p, size_t cont)
{
    struct stat s;
    ssize_t n;  
    int df,aux;

    if (stat (f, &s) == -1 || (df = open(f, O_RDONLY)) == -1)
		return -1;     
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
	cont=s.st_size;
    if ((n = read(df, p, cont)) == -1) {
		aux=errno;
		close(df);
		errno=aux;
		return -1;
    }
    close (df);
    return n;
}



ssize_t writeFile(char *f, void *p, size_t cont) {
    if (!f || !p) {
        errno = EFAULT;
        return -1;
    }

    int fd = open(f, O_WRONLY | O_CREAT, 0644);
    if (fd == -1)
        return -1;

    ssize_t n = write(fd, p, cont);
    if (n == -1) {
        close(fd);
        return -1;
    }

    close(fd);
    return n;
}



void do_pmap (void) { 
	pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4]= {"pmap", elpid, NULL};
   
    sprintf (elpid, "%d", (int)getpid());
   
    if ((pid = fork()) == -1){
    	perror ("Imposible crear proceso");
      	return;
	}
    if (pid==0){
    	if (execvp(argv[0], argv)==-1)
        	perror("cannot execute pmap (linux, solaris)"); 
      	argv[0] = "procstat"; 
      	argv[1] = "vm"; 
      	argv[2]=elpid; 
      	argv[3]=NULL;
     	if (execvp(argv[0], argv) == -1)/*No hay pmap, probamos procstat FreeBSD */
        	perror("cannot execute procstat (FreeBSD)");
         
      	argv[0] = "procmap"; 
      	argv[1] = elpid;
      	argv[2]=NULL;    
    	if (execvp(argv[0], argv) == -1)  /*probamos procmap OpenBSD*/
        	perror("cannot execute procmap (OpenBSD)");
         
      	argv[0]="vmmap";
      	argv[1]="-interleave";
      	argv[2]=elpid;
      	argv[3]=NULL;
      	if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
        	perror("cannot execute vmmap (Mac-OS)");

      	exit(1);
	}
	waitpid (pid, NULL, 0);
}

void fillMem (void *p, size_t cont, unsigned char byte) {
	unsigned char *arr=(unsigned char *) p;
	size_t i;
	for (i=0; i<cont;i++)
		arr[i]=byte;
}




