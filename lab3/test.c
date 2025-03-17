#include <stdio.h>
#include <dirent.h>
#include <string.h>

void reverse_string(char *str){
    int length = strlen(str);
    int i,j;
    char temp;
    for(i = 0, j = length - 2; i<j; i++, j--){
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3){
        printf("Program takes two arguments\n");
        return 1;
    }

    struct dirent *curr_file;
    DIR *source_dir = opendir(argv[1]);
    DIR *dest_dir = opendir(argv[2]);
    if (!source_dir || !dest_dir){
        printf("Could not open directory: %s\n", argv[1]);
        return 1;
    }
    while ((curr_file = readdir(source_dir)) != NULL){
        if (strlen(curr_file->d_name) >= 4 && strncmp(".txt", curr_file->d_name + strlen(curr_file->d_name) - 4, 4) == 0)
        {
            char source_file_path[1024];
            char dest_file_path[1024];
            char buffer[1024];

            snprintf(source_file_path,sizeof(source_file_path),"%s/%s",argv[1],curr_file->d_name);

            char *dest_file_name = strtok(curr_file->d_name, ".");
            snprintf(dest_file_path,sizeof(dest_file_path),"%s/%s_reversed.txt",argv[2],dest_file_name);

            FILE *source_file = fopen(source_file_path, "r");
            FILE *dest_file = fopen(dest_file_path,"w");

            while (fgets(buffer, sizeof(buffer), source_file) != NULL)
            {
                reverse_string(buffer);
                fputs(buffer,dest_file);
            }
            fclose(source_file);
            fclose(dest_file);
        }
    }
    closedir(source_dir);
    closedir(dest_dir);
    return 0;
}