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

int main() {
	while (1){
		putchar('$');
		char op[maxn];
		int ok = fgets(op, maxn - 1, stdin);
		if (!ok){
			puts("");
			continue;
		}else{


		}
	}
	return 0;
}
