#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 10000  //total number of points

struct point
{
    double x;
    double y;
}*random_set;

int numbers_of_points_in_circle = 0;

void generatePoints(){
    random_set = malloc(sizeof(struct point)*SIZE);
    srand((unsigned)time(NULL)); 
    for(int i=0;i<SIZE;i++){
        //x and y are in range[-1,1]
        random_set[i].x = rand()/(double)(RAND_MAX/2)-1;
        random_set[i].y = rand()/(double)(RAND_MAX/2)-1;
    }
}

bool isInCircle(struct point p){
    bool result = true;
    //point(x,y),if x^2+y^2>1, the point is not in the Unit circle
    if((p.x*p.x+p.y*p.y)>1)
        result = false;
    return result;
}

void *inCircleCount(){
    generatePoints();
    for(int i=0;i<SIZE;i++){
        //store that result in a global variable. 
        if(isInCircle(random_set[i]))
            numbers_of_points_in_circle++;
    }
}

int main(int argc, char *argv[]){
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // creates a separate thread 
    int ret = pthread_create(&tid,&attr,inCircleCount,NULL);
    if(ret != 0){
        printf("fail to create thread");
        exit(1);
    }
    //wait for separate thread exit.
    pthread_join(tid,NULL);
    //the parent thread will calculate and output the estimated value of pi
    printf("SIZE = %d\n",SIZE);
    printf("pi:%lf\n",4*(double)numbers_of_points_in_circle/SIZE);
    free(random_set);
    return 0;
}
