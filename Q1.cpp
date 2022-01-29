#include <iostream>
#include <mpi.h>
#include <math.h>
#include <fstream>
#include <string>
using namespace std;



bool checkPrime(int lowerLimit, int upperLimit, int num)
{
    if(num == 2)
        return 0;

    if(num<2 || num%2==0)
        return 1;

    for(int i=lowerLimit;i<=upperLimit;i+=2)
    {
        if(num%i==0)
            return 1;
    }

    return 0;
}

int main( int argc, char **argv ) {
    ifstream inputFile;

    inputFile.open("input.txt");
    string line;
    getline(inputFile, line);
    inputFile.close();


    int rank, numprocs;

    // initiate MPI
    MPI_Init( &argc, &argv );

    // get size of the current communicator
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );

    // get current process rank
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    // double start_time = MPI_Wtime();
    // int primelist [] = {3,5,7,11,13,17,19,}
    int root_rank = 0;
    int n = stoi(line);
    int sqRoot = sqrt(n);
    int lim = numprocs; // 10 = 11 - 1
    int val = sqRoot/lim; // number of values for 1 thread
    int answer = 0;
    
    if(val == 0)
    {
        // int reduction_result = 0;

        // int lowerLimit = rank+2;
        // int upperLimit = rank+2;
        // bool f=0;
        // if(lowerLimit >= n)
        // {
        //     f = checkPrime(lowerLimit, upperLimit, n);
        // }
        // MPI_Reduce(&f, &reduction_result, 1, MPI_INT, MPI_LOR, root_rank, MPI_COMM_WORLD);
        


        ofstream outputFile;
        outputFile.open("output.txt");
        if(n==2 || n==3 || n==5 || n==7 || n==11)
        {
            outputFile<<"YES";
        }
        else if(n==1 || n%2==0 || n%3==0 || n%5==0 || n%7==0 || n%11 == 0)
        {
            outputFile<<"NO";
        }
        else
            outputFile<<"YES";

        outputFile.close();

        // if(rank == root_rank)
        // {
            
        //     // printf("result = %d\n", reduction_result);
        //     if(reduction_result > 1)
        //         outputFile<<"NO";
        //     else
        //         outputFile<<"YES";

        //     outputFile.close();
        // }

        // }
    }

    else
    {
        int reduction_result = 0;

        int lowerLimit = val*rank;
        if(rank == 0)
            lowerLimit = 3;
        int upperLimit = lowerLimit + val-1;
        if(rank == lim-1 || upperLimit > sqRoot)
            upperLimit = sqRoot;

        bool f=0;
        if(lowerLimit <= upperLimit)
        f = checkPrime(lowerLimit, upperLimit, n);

        // printf("rank = %d ------  LL = %d,  UL = %d,   f = %d\n", rank, lowerLimit, upperLimit, f);

        MPI_Reduce(&f, &reduction_result, 1, MPI_INT, MPI_LOR, root_rank, MPI_COMM_WORLD);


        if(rank == root_rank)
        {
            ofstream outputFile;
            outputFile.open("output.txt");
            // printf("result = %d\n", reduction_result);
            if(reduction_result > 0)
                outputFile<<"NO";
            else
                outputFile<<"YES";

            outputFile.close();
        }
    }

    




    
    





        

    



    

    // enter your code here

    MPI_Barrier( MPI_COMM_WORLD );
    // double end_time = MPI_Wtime() - start_time;
    // double maxTime;
    // // get max program run time for all processes
    // MPI_Reduce( &end_time, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    // if ( rank == 0 ) {
    //     cout<<"Total time (s): "<<maxTime<<"\n";
    // }

    // shut down MPI and close
    MPI_Finalize();
    return 0;
}