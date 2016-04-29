#include "ucode.c"

#define MAX_READ_SZ 1024

int main(int argc, char *argv[]){

	int from_fd, to_fd;
	char *l2u_hello = "**** tyler l2u ****\r\n";
	char *err1 = "l2u <src> <dest>\n\r";
	char *err2 = "l2u src file doesn't exit\n\r";
	char *err3 = "creat dest error\n\r";
	int size_remain;
	int n, i;
	char buf[MAX_READ_SZ];
	int read_amount = MAX_READ_SZ;
	int redin, redout;

	//printf("l2u tcruz\n");

	redin = is_stdin_red();
	redout = is_stdout_red();

	//say hello if not between pipes
	if(!(redin && redout)){
		write(2, l2u_hello, strlen(l2u_hello));
	}

	if(argc < 3){
		//write(2, err1, strlen(err1));
		//exit(1);
		from_fd = 0;
		to_fd = 1;
		read_amount = 1;
	}
	else{
		from_fd = open(argv[1], O_RDONLY);

		if(from_fd < 0){
			printf("%s\n", argv[1]);
			write(2, err2, strlen(err2));
			close(2);
			exit(1);
		}

		to_fd = open(argv[2],  O_WRONLY|O_CREAT|O_TRUNC);

		if(to_fd < 0){

			write(2, err3, strlen(err3));
			close(2);
			exit(1);
		}
	}

	bzero(buf, MAX_READ_SZ);

	n = read(from_fd, buf, read_amount);

	while(n > 0){

		for(i = 0; i < n; i++){
			if(buf[i] >= 'a' && buf[i] <= 'z')
				buf[i]-=32;

			//readin from 0 stdin, or redirected stdin
			if (from_fd==0){
        		if (buf[i]=='\r' && !redin) 
  	       			buf[i]='\n';     // change \r to \n
  	       		else if(buf[i]=='\r' && redin)
  	       			continue;

  	   			write(1, &buf[i], 1);
       			if (redin && redout) // cat in between pipes as in cat f | CAT | cat
  	       			continue;       // do not spit out anything

  	       		//getting from term, so put carriage return
  	   			if ((!redout) && (buf[i]=='\n')){
  	   				write(2, "\r", 1);
	  	   		}

	  	   		//still echo to term if getting from term and out if redirected
	  	   		if (redout){
	  	   			write(to_fd, &buf[i], 1);
	  	   			if (buf[i]=='\n')
	  	   				write(2, "\r", 1);
	  	   		}

  			}
  			else{//getting from a file or something
  				write(to_fd, &buf[i], 1);
  				if (buf[i]=='\n'){
  					write(2, "\r", 1);
  				}
  			}
		}

		//write(to_fd, buf, n);

		n = read(from_fd, buf, read_amount);
	}

	close(from_fd);
	close(to_fd);

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