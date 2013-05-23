 /* mmm, given a lisp file create a situable array of pointers of char so
  * could be load at init. 
  */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *fp;
    char buf[1024];
    int len;

    if(argc < 2) {
	    printf("Usage: %s file\n", argv[0]);
	    return 0;
    }
    
    fp=fopen(argv[1],"r");

    if(fp==NULL) {
	    printf("Error opening %s",argv[1]);
	    return 1;
    }
    printf("char *lisp_functions[] = {\n");

    while(fgets(buf,1024,fp) != NULL) {
	    len=strlen(buf);
	    if(buf[len-1] == '\n') 
		    buf[len-1] = '\0';
	    printf("\"%s\\n\",\n",buf);
    }
    
    printf("\"\\n\"\n};\n");   
    return 0;
}


    
    
