#define FUSE_USE_VERSION 30
#define _CRT_SECURE_NO_WARNINGS
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

char dir_list[256][256];
int curr_dir_idx = -1;

char files_list[256][256];
int curr_file_idx = -1;

char files_content[256][256];
int curr_file_content_idx = -1;

void add_dir(const char *dir_name) {
    curr_dir_idx++;
    strcpy(dir_list[curr_dir_idx], dir_name);
}

int is_dir(const char *path) {
    path++; // Eliminating "/" in the path

    for (int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++)
        if (strcmp(path, dir_list[curr_idx]) == 0)
            return 1;

    return 0;
}

static int do_mkdir(const char *path, mode_t mode) {
    path++;
    add_dir(path);

    return 0;
}

void add_file(const char *filename) {
    curr_file_idx++;
    strcpy(files_list[curr_file_idx], filename);

    curr_file_content_idx++;
    strcpy(files_content[curr_file_content_idx], "");
}

int is_file(const char *path) {
    path++; // Eliminating "/" in the path

    for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++)
        if (strcmp(path, files_list[curr_idx]) == 0)
            return 1;

    return 0;
}

static int do_mknod(const char *path, mode_t mode, dev_t rdev) {
    path++;
    add_file(path);

    return 0;
}

int get_file_index(const char *path) {
    path++; // Eliminating "/" in the path

    for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++)
        if (strcmp(path, files_list[curr_idx]) == 0)
            return curr_idx;

    return -1;
}

void write_to_file(const char *path, const char *new_content) {
    int file_idx = get_file_index(path);

    if (file_idx == -1) // No such file
        return;

    strcpy(files_content[file_idx], new_content);
}

static int do_getattr(const char *path, struct stat *st) {
    st->st_uid = geteuid();
    st->st_gid = getegid();
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);

    if (strcmp(path, "/") == 0 || is_dir(path) == 1) {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
    } else if (is_file(path) == 1) {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = strlen(files_content[get_file_index(path)]);
    } else {
        return -ENOENT;
    }

    return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
    int file_idx = get_file_index(path);

    if (file_idx == -1)
        return -1;

    char *content = files_content[file_idx];

    size_t content_length = strlen(content);

    if (offset >= content_length) {
        return 0; // EOF
    }

    size_t bytes_to_read = (offset + size > content_length) ? content_length - offset : size;

    memcpy(buffer, content + offset, bytes_to_read);

    return bytes_to_read;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    filler(buffer, ".", NULL, 0); // Current Directory
    filler(buffer, "..", NULL, 0); // Parent Directory

    if (strcmp(path, "/") == 0) {
        for (int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++)
            filler(buffer, dir_list[curr_idx], NULL, 0);

        for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++)
            filler(buffer, files_list[curr_idx], NULL, 0);
    }

    return 0;
}

static int do_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info) {
    write_to_file(path, buffer);

    return size;
}

static int do_create(const char *path, mode_t mode, struct fuse_file_info *info) {
    path++;

    // Check if the file already exists
    if (is_file(path)) {
        // If the file already exists, just update its timestamp
        struct stat st;
        if (do_getattr(path, &st) == 0) {
            struct timespec times[2];
            times[0].tv_nsec = UTIME_OMIT;
            times[1].tv_nsec = UTIME_OMIT;

            if (utimensat(AT_FDCWD, path, times, 0) == -1) {
                return -errno;
            }

            return 0;  // Success
        } else {
            return -ENOENT;  // No such file or directory
        }
    }

    // If the file doesn't exist, create it
    add_file(path);

    // Set the mode for the new file
    mode_t file_mode = S_IFREG | mode;

    // Update the file's metadata
    int file_idx = get_file_index(path);
    struct stat st;
    st.st_uid = geteuid();
    st.st_gid = getegid();
    st.st_mode = file_mode;
    st.st_atime = time(NULL);
    st.st_mtime = time(NULL);

    return 0;  // Success
}




static int do_utimens(const char *path, const struct timespec tv[2]) {
    struct stat st;
    int res;

    res = lstat(path, &st);
    if (res == -1)
        return -errno;

    // Convert struct timespec to time_t
    time_t atime = tv[0].tv_sec;
    time_t mtime = tv[1].tv_sec;

    // Update the access and modification times based on atime and mtime.
    // If atime or mtime is -1, do not update the respective time.

    if (atime != -1 || mtime != -1) {
        if (atime != -1)
            st.st_atime = atime;
        if (mtime != -1)
            st.st_mtime = mtime;
    }

    res = utimensat(0, path, &st, 0);
    if (res == -1)
        return -errno;

    return 0;
}

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .readdir = do_readdir,
    .read = do_read,
    .mkdir = do_mkdir,
    .mknod = do_mknod,
    .write = do_write,
    .create = do_create,
    .utimens = do_utimens
};


int main(int argc, char *argv[]) {
    char current_path[256]; // Define a character array to store the current path
    getcwd(current_path, sizeof(current_path));
    return fuse_main(argc, argv, &operations, current_path);
}