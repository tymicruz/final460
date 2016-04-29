#include "ucode.c"

#define MAX_RD_SZ 1024

int main(int argc, char * argv[]){

	char buf[MAX_RD_SZ];
	char line[MAX_RD_SZ];

	char *more_hello = "****tcruz more*****\n\r";
	char *err1 = "can't more from stdin\r\n";
	char *err2 = "can't more, file doesn't exist\r\n";
	char c;
	STAT s;
	int fd, redin, redout;
	int size_remain;
	int i = 0, n = 0, j = 0, k = 0;
	int print_amount = 0;
	int curr_fd;
	int temp_fd;

	redin = is_stdin_red();
	redout = is_stdout_red();

	bzero(buf, MAX_RD_SZ);
	gettty(buf);
	curr_fd = open(buf, O_WRONLY);

	if(!redin && argc < 2){
		write(curr_fd, err1, strlen(err1));
		close(curr_fd);
		exit(0);
	}

	//write to stderr because io may have directed away from stdout

	bzero(buf, MAX_RD_SZ);
	bzero(line, MAX_RD_SZ);

	if(argc <= 1){
		fd = 0;
	}
	else{

		fd = open(argv[1], O_RDONLY);

		if(fd < 0){
			write(curr_fd, err2, strlen(err2));
			close(curr_fd);
			exit(1);
		}
	}

	if(!(redin && redout)){
		write(curr_fd, more_hello, strlen(more_hello));
	}

	if(redin && !redout){
		gettty(buf);
		temp_fd = open(buf, O_RDONLY);
		bzero(buf, MAX_RD_SZ);
	}

	fstat(fd, &s);
	size_remain = s.st_size;

	//n = read(fd, buf, MAX_RD_SZ);
	print_amount = 24;

	n = 1;

	while(1){
		for(i = 0; i < print_amount && n; i++){
			n = get_line_more(fd, buf);

			if(!redout){
				if(n)
					printf("%s\r", buf);
				else{
					printf("%s\r\n", buf);
					close(curr_fd);
					exit(1);
				}
			}
			else
				{
					printf("%s", buf);
					exit(1);
				}
		}

		if(!redout){
			//get new print amount
			if(!redin){
				c = getc();
			}else{
				read(temp_fd, &c, 1);
			}
	
			

			switch(c){
				case ' ': print_amount = 24; break;
				case '\r' : print_amount = 1; break;
				default: print_amount = 0; break;
		}
	}
}

//close(fd);
close(temp_fd);
close(curr_fd);
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

int get_line_more(int fd, char *buf){

	int n;

	while(n = read(fd, buf, 1) > 0){
		
		if(*buf == '\n'){
			*(buf+1) = 0;
			return 1;
			break;
		}

		buf++;
	}

	*buf = 0;

		//end
	return 0;
}







//for(; i < print_amount && j < n; j++){
		// 	line[k] = buf[j];
		// 	line[k + 1] = 0;

		// 	if(buf[j] == '\n'){
		// 		i++;
		// 		if(redout){

		// 			printf("%s", line);
		// 		}else{
		// 			printf("%s\r", line);
		// 		}

		// 		k = 0;
		// 		bzero(line, MAX_RD_SZ);
		// 	}else{
		// 		k++;
		// 	}
		// }



		// //need more but exhausted buf
		// if(i < print_amount){
		// 	j = 0;

		// 	bzero(buf, MAX_RD_SZ);
		// 	n = read(fd, buf, MAX_RD_SZ);

		// 	//leftover in line but no more in file
		// 	if(n <= 0){
		// 		if(redout){

		// 			printf("%s", line);
		// 		}else{
		// 			printf("%s\r", line);
		// 		}

		// 		bzero(line, MAX_RD_SZ);
		// 		//return done
		// 	}
		// }

		// if(!redout && i >= print_amount){
		// 	//get new print amount
		// 	i = 0;
		// 	c = getc();

		// 	switch(c){
		// 		case ' ': print_amount = 24; break;
		// 		case '\r' : print_amount = 1; break;
		// 		default: print_amount = 0; break;
		// 	}
		// }
