#include "ucode.c"

int console, s0, s1;

int main(argc, argv) int argc; char* argv[];
{
	int stdin, stdout, stderr;
	//return fds for stdin and stdout
	stdin  = open("/dev/tty0", O_RDONLY); 	// stdin is READ //0
	stdout = open("/dev/tty0", O_WRONLY);					 //1
	stderr = open("/dev/tty0", O_WRONLY);	

	// printf() now possible
	printf("** tcruz init: fork login process **\n");

    printf("tcruz init fork login to serial port 0\n");
    s0 = fork();
    if (s0){
       	printf("tcruz init fork login to serial port 1\n");
       	s1 = fork();
  		if (s1){
       		printf("tcruz init fork login to qemu terminal\n");
	 		console = fork();
       	if (console)
          	parent();
       	else
	    	exec("login /dev/tty0");
      }
       else
         exec("login /dev/ttyS1");
    }
    else
      exec("login /dev/ttyS0");
}

int login()
{
	exec("login /dev/tty0");
}


int parent()
{
  int pid, status;
  printf("tcruz init: waiting for ZOMBIE children ...\n");
  while(1){
  	pid = wait(&status);

  	if(status == 99){
  		printf("peace out ");
  		if(pid == console){
  			printf("console\n");
  		}else if(pid == s0){
  			printf("serial 0\n");
  		}
  		else if(pid == s1){
  			printf("serial 1\n");
  		}

  		exit(1);
  	}
    
    if (pid==console){
		console = fork();
      	if (console)
	 		continue;
       else
         exec("login /dev/tty0");
    }
    else if (pid==s0){

    	s0 = fork();
       	if (s0)
	 		continue;
       	else
         	exec("login /dev/ttyS0");
    }
    else if (pid==s1){

    	s1 = fork();
       	if (s1)
	 		continue;
       	else
         	exec("login /dev/ttyS1");
    }

    printf("tcruz init: I just buried an orphan child task %d\n", pid);
  }
}