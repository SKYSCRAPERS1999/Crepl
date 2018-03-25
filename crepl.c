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
			ok = 1; break;
		}
	}
	if (ok == 0) return 0;
	for ( ; i + 1 < n; i++){
		if (s[i] == '('){
			while (i + 1 < n && s[i + 1] != ')'){
				name[j++] = s[++i];
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
		    int n = strlen(op);
			FILE* fp = tmpfile();
			printf("Temporary file created\n");
			
			fwrite(op, 1, sizeof(op), fp);
			int pid = fork();
			if (pid == -1){
				perror("fork error!\n");
			}else if (pid == 0){
				char name[50];
				if (is_func(op, n, name)){
					char* myargs[20];
					myargs[0] = strdup("gcc");
					myargs[1] = strdup("-shared");
					myargs[2] = strdup("-o");
					myargs[3] = strdup(strcat(name, ".so"));
					myargs[4] = strdup("fPIC");
					myargs[5] = NULL;
					execvp(myargs[0], myargs);	
					
				}else{
					
				}

			}else{
				wait(NULL);
				

			}

			fclose(fp);
		}
	}
	return 0;
}
