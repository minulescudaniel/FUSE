#define FUSE_USE_VERSION 30
#include <string.h>
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include<fcntl.h>
#include<string.h>

struct file{
	int inode;
	mode_t permissions;
	char*path;
	int gid;
	int uid;
	char content[1024];
	time_t accest;
	time_t mtime;
    int size;
};

struct directory{
	int inode;
	char*path;
	int gid;
	int uid;
	struct file*list_files[264];
	int number_of_directories_current_folder;
    int number_of_files_current_folder;
	struct directory*list_directories[264];
	mode_t permissions;
	time_t accest;
	time_t mtime;
};

struct directory*current_directory;
int inode=0;



char*getname(char*path)
{
	fprintf(stderr,"CALE: %s\n",path);
	char aux[100];
	char*name=(char*)malloc(100*sizeof(char));
	int k=0;
	for(int i=strlen(path)-1;i>=0;i--)
	{
		if(path[i]!='/')
			{
				aux[k]=path[i]; 
				k++;}
		else
		break;
	}
	aux[strlen(aux)]='\0';
	k=0;
	for(int i=strlen(aux)-1;i>=0;i--)
	{
		name[k]=aux[i];
		k++;
	}
	name[strlen(name)]='\0';
	fprintf(stderr,"Ce se afiseaza: %s\n",name);
	return name;
}

struct directory* getdir(struct directory* directory, const char* path) {
    if (strcmp(path, "/") == 0)
        return current_directory;

    if (path[0] == '/')
        path++;

    for (int i = 0; i < directory->number_of_directories_current_folder; i++) {
        if (strcmp(directory->list_directories[i]->path, path) == 0) {
            return directory->list_directories[i];
        }
    }

    for (int i = 0; i < directory->number_of_directories_current_folder; i++) {
        struct directory* found_directory = getdir(directory->list_directories[i], path);
        if (found_directory != NULL) {
            return found_directory;
        }
    }

    return NULL;  
}



int is_file(struct directory* directory, const char* path) {
    if (path[0] == '/')
        path++; // Eliminating "/" in the path

    for (int curr_idx = 0; curr_idx < directory->number_of_files_current_folder; curr_idx++) {
        if (strcmp(path, directory->list_files[curr_idx]->path) == 0) {
            return 1; // Returnează 1 dacă găsește fișierul
        }
    }

    for (int i = 0; i < directory->number_of_directories_current_folder; i++) {
        if (is_file(directory->list_directories[i], path)) {
            return 1; 
        }
    }

    return 0;  
}



char* directory_name(char*path)
{
    if(strchr(path,'/')==0)
    return "/";
    char*directory_name=(char*)malloc(100*sizeof(char));
    int index=0;
    for(int i=strlen(path)-1;i>=0;i--)
    {
        if(path[i]=='/')
        {
        index=i;
        break;
        }
    }
    int counter=0;
    for(int i=0;i<index;i++)
    {
        directory_name[counter]=path[i];
        counter++;
    }
    return directory_name;

}



static int do_getattr(const char *path, struct stat *st)
{
    if (strlen(path) != 1 && path[0] == '/')
        path++;

    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);
    st->st_size = 0;

    if (strcmp(path, "/") == 0 || getdir(current_directory, path) != NULL)
    {
        struct directory *dir = getdir(current_directory, path);
        if (dir) {
            st->st_mode = dir->permissions;
            st->st_nlink = 2 + dir->number_of_directories_current_folder + dir->number_of_files_current_folder;
            return 0;
        } else {
            fprintf(stderr, "Error: Directory not found for path %s\n", path);
            return -ENOENT;
        }
    }
    else if (is_file(current_directory, path) == 1)
    {
        struct directory *dir = getdir(current_directory, directory_name(path));
        if (dir) {
            for (int i = 0; i < dir->number_of_files_current_folder; i++)
            {
                if (dir->list_files[i] && dir->list_files[i]->path && strcmp(dir->list_files[i]->path, path) == 0)
                {
                    st->st_ino = dir->list_files[i]->inode;
                    st->st_size = dir->list_files[i]->size;
                    st->st_mode = dir->list_files[i]->permissions;
                    st->st_gid = dir->list_files[i]->gid;
                    st->st_uid = dir->list_files[i]->uid;
                    st->st_nlink = 1;
                    return 0;
                }
            }
            fprintf(stderr, "Error: File not found for path %s\n", path);
            return -ENOENT;
        } else {
            fprintf(stderr, "Error: Parent directory not found for file %s\n", path);
            return -ENOENT;
        }
    }
    else
    {
        fprintf(stderr, "Error: Unknown error for path %s\n", path);
        return -ENOENT;
    }
}


static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    filler(buffer, ".", NULL, 0); // Current Directory
    filler(buffer, "..", NULL, 0); // Parent Directory

    struct directory *searched_directory = getdir(current_directory, path);
    if (searched_directory != NULL) {
        for (int i = 0; i < searched_directory->number_of_directories_current_folder; i++) {
            filler(buffer, getname(searched_directory->list_directories[i]->path) , NULL, 0);
        }

        for (int i = 0; i < searched_directory->number_of_files_current_folder; i++) {
            fprintf(stderr,"CALEA DE FISIER:%s \n",getname( searched_directory->list_files[i]->path));
            filler(buffer,getname( searched_directory->list_files[i]->path), NULL, 0);
        }
    }

    return 0;
}



static int do_mknod( const char *path, mode_t mode, dev_t rdev )
{   
	path++;
	fprintf(stderr,"Asta este calea: %s",path);
	if(is_file(current_directory,path)==1)
	return -EEXIST;
	char cale[1000];
	int end_path_directory=0;
	if(!strchr(path,'/')){
		current_directory->list_files[current_directory->number_of_files_current_folder]=(struct file*)calloc(1,sizeof(struct file));
		current_directory->list_files[current_directory->number_of_files_current_folder]->path=(char*)calloc(sizeof(path),1);

		strcpy(current_directory->list_files[current_directory->number_of_files_current_folder]->path,path);
		current_directory->list_files[current_directory->number_of_files_current_folder]->path[strlen(path)]='\0';
	    current_directory->list_files[current_directory->number_of_files_current_folder]->accest=time(NULL);
	    current_directory->list_files[current_directory->number_of_files_current_folder]->mtime=time(NULL);
	    current_directory->list_files[current_directory->number_of_files_current_folder]->uid=1000;
	    current_directory->list_files[current_directory->number_of_files_current_folder]->gid=1000;
		inode++;
		current_directory->list_files[current_directory->number_of_files_current_folder]->inode=inode;
		current_directory->list_files[current_directory->number_of_files_current_folder]->size=0;
	    current_directory->list_files[current_directory->number_of_files_current_folder]->permissions=__S_IFREG|0777;
		strcpy(current_directory->list_files[current_directory->number_of_files_current_folder]->content,"");
		current_directory->number_of_files_current_folder++;
		return 0;
	}
	
	else{
		for(int i=strlen(path)-1;i>=0;i--)
	{
		if(path[i]=='/')
			{end_path_directory=i;
			break;
			}
	}
	int index=0;
	for(int i=0;i<end_path_directory;i++)
	{
      cale[index]=path[i];
	  index++;
	}
	cale[index]='\0';
	struct directory*searched_director=getdir(current_directory,cale);
	if(searched_director){
    searched_director->list_files[searched_director->number_of_files_current_folder]=(struct file*)calloc(1,sizeof(struct file));
	searched_director->list_files[searched_director->number_of_files_current_folder]->path=calloc(strlen(path),1);
	strcpy(searched_director->list_files[searched_director->number_of_files_current_folder]->path,path);
	searched_director->list_files[searched_director->number_of_files_current_folder]->accest=time(NULL);
	searched_director->list_files[searched_director->number_of_files_current_folder]->mtime=time(NULL);
	searched_director->list_files[searched_director->number_of_files_current_folder]->uid=getuid();
	searched_director->list_files[searched_director->number_of_files_current_folder]->gid=getgid();
    searched_director->list_files[searched_director->number_of_files_current_folder]->size=0;
	searched_director->list_files[searched_director->number_of_files_current_folder]->permissions=__S_IFREG|0777;
    strcpy(searched_director->list_files[searched_director->number_of_files_current_folder]->content,"");
	searched_director->number_of_files_current_folder++;
	}
  	return 0;
	}
  }


 static int do_create(const char *path, mode_t mode, struct fuse_file_info *fi)
 {
    
 	do_mknod(path,mode,NULL);
    

     return 0; 
 }


static int do_utimens(const char *path, const struct timespec tv[2])
  {
      
     path++;
	
        struct utimbuf utbuf;
        utbuf.actime = tv[0].tv_sec;
         utbuf.modtime = tv[1].tv_sec;

     return 0; 
	  }



 static int do_mkdir(const char *path, mode_t mode)
 {
    path++;
    char cale[1000] = {0};  

    if (!strchr(path, '/')) {
        current_directory->list_directories[current_directory->number_of_directories_current_folder] =(struct directory *)calloc(1, sizeof(struct directory));


        current_directory->list_directories[current_directory->number_of_directories_current_folder]->path =strdup(path);

       
        current_directory->list_directories[current_directory->number_of_directories_current_folder]->accest = time(NULL);
        current_directory->list_directories[current_directory->number_of_directories_current_folder]->mtime = time(NULL);
        current_directory->list_directories[current_directory->number_of_directories_current_folder]->uid = 1000;
        current_directory->list_directories[current_directory->number_of_directories_current_folder]->gid = 1000;
        inode++;
        current_directory->list_directories[current_directory->number_of_directories_current_folder]->inode = inode;

        current_directory->list_directories[current_directory->number_of_directories_current_folder]->permissions = __S_IFDIR | 0777;

        current_directory->number_of_directories_current_folder++;
        return 0;
    } else {
        int end_path_directory = 0;  
        for (int i = strlen(path) - 1; i >= 0; i--) {
            if (path[i] == '/') {
                end_path_directory = i;
                break;
            }
        }
        int index = 0;
        for (int i = 0; i < end_path_directory; i++) {
            cale[index] = path[i];
            index++;
        }
        cale[index] = '\0';
        struct directory *searched_director = getdir(current_directory, cale);
        if (searched_director) {
            searched_director->list_directories[searched_director->number_of_directories_current_folder] =
                (struct directory *)calloc(1, sizeof(struct directory));
            searched_director->list_directories[searched_director->number_of_directories_current_folder]->path =
                strdup(path);
            searched_director->list_directories[searched_director->number_of_directories_current_folder]->accest = time(NULL);
            searched_director->list_directories[searched_director->number_of_directories_current_folder]->mtime = time(NULL);
            searched_director->list_directories[searched_director->number_of_directories_current_folder]->uid = getuid();
            searched_director->list_directories[searched_director->number_of_directories_current_folder]->gid = getgid();
            searched_director->list_directories[searched_director->number_of_directories_current_folder]->permissions = __S_IFDIR | 0777;

            searched_director->number_of_directories_current_folder++;
        }
        return 0;
    }
 }


void *do_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
    current_directory = (struct directory *)calloc(1, sizeof(struct directory));
    current_directory->number_of_directories_current_folder = 0;
    current_directory->number_of_files_current_folder = 0;
    current_directory->path = strdup("/");
    current_directory->permissions =__S_IFDIR | 0777; 
    current_directory->uid = getuid();
    current_directory->gid = getgid();
    current_directory->mtime = time(NULL);
    current_directory->accest = time(NULL);

    return NULL;
}

static int do_unlink(const char *path) {
    path++;

    if (!is_file(current_directory, path))
        return -ENOENT;

    struct directory *dir = getdir(current_directory, directory_name(path));
    if (dir) {
        for (int i = 0; i < dir->number_of_files_current_folder; i++) {
            if (dir->list_files[i] && dir->list_files[i]->path && strcmp(dir->list_files[i]->path, path) == 0) {
                free(dir->list_files[i]->path);
                free(dir->list_files[i]);
                for (int j = i; j < dir->number_of_files_current_folder - 1; j++) {
                    dir->list_files[j] = dir->list_files[j + 1];
                }
                dir->number_of_files_current_folder--;
                return 0;
            }
        }
    }

    
}


void delete_directory(struct directory *dir) {
    for (int i = 0; i < dir->number_of_files_current_folder; i++) {
        free(dir->list_files[i]->path);
        free(dir->list_files[i]);
    }
    for (int i = 0; i < dir->number_of_directories_current_folder; i++) {
        delete_directory(dir->list_directories[i]);
        free(dir->list_directories[i]->path);
        free(dir->list_directories[i]);
    }
}




static int do_rmdir(const char *path) {
    path++;

    if (!getdir(current_directory, path))
        return -ENOENT;

    struct directory *parent_dir = getdir(current_directory, directory_name(path));
    if (parent_dir) {
        for (int i = 0; i < parent_dir->number_of_directories_current_folder; i++) {
            if (parent_dir->list_directories[i] && parent_dir->list_directories[i]->path &&
                strcmp(parent_dir->list_directories[i]->path, path) == 0) {
                delete_directory(parent_dir->list_directories[i]);
                free(parent_dir->list_directories[i]->path);
                free(parent_dir->list_directories[i]);
                for (int j = i; j < parent_dir->number_of_directories_current_folder - 1; j++) {
                    parent_dir->list_directories[j] = parent_dir->list_directories[j + 1];
                }
                parent_dir->number_of_directories_current_folder--;
                return 0;
            }
        }
    }

    return -ENOENT; 
}


static int do_rename(const char *old_path, const char *new_path) {
    if (old_path[0] == '/')
        old_path++;
    if (new_path[0] == '/')
        new_path++;

    if (is_file(current_directory, old_path)) {
        int index = -1;
        struct directory *dir = getdir(current_directory, directory_name(old_path));
        if (dir) {
            for (int i = 0; i < dir->number_of_files_current_folder; i++) {
                if (strcmp(dir->list_files[i]->path, old_path) == 0) {
                    index = i;
                    break;
                }
            }

            if (index != -1) {
                char *new_pathx = (char *)realloc(dir->list_files[index]->path, sizeof(char) * (strlen(new_path) + 1));
                if (new_pathx == NULL) {
                    return -ENOMEM;
                }
                dir->list_files[index]->path = new_pathx;
                strcpy(dir->list_files[index]->path, new_path);
                return 0;
            } else {
                return -ENOENT;
            }
        } else {
            fprintf(stderr, "Unable to find the host folder for the file!\n");
            return -ENOENT;
        }
    }

    if (getdir(current_directory, old_path)) {
        struct directory *dir = getdir(current_directory, old_path);
        char *new_pathx = (char *)realloc(dir->path, sizeof(char) * (strlen(new_path) + 1));
        if (new_pathx == NULL) {
            
            return -ENOMEM;
        }
        dir->path = new_pathx;
        strcpy(dir->path, new_path);
        return 0;
    }

    return -ENOENT;
}



static int do_chmod(const char *path, mode_t mode) {
    if(path[0]=='/')
    path++;
    if(is_file(current_directory,path))
    {
        struct directory*dir=getdir(current_directory,directory_name(path));
        for(int i=0;i<dir->number_of_files_current_folder;i++)
        {
            if(strcmp(dir->list_files[i]->path,path)==0)
            {
                dir->list_files[i]->permissions=mode;
                return 0;
            }
        }
    }
    if(getdir(current_directory,path))
    {
        struct directory*dir=getdir(current_directory,path);
        dir->permissions=mode;
        return 0;
    }
    return -EACCES;
}

static int do_open(const char *path, struct fuse_file_info *fi) {
    path++;

    if (!is_file(current_directory, path))
        return -ENOENT;
    

    
     
    return 0;
    
}





static int do_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    path++;

    if (!is_file(current_directory, path))
        return -ENOENT;

    struct directory *dir = getdir(current_directory, directory_name(path));
    if (dir) {
        for (int i = 0; i < dir->number_of_files_current_folder; i++) {
            if (dir->list_files[i] && dir->list_files[i]->path && strcmp(dir->list_files[i]->path, path) == 0) {
                if (offset < dir->list_files[i]->size) {
                    size_t remaining_size = dir->list_files[i]->size - offset;
                    size_t read_size = size > remaining_size ? remaining_size : size;

                
                    if (offset + read_size > sizeof(dir->list_files[i]->content)) {
                        fprintf(stderr, "Error: Buffer overflow in do_read for file %s\n", path);
                        return -EFBIG;
                    }

                  
                    strncpy(buf, dir->list_files[i]->content + offset, read_size);

                    return read_size;
                } else {
                    
                    return 0;
                }
            }
        }
        fprintf(stderr, "Error: File not found for path %s\n", path);
        return -ENOENT;
    } else {
        fprintf(stderr, "Error: Parent directory not found for file %s\n", path);
        return -ENOENT;
    }
}


    

static int do_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    path++;

    if (!is_file(current_directory, path))
        return -ENOENT;

    struct directory *dir = getdir(current_directory, directory_name(path));
    for (int i = 0; i < dir->number_of_files_current_folder; i++) {
        if (strcmp(dir->list_files[i]->path, path) == 0) {
            if (offset + size > sizeof(dir->list_files[i]->content)) {
                return -EFBIG;
            }

            
            strncpy(dir->list_files[i]->content + offset, buf, size);

           
            dir->list_files[i]->mtime = time(NULL);
            dir->list_files[i]->size = offset + size;

       
            dir->list_files[i]->content[dir->list_files[i]->size] = '\0';

            return size;
        }
    }

    return -ENOENT; 
}


static int do_setattr(const char *path, struct stat *st, struct fuse_file_info *fi) {
    path++;

    if (!is_file(current_directory, path))
        return -ENOENT;

    struct directory *dir = getdir(current_directory, directory_name(path));
    for (int i = 0; i < dir->number_of_files_current_folder; i++) {
        if (strcmp(dir->list_files[i]->path, path) == 0) {
       
            dir->list_files[i]->accest = st->st_atime;
            dir->list_files[i]->mtime = st->st_mtime;
            dir->list_files[i]->permissions = st->st_mode;

            return 0;
        }
    }

    return -ENOENT; 
}

static int do_truncate(const char *path, off_t size) {
    path++;

    if (!is_file(current_directory, path))
        return -ENOENT;

    struct directory *dir = getdir(current_directory, directory_name(path));
    for (int i = 0; i < dir->number_of_files_current_folder; i++) {
        if (strcmp(dir->list_files[i]->path, path) == 0) {
            
            dir->list_files[i]->size = size;

            
            dir->list_files[i]->content[dir->list_files[i]->size] = '\0';

            return 0;
        }
    }

    return -ENOENT;
}


static int do_chown(const char *path, uid_t uid, gid_t gid) {
    path++;

    if (is_file(current_directory, path)) {
        struct directory *dir = getdir(current_directory, directory_name(path));
        for (int i = 0; i < dir->number_of_files_current_folder; i++) {
            if (strcmp(dir->list_files[i]->path, path) == 0) {
               
                dir->list_files[i]->uid = uid;
                dir->list_files[i]->gid = gid;

                return 0;
            }
        }
    } else if (getdir(current_directory, path)) {
        struct directory *dir = getdir(current_directory, path);
        
        dir->uid = uid;
        dir->gid = gid;

        return 0;
    }

    return -ENOENT; 
}



static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .readdir	= do_readdir,
    .unlink     = do_unlink,
    .mknod		= do_mknod,
    .mkdir      = do_mkdir,
	.create     = do_create,
	.utimens    = do_utimens,
    .rmdir      =do_rmdir,
	.init       = do_init,
    .rename     =do_rename,
    .chmod      =do_chmod,
    .open       =do_open,
    .read       =do_read,
    .write      =do_write,
    .truncate   = do_truncate,
    .chown      = do_chown,
};


int main( int argc, char *argv[] )
{
	umask(0022);
	return fuse_main( argc, argv, &operations, NULL );
}