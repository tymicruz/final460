#include "ucode.c"

#define MAX_RD_SZ 1024

int main(int argc, char * argv[]){

	char buf[MAX_RD_SZ];
	char *cat_hello = "tcruz cat ^   ^\n\r           o o\n\r           =v=\n\r           ---\n\r***********************\n\r";
	char *cat_fail = "fail cat  ^   ^ (file doesn't exist)\n\r           _ _\n\r           =v=\n\r            _ \n\r***********************\n\r";
	char *redin_message ="redin redirected\n\r";
	char *redout_message ="redout redirected\n\r";
	int read_amount;
	char c;
	STAT s;
	int fd, redin, redout;
	int size_remain;
	int i = 0, n = 0;

	redin = is_stdin_red();
	redout = is_stdout_red();

	//write to stderr because io may have directed away from stdout

	bzero(buf, MAX_RD_SZ);

	// if(redin){
	// 	write(2, redin_message, strlen(redin_message));
	// }
	// if(redout){
	// 	write(2, redout_message, strlen(redout_message));
	// }


	//if argc < 1 I should exit

	if(argc <= 1){
		fd = 0;
		read_amount = 1;

	}
	else{
		fd = open(argv[1], O_RDONLY);

		if(fd < 0){
			write(2, cat_fail, strlen(cat_fail));
			close(2);
			exit(1);
		}

		read_amount = MAX_RD_SZ;
	}

	//don't print if we aint in the
	if(!(redin && redout)){
		write(2, cat_hello, strlen(cat_hello));
	}

	fstat(fd, &s);
	size_remain = s.st_size;

	//printf("size_remain: %d\n", size_remain);
	n = read(fd, buf, read_amount);

	while(n > 0){
		for (i=0; i<n; i++){

			//readin from 0 stdin, or redirected stdin
			if (fd==0){
        		if (buf[i]=='\r' && !redin) 
  	       			buf[i]='\n';     // change \r to \n
  	       		else if(buf[i]=='\r' && redin)
  	       			continue;


  	   			write(1, &buf[i], 1);
       			if (redin && redout) // cat in between pipes as in cat f | CAT | cat
  	       			continue;       // do not spit out anything

  	       		//getting from term, so put carriage return
  	   			if ((!redin) && (buf[i]=='\n')){
  	   				write(2, "\r", 1);
	  	   		}

	  	   		//still echo to term if getting from term and out if redirected
	  	   		if (redout){
	  	   			write(2, &buf[i], 1);
	  	   			if ( buf[i]=='\n')
	  	   				write(2, "\r", 1);
	  	   		}

  			}
  			else{//getting from a file or something
  				write(1, &buf[i], 1);
  				if (buf[i]=='\n'){
  					write(2, "\r", 1);
  				}
  			}
    	}

    	n = read(fd, buf, read_amount);
}

	exit(0);
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