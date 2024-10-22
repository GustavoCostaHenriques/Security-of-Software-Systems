
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
/*#include <pwd.h> */  /* Using custom made pwd() */
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "include-map.h"

#ifdef MAPPING_CHDIR
/* Keep track of the path the user has chdir'd into and respond with
 * that to pwd commands.  This is to avoid having the absolue disk
 * path returned, which I want to avoid.
 */
char mapped_path[ MAXPATHLEN ] = "/";

char *
#ifdef __STDC__
map_dir_getwd(char *path)
#else
     map_dir_getwd( path )
     char *path;
#endif
{
  strcpy( path, mapped_path );    /* copies mapped_path to path without doing a size check */
  return path;
}

/* Make these globals rather than local to map_dir_chdir to avoid stack overflow */
char pathspace[ MAXPATHLEN ];                     /* This buffer can get overflowed too */
char old_mapped_path[ MAXPATHLEN ];

void
#ifdef __STDC__
/* appends /dir to mapped_path if mapped_path != /, else appends simply dir */
do_elem(char *dir)            
#else
     do_elem( dir )
     char *dir;
#endif
{
  /* . */
  if( dir[0] == '.' && dir[1] == '\0' ){
	/* ignore it */
	return;
      }
      
      /* .. */
      if( dir[0] == '.' && dir[1] == '.' && dir[2] == '\0' ){
              char *last;
              /* lop the last directory off the path */
              if (( last = strrchr( mapped_path, '/'))){
		/* If start of pathname leave the / */
		if( last == mapped_path )
		  last++;
		*last = '\0';
              }
              return;
      }
      
      /* append the dir part with a / unless at root */
      if( !(mapped_path[0] == '/' && mapped_path[1] == '\0') )
	strcat( mapped_path, "/" ); 
      strcat( mapped_path, dir ); 
}

int
#ifdef __STDC__
map_dir_chdir(char *orig_path)
#else
map_dir_chdir( orig_path )
      char *orig_path;
#endif
{
      int ret;
      char *sl, *path;

      strcpy( old_mapped_path, mapped_path );  /* old_mapped_path is initially / */
      path = &pathspace[0];
      
      strcpy( path, orig_path );  
	
	/* set the start of the mapped_path to / */
	if( path[0] == '/' ){
	  mapped_path[0] = '/';
	  mapped_path[1] = '\0';
	  path++;
	}
	
      while( (sl = strchr( path, '/' )) ){
              char *dir;
              dir = path;
              *sl = '\0';
              path = sl + 1;
              if( *dir )
		do_elem( dir );    /* appends directory names to mapped_path */
              if( *path == '\0' )
		break;
      }
      if( *path )
	{
	  do_elem( path );       
	}

      
      if( (ret = chdir( mapped_path )) < 0 ){        /* change to the specified path */
	printf("couldn't chdir to %s !\n", mapped_path);
	strcpy( mapped_path, old_mapped_path );   /* change mapped_path back to original, i.e root */
        printf("mapped_path changed to %s\n", mapped_path);
      }

      return ret;
}


/* From now on use the mapping version */

#define getwd(d) map_dir_getwd(d)
#define getcwd(d,u) map_dir_getwd(d)  

#endif /* MAPPING_CHDIR */



/* Define pwd */

void
#ifdef __STDC__
pwd(void)
#else
pwd()
#endif
{
  char path[MAXPATHLEN + 1];      /* Path to return to client */
  
#ifndef MAPPING_CHDIR
#ifdef HAVE_GETCWD
  extern char *getcwd();
#else
#ifdef __STDC__
  extern char *getwd(char *);
#else
  extern char *getwd();
#endif
#endif
#endif /* MAPPING_CHDIR */
  
#ifdef HAVE_GETCWD
  if (getcwd(path,MAXPATHLEN) == (char *) NULL)   
#else
    if (getwd(path) == (char *) NULL)            
#endif
      {
	printf("Couldn't get current directory!\n");
      }
    else{
      printf("Current directory = %s\n", path);
    }
}


int main(int argc, char **argv){

  char orig_path[MAXPATHLEN + 20];
  FILE *f;

  assert (argc == 2);
  f = fopen(argv[1], "r");
  assert(f != NULL);

  fgets(orig_path, MAXPATHLEN + 20, f);  /* get path name */
  fclose(f);

  map_dir_chdir(orig_path);  
  pwd(); 

  return 0;
}


