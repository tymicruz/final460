#include "ucode.c"

int main(int argc, char * argv[]){

	char pattern[32];
	char file_name[32];
	char buf[MAX_RD_SZ];
	char line[MAX_RD_SZ];
	int fd = 0;//default to stdin
	int n, redin, redout;
	int i = 0;

	char *grep_hello = "***** tcruz grep ******\r\n";
	char *err1 = "grep <pattern> <file>\n\r";
	char *err2 = "file doesn't exist\n\r";
	int curr_fd;

	redin = is_stdin_red();
	redout = is_stdout_red();

	bzero(buf, MAX_RD_SZ);
	gettty(buf);
	curr_fd = open(buf, O_WRONLY);
	bzero(buf, MAX_RD_SZ);

	if(!(redin && redout))
		write(curr_fd, grep_hello, strlen(grep_hello));

	if(argc >= 3){
		strcpy(pattern, argv[1]);
		strcpy(file_name, argv[2]);

		//open file for read
		fd = open(file_name, O_RDONLY);

		if(fd < 0){
			write(curr_fd, err2, strlen(err2));
			close(curr_fd);
			exit(1);
		}
	}
	else if(argc == 2){
		strcpy(pattern, argv[1]);
		bzero(file_name, 32);
		//read from stdin
	}else{
		write(curr_fd, err1, strlen(err1));
		close(curr_fd);
		exit(1);
	}
	bzero(buf, MAX_RD_SZ);

	i = 0;
	n = read(fd, &buf[i], 1);

	while(n > 0){

		//reading from stdin and not redirected
		//we need to see what we are typing
		if (fd==0 && !redin){
			if (buf[i]=='\r'){
				buf[i] = '\n';
				write(curr_fd, "\n\r", 2);
			}  // change \r to \n
			else{
				write(curr_fd, &buf[i], 1);
			}
		}

		if(buf[i] == '\n'){
			
			if(match(pattern, line)){
				printf("%s\n", line);
			}

			i = 0;
			bzero(line, MAX_RD_SZ);
			bzero(buf, MAX_RD_SZ);

		}else{
			line[i] = buf[i];
			line[i+1] = 0;
			i++;
		}
		
		n = read(fd, &buf[i], 1);

		if(n <= 0){
			//check line one more time
			if(match(pattern, line)){
				printf("%s\n", line);
			}
		}
	}

	close(curr_fd);
}

int match(char *pattern, char *string){
	char *cpp, *cps;

	cpp = pattern;
	cps = string;

	while(*cps){
		if(*cpp == *cps){
			cpp++;//move to next letter of pattern
		}else{
			cpp = pattern; // go to beginning of pattern if no match
		}

		if(*cpp == 0){
			//got to the end of a pattern, so match was found
			return 1;
		}

		cps++;
	}

	return 0;
}

int is_stdout_red(){
	STAT norm, curr;
	int n, c;
	char buf[32];

	gettty(buf);

	n = stat(buf, &norm);
	c = fstat(1, &curr);

	//if(n < 0 || c < 0 return )

	if(norm.st_dev == curr.st_dev && norm.st_ino == curr.st_ino){
		return 0;
	}	

	return 1;
}

int is_stdin_red(){
	STAT norm, curr;
	int n, c;
	char buf[32];

	gettty(buf);

	n = stat(buf, &norm);
	c = fstat(0, &curr);

	//if(n < 0 || c < 0 return )

	if(norm.st_dev == curr.st_dev && norm.st_ino == curr.st_ino){
		return 0;
	}	

	return 1;
}