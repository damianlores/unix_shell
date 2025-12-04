#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include "file_system.h"
#include "dynamic_list.h"
#include "types.h"


void init_dir_params(tDirParams* dirParams) {
	dirParams->long_format = false;
	dirParams->link = false;
	dirParams->show_hid = false;
	dirParams->rec_mode = REC_OFF;
}
char letterTF(mode_t m)
{
	switch (m&S_IFMT) { // bit-by-bit AND bit with format bits, 0170000
    	case S_IFSOCK: return 's'; // socket
        case S_IFLNK: return 'l'; // symbolic link
        case S_IFREG: return '-'; // fichero normal
        case S_IFBLK: return 'b'; // block device
        case S_IFDIR: return 'd'; // directory 
        case S_IFCHR: return 'c'; // char device
        case S_IFIFO: return 'p'; // pipe
        default: return '?'; // unknown, should not appear
	}
}
/*las siguientes funciones devuelven los permisos de un fichero en formato rwx----*/
/*a partir del campo st_mode de la estructura stat */
/*las tres son correctas pero usan distintas estrategias de asignación de memoria*/
char* mode_to_str(mode_t m, char *permissions) {
    strcpy (permissions,"---------- ");
    
    permissions[0]=letterTF(m);
    if (m&S_IRUSR) permissions[1]='r';    // owner
    if (m&S_IWUSR) permissions[2]='w';
    if (m&S_IXUSR) permissions[3]='x';
    if (m&S_IRGRP) permissions[4]='r';    // group
    if (m&S_IWGRP) permissions[5]='w';
    if (m&S_IXGRP) permissions[6]='x';
    if (m&S_IROTH) permissions[7]='r';    // other
    if (m&S_IWOTH) permissions[8]='w';
    if (m&S_IXOTH) permissions[9]='x';
    if (m&S_ISUID) permissions[3]='s';    // setuid, setgid & stickybit
    if (m&S_ISGID) permissions[6]='s';
    if (m&S_ISVTX) permissions[9]='t';
    
    return permissions;
}
void modes_to_flags(char** args, int* mode) {
	for (int i = 0; args[i] != NULL; i++)
      if (!strcmp(args[i],"cr")) *mode|=O_CREAT;
      else if (!strcmp(args[i],"ex")) *mode|=O_EXCL;
      else if (!strcmp(args[i],"ro")) *mode|=O_RDONLY; 
      else if (!strcmp(args[i],"wo")) *mode|=O_WRONLY;
      else if (!strcmp(args[i],"rw")) *mode|=O_RDWR;
      else if (!strcmp(args[i],"ap")) *mode|=O_APPEND;
      else if (!strcmp(args[i],"tr")) *mode|=O_TRUNC; 
      else {
      	printf("Warning! Invalid entry: %s. Default mode was added: O_RDONLY\n", args[i]);
      	break;
      }
}
void flags_to_str_arr(int flag, char** modes) {
	int i = 0;
    if (flag & O_RDWR) { modes[i] = "O_RDWR"; i++; }
    if (flag & O_WRONLY) { modes[i] = "O_WRONLY"; i++; }
    if (flag & O_CREAT) { modes[i] = "O_CREAT"; i++; }
    if (flag & O_EXCL) { modes[i] = "O_EXCL"; i++; }
    if (flag & O_TRUNC) { modes[i] = "O_TRUNC"; i++; }
    if (flag & O_APPEND) { modes[i] = "O_APPEND"; i++; }
    modes[i] = NULL;
}
void copy_open_file_flags(char** dest, char** src) {
	int i = 0;
	while(src[i] != NULL) {
		dest[i] = src[i];
    	i++;
	}
	dest[i] = NULL;
}
int isDir(char* dir) { // whether dir is a directory or not
	struct stat s;
	if (lstat(dir,&s)==-1)       // if cannot access -> not a directory
    	return 0;
	return (S_ISDIR(s.st_mode));
}



// LIST PRINTING FUNCTIONS



void printLastNH (tListH L, int n) {
	tPosH pos = firstH(L);
    
    if (pos == LNULL || n == 0) return;

    size_t total = countH(L);
    size_t start = (n > 0 && n < total) ? total - n : 0;

    for (int i = 0; i < start; i++) {
    	pos = nextH(L, pos);
    	if (pos == LNULL) break;
    }
	while (pos != LNULL) {
        printf("%ld -> %s\n", pos->data.id, pos->data.input);
        pos = nextH(L, pos);
    }
}
void printListF(tListF L) {
	tPosF pos = firstF(L);
	tOFilesItem item;
    while (pos != NULL) {
    	item = getItemF(L, pos);
    	
    	printf("descriptor: %d -> ", item.fd);
        if (item.dup_of == -1) printf("%s", item.name);
        else printf("duplicate of %d (%s)", item.dup_of, item.name);
        int i = 0;
        while (item.mode[i] != NULL) { printf(" %s", item.mode[i]); i++; }
        putchar('\n');

        pos = nextF(L, pos);
    }
}
char* get_alloc_type_str(tMemType type) {
	switch (type) {
		case MEM_MALLOC: return "malloc";
		case MEM_MMAP: return "mmap";
		case MEM_SHARED: return "shared";
		default: return "";
	}
}
void printListM(tListM L) {
	char* type;
	tPosM pos = firstM(L);
	tItemM item;
    while (pos != NULL) {
    	item = getItemM(L, pos);
    	type = get_alloc_type_str(item.alloc_mode);
        printf("%-16p %-16ld %-30s %s",
        	item.addr,
        	item.size,
        	item.time,
        	type);
        if (item.alloc_mode == MEM_MMAP) printf(" (descriptor %d)", item.fd);
        if (item.alloc_mode == MEM_SHARED) printf(" (key %d)", item.key);
        putchar('\n');
        
        pos = nextM(L, pos);
    }
}
void printListTypeM(tListM L, tMemType type) {
	char* type_str = get_alloc_type_str(type);
	tPosM pos = firstM(L);
	tItemM item;
    while (pos != NULL) {
    	item = getItemM(L, pos);
    	if (item.alloc_mode == type)
        	printf("%-16p %-16ld %-30s %s",
        		item.addr,
        		item.size,
        		item.time,
        		type_str);
			if (type == MEM_MMAP) printf(" (descriptor %d)", item.fd);
     		if (type == MEM_SHARED) printf(" (key %d)", item.key);
     	putchar('\n');
     	
        pos = nextM(L, pos);
    }
}


void concatPath (char* dest, const char* path, const char* addition) {
    if (addition[0] == '/') { 
    	strcpy(dest,path);
    } else {
		strcpy(dest,path);
		strcat(dest,"/");
		strcat(dest, addition);
	}
}

void print_dir(char* path, tDirParams activeParams) {

	DIR *dir = opendir(path); 

	if (!dir) {
		perror("Error opening directory");
		return; 
	}
	
	struct dirent *object;
	char object_path[PATH_MAX];
	
	if (activeParams.rec_mode == REC_BEFORE) {
		while ((object = readdir(dir)) != NULL) {
			concatPath(object_path, path, object->d_name);
			if (isDir(object_path)) {
			    if ((strcmp(object->d_name, ".") != 0) &&
			    	((strcmp(object->d_name, "..") != 0))) {
                    if ((activeParams.show_hid) || (object->d_name[0] != '.'))
                        print_dir(object_path, activeParams);
                }
			}
		}
	}
	closedir(dir);

	dir = opendir(path);
	printf("******%s\n", path);
    while ((object = readdir(dir)) != NULL) {
        concatPath(object_path, path, object->d_name);
        if ((activeParams.show_hid == 1) || (object->d_name[0] != '.'))
            print_file(object_path, activeParams);
    }
    closedir (dir);
    
    if (activeParams.rec_mode == REC_AFTER) {
   		DIR *dir = opendir(path);
		while ((object = readdir(dir)) != NULL) {
			concatPath(object_path, path, object->d_name);
			if (isDir(object_path)) {
			    if ((strcmp(object->d_name, ".") != 0) &&
			    	((strcmp(object->d_name, "..") != 0))) {
                    if ((activeParams.show_hid) || (object->d_name[0] != '.'))
                        print_dir(object_path, activeParams);
                }
			}
		}
		closedir (dir);
	}

}

char* getFilename(char *path) {
    char *filename = strrchr(path, '/');
	if (filename == NULL) {
        return path;
    } else {
        return filename + 1;
    }
}

void print_file(char* path, tDirParams activeParams) {
	struct stat st;

    if (lstat(path, &st) == -1) {
    	perror(path);
    	return;
    }
    
 	char filename[NAME_MAX];
	strcpy(filename, getFilename(path));
 	
	if (!activeParams.long_format) { 
		printf("%10ld\t%s", st.st_size, filename); 
	} else {
		struct tm *tm_info = localtime(&st.st_mtime); 
		struct passwd *user = getpwuid(st.st_uid); 
		struct group *group = getgrgid(st.st_gid); 
		
		char permissions[11]; 
		mode_to_str(st.st_mode, permissions); 
		permissions[sizeof(permissions) - 1] = '\0';
		
		printf("%d/%02d/%02d-%02d:%02d ",
		tm_info->tm_year+1900,
		tm_info->tm_mon,
		tm_info->tm_mday,
		tm_info->tm_hour,
		tm_info->tm_min);
		
		printf("%3ld (%8ld)\t %s\t %s\t %s %10ld\t%s", 
			st.st_nlink,
			st.st_ino, user ? user->pw_name : "unknown",
			group ? group->gr_name : "unknown", 
			permissions, 
			st.st_size,
			filename);
	}
	if (activeParams.link && (letterTF(st.st_mode) == 'l')) {
		char linkpath[PATH_MAX];
		if (readlink(path, linkpath, NAME_MAX) == -1) 
			perror("Inaccessible linked path");
		else
			printf(" -> %s", linkpath);
	}
	putchar('\n');
}

void delete_dir(char* path) {
	DIR *dir = opendir(path);
	if (!dir) {
		perror(path);
		return; 
	}
	struct dirent *object; 
	char object_path[PATH_MAX];
	while ((object=readdir(dir))) {
	
		if (strcmp(object->d_name, ".") == 0 || strcmp(object->d_name, "..") == 0) {
            continue;
        }
        
		concatPath(object_path, path, object->d_name);
		if (isDir(object_path)) {
			delete_dir(object_path);
		} else {
			remove(object_path);
		}
	}
	closedir(dir);
	remove(path);
}



