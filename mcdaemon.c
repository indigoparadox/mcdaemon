
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define FIFO_PATH "/minecraft/console"
#define MC_JAVA_MEM "1024M"
#define COMMAND_STOP "stop\n"

int gi_pipe_fifo_out;

void signal_handler( int i_signum_in ) {
   if( SIGTERM == i_signum_in ) {
      /* Send the daemon the stop command and wait for it to close. */
      write( gi_pipe_fifo_out, COMMAND_STOP, sizeof( COMMAND_STOP ) );
      wait( NULL );
      
      /* Cleanup */
      close( gi_pipe_fifo_out );

      _exit( 0 );
   }
}

int main( void ) {
   int i_pipe_fifo_in, /* The FIFO ID. */
      i_mc_pid; /* The PID of the forked child process. */
   char* mc_argv[] = {
      "/usr/bin/java",
      "-Xmx"MC_JAVA_MEM,
      "-Xms"MC_JAVA_MEM,
      "-jar",
      "/minecraft/craftbukkit.jar"
   };

/* Fork and start the MC daemon. */
   i_mc_pid = fork();
   if( 0 == i_mc_pid ) {
      /* This is the child process. */

      /* Open the console FIFO. */
      i_pipe_fifo_in = open( FIFO_PATH, O_RDONLY | O_NONBLOCK );
      if( -1 == i_pipe_fifo_in ) {
         fprintf( stderr, "Unable to open console pipe: " FIFO_PATH );
         return 1;
      }
      
      /* Close existing stdin/stdout and attach them to the parent's pipes. */
      close( 0 );
      dup2( i_pipe_fifo_in, 0 );

      /* Start the daemon. */
      execv( mc_argv[0], mc_argv );

   } else if( 0 < i_mc_pid ) {
      /* This is the parent process. */

      /* Open the console FIFO. */
      gi_pipe_fifo_out = open( FIFO_PATH, O_WRONLY | O_NONBLOCK );
      if( -1 == gi_pipe_fifo_out ) {
         //write( i_pipe_fifo, "Test", sizeof( "Test" ) );
         fprintf( stderr, "Unable to open console pipe: " FIFO_PATH );
      }

      /* Wait until we get SIGTERM or the child dies. */
      wait( NULL );
   }

   return 0;
}

