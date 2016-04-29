#include "ucode.c"

#define MAX_READ_SZ 1024

int main(int argc, char *argv[]){

	int from_fd, to_fd;

	char *err1 = "cp <src> <dest>\n\r";
	char *err2 = "cp src file doesn't exit\n\r";
	char *err3 = "creat dest error\n\r";
	int n;
	char buf[MAX_READ_SZ];
	int read_amount = MAX_READ_SZ;

	printf("***** cp tcruz ******\n");

	bzero(buf, MAX_RD_SZ);
	gettty(buf);
	curr_fd = open(buf, O_WRONLY);
	bzero(buf, MAX_RD_SZ);

	if(argc < 3){
		write(curr_fd, err1, strlen(err1));
		close(curr_fd);
		exit(1);
	}

	from_fd = open(argv[1], O_RDONLY);

	if(from_fd < 0){
		printf("%s\n", argv[1]);
		write(curr_fd, err2, strlen(err2));
		close(curr_fd);
		exit(1);
	}

	to_fd = open(argv[2],  O_WRONLY|O_CREAT|O_TRUNC);

	if(to_fd < 0){

		write(curr_fd, err3, strlen(err3));
		close(curr_fd);
		exit(1);
	}

	bzero(buf, MAX_READ_SZ);

	n = read(from_fd, buf, read_amount);

	while(n > 0){

		write(to_fd, buf, n);

		n = read(from_fd, buf, read_amount);
	}

	close(from_fd);
	close(to_fd);
	close(curr_fd);

}