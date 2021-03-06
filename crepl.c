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
#include <dlfcn.h>
#include <math.h>

#define maxn 1024
char func_list[maxn][50];
int N = 0;
char* strdup(const char * s);
int mkstemp(char *);

int is_func(char* s, int n, char* name){
	int i, j = 0; int ok = 0;
	char check[50]; sscanf(s, "%s", check);
	if (strcmp(check, "ls") == 0 || strcmp(check, "list") == 0){
		for (i = 0; i < N; i++){
			printf("%d: %s\n", i, func_list[i]);
		}
		return -1;
	}
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

char* rand_str(char *str, const int len){
    int i; char s[50];
    for (i = 0; i < len; ++i) s[i] = 'A' + rand() % 26;
    s[i]= '\0';
	strcat(str, s);
	return str;
}

int main() {
	
	srand(time(NULL));
	
	int fd_null = open("/dev/null", O_TRUNC|O_RDWR);
	dup2(fd_null, 2);

	while (1){
		printf(">> ");
		char op[maxn];
		char* ok = fgets(op, maxn - 1, stdin);
		if (ok == NULL){
			puts("");
			continue;
		}else{
		    int n = strlen(op); char name[50]; 
			char func_name[50] = "_expr_";
			
			int isf = is_func(op, n, name);
			if (isf < 0) continue;
			
			char filename[50]; strcpy(filename, name);
			strcpy(filename + strlen(filename), "XXXXXX");
			
			//printf("%s\n",filename);
			int fd = mkstemp(filename);
			if (fd < 0){
				perror("create file error!\n");
				exit(1);
			}
			//printf("Temporary file created\n");
			if (isf){
				if (write(fd, op, n) == -1){
					perror("write error!\n");
					exit(1);
				}else{
					printf("%s created\n", name);
				}
			}else{
				rand_str(func_name, 5);	strcat(func_name, "_"); strcat(func_name, name);
				if (write(fd, "int ", 4) == -1||
					write(fd, func_name, strlen(func_name)) == -1 
					|| write(fd, "(){return ", 10) == -1
					|| write(fd, op, strlen(op)) == -1
					|| write(fd, ";}", 2) == -1){
					perror("write error!\n");
					exit(1);
				}
			}
			int pid = fork(); 
			if (pid == -1){
				perror("fork error!\n");
				exit(1);
			} else if (pid == 0){
				//printf("Child: (pid: %d)\n\n", getpid());
				
				char* myargs[20];
				if (isf){
					myargs[0] = strdup("/usr/bin/gcc");
					myargs[1] = strdup("-shared");
					myargs[2] = strdup("-g");
					myargs[3] = strdup("-o");
					myargs[4] = strdup(strcat(name, ".so"));
					myargs[5] = strdup( "-x");
					myargs[6] = strdup("c");
					myargs[7] = strdup("-fPIC");
					myargs[8] = strdup(filename);
					myargs[9] = NULL;
					//for (int i = 0; i < 9; i++)	printf("%s\n", myargs[i]);
				}else{
					myargs[0] = strdup("/usr/bin/gcc");
					myargs[1] = strdup("-shared");
					myargs[2] = strdup("-g");
					myargs[3] = strdup("-o");
					myargs[4] = strdup(strcat(func_name, ".so"));
					myargs[5] = strdup( "-x");
					myargs[6] = strdup("c");
					myargs[7] = strdup("-fPIC");
					myargs[8] = strdup(filename);
					myargs[9] = NULL;
					//for (int i = 0; i < 9; i++)	printf("%s\n", myargs[i]);
				}
				execvp(myargs[0], myargs);
			
			}else{
				wait(NULL);
				
				unlink(filename);
				close(fd);
				
				char so_name[50] = "./";	
				if (isf) strcat(so_name, name), strcat(so_name, ".so");
				else strcat(so_name, func_name), strcat(so_name, ".so");

				void* handle = dlopen(so_name, RTLD_NOW|RTLD_GLOBAL);

				if (handle == NULL) {
					perror("open error!\n");
					exit(1);
				}
				
				if (!isf){
					int (*func)() = (int (*)())dlsym(handle, func_name); // 查找XXX对应的函数
					int value = func(); // 通过函数指针调用
					printf(">> %s%d\n", op, value);	
					
				}else{
					strcpy(func_list[N], name);
					++N;
				}
				
			}
		}
	}
	return 0;
}
