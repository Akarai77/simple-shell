#include<stdio.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#define BUFFER_SIZE 1024
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

char **split_string(const char *str,char* delimiter) {
    char **tokens = malloc(5 * sizeof(char *));
    if (tokens == NULL) {
        perror(RED"malloc"RESET);
        exit(EXIT_FAILURE);
    }

    char *token;
    int i = 0;
    char *str_copy = strdup(str);
    if (str_copy == NULL) {
        perror(RED"strdup"RESET);
        exit(EXIT_FAILURE);
    }

    token = strtok(str_copy, delimiter);
    while (token != NULL) {
        tokens[i] = strdup(token);
        if (tokens[i] == NULL) {
            perror(RED"strdup"RESET);
            exit(EXIT_FAILURE);
        }
        i++;
        token = strtok(NULL, delimiter);
    }
    tokens[i] = NULL;
    free(str_copy);
    return tokens;
}


void echo(char buf[],char** tokens){
	char **minitokens1,**minitokens2;
	minitokens1 = split_string(buf,">");
	minitokens2 = split_string(buf,"\"");
	if(minitokens1[1] == NULL && minitokens2[1] == NULL){
		if(tokens[1] == NULL)
			printf(GREEN"echo"RESET);
		else {
			int i=1;;
			while(tokens[i]!=NULL)
				printf("%s ",tokens[i++]);
		}
	} else if(minitokens1[1] == NULL){
		printf(GREEN"%s"RESET,minitokens2[1]);
	} else {
		FILE *file;
		file = fopen(minitokens1[1],"w");
		if(file == NULL){
			perror(RED"fopen"RESET);
			return;
		}
		printf(GREEN"File %s created successfully"RESET,minitokens1[1]);
		char **miniminitokens1,**miniminitokens2;
		miniminitokens1 = split_string(minitokens1[0],"\"");
		miniminitokens2 = split_string(minitokens1[0]," ");
		if(miniminitokens1[1]!=NULL){
			fprintf(file,miniminitokens1[1]);
		} else {
			int j=1;
			char text[100000];
			while(miniminitokens2[j]!=NULL){
				fprintf(file,miniminitokens2[j++]);
			fprintf(file," ");
		}
	}
	fclose(file);
	}
}



void  getCurrentDirectory(){
	char buf[1024];
	if(getcwd(buf,sizeof(buf))!=NULL)
		printf(GREEN"%s"RESET,buf);
	else
		perror(RED"error"RESET);
}

void listDirectory(){
	DIR *dir = opendir(".");
	if(dir == NULL){
		perror(RED"opendir"RESET);
		return;
	}

	struct dirent* entry;
	while((entry=readdir(dir))!=NULL){
		printf(GREEN"%s "RESET,entry->d_name);
	}

	if(closedir(dir)==-1)
		perror(RED"closedir"RESET);
}

void makeDirectory(const char* pathname){
	if(mkdir(pathname,0777) == -1){
		if(errno == EEXIST)
			printf(RED"Directory %s already exists"RESET,pathname);
		else
			perror(RED"mkdir"RESET);
	} else {
		printf(GREEN"Directory %s created successfully"RESET,pathname);
	}
}

void removeDirectory(const char* pathname){
	if(rmdir(pathname) == -1){
		if(errno == ENOENT)
			printf(RED"Directory %s does not exist"RESET,pathname);
		else
			perror("rmdir");
	} else {
		printf(GREEN"Directory %s successfully deleted"RESET,pathname);
	}
}

void changeDirectory(const char* pathname){
	if(chdir(pathname) == -1){
		if(errno == ENOENT)
			printf(RED"Directory %s does not exist"RESET,pathname);
		else 
			perror("chdir");
	} else {
		printf(GREEN"Changed Current Working Directory to %s"RESET,pathname);
	}
}


void copy_file(const char *src_path, const char *dest_path) {
    FILE *src_file, *dest_file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read, bytes_written;  src_file = fopen(src_path, "rb");
    if (src_file == NULL) {
        perror(RED"fopen"RESET);
        return;
    }

    dest_file = fopen(dest_path, "wb");
    if (dest_file == NULL) {
        perror(RED"fopen"RESET);
        fclose(src_file);
        return;
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_file)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_read, dest_file);
        if (bytes_written != bytes_read) {
            fprintf(stderr, RED"Error writing to destination file."RESET);
            fclose(src_file);
            fclose(dest_file);
	    return;
	}
    }

    printf(GREEN"File %s Copied successfully to %s"RESET,src_path,dest_path);
    fclose(src_file);
    fclose(dest_file);
}



int move_file(const char *src_path, const char *dest_path) {
    if (rename(src_path, dest_path) != 0) {
        perror(RED"rename"RESET);
    } else {
	printf(GREEN"File %s moved successfully to %s"RESET,src_path,dest_path);
   }
}


int remove_file(const char* pathname){
	if(remove(pathname) == 0)
		printf(GREEN"File %s deleted successfully"RESET,pathname);
	else{
		if(errno == ENOENT)
			printf(RED"File %s does not exist"RESET,pathname);
		else
			perror(RED"remove"RESET);
	}
}


int main(){
	printf("SimpleShell2.0 --version=\"1.0\"");
	printf("Copyright (C) 2023 Free Software Foundation, Inc.\n");
	printf("This is free software; see the source for copying conditions.  There is NO ");
	printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
	printf(">>");
	while(1){
		char ch;
		char buf[100];
		char** tokens;
		char* command;
		char** echo_command;
		if (fgets(buf, sizeof(buf), stdin)) {
			if(buf[0] == '\n'){
				printf(">>");
				continue;
			}
    			if (buf[strlen(buf) - 1] == '\n') {
       			 buf[strlen(buf) - 1] = '\0';
    			}
		}
		tokens = split_string(buf," ");
		echo_command=split_string(buf,">");
		command=tokens[0];
		if(strcmp(command,"pwd") == 0){
			getCurrentDirectory();
		} else if(strcmp(command,"echo") == 0 || strcmp(echo_command[0],"echo")==0){
			echo(buf,tokens);
		} else if(strcmp(command,"ls") == 0){
			listDirectory();
		} else if(strcmp(command,"mkdir")==0){
			makeDirectory(tokens[1]);
		} else if(strcmp(command,"rmdir") == 0){
			removeDirectory(tokens[1]);
		} else if(strcmp(command,"cd") == 0){
			changeDirectory(tokens[1]);
		} else if(strcmp(command,"cp") == 0){
			copy_file(tokens[1],tokens[2]);
		} else if(strcmp(command,"mv") == 0){
			move_file(tokens[1],tokens[2]);
		} else if(strcmp(command,"rm")==0){
			remove_file(tokens[1]);
		} else if(strcmp(command,"exit") == 0){
			exit(0);
		} else {
			printf("command %s not found",command);
		}
		printf("\n>>");
	}
}

