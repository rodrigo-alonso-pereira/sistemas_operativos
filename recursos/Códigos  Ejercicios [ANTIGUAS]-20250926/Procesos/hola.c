#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

main()
{
  int pid;
  if ( (pid = fork()) == -1) {
    printf("No se pudo crear hijo\n");
    exit(-1);
  }
  if (pid == 0) { // soy el hijo
    printf("Soy el hijo y mi pid = %d\n", getpid());
    printf("Hijo: el pid de mi padre es = %d\n", getppid());
    printf("Hijo: var pid: %d\n",pid);
    //exit(0);
    while(1);
  }
  else { // soy el padre
    printf("Soy el padre y mi pid = %d\n", getpid());
    printf("Padre: el pid de mi hijo es %d\n", pid);
    printf("Padre: el pid de mi padre es %d\n", getppid());
    printf("Padre: var pid: %d\n",pid);
    //exit(0);
    while(1);
  }
}
