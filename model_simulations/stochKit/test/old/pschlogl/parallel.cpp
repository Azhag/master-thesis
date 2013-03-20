//******************************************************************************
//*  FILE:  parallel.cpp
//*
//*  AUTHOR: HongLi
//*
//*  CREATED: Mar 06, 2005
//*
//*  LAST MODIFIED:
//*             BY:
//*             TO:
//*  SUMMARY:
//*
//*
//*  NOTES:
//*
//*
//*
//*  TO DO:
//*
//*
//****************************************************************************|
//        1         2         3         4         5         6         7         8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890

#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "Random.h"
#include "SchloglStats.h"

/*
#define SIMPLE_SPRNG
#define USE_MPI
#include "sprng.h"
#define RFUNC sprng()
*/

#ifndef _NO_SPRNG
#define SIMPLE_SPRNG
#include "sprng.h"
#endif

#define WORKTAG 1
#define DIETAG 2

#define MAX_NODE_NUM 32

#define unit_result_t   double	
#define unit_of_work_t	int

//int DimerStat(int iterations, const char* outFile);

static void master(void);
static void slave(void);
static unit_of_work_t get_next_work_item(void);
static unit_result_t do_work(unit_of_work_t work);

//For the simulation
int TotalIte=10000; //How many iterations totally need
int NodeIte=500;  //How many iterations each node do once
char ModelName[50] = "SLG"; //for the result
int CalledTimes=0; //How many this salve has been called

double timingNodeWork[MAX_NODE_NUM];

//End


int myrank;

int main(int argc, char **argv)
{

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
  
    /* Find out my identity in the default communicator */
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    /*************** SPRNG Initialization *************************/
      //init_sprng(SPRNG_LCG, static_cast<int>(curTime) , SPRNG_DEFAULT);
      //init_sprng(SPRNG_LCG, 1938 , SPRNG_DEFAULT);
	// srand(1938);
	time_t curTime = time(0);
#ifndef _NO_SPRNG
      //init_sprng(SPRNG_LCG, static_cast<int>(curTime) , SPRNG_DEFAULT);
	CSE::Math::Seeder(static_cast<unsigned int>(curTime), curTime);
#else
	CSE::Math::Seeder(static_cast<unsigned int>(curTime)*myrank, curTime);
#endif    
    //srand(static_cast<unsigned int>(curTime)*myrank);
    //printf("Process %d, print information about stream:\n", myid);
    //print_sprng();

    if (myrank == 0)
        master();
    else
        slave();

    /* Shut down MPI */
    MPI_Finalize();
  
    return 0;
}


static void master(void)
{
    int ntasks, rank, realtasks;
    unit_of_work_t work;
    unit_of_work_t tmp_work;
    unit_result_t result;
    MPI_Status status;
    unit_result_t TotalWorkingTime = 0.0;


    fprintf(stderr, "Doing the simulations, please wait ...\n");
  
    /* Find out how many processes there are in the default
       communicator */
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    /* Seed the slaves; send one unit of work to each slave. */
    realtasks = ntasks;
    for (rank = 1; rank < ntasks; ++rank) {
        
        timingNodeWork[rank] = 0.0;

        /* Find the next item of work to do */
        work = get_next_work_item();
        tmp_work = work; 
        if(work == 0)
        {
           realtasks = rank;
           break;
        }

        /* Send it to each rank */
        MPI_Send(&tmp_work,             /* message buffer */
             1,                 /* one data item */
             MPI_INT,           /* data item is an integer */
             rank,              /* destination process rank */
             WORKTAG,           /* user chosen message tag */
             MPI_COMM_WORLD);   /* default communicator */
    }
    /* Loop over getting new work requests until there is no more work
       to be done */
    work = get_next_work_item();
  
    while (work != 0) {

        /* Receive results from a slave */
        MPI_Recv(&result,           /* message buffer */
             1,                 /* one data item */
             MPI_DOUBLE,        /* of type double real */
             MPI_ANY_SOURCE,    /* receive from any sender */
             MPI_ANY_TAG,       /* any type of message */
             MPI_COMM_WORLD,    /* default communicator */
             &status);          /* info about the received message */
   
        /*Get the total working time for each node*/ 
        timingNodeWork[status.MPI_SOURCE] += result;

        /* Send the slave a new work unit */
        tmp_work = work;
        MPI_Send(&tmp_work,             /* message buffer */
             1,                 /* one data item */
             MPI_INT,           /* data item is an integer */
             status.MPI_SOURCE, /* to who we just received from */
             WORKTAG,           /* user chosen message tag */
             MPI_COMM_WORLD);   /* default communicator */

        /* Get the next unit of work to be done */
         work = get_next_work_item();
    }

    /* There's no more work to be done, so receive all the outstanding
       results from the slaves. */
    for (rank = 1; rank < realtasks; ++rank) {
        MPI_Recv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
             MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        timingNodeWork[status.MPI_SOURCE] += result;
    }

    /* Tell all the slaves to exit by sending an empty message with the
       DIETAG. */
    for (rank = 1; rank < ntasks; ++rank) {
        MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
        if(timingNodeWork[rank] !=0) 
            fprintf(stderr, "Node %d working time: %f\n", rank, timingNodeWork[rank]);
        TotalWorkingTime += timingNodeWork[rank];
    }
  
    fprintf(stderr, "\nTotal working time: %f \n\n", TotalWorkingTime);

    char command[40]="";
    char DoCat[40]="";
   /* 
    fprintf(stderr, "\nPutting all the results in one file may cost lots of time,\n");
    fprintf(stderr, "Do you need put all results in one file, y/n? \n");
    scanf("%s", DoCat);
    fprintf(stderr, "This will take a little while, Please wait ... \n");
    if((strcmp(DoCat,"y")==0)||(strcmp(DoCat,"Y")==0)
             ||(strcmp(DoCat,"yes")==0)||(strcmp(DoCat,"YES")==0)){
        strcpy(command, "rm ./result/result.txt");
        system(command);
        strcpy(command, "cat ./result/ > ./result/result.txt");
        system(command);
        fprintf(stderr, "The results are all in in the ./result/result.txt\n\n"); 
     }
    */    
    fprintf(stderr, "Finalizing, please wait ...\n\n");
    
    for(rank = realtasks; rank < ntasks; rank++){
       sprintf(command, "rm -rf ./result/%d%s", rank, ModelName);
       system(command);
    }
}


static void slave(void)
{
    unit_of_work_t work;
    MPI_Status status;
    unit_result_t result= 0.0;
    unit_result_t tmp_result= 0.0;
    
    char command[40]="";
    sprintf(command, "rm -rf ./result/%d%s", myrank, ModelName);
    system(command);

    sprintf(command, "mkdir ./result/%d%s", myrank, ModelName);
    system(command);

    while (1) {

        /* Receive a message from the master */
        MPI_Recv(&work, 1, MPI_INT, 0, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);

        /* Check the tag of the received message. */
        if (status.MPI_TAG == DIETAG) {
          return;
        }

        /* Do the work */
        CalledTimes++;  //The time to be called ++
        result = do_work(work);
        tmp_result = result;
    
        /* Send the result back */
        MPI_Send(&tmp_result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
}

static unit_of_work_t get_next_work_item(void)
{
   int lastIte;
 
   if((TotalIte-NodeIte)>=0){
       TotalIte = TotalIte - NodeIte;
       return NodeIte;
   }else
   {
     lastIte = TotalIte; 
     TotalIte = 0;
     return lastIte;
   }
}

static unit_result_t do_work(unit_of_work_t work)
{
    double Wtimebegin=0, Wtimeend = 0;
    struct timeval thetime;
    if (gettimeofday(&thetime, NULL) == 0)
        Wtimebegin = thetime.tv_sec + thetime.tv_usec * 0.000001;
    else
       fprintf(stderr, "Get Time Error!");
 
    //char command[1000]="";   
    char workdir[500]=""; 

    /*
    //For StochKit
    sprintf(command, "%s/%s %d  ./result/%d%s/%d.txt", 
                DirName, MethodName, work,  
                myrank, ModelName, CalledTimes);
    */
    /*
    sprintf(command, "../%s/%s %d ../%s/%s ./result/%d%s/%d.txt", 
                DirName, MethodName, work, DirName, 
                InputFileName, myrank, DirName, CalledTimes);
    */
    
    //system(command);
    
    sprintf(workdir, "./result/%d%s/%d.txt", myrank, ModelName, CalledTimes);
    SchloglStats(work, workdir); 
    
    if (gettimeofday(&thetime, NULL) == 0)
       Wtimeend = thetime.tv_sec + thetime.tv_usec * 0.000001;
    else
       fprintf(stderr, "Get Time Error!");
    
    return Wtimeend - Wtimebegin;
}
