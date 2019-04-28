#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 1000000  //number of points in one thread
#define THREAD_NUM 10 //total number of threads

pthread_t *tid;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
    int counter = 0;
    for(int i=0;i<SIZE;i++){
        //store that result in a global variable. 
        if(isInCircle(random_set[i]))
            counter++;  
    }
    //Protect against race conditions on updates to the shared global variable by using mutex locks
    pthread_mutex_lock(&mutex);
    numbers_of_points_in_circle += counter;
    pthread_mutex_unlock(&mutex);
}

void multithreadCreate(){
	tid = malloc(sizeof(pthread_t)*THREAD_NUM);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // creates threads
    for(int i=0;i<THREAD_NUM;i++){
        int ret = pthread_create(&tid[i],&attr,inCircleCount,NULL);
        if(ret != 0){
            printf("fail to create thread");
            exit(1);
        }
    }
    printf("%d threads have been created\n",THREAD_NUM);
    for(int i=0;i<THREAD_NUM;i++){
        int ret = pthread_join(tid[i],NULL);
        if(ret != 0){
            printf("fail to join thread");
            exit(1);
        }
    }
    pthread_mutex_destroy(&mutex);
    free(tid);
}

int main(int argc, char *argv[]){
 	multithreadCreate();
    //the parent thread will calculate and output the estimated value of pi
    printf("Each thread generates %d points\n",SIZE);
    printf("pi:%lf\n",4*(double)numbers_of_points_in_circle/(SIZE * THREAD_NUM));
    free(random_set);
    return 0;
}
