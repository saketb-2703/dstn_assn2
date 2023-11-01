#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/xattr.h>
#include <sys/acl.h>
#include <unistd.h>
#include <fcntl.h>

#define PATH_LEN 1000

time_t cur_time;
char fileType;

// whiteout, links, total
void printFileType(char* path, mode_t mode){ 
    char* sym_file = (char*)malloc(PATH_LEN * sizeof(char));
    ssize_t len = readlink(path, sym_file, sizeof(sym_file) - 1);
    if (len != -1)
        fileType = 'l';
    else
        fileType = S_ISDIR(mode) ? 'd' : S_ISBLK(mode) ? 'b' : S_ISCHR(mode) ? 'c' : S_ISLNK(mode) ? 'l' : S_ISSOCK(mode) ? 's' : S_ISFIFO(mode) ? 'p' : '-';
    printf("%c", fileType);
}   
void printFilePermissions(mode_t mode){
    mode & S_IRUSR ? printf("r") : printf("-");
    mode & S_IWUSR ? printf("w") : printf("-");
    mode & S_IXUSR ? printf("x") : printf("-");
    mode & S_IRGRP ? printf("r") : printf("-");
    mode & S_IWGRP ? printf("w") : printf("-");
    mode & S_IXGRP ? printf("x") : printf("-");
    mode & S_IROTH ? printf("r") : printf("-");
    mode & S_IWOTH ? printf("w") : printf("-");
    mode & S_IXOTH ? printf("x") : printf("-");
}
void printFileSpecial(char* path){
    ssize_t attr_len = listxattr(path, NULL, 0, XATTR_NOFOLLOW);
    if (attr_len > 0){
        printf("@");
    }
    acl_t acl = acl_get_link_np (path, ACL_TYPE_EXTENDED);
    if(acl != NULL){
        printf("+");
    }
}
void printFileNoLinks(nlink_t nlink){
    printf("%hu", nlink);
}
void printFileOwner(uid_t uid){
    struct passwd *pw = getpwuid(uid);
    if(pw != NULL)
        printf("%s", pw->pw_name);
    else
        printf("%u", uid);
}
void printFileGroup(gid_t gid){
    struct group *grp = getgrgid(gid);
    if(grp != NULL)
        printf("%s", grp->gr_name);
    else
        printf("%u", gid);
}
void printFileSize(struct stat* fileStats, off_t size){
    if(fileType == 'c' || fileType == 'b')
        printf("%d", fileStats->st_rdev);
    else
        printf("%lld", size);
}

void printFileTime(struct timespec mtimespec){
    time_t file_modified_time = mtimespec.tv_sec;
    struct tm *file_time = localtime(&file_modified_time);

    char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};  

    if (file_time != NULL) {
        int hours = file_time->tm_hour;
        int minutes = file_time->tm_min;
        int year = file_time->tm_year + 1900;
        int month = file_time->tm_mon;
        int day = file_time->tm_mday;
        printf("%s %2d", months[month], day);
        printf("|");
        double diff_modified_seconds = difftime(cur_time, file_modified_time);
        double six_months_seconds = 6 * 30 * 24 * 60 * 60;
        if(diff_modified_seconds <= six_months_seconds)
            printf("%02d:%02d", hours, minutes);
        else    
            printf("%04d", year);
    }
}
void printFileName(char* path, char filename[]){
    printf("%s", filename);

    char* sym_file = (char*)malloc(PATH_LEN * sizeof(char)); // Buffer to store the path of the symbolic link
    ssize_t len = readlink(path, sym_file, sizeof(sym_file) - 1);
    if (len != -1) {
        sym_file[len] = '\0';  // Null-terminate the string
        printf(" -> %s", sym_file);
    }
}
void printFileStat(char* path, struct dirent* file, struct stat* fileStats){
    printFileType(path, fileStats->st_mode);
    printFilePermissions(fileStats->st_mode);
    printFileSpecial(path);
    printf("|");
    printFileNoLinks(fileStats->st_nlink);
    printf("|");
    printFileOwner(fileStats->st_uid);
    printf("|");
    printFileGroup(fileStats->st_gid);
    printf("|");
    printFileSize(fileStats, fileStats->st_size);
    printf("|");
    printFileTime(fileStats->st_mtimespec);
    printf("|");
    printFileName(path, file->d_name);

    printf("\n");
}


int main(int argc, char* argv[]) {
    int pipefd[2];
    pipe(pipefd);

    if (!fork()) {
        // Child process: Execute column command
        int fd = open("list.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, 1);
        dup2(pipefd[0], 0); // Connect the read end of the pipe to stdin
        close(pipefd[1]); // Close the write end of the pipe

        // Execute the column command
        char* args[] = {"column", "-t", "-s", "|", NULL};
        execvp("column", args);
        perror("execvp");
        exit(1);
    } else {
        // Parent process: Generate output and write to the pipe
        close(pipefd[0]); // Close the read end of the pipe

        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], 1);
        close(pipefd[1]); // Close the original stdout (now connected to the pipe)

        time_t cur_time = time(NULL);

        char* directory = (char*)malloc(PATH_LEN * sizeof(char));
        if (argc == 2) {
            directory = argv[1];
        } else {
            directory = ".";
        }

        DIR* drc = opendir(directory);
        struct dirent* file = (struct dirent*)(malloc(sizeof(struct dirent)));
        int sum = 0;

        while ((file = readdir(drc)) != NULL) {
            if (file->d_name[0] == '.')
                continue;
            char path[PATH_LEN];
            sprintf(path, "%s/%s", directory, file->d_name);
            struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
            if (stat(path, fileStats) == -1) {
                perror("statError");
                exit(1);
            }

            // Output file attributes to be piped to 'column'
            // You can replace this section with your functions or the rest of your code
            // printing file attributes in the desired format
            printFileStat(path, file, fileStats);
            // Example: Outputs file name, file type (directory or not), file size

            sum += fileStats->st_blocks;
        }

        printf("Total %d\n", sum);
    }

    return 0;
}





