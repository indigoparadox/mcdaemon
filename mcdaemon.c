
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_PATH "/minecraft/console"
#define MC_JAVA_MEM "1024M"

int main( void ) {
   int i_pipe_fifo; /* The FIFO ID. */
   char* mc_argv[] = {
      "/usr/bin/java",
      "-Xmx"MC_JAVA_MEM,
      "-Xms"MC_JAVA_MEM,
      "-jar",
      "/minecraft/craftbukkit.jar"
   };

   /* Open the console FIFO. */
   i_pipe_fifo = open( FIFO_PATH, O_RDONLY | O_NONBLOCK );
   if( -1 == i_pipe_fifo ) {
      //write( i_pipe_fifo, "Test", sizeof( "Test" ) );
      fprintf( stderr, "Unable to open console pipe: " FIFO_PATH );
      return 1;
   }
   
   /* Close existing stdin/stdout and attach them to the parent's pipes. */
   close( 0 );
   dup2( i_pipe_fifo, 0 );

   /* Start the daemon. */
   execv( mc_argv[0], mc_argv );

   return 0;
}

