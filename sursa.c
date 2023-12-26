#define FUSE_USE_VERSION 30
#define _CRT_SECURE_NO_WARNINGS
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
#include<time.h>

struct file{
	int inode;
	mode_t permissions;
	char*path;
	int gid;
	int uid;
	char*content;
	time_t accest;
	time_t mtime;
    int size;
    int nlinks;
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
    int nlinks;
	mode_t permissions;
	time_t accest;
	time_t mtime;
};

struct directory*current_directory;
int inode;
char*persistency_directory="/home/alex/Desktop/Memory/";


const char* getname(char* path)
{
    if (strchr(path, '/') == 0)
        return path;
    char* copy = strdup(path);
    char* p = strtok(copy, "/\n");
    while (p)
    {
        const char* name = p;
        p = strtok(NULL, "/\n");
        if (p == NULL)
        {
            return name;
        }

    }

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

void add_information_atribute_structuri(char*information_buffer,char type)
{
    int fileDescriptor=-1;
    if(type=='f')
    fileDescriptor = open("Atribute_fisiere.txt",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    else
    fileDescriptor=open("Atribute_directoare.txt",O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (fileDescriptor == -1) {
        perror("Error opening file");
        return ; // Return an error code
    }

    
    ssize_t bytesWritten = write(fileDescriptor, information_buffer, strlen(information_buffer));

    if (bytesWritten == -1) {
        perror("Error writing to file");
        close(fileDescriptor); // Close the file descriptor before returning
        return ; // Return an error code
    }

    close(fileDescriptor);

    printf("Data has been written to the file successfully.\n");

    return; // Return success
}
    
void update_information(char*searched_path,char*new_path,mode_t permissions,int inode,int size,int nlinks,int gid,int uid,time_t atime,time_t mtime ,char type)
{
                int fd=-1;
                if(type=='f')
                fd=open("Atribute_fisiere.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                else
                fd=open("Atribute_directoare.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                char linii[100][100];
                char*content=(char*)malloc(40960);
                read(fd,content,40960);
                char*line=strtok(content,"\n");
                char*searched=strdup(searched_path);
               
                strcat(searched,":");
                int counter=0;
                while(line)
                {
                    if(strstr(line,searched)==NULL)
                    {
                        strcpy(linii[counter],line);
                        counter++;
                    }
                    else
                    {
                        strcpy(linii[counter],new_path);
                        strcat(linii[counter],": ");


                        char modeString[10]; 
                        sprintf(modeString, "%o", permissions);
                        strcat(linii[counter],modeString);
                        strcat(linii[counter]," ");

                        char inodeString[20];  
                        sprintf(inodeString,"%d",inode);
                        strcat(linii[counter],inodeString);
                        strcat(linii[counter]," ");

                        if(type=='f'){
                        char sizeString[20];  
                        sprintf(sizeString,"%d",size);
                        strcat(linii[counter],sizeString);
                        strcat(linii[counter]," ");
                        }

                        char nlinksString[20];
                        sprintf(nlinksString,"%d",nlinks);
                        strcat(linii[counter],nlinksString);
                        strcat(linii[counter]," ");

                        char gidString[20];
                        sprintf(gidString,"%d",gid);
                        strcat(linii[counter],gidString);
                        strcat(linii[counter]," ");

                        char uidString[20];
                        sprintf(uidString,"%d",uid);
                        strcat(linii[counter],uidString);
                        strcat(linii[counter]," ");

                        char*timestamp1[30];
                        sprintf(timestamp1,"%ld",atime);
                        strcat(linii[counter],timestamp1);
                        strcat(linii[counter]," ");

                        char*timestamp2[30];
                        sprintf(timestamp2,"%ld",mtime);
                        strcat(linii[counter],timestamp2);
                        linii[counter][strlen(linii[counter])]='\0';
                        counter++;
                    }

                    line=strtok(NULL,"\n");
                }
                close(fd);
                for(int i=0;i<counter;i++)
                {
                    fprintf(stderr,"%s\n",linii[i]);
                }
                fprintf(stderr,"\n");
                
                int modifyfd;
                if(type=='f')
                {
                    remove("Atribute_fisiere.txt");
                    modifyfd=open("Atribute_fisiere.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                }
                else
                {
                    remove("Atribute_directoare.txt");
                    modifyfd=open("Atribute_directoare.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                }
                for(int i=0;i<counter;i++)
                {
                    linii[i][strlen(linii[i])]='\0';
                    strcat(linii[i],"\n");
                    linii[i][strlen(linii[i])]='\0';
                    write(modifyfd,linii[i],strlen(linii[i]));
                    
                }
                close(modifyfd);
}



char*create_buffer_information(char*path,mode_t mode,int inode,int size,int nlinks,int gid,int uid,time_t atime,time_t mtime,char type)
{
            char*nou=(char*)malloc(100);
            strcpy(nou,"");
            strcat(nou,path);
            strcat(nou,": ");

            char modeString[10]; 
            sprintf(modeString, "%o",mode);
            strcat(nou,modeString);
            strcat(nou," ");


            char inodeString[10]; 
            sprintf(inodeString, "%o",inode);
            strcat(nou,inodeString);
            strcat(nou," ");

            if(type=='f'){
            char sizeString[20];  
            sprintf(sizeString,"%d",size);
            strcat(nou,sizeString);
            strcat(nou," ");
            }

            char nlinksString[20];
            sprintf(nlinksString,"%d",nlinks);
            strcat(nou,nlinksString);
            strcat(nou," ");

            char gidString[20];
            sprintf(gidString,"%d",gid);
            strcat(nou,gidString);
            strcat(nou," ");

            char uidString[20];
            sprintf(uidString,"%d",uid);
            strcat(nou,uidString);
            strcat(nou," ");

            char*timestamp1[30];
            sprintf(timestamp1,"%ld",atime);
            strcat(nou,timestamp1);
            strcat(nou," ");

            char*timestamp2[30];
            sprintf(timestamp2,"%ld",mtime);
            strcat(nou,timestamp2);
            return nou;
}


static int do_getattr(const char *path, struct stat *st)
{
    if (strlen(path) != 1 && path[0] == '/')
        path++;

    
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);
    st->st_size = 0;

    if (strcmp(path, "/") == 0 || getdir(current_directory, path) != NULL)
    {
        struct directory *dir = getdir(current_directory, path);
        if (dir) {
            st->st_mode = dir->permissions;
            st->st_nlink = dir->nlinks;
            st->st_ino=dir->inode;
            st->st_uid=dir->uid;
            st->st_gid=dir->gid;

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
                    st->st_nlink = dir->list_files[i]->nlinks;
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
            filler(buffer,getname( searched_directory->list_files[i]->path), NULL, 0);
        }
    }

    return 0;
}


void create_file(struct directory*director,char*path,mode_t mode)
{
    director->list_files[director->number_of_files_current_folder]=(struct file*)calloc(1,sizeof(struct file));
    director->list_files[director->number_of_files_current_folder]->path=strdup(path);
    director->list_files[director->number_of_files_current_folder]->accest=time(NULL);
    director->list_files[director->number_of_files_current_folder]->mtime=time(NULL);
    director->list_files[director->number_of_files_current_folder]->size=0;
    director->list_files[director->number_of_files_current_folder]->gid=getgid();
    director->list_files[director->number_of_files_current_folder]->uid=getuid();
    director->list_files[director->number_of_files_current_folder]->permissions=__S_IFREG|mode;
    
    director->list_files[director->number_of_files_current_folder]->inode=inode;
    inode++;
    director->list_files[director->number_of_files_current_folder]->nlinks=1;
    director->list_files[director->number_of_files_current_folder]->content=(char*)malloc(100);

     char*searched=strdup(path);
     strcat(searched,": ");
     int fd=open("Atribute_fisiere.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
     char*buffer=(char*)malloc(40960);
     read(fd,buffer,40960);
     close(fd);
     if(strstr(buffer,searched)==NULL)
    {
    char*information=create_buffer_information(director->list_files[director->number_of_files_current_folder]->path,director->list_files[director->number_of_files_current_folder]->permissions,director->list_files[director->number_of_files_current_folder]->inode,director->list_files[director->number_of_files_current_folder]->size,director->list_files[director->number_of_files_current_folder]->nlinks,director->list_files[director->number_of_files_current_folder]->gid,director->list_files[director->number_of_files_current_folder]->uid,director->list_files[director->number_of_files_current_folder]->accest,director->list_files[director->number_of_files_current_folder]->mtime,'f');
    strcat(information,"\n");
    add_information_atribute_structuri(information,'f');
    }

   
    director->number_of_files_current_folder++;

    remove("Inode.txt");
    int fd2=open("Inode.txt",O_RDWR|O_CREAT,S_IRUSR | S_IWUSR);
    char*number=(char*)malloc(3);
    sprintf(number,"%d",inode);
    number[strlen(number)]='\0';
    write(fd2,number,strlen(number));
    close(fd2);

    
}

static int do_mknod( const char *path, mode_t mode, dev_t rdev)
{   
	path++;
	if(is_file(current_directory,path)==1)
	return -EEXIST;
	if(!strchr(path,'/')){
    struct directory*director=getdir(current_directory,"/");
    if(director){
	    create_file(director,path,mode);
    }

    return 0;
	}
	
	else{
     struct directory*director=getdir(current_directory,directory_name(path));
     if(director)
     {
        create_file(director,path,mode);
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


void create_directory(struct directory*director,char*path,mode_t mode)
{
    director->list_directories[director->number_of_directories_current_folder]=(struct directory*)calloc(1,sizeof(struct directory));
    director->list_directories[director->number_of_directories_current_folder]->path=strdup(path);
    director->list_directories[director->number_of_directories_current_folder]->accest=time(NULL);
    director->list_directories[director->number_of_directories_current_folder]->mtime=time(NULL);
    director->list_directories[director->number_of_directories_current_folder]->gid=getgid();
    director->list_directories[director->number_of_directories_current_folder]->uid=getuid();
   
    director->list_directories[director->number_of_directories_current_folder]->inode=inode;
    inode++;
    director->list_directories[director->number_of_directories_current_folder]->nlinks=2;
    director->list_directories[director->number_of_directories_current_folder]->permissions=__S_IFDIR | mode;



      char*searched=strdup(path);
      strcat(searched,": ");
      int fd=open("Atribute_directoare.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
      char*buffer=(char*)malloc(4096);
      read(fd,buffer,4096);
      close(fd);
      if(strstr(buffer,searched)==NULL)
     {
     char*information=create_buffer_information(director->list_directories[director->number_of_directories_current_folder]->path,director->list_directories[director->number_of_directories_current_folder]->permissions,director->list_directories[director->number_of_directories_current_folder]->inode,0,director->list_directories[director->number_of_directories_current_folder]->nlinks,director->list_directories[director->number_of_directories_current_folder]->gid,director->list_directories[director->number_of_directories_current_folder]->uid,director->list_directories[director->number_of_directories_current_folder]->accest,director->list_directories[director->number_of_directories_current_folder]->mtime,'d');
     strcat(information,"\n");
     add_information_atribute_structuri(information,'d');
     }
   
     director->number_of_directories_current_folder++;


     remove("Inode.txt");
     int fd2=open("Inode.txt",O_RDWR|O_CREAT,S_IRUSR | S_IWUSR);
     char*number=(char*)malloc(3);
     sprintf(number,"%d",inode);
     number[strlen(number)]='\0';
     write(fd2,number,strlen(number));
     close(fd2);

}

 static int do_mkdir(const char *path, mode_t mode)
 {
    path++;
    if (!strchr(path, '/')) {
    struct directory*director=getdir(current_directory,"/");
    if(director){
    create_directory(director,path,mode);
    }     
    return 0;
    } 
    else 
    {
    struct directory*director=getdir(current_directory,directory_name(path));
    if(director){
    create_directory(director,path,mode);
    }   
        return 0;
    }
 }


void create_instance(char*buf,char type)
{
    
    char path[50];
    int counter=0;
    for(int i=0;i<strlen(buf);i++)
    {
        if(buf[i]!=':')
        {
         path[counter]=buf[i];
         counter++;
        }
        else
        break;
    }
    path[counter]='\0';
    struct directory*parent_directory=getdir(current_directory,directory_name(path));
    int index=0; 
    counter+=2;
 
    mode_t permissions;
    char*attribute1=(char*)malloc(30);
    index=0;
    for(int i=counter;i<strlen(buf);i++)
    {   if(buf[i]!=' ')
        {attribute1[index]=buf[i];
        index++;
        counter++;
        }
        else
        break;
    }
    attribute1[index]='\0';
    permissions=(mode_t)strtoul(attribute1, NULL, 8);
    index=0;

    counter++;
    int inode;
    char*attribute2=(char*)malloc(30);
    index=0;
    for(int i=counter;i<strlen(buf);i++)
    {   if(buf[i]!=' ')
        {attribute2[index]=buf[i];
        index++;
        counter++;
        }
        else
        break;
    }
    attribute2[index]='\0';
    inode=atoi(attribute2);
    index=0;

    
    int size;
    if(type=='f')
    {
        counter++;
        char*attribute3=(char*)malloc(30);
        index=0;
        for(int i=counter;i<strlen(buf);i++)
        {   
            if(buf[i]!=' ')
            {attribute3[index]=buf[i];
            index++;
            counter++;
            }
            else
            break;
        }
        attribute3[index]='\0';
        size=atoi(attribute3);
        index=0;
    }

        counter++;
        int nlinks;
        char*attribute4=(char*)malloc(30);
        index=0;
        for(int i=counter;i<strlen(buf);i++)
        {   
            if(buf[i]!=' ')
            {attribute4[index]=buf[i];
            index++;
            counter++;
            }
            else
            break;
        }
        attribute4[index]='\0';
        nlinks=atoi(attribute4);
        index=0;

        counter++;
        int gid;
        char*attribute5=(char*)malloc(30);
        index=0;
        for(int i=counter;i<strlen(buf);i++)
        {   
            if(buf[i]!=' ')
            {attribute5[index]=buf[i];
            index++;
            counter++;
            }
            else
            break;
        }
        attribute5[index]='\0';
        gid=atoi(attribute5);
        index=0;


        counter++;
        int uid;
        char*attribute6=(char*)malloc(30);
        index=0;
        for(int i=counter;i<strlen(buf);i++)
        {   
            if(buf[i]!=' ')
            {attribute6[index]=buf[i];
            index++;
            counter++;
            }
            else
            break;
        }
        attribute6[index]='\0';
        uid=atoi(attribute6);
        index=0;


        counter++;
        int atime;
        char*attribute7=(char*)malloc(30);
        index=0;
        for(int i=counter;i<strlen(buf);i++)
        {   
            if(buf[i]!=' ')
            {attribute7[index]=buf[i];
            index++;
            counter++;
            }
            else
            break;
        }
        attribute7[index]='\0';
        atime=atoi(attribute7);
        index=0;

        counter++;
        int mtime;
        char*attribute8=(char*)malloc(30);
        index=0;
        for(int i=counter;i<strlen(buf);i++)
        {   
            if(buf[i]!=' ')
            {attribute8[index]=buf[i];
            index++;
            counter++;
            }
            else
            break;
        }
        attribute8[index]='\0';
        mtime=atoi(attribute8);
        index=0;

       if(type=='d')
       {
        struct directory * dir=(struct directory*)malloc(sizeof(struct directory));
        dir->path=strdup(path);
        dir->permissions=permissions;
        dir->inode=inode;
        dir->nlinks=nlinks;
        dir->gid=gid;
        dir->uid=uid;
        dir->accest=atime;
        dir->mtime=mtime;
        parent_directory->list_directories[parent_directory->number_of_directories_current_folder]=dir;
        parent_directory->number_of_directories_current_folder++;
       }
       else
       {
        struct file * rfile=(struct file*)malloc(sizeof(struct file));
        rfile->path=strdup(path);
        rfile->permissions=permissions;
        rfile->inode=inode;
        rfile->size=size;
        rfile->nlinks=nlinks;
        rfile->gid=gid;
        rfile->uid=uid;
        rfile->accest=atime;
        rfile->mtime=mtime;
       
        char*fpath=strdup(persistency_directory);
        char inodeString[10];
        sprintf(inodeString,"%d",inode);
        inodeString[strlen(inodeString)]='\0';
        strcat(fpath,inodeString);
        strcat(fpath,".txt");
        if(access(fpath,F_OK)==0)
        {
          int gfd=open(fpath,O_RDONLY);
          char*con=(char*)malloc(4096);
          int readbytes=read(gfd,con,4096);
          rfile->content=(char*)malloc(readbytes+1);
          strcpy(rfile->content,con);
          close(gfd);
        }
        else 
        {
            rfile->content=(char*)malloc(100);
        }
        parent_directory->list_files[parent_directory->number_of_files_current_folder]=rfile;
        parent_directory->number_of_files_current_folder++;
       }







}

void reconstruct()
{
   int fd=open("Atribute_directoare.txt",O_RDWR);
   char*buffer1=(char*)malloc(4096);
   read(fd,buffer1,4096);
   char*pointer1=strtok(buffer1,"\n");
   while(pointer1)
   {
    if(pointer1[0]!='/'){
    create_instance(pointer1,'d');
    }
    pointer1=strtok(NULL,"\n");
   }
   close(fd);

   int fdf=open("Atribute_fisiere.txt", O_RDWR);
   char*buffer2=(char*)malloc(4096);
   read(fdf,buffer2,4096);
   char*pointer2=strtok(buffer2,"\n");
   while(pointer2)
   {
    if(pointer2[0]!='/'){
    create_instance(pointer2,'f');
    }
    pointer2=strtok(NULL,"\n");
   }
   close(fdf);
   
     
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
    current_directory->nlinks=2;
    current_directory->inode=0;
    
    
     char*searched=strdup("/");
     strcat(searched,": ");
     int fd1=open("Atribute_directoare.txt",O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
     char*buffer1=(char*)malloc(4096);
     read(fd1,buffer1,4096);
     close(fd1);
     if(strstr(buffer1,searched)==NULL)
    {
    char*information=create_buffer_information("/",current_directory->permissions,current_directory->inode,0,current_directory->nlinks,current_directory->gid,current_directory->uid,current_directory->accest,current_directory->mtime,'d');
    strcat(information,"\n");
    add_information_atribute_structuri(information,'d');
    }


    int fd=open("Inode.txt",O_RDWR|O_CREAT,S_IRUSR | S_IWUSR);
    char*buffer=(char*)malloc(10);
    read(fd,buffer,5);
    inode=atoi(buffer);
    close(fd);
    if(inode==0)
    inode++;
    

    reconstruct();
    return NULL;
}

static int do_unlink(const char *path) {
    int to_delete=-1;
    path++;

    if (!is_file(current_directory, path))
        return -ENOENT;

    struct directory *dir = getdir(current_directory, directory_name(path));
    if (dir) {
        for (int i = 0; i < dir->number_of_files_current_folder; i++) {
            if (dir->list_files[i] && dir->list_files[i]->path && strcmp(dir->list_files[i]->path, path) == 0) {
                to_delete=dir->list_files[i]->inode;
                free(dir->list_files[i]->path);
                free(dir->list_files[i]);
                for (int j = i; j < dir->number_of_files_current_folder - 1; j++) {
                    dir->list_files[j] = dir->list_files[j + 1];
                }
                dir->number_of_files_current_folder--;
                char information[100][100];
                int line=0;
                int bytesRead=0;
                char buffer[4096];

                int fd=open("Atribute_fisiere.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                bytesRead = read(fd, buffer, sizeof(buffer));
                buffer[bytesRead] = '\0';
                char*searched=strdup(path);
                strcat(searched,": ");
                char*p=strtok(buffer,"\n");
                while(p){
                   
                   
                    if(!strstr(p,searched))
                    {
                        strcpy(information[line],p);
                        line++;
                    }
                    
                    p=strtok(NULL,"\n");
                }
                close(fd);
                remove("Atribute_fisiere.txt");
                int new_open=open("Atribute_fisiere.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                
                for(int i=0;i<line;i++)
                {
                    information[i][strlen(information[i])]='\0';
                    strcat(information[i],"\n");
                    information[i][strlen(information[i])]='\0';
                    write(new_open,information[i],strlen(information[i]));
                    
                }
                close(new_open);

                char*filepathrm=strdup(persistency_directory);
                char inodeString[5];
                sprintf(inodeString,"%d",to_delete);
                strcat(filepathrm,inodeString);
                strcat(filepathrm,".txt");
                if(access(filepathrm,F_OK)==0)
                remove(filepathrm);
                return 0;
            }
        }
        
    }

    
}




static int do_rmdir(const char *path) {
    path++;

    if (!getdir(current_directory, path))
        return -ENOENT;

        char information[264][1000];
        int index=0;
        int openfile=open("Atribute_directoare.txt",O_RDONLY);
        char*buffer=(char*)malloc(4096);
        read(openfile,buffer,4096);
        close(openfile);
        char*line=strtok(buffer,"\n");
        char*searched=strdup(path);
        strcat(searched,": ");
        while(line){
            if(!strstr(line,searched))
            {
                line[strlen(line)]='\0';
                strcpy(information[index],line);
                strcat(information[index],"\n");
                index++;
            }
            line=strtok(NULL,"\n");
        }
        struct directory*dir1=getdir(current_directory,path);
        for(int i=0;i<dir1->number_of_files_current_folder;i++)
        {
            char*aux=(char*)malloc(100);
            strcpy(aux,"/");
            strcat(aux,dir1->list_files[i]->path);
            do_unlink(aux);
        }

        int found=-1;
        struct directory*dir2=getdir(current_directory,directory_name(path));
        for(int i=0;i<dir2->number_of_directories_current_folder;i++)
        {
            if(strcmp(dir2->list_directories[i]->path,path)==0)
            {
             found=i;
            }
        }
        for(int i=found;i<dir2->number_of_directories_current_folder-1;i++)
        {
            dir2->list_directories[i]=dir2->list_directories[i+1];
        }
        dir2->number_of_directories_current_folder--;

    remove("Atribute_directoare.txt");
    int ufd=open("Atribute_directoare.txt",O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
    for(int i=0;i<index;i++)
    {
        
        write(ufd,information[i],strlen(information[i]));
    }
    close(ufd);        

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
                update_information(old_path,new_path,dir->list_files[index]->permissions,dir->list_files[index]->inode,dir->list_files[index]->size,dir->list_files[index]->nlinks,dir->list_files[index]->gid,dir->list_files[index]->uid,dir->list_files[index]->accest,dir->list_files[index]->mtime,'f');
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
        update_information(old_path,new_path,dir->permissions,dir->inode,0,dir->nlinks,dir->gid,dir->uid,dir->accest,dir->mtime,'d');
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
                update_information(path,path,dir->list_files[i]->permissions,dir->list_files[i]->inode,dir->list_files[i]->size,dir->list_files[i]->nlinks,dir->list_files[i]->gid,dir->list_files[i]->uid,dir->list_files[i]->accest,dir->list_files[i]->mtime,'f');
                return 0;
            }
        }
    }
    if(getdir(current_directory,path))
    {
        struct directory*dir=getdir(current_directory,path);
        dir->permissions=mode;
        update_information(path,path,dir->permissions,dir->inode,0,dir->nlinks,dir->gid,dir->uid,dir->accest,dir->mtime,'d');
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
            struct file *file = dir->list_files[i];

            if (offset + size > file->size) {
                char *new_content = (char *)realloc(file->content, offset + size + 1);
                if (new_content == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed in do_write for file %s\n", path);
                    return -ENOMEM;
                }
                file->content = new_content;

                file->size = offset + size;
            }

            strncpy(file->content + offset, buf, size);
            file->content[file->size] = '\0'; 

            file->accest = time(NULL);
            file->mtime = time(NULL);

            update_information(path,path,file->permissions,file->inode,file->size,file->nlinks,file->gid,file->uid,file->accest,file->mtime,'f');
            char inodeString[5];
            sprintf(inodeString,"%d",file->inode);
            char*filename=strdup(persistency_directory);
            strcat(filename,inodeString);
            strcat(filename,".txt");
            if(access(filename, F_OK) == 0)
            {
                remove(filename);
                int fd=open(filename,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
                write(fd,file->content,strlen(file->content));
            }
            else
            {
                int fd=open(filename,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
                write(fd,file->content,strlen(file->content));
            }
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
                update_information(path,path,dir->list_files[i]->permissions,dir->list_files[i]->inode,dir->list_files[i]->size,dir->list_files[i]->nlinks,dir->list_files[i]->gid,dir->list_files[i]->uid,dir->list_files[i]->accest,dir->list_files[i]->mtime,'f');
                return 0;
            }
        }
    } else if (getdir(current_directory, path)) {
        struct directory *dir = getdir(current_directory, path);
        
        dir->uid = uid;
        dir->gid = gid;
        update_information(path,path,dir->permissions,dir->inode,0,dir->nlinks,dir->gid,dir->uid,dir->accest,dir->mtime,'d');
        return 0;
    }

    return -ENOENT; 
}

static int do_readlink(const char *path, char *buf, size_t size) {
    path++;

    if (!is_file(current_directory, path))
        return -ENOENT;

    struct directory *dir = getdir(current_directory, directory_name(path));
    for (int i = 0; i < dir->number_of_files_current_folder; i++) {
        if (dir->list_files[i] && dir->list_files[i]->path && strcmp(dir->list_files[i]->path, path) == 0) {
            if (dir->list_files[i]->content != NULL) {
                strncpy(buf, dir->list_files[i]->content, size);
                return 0;
            }
        }
    }

    return -ENOENT;
}

static int do_symlink(const char *target, const char *link_path) {
    link_path++;

    if (!strchr(link_path, '/')) {
        struct directory *dir = getdir(current_directory, "/");
        if (dir) {
            create_file(dir, link_path,__S_IFLNK | 0644); // Set the mode as needed
            dir->list_files[dir->number_of_files_current_folder - 1]->content = strdup(target);
            return 0;
        }
    } else {
        struct directory *dir = getdir(current_directory, directory_name(link_path));
        if (dir) {
            create_file(dir, link_path, __S_IFLNK | 0644); // Set the mode as needed
            dir->list_files[dir->number_of_files_current_folder - 1]->content = strdup(target);
            return 0;
        }
    }

    return -ENOENT;
}

static int do_link(const char *old_path, const char *new_path) {
    old_path++;
    new_path++;

    if (!is_file(current_directory, old_path))
        return -ENOENT;

    struct directory *dir = getdir(current_directory, directory_name(new_path));
    if (dir) {
        for (int i = 0; i < dir->number_of_files_current_folder; i++) {
            if (strcmp(dir->list_files[i]->path, new_path) == 0) {
                return -EEXIST; // Link destination already exists
            }
        }

        struct directory *old_dir = getdir(current_directory, directory_name(old_path));
        if (old_dir) {
            for (int i = 0; i < old_dir->number_of_files_current_folder; i++) {
                if (strcmp(old_dir->list_files[i]->path, old_path) == 0) {
                    // Create a new hard link
                    struct file *new_link = (struct file *)calloc(1, sizeof(struct file));
                    new_link->path = strdup(new_path);
                    new_link->accest = old_dir->list_files[i]->accest;
                    new_link->mtime = old_dir->list_files[i]->mtime;
                    new_link->size = old_dir->list_files[i]->size;
                    new_link->gid = old_dir->list_files[i]->gid;
                    new_link->uid = old_dir->list_files[i]->uid;
                    new_link->permissions = old_dir->list_files[i]->permissions;
                   
                    new_link->inode = inode;
                    inode++;
                    new_link->content = old_dir->list_files[i]->content;
                    old_dir->list_files[i]->nlinks++;
                    // Add the new link to the directory
                    dir->list_files[dir->number_of_files_current_folder++] = new_link;


                    char*searched=strdup(new_path);
                    strcat(searched,": ");
                    int fd=open("Atribute_fisiere.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    char*buffer=(char*)malloc(40960);
                    read(fd,buffer,40960);
                    close(fd);
                    if(strstr(buffer,searched)==NULL)
                    {
                    update_information(old_path,old_path, old_dir->list_files[i]->permissions,old_dir->list_files[i]->inode,old_dir->list_files[i]->size,old_dir->list_files[i]->nlinks,old_dir->list_files[i]->gid,old_dir->list_files[i]->uid,old_dir->list_files[i]->accest,old_dir->list_files[i]->mtime,'f');
                    char*information=create_buffer_information(new_path,new_link->permissions,new_link->inode,new_link->size,new_link->nlinks,new_link->gid,new_link->uid,new_link->accest,new_link->mtime,'f');
                    add_information_atribute_structuri(information,'f');
                    }


                    remove("Inode.txt");
                    int fdinode=open("Inode.txt",O_RDWR|O_CREAT,S_IRUSR | S_IWUSR);
                    char*number=(char*)malloc(3);
                    number[strlen(number)]='\0';
                    sprintf(number,"%d",strlen(number));
                    write(fdinode,number,5);
                    close(fdinode);
                    return 0;
                }
            }
        }
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
    .readlink   = do_readlink,
    .symlink    = do_symlink,
    .link       = do_link,

};


int main( int argc, char *argv[] )
{
	umask(0022);
	return fuse_main( argc, argv, &operations, NULL );
}
