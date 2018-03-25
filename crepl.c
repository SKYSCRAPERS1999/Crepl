#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

#define maxn 1024

int is_func(char* s, int n, char* name){
	int i, j = 0; int ok = 0;
	for (i = 0; i + 3 < n; i++){
		if (s[i] == 'i' && s[i + 1] == 'n' && s[i + 2] == 't'){
			ok = 1; 
			i = i + 3;
			break;
		}
	}
	if (ok == 0) return 0;
	for ( ; i + 1 < n; i++){
		if (isalpha(s[i]) || s[i] == '_'){
			while (i < n && s[i] != '('){
				name[j++] = s[i++];
			}
			name[j] = '\0';
			return 1;
		}
	}
	return 0;
}
int main() {
	while (1){
		printf("$ ");
		char op[maxn];
		char* ok = fgets(op, maxn - 1, stdin);
		if (ok == NULL){
			puts("");
			continue;
		}else{
		    int n = strlen(op); char name[50];
			int isf = is_func(op, n, name);
		
			char* filename = strdup(strcat(name, ".c"));
			int fd = mkstemp(filename);

			printf("Temporary file created\n");
			
			if (write(fd, op, maxn - 1) == -1){
				perror("write error!\n");
				exit(1);
			}
			int pid = fork();
			if (pid == -1){
				perror("fork error!\n");
				exit(1);
			}
			if (pid == 0){
				if (isf){
					char* myargs[20];
					myargs[0] = strdup("gcc");
					myargs[1] = strdup("-shared");
					myargs[2] = strdup("-o");
					myargs[3] = strdup(strcat(name, ".so"));
					myargs[4] = strdup("fPIC");
					myargs[5] = strdup(filename);
					myargs[6] = NULL;
					execvp(myargs[0], myargs);

					perror("execve error!\n");
					exit(1);
					
				}else{
					
				}

			}else{
				wait(NULL);
				

			}

		}
	}
	return 0;
}
