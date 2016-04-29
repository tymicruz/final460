//login.c  argv[0] = login, argv[1] = /dev/ttyX

#include "ucode.c"

int stdin, stdout, stderr;
char *passwd_file = "/etc/passwd";

main(int argc, char *argv[]){

	int passwd_fd;//holds fd of /etc/passwd file
	int size_remain;
	int i;
	int num_tokens;
	int uid, gid;
	char user[64], pass[64];
	
	char line[448]; //(8 * 64) = 512
	char tokens[7][64];

	STAT s;

	//STEP 1:
	//close fds 0, 1 inherited from init
	close(0);
	close(1);

	//STEP 2:
	//get fds for std: in, out, and err.
	stdin = open(argv[1], 0);
	stdout = open(argv[1], 1);
	stderr = open(argv[1], 2);

	//STEP 3:
	//set tty name string of PROC.tty
	settty(argv[1]);

	while(1){

		//STEP 4:
		//open /etc/passwd file for READ (defined in type.h)
		//could put this outside of this loop if lseek worked
		passwd_fd = open(passwd_file, O_RDONLY);

		//STEP 5:
		//get username
		printf("login: ");		
		gets(user);

		//get password
		printf("password: ");
		gets(pass);

		stat(passwd_file, &s);
		size_remain = s.st_size;

		printf("size_of passwd: %d\n", size_remain);

		while(1){

			for(i = 0; size_remain > 0 && i < 448 - 1; i++){
				read(passwd_fd, &line[i], 1);
				size_remain--;

				//newline put null char
				if(line[i] == '\n') {
					line[i] = 0;
					break;
				}

				//last char, put null char
				if(size_remain <= 0){
					line[i + 1] = 0;
					break;
				}
			}

			//here we have one line
			num_tokens = tokenize(line, tokens, ':');

			//bad line in /etc/passwd file
			if(num_tokens != 7){
				continue;
			}

			//0=usrname, 1=pssword, 2=gid, 3=uid, 4=fullname, 5=HOMEDIR, 6=program
			//printf("%s %s\n", user, tokens[0]);
			//printf("%s %s\n", pass, tokens[1]);

			if(strcmp(user, tokens[0]) == 0 && strcmp(pass, tokens[1]) == 0){
				printf("wassup, %s!\n", tokens[4]);

				//change uid and gid
				uid = atoi(tokens[3]);
				gid = atoi(tokens[2]);
				chuid(uid, gid);

				//change CWD to HOMEDIR
				chdir(tokens[5]);

				//close opened /etc/passwd file
				close(passwd_fd);
				exec(tokens[6]);

				printf("failed execution of %s\n", tokens[6]);
				

			}//else{
				//printf("fail\n");
			//}


			if(size_remain <= 0){
				printf("login failed, playboi...\n");
				break;
			}

		}

		//seek to beginning doesn't work
		//lseek(passwd_fd, 0);
		close(passwd_fd);
	}
}

//return negative or 0 if line is not good
//will return number of tokens 
int tokenize(char *line, char tokens[7][64], char delimiter){

	int i = 0, j = 0;

	while(*line && i < 7){


		if(*line == delimiter){
			tokens[i][j] = 0; //null term this token

			if(j)
				i++;

			j = 0;
			line++;
			continue;
		}

		tokens[i][j++] = *line;

		line++;
	}

	if(j == 0){
		return i;
	}

	return i + 1;
}