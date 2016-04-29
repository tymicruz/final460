

#include "ucode.c"

char *easy_cmds[] = {"cd", "su", "logout", "echo", "exit", 0};

main (int argc, char *argv[])
{
	char cmd_line[640];
	char tokens [10][64];
	int num_tokens;
	int i, pid, status, ino, dev;
	//int passwd_fd;

	printf("Welcome to the Cruz Shell!\n");
	printf("Enter ? for menu\n");

	while(1){

		getcwd(cmd_line);
		printf("%s ", cmd_line);
		printf("cruzsh: ");
		bzero(cmd_line, 640);
		gets(cmd_line);
		i = remove_extra_space(cmd_line);

		if(i >= 0){
			//if i == 0, no spaces were removed
			//if i > 0 space was removed

		}else{
			//nothing or all spaces line
			continue;
		}

		num_tokens = tokenize(cmd_line, tokens, ' ');
		if(num_tokens <= 0)
			continue;

		if(is_easy(tokens[0])){
			printf("EASY!\n");
			do_easy(num_tokens, tokens);
			continue;
		}

		pid = fork();

		if(pid){
			//printf("wait for child...\n");
			pid = wait(&status);
			//printf("child died with %d\n", status);

		}else{
			//child does something and exit
			
			do_pipe(cmd_line, 0);
			//printf("hi, from child.\n");
			exit(99);
		}

	}
		
}

int do_pipe(char *cmd_line, int *pd){

	int has_pipe;
	char head[128], tail[128];
	int pid;
	int lpd[2];

	if(pd){
		close(pd[0]); dup2(pd[1], 1); close(pd[1]);
	}

	has_pipe = pipe_scan(cmd_line, head, tail);

	if(has_pipe){
		// printf("has pipe\n");
		// printf("h:%s.\n", head);
		// printf("t:%s.\n", tail);
		if (pipe(lpd) < 0){ // create LEFT side pipe
         	printf("*process (%d) pipe fail\n*", getpid());
         	exit(1);
      	}

		pid = fork();

		if(pid){
			close(lpd[1]); dup2(lpd[0], 0); close(lpd[0]);
			do_command(tail);

		}else{
			do_pipe(head, lpd);
		}


	}else{
		do_command(cmd_line);
		exit(9);
	}
}

int do_command(char * cmd_line){

	int r;
	//scan for redirection
	//cmd_line will be chopped if io redirect is found
	r = rio_scan(cmd_line);

	exec(cmd_line);
	//exit();
}

//won't receive an empty line here
int pipe_scan(char * cmd_line, char *head, char *tail){

	int has_pipe = 0, i = 0;
	char *cp = cmd_line;
	int len = strlen(cmd_line);

	while(*cp){

		if(*cp == '|'){
			has_pipe++;
			break;
		}

		cp++;
	}

	if(!has_pipe){
		return 0;
	}

	//go to end start search
	cp = &(cmd_line[len - 1]);
	i = len - 1;

	while(i >= 0){
		cp = &(cmd_line[i--]);

		if(*cp == '|'){
			break;
		}
	}

	//now cp points right most pipe
	*cp = 0;
	strcpy(head, cmd_line);
	cp++;
	strcpy(tail, cp);

	remove_extra_space(head);
	remove_extra_space(tail);

	return 1;
}

//redirect io scan
//return 1 if redirect io is found
//return 0 if not found
//return -1 if can not redirect to file

int rio_scan(char * cmd_line){
	int has_rio = 0;
	int fd;
	int in = 0;
	int out = 0;
	char file_name[32];
	char *cp = cmd_line;


	//int len = strlen(cmd_line);
	while(*cp){
		if(*cp == '<'){
			//redirect input (stdin))
			in = 1;
			*cp = 0;//null terminate the cmd_line to only be head
			cp++; //move cp to beginning of file name
			break;
		}
		if(*cp == '>'){
			//redirect output (stdout)
			out = (O_WRONLY|O_CREAT|O_TRUNC);
			*cp = 0;//null terminate the cmd_line to only be head
			cp++; //move cp to beginning of file name

			if(*cp == '>'){
				out = (O_WRONLY|O_CREAT|O_APPEND);
				cp++; //more cp to beginning of file name
			}

			break;
		}	
		cp++;
	}

	if(!in && !out){
		return 0;
	}

	remove_extra_space(cmd_line);
	//printf("exec:.%s.\n", cmd_line);
	//get file name to redirect to
	strcpy(file_name, cp);
	remove_extra_space(file_name);
	//printf("to:.%s.\n", file_name);

	//redirect input
	if(in){
		fd = open(file_name, O_RDONLY);

		if(fd < 0){ //file doesn't exist
			//printf("%s doesn't exist\n", file_name);
			//creat file file
			return -1;
		}

		dup2(fd, 0);
		close(fd);
		return 1;
	}

	//redirect output
	if(out){
		fd = open(file_name, out);

		//file doesn't exist
		if(fd < 0){
			return -1;
		}

		dup2(fd, 1);
		close(fd);
		return 2;
	}

	//should never get here
	return -1;

}

int tokenize(char *line, char tokens[10][64], char delimiter){

	int i = 0, j = 0;

	while(*line && i < 10){


		if(*line == delimiter){
			tokens[i][j] = 0; //null term this token

			if(j)
				i++;

			j = 0;
			line++;
			continue;
		}

		tokens[i][j++] = *line;
		tokens[i][j] = 0;

		//overflow of a token
		//if(j >= 64)

		line++;
	}

	if(j == 0){
		return i;
	}


	return i + 1;
}

int remove_extra_space(char *cmd_line){
	
	char buf[640];
	char *cp;
	int space = 1, i = 0;
	int len = strlen(cmd_line);

	cp = cmd_line;

	while(*cp){

		if(*cp != ' '){
			space = 0;
		}

		if(space == 0){

			buf[i++] = *cp;
		}

		if(*cp == ' '){
			space = 1;
		}

		cp++;
	}

	//all spaces or nothing
	if(i == 0){
		strcpy(cmd_line, "");
		return -1;
	}

	buf[i] = 0;//null terminate the buffer

	if(buf[i - 1] == ' '){
		//buf[strlen(buf) - 1] = 0;
		i--;
		buf[i] = 0;
	}

	strcpy(cmd_line, buf);

	//return #of spaces removed
	return i == len ? 0 : len - i; 
}

int is_easy(char *cmd){

	int i = 0;

	for(i=0; easy_cmds[i]; i++){

		if(strcmp(cmd, easy_cmds[i]) == 0){
			return 1;
		}
	}

	return 0;
}

int do_easy(int num_tokens, char tokens[10][64]){

	int i;

		if(strcmp(tokens[0], "cd")==0){
			if (num_tokens < 2){
				printf("missing 2nd parameter for cd\n");
				chdir("/");
			}else{
				chdir(tokens[1]);
			}

			return 1;
		}

		if(strcmp(tokens[0], "logout")==0){
			chdir("/"); 
			exit(1);
			return 1;
		}

		if(strcmp(tokens[0], "su")==0){
			//prompt for password
			//open password file /etc/passwd
			//check to see if matches root password
			//if matches root do chuid(uid, gid)
			su();
			return 1;
		}

		if(strcmp(tokens[0], "echo") == 0){
			for(i = 1; i < num_tokens; i++){
				printf("%s ", tokens[i]);
			}

			return 1;
		}

		if(strcmp(tokens[0], "exit") == 0){
			exit(99);
		}

		printf("easy but not yet implemented\n");
		return 0;

}

int su()
{
	int curr_id;
  	char buf[64];

  	curr_id = getuid(); 

  	if (curr_id == 0){
    	printf("YOU ARE ALREADY SUPER\n");
    	return 0;
  	} 

  	bzero(buf, 64);
  	printf("SUPER PASS: ");
  	gets(buf);

  	//password for root hardcoded
  	//otherwise we would open passwd file in etc/
  	//then check password match with all uid == 0
  	if (strcmp(buf, "12345")==0){
      	setuid(0, 0);    
  	} 
  	else
    {
    	printf("invalid password.\n");
    }
}  
