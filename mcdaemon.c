
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_PATH "/home/srv/minecraft/console"
#define MC_JAVA_MEM "1024"

int main( void ) {
   int i_pipe_fifo, /* The FIFO ID. */
      i_pipe_mc_stdin[2], /* The child daemon's stdin. */
      i_pipe_mc_stdout[2], /* The child daemon's stdout. */
      i_mc_pid; /* The child daemon's PID. */
   char* mc_argv[] = {
      "/usr/bin/java",
      "-Xmx"MC_JAVA_MEM,
      "-Xms"MC_JAVA_MEM,
      "-jar ./craftbukkit.jar"
   };

   /* Create the pipes to talk to the daemon. */
   pipe( i_pipe_mc_stdin );
   pipe( i_pipe_mc_stdout );
   
   /* Fork and start the MC daemon. */
   i_mc_pid = fork();
   if( 0 == i_mc_pid ) {
      /* This is the child process. */

      /* Close existing stdin/stdout and attach them to the parent's pipes. */
      close( 0 );
      close( i_pipe_mc_stdin[1] );
      dup2( i_pipe_mc_stdin[0], 0 );
      close( 1 );
      close( i_pipe_mc_stdout[1] );
      dup2( i_pipe_mc_stdout[1], 1 );

      /* Start the daemon. */
      execv( mc_argv[0], mc_argv );

   } else if( 0 < i_mc_pid ) {
      /* This is the parent process. */

      /* Open the console FIFO. */
      i_pipe_fifo = open( FIFO_PATH, O_RDONLY | O_NONBLOCK );
      if( -1 == i_pipe_fifo ) {
         //write( i_pipe_fifo, "Test", sizeof( "Test" ) );
         fprintf( stderr, "Unable to open console pipe: " FIFO_PATH );
      }

      /* Cleanup */
      close( i_pipe_fifo );
   }

   return 0;
}

