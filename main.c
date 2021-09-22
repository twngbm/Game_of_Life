#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#define ON "■"
#define OFF " "
//#define OFF "□"

int row;
int col;
int tick=0;

int getSurrund(int ** grid,int x,int y){
    int ans=0;
    for(int i=x-1;i<=x+1;i++){
        for(int j=y-1;j<=y+1;j++){
            if((i==x)&&(j==y)){
                continue;
            }
            if(grid[i][j]){
                ans++;
            }
        }
    }
    return ans;
}


void drawScreen(int ***buffer){
    printf("\e[1;1H\e[2J");//clear screen
    for(int i=1;i<row;i++){
        for(int j=1;j<col;j++){
            if(buffer[tick][i][j]){
                printf(ON);
            }
            else{
                printf(OFF);
            }
        }
        printf("\n");
    }

}

void updateCell(int ***buffer){
    int tock=(tick+1)%2;
    int cv;
    int nv;
    int sur;
    for(int i=1;i<row;i++){
        for(int j=1;j<col;j++){
           cv=buffer[tick][i][j];
           sur=getSurrund(buffer[tick],i,j);
           if(cv==1){
               if(sur<2){
                   nv=0;
               }
               else if(sur>3){
                   nv=0;
               }
               else{
                   nv=1;
               }
           } 
           else{
               if(sur==3){
                   nv=1;
               }
               else{
                   nv=0;
               }
           }
           buffer[tock][i][j]=nv;
        }
    }


}



int main(int argc,char *argv[]){
    struct winsize w;
    
    ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);
    
    row=w.ws_row;
    col=w.ws_col;
    //initinal two buffer for game of life
    int ***buffer=(int ***)malloc(sizeof(int **)*2);
    buffer[0]=(int **)malloc(sizeof(int*)*(row+2));
    for(int i=0;i<=row+1;i++){
        buffer[0][i]=(int*)calloc(col+2,sizeof(int));
    }
    buffer[1]=(int **)malloc(sizeof(int*)*(row+2));
    for(int i=0;i<=row+1;i++){
        buffer[1][i]=(int*)calloc(col+2,sizeof(int));
    }

    //Initinal Seed
    char seedName[100];
    if(argc==1){
        printf("Input A Seed Name or \"random\" for random seed.(EX:a.out seed1.txt)\n");
        printf("Using random seed.");
        for(int i=1;i<row;i++){
            for(int j=1;j<col;j++){
                buffer[0][i][j]=rand()%2;
            }
        }
    }
    else if(strcmp(argv[1],"random")==0){
        for(int i=1;i<row;i++){
            for(int j=1;j<col;j++){
                buffer[0][i][j]=rand()%2;
            }
        }
    }
    else{
        FILE *input=fopen(argv[1],"r");
        if(!input){
            printf("No Such File. Abord.\n");
            return 0;
        }
        char *line=NULL;
        size_t len=0;
        for(int i=1;i<row;i++){
            int x=getline(&line, &len,input);
            char *c=strtok(line,",");
            for(int j=1;j<col;j++){
                buffer[0][i][j]=c[0]-'0';
                c=strtok(NULL,",");
            }
        }
        fclose(input);
    }
    printf("\e[?25l");//hide cursor
    drawScreen(buffer);
    while(1){
        usleep(100000);
        updateCell(buffer);
        tick++;
        tick%=2;
        drawScreen(buffer);
    }
    
    //free(buffer);
    
    return 0;
}
