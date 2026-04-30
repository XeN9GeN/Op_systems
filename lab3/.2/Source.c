#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h> 
#include <limits.h>

#define BUFFER_SIZE 1024
#define SUCCESS_STRCMP 0
#define SUCCESS_STAT 0 

#define SUCCESS_RETURN_CREATE_DIR 0
#define SUCCESS_RETURN_REMOVE_DIR 0

#define FAILURE_RETURN_FILE_DESCR -1
#define FAILURE_RETURN_CLOSE_FILE -1
#define SUCCESS_RETURN_REMOVE_FILE 0

#define SUCCESS_RETURN_LINK 0
#define FAILURE_RETURN_READLINK -1
#define ERROR_VAL -1

#define SUCCESS_RET 0
#define FAILURE_RET -1

int create_directory(const char* file_path) {
    if (mkdir(file_path, 0755) == SUCCESS_RETURN_CREATE_DIR) {
        printf("Dir was created: %s\n", file_path);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: while creating directory");
        return FAILURE_RET;
    }
}

int list_directory(const char* file_pth) {
    DIR* dir = opendir(file_pth);
    if (dir == NULL) {
        perror("ERROR: Directory wasn't open");
        return FAILURE_RET;
    }

    struct dirent* entry;
    printf("Directory contents '%s' :\n", file_pth);
    while ((entry = readdir(dir)) != NULL) {
        printf("  Inode: %-10llu | Name: %s\n", (unsigned long long)entry->d_ino, entry->d_name);
    }
    closedir(dir);
    return SUCCESS_RET;
}

int delete_file(const char* file_pth) {
    if (unlink(file_pth) == SUCCESS_RETURN_REMOVE_FILE) {
        printf("File was removed: %s\n", file_pth);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: File wasn't deleted");
        return FAILURE_RET;
    }
}

int remove_directory(const char* file_pth) {
    DIR* dir = opendir(file_pth);
    if (dir == NULL) {
        perror("ERROR: Cannot open directory for recursive removal");
        return FAILURE_RET;
    }

    struct dirent* entry;
    char path[PATH_MAX];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", file_pth, entry->d_name);

        struct stat st;
        if (stat(path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                remove_directory(path);
            }
            else {
                delete_file(path);
            }
        }
    }
    closedir(dir);


    if (rmdir(file_pth) == SUCCESS_RETURN_REMOVE_DIR) {
        printf("Directory was removed: %s\n", file_pth);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: Directory wasn't removed");
        return FAILURE_RET;
    }
}

int create_file(const char* file_pth) {
    int file_descriptor = open(file_pth, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (file_descriptor == FAILURE_RETURN_FILE_DESCR) {
        perror("ERROR: File wasn't created");
        return FAILURE_RET;
    }

    printf("File was successfully created: %s\n", file_pth);
    if (close(file_descriptor) == FAILURE_RETURN_CLOSE_FILE) {
        perror("ERROR: File descriptor didn't close properly");
        return FAILURE_RET;
    }
    return SUCCESS_RET;
}

int read_file(const char* file_pth) {
    int file_descriptor = open(file_pth, O_RDONLY);
    if (file_descriptor == FAILURE_RETURN_FILE_DESCR) {
        perror("ERROR: File wasn't opened");
        return FAILURE_RET;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    printf("FILE contents '%s' :\n", file_pth);

    while ((bytes_read = read(file_descriptor, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    printf("\n");

    if (close(file_descriptor) == FAILURE_RETURN_CLOSE_FILE) {
        perror("ERROR: File descriptor didn't close properly");
        return FAILURE_RET;
    }
    return SUCCESS_RET;
}

int create_symlink(const char* target, const char* link_pth) {
    if (symlink(target, link_pth) == SUCCESS_RETURN_LINK) {
        printf("Symb link was created: %s -> %s\n", link_pth, target);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: Symlink wasn't created");
        return FAILURE_RET;
    }
}

int read_symlink(const char* link_pth) {
    char buffer[PATH_MAX];
    ssize_t len = readlink(link_pth, buffer, sizeof(buffer) - 1);

    if (len != ERROR_VAL) {
        buffer[len] = '\0';
        printf("Link '%s' points on: %s\n", link_pth, buffer);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: Can't read symbol link");
        return FAILURE_RET;
    }
}

int read_file_via_symlink(const char* link_pth) {
    return read_file(link_pth);
}

int delete_hard_symlink(const char* link_pth) {
    if (unlink(link_pth) == SUCCESS_RETURN_LINK) {
        printf("Symbol link removed %s\n", link_pth);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: Link wasn't removed");
        return FAILURE_RET;
    }
}

int create_hardlink(const char* target, const char* link_pth) {
    if (link(target, link_pth) == SUCCESS_RETURN_LINK) {
        printf("Hard link was created: %s -> %s\n", link_pth, target);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: Hardlink wasn't created");
        return FAILURE_RET;
    }
}

int read_chmod_file(const char* file_pth) {
    struct stat st;
    if (stat(file_pth, &st) == SUCCESS_STAT) {
        printf("File: %s\n", file_pth);
        printf("Acces right(mode): %o\n", st.st_mode & 0777);
        printf("Amout of hard links: %lu\n", (unsigned long)st.st_nlink);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: Cannot get file status");
        return FAILURE_RET;
    }
}

int change_permissions(const char* file_pth, const char* mode_str) {
    mode_t new_mode = strtol(mode_str, NULL, 8);

    if (chmod(file_pth, new_mode) == 0) {
        printf("Access rights for '%s' changed on %o\n", file_pth, new_mode);
        return SUCCESS_RET;
    }
    else {
        perror("ERROR: Cannot change permissions");
        return FAILURE_RET;
    }
}



int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Использование: %s <путь>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* file_name = argv[1];
    char* cmd = strrchr(argv[0], '/');
    if (cmd!=NULL) {
        cmd++;
    }
    else {
        cmd = argv[0];
    }



    if (strcmp(cmd, "mymkdir") == SUCCESS_STRCMP) {
        create_directory(file_name);
    }
    else if (strcmp(cmd, "mylsdir") == SUCCESS_STRCMP) {
        list_directory(file_name);
    }
    else if (strcmp(cmd, "myrmdir") == SUCCESS_STRCMP) {
        remove_directory(file_name);
    }

    else if (strcmp(cmd, "mymkfile") == SUCCESS_STRCMP) {
        create_file(file_name);
    }
    else if (strcmp(cmd, "mylsfile") == SUCCESS_STRCMP) {
        read_file(file_name);
    }
    else if (strcmp(cmd, "myrmfile") == SUCCESS_STRCMP) {
        delete_file(file_name);
    }

    else if (strcmp(cmd, "mymksymlink") == SUCCESS_STRCMP) {
        if (argc < 3) {
            fprintf(stderr, "ERROR: wrong amount of args\n");
            exit(EXIT_FAILURE);
        }
        create_symlink(file_name, argv[2]);
    }
    else if (strcmp(cmd, "mylssymlink") == SUCCESS_STRCMP) {
        read_symlink(file_name);
    }
    else if (strcmp(cmd, "mylssymlinkfile") == SUCCESS_STRCMP) {
        read_file_via_symlink(file_name);
    }

    else if (strcmp(cmd, "mymkhardlink") == SUCCESS_STRCMP) {
        if (argc < 3) {
            fprintf(stderr, "ERROR: wrong amount of args\n");
            return FAILURE_RET;
        }
        create_hardlink(file_name, argv[2]);
    }
    else if (strcmp(cmd, "myrmhard_softlink") == SUCCESS_STRCMP) {
        delete_hard_symlink(file_name);
    }
                                                                                    
    else if (strcmp(cmd, "lschmod") == SUCCESS_STRCMP) {
        read_chmod_file(file_name);
    }
    else if (strcmp(cmd, "chmod") == SUCCESS_STRCMP) {
        if (argc < 3) {
            fprintf(stderr, "ERROR: wrong amount of args\n");
            return FAILURE_RET;
        }
        change_permissions(file_name, argv[2]);
    }

    exit(EXIT_SUCCESS);

}
