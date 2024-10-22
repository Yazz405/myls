#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/**
 *Refrences:
 * - http://codewiki.wikidot.com/c:struct-stat
 * - https://pubs.opengroup.org/onlinepubs/009604499/functions/getgrgid.html
 * - https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
 * - https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html
 */



void list_directory(const char* directory_name);

int main(int argc, char** argv) {
  // Process arguments
  if (argc == 1) {
    list_directory(".");
  } else if (argc == 2) {
    list_directory(argv[1]);
  } else {
    fprintf(stderr, "Please specify one or zero command line arguments.\n");
  }

  return 0;
}

void list_directory(const char* directory_name) {
  struct dirent* current;
  struct stat dir_info;
  struct passwd* fileOwner;
  struct group* fileGroup;

  int errorCheck;

  //open the directory
  DIR* directory = opendir(directory_name);

  //Check if opendir failed
  if (directory == NULL) {
    fprintf(stderr, "Failed to open directory: no such file or directory\n");
    exit(1);
  }

  //read the first entry in the directory
  current = readdir(directory);

  while (current != NULL) {
    //retrieve the information from the current directory

    char buffer[strlen(directory_name) + strlen(current->d_name) + 2];
    strcpy(buffer, directory_name);
    strcat(buffer, "/");
    strcat(buffer, current->d_name);

    errorCheck = stat(buffer, &dir_info);
    if (errorCheck == -1) {
      fprintf(stderr, "stat failed\n");
    }

    //print out the permissions
    printf( (S_ISDIR(dir_info.st_mode)) ? "d" : "-");
    printf( (dir_info.st_mode & S_IRUSR) ? "r" : "-");
    printf( (dir_info.st_mode & S_IWUSR) ? "w" : "-");
    printf( (dir_info.st_mode & S_IXUSR) ? "x" : "-");
    printf( (dir_info.st_mode & S_IRGRP) ? "r" : "-");
    printf( (dir_info.st_mode & S_IWGRP) ? "w" : "-");
    printf( (dir_info.st_mode & S_IXGRP) ? "x" : "-");
    printf( (dir_info.st_mode & S_IROTH) ? "r" : "-");
    printf( (dir_info.st_mode & S_IWOTH) ? "w" : "-");
    printf( (dir_info.st_mode & S_IXOTH) ? "x " : "- ");

    //print the file owner
    fileOwner = getpwuid(dir_info.st_uid);
    if (fileOwner == NULL) {
      fprintf(stderr, "matching entry is not found");
      exit(1);
    }

    printf("%s ", fileOwner->pw_name);

    //print the file group
    if ((fileGroup = getgrgid(dir_info.st_gid)) != NULL) {
      printf("%s ", fileGroup->gr_name);
    } else {
      fprintf(stderr, "matching entry is not found");
    }

    //print the file size
    printf("%ld ",dir_info.st_size );

    //print the file name
    printf("%s \n", current->d_name);

    //read the next entry in the directory
    current = readdir(directory);
  }
}
