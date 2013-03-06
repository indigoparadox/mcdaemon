
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define FIFO_PATH "/minecraft/console"
#define MC_JAVA_MEM "1024M"
#define COMMAND_STOP "stop\n"

int gi_pipe_mc_stdin[2];

void signal_handler( int i_signum_in ) {
   if( SIGTERM == i_signum_in ) {
      /* Send the daemon the stop command and wait for it to close. */
      write( gi_pipe_mc_stdin[1], COMMAND_STOP, sizeof( COMMAND_STOP ) );
      wait( NULL );
      
      /* Cleanup */
      close( gi_pipe_mc_stdin[1] );

      _exit( 0 );
   }
}

int main( void ) {
   int i_mc_pid; /* The PID of the forked child process. */
   char* mc_argv[] = {
      "/usr/bin/java",
      "-Xmx"MC_JAVA_MEM,
      "-Xms"MC_JAVA_MEM,
      "-jar",
      "/minecraft/craftbukkit.jar"
   };

   /* Setup a pipe to send commands to the child. */
   if( pipe( gi_pipe_mc_stdin ) ) {
      fprintf( stderr, "Unable to open communication pipe." );
      _exit( 1 );
   }

   signal( SIGTERM, signal_handler );

   /* Fork and start the MC daemon. */
   i_mc_pid = fork();
   if( 0 == i_mc_pid ) {
      /* This is the child process. */

      /* Close existing stdin/stdout and attach them to the parent's pipes. */
      close( 0 );
      dup2( gi_pipe_mc_stdin[0], 0 );

      /* Start the daemon. */
      execv( mc_argv[0], mc_argv );

   } else if( 0 < i_mc_pid ) {
      /* This is the parent process. */

      close( gi_pipe_mc_stdin[0] );

      /* Wait until we get SIGTERM or the child dies. */
      wait( NULL );
   }

   return 0;
}

