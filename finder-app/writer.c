#include <stdio.h>
#include <syslog.h>

int main(int argc, char *argv[]){

    if(argc != 3){
        syslog(LOG_ERR, "ERROR: Incorrect number of args\r\n");
        return(1);
    }

    openlog(NULL,0,LOG_USER);

    FILE *fp;

    fp = fopen(argv[1], "w");

    if (fp == NULL) {
        syslog(LOG_ERR, "Error: Could not create/open the file.\n");
        fclose(fp);
        return (1); 
    }

    syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
    
    fprintf(fp, "%s", argv[2]);

    fclose(fp);

    return(0);
}