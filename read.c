#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

const int MAX_CHAR = 4096;

void show_depth(int count) {
    for (int i = 0; i < count; i++) {
        printf("    ");
    }
}

/* 
desired printing format is:

/home/some_project 
    Makefile				
    Include
 	    Common.h
        util.h 
    src			
        Main.c
        util.c
*/
void list_directory(const char *path, int depth) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if(entry-> d_type != DT_DIR){
            // print spaces for depth tree  
            show_depth(depth);
            printf("%s\n", entry->d_name);
        }
        else if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // print the name
            show_depth(depth);
            printf("%s\n", entry->d_name);

            // recursive calls in case of a subdirectories
            char new_dir_path[MAX_CHAR]; 
            sprintf(new_dir_path, "%s/%s", path, entry->d_name);
            // char new_dir_path[255];
            // strcpy(new_dir_path, path); 
            // strcat(new_dir_path, "/");
            // strcat(new_dir_path, entry->d_name);
            list_directory(new_dir_path, depth + 1);
        }
    }

    closedir(dir);
}
// get the current working directory and print it
int print_pwd(){
    char cwd[MAX_CHAR];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("Error getting working directory");
        return 1;
    }
    return 0;
}

int main() {
    if (print_pwd() > 1) 
        return 1;
    list_directory(".", 1);
    return 0;
}
