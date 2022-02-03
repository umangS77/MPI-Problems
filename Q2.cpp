#include <iostream>
#include <mpi.h>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#define MAX 101
int main( int argc, char **argv ) 
{
    int rank, numprocs;

    // initiate MPI
    MPI_Init( &argc, &argv );

    // get size of the current communicator
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );

    // get current process rank
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double start_time = MPI_Wtime();

    // enter your code here

    int nodes, edges;
    // int indegree[MAX] = {0};
    int graph[MAX][MAX];
    int edgeWeight[MAX][MAX];

    if(rank == 0) // root node
    {
        ifstream inp(argv[1]);
        inp >> nodes >> edges;
        int cliqueCount = 0, u, v, w;

        for(int i=0;i<edges;i++)
        {
            inp>>u>>v>>w;
            graph[u][v] = graph[v][u] = 1;
            edgeWeight[u][v] = edgeWeight[v][u] = w;
            // indegree[u]++;
            // indegree[v]++;
        }

    }

    MPI_Bcast(&nodes, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&edges, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for(int i=1;i<(nodes+1);i++)
    {
        MPI_Bcast(&(graph[i]), nodes + 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&(edgeWeight[i]), nodes + 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    int temp_ar1[4] = {0};
    int output_ar1[4] = {0};
    int temp_ar2[7] = {0};
    int output_ar2[7] = {0};
    int remainder = (numprocs - rank == 1) ? (nodes%numprocs) : 0;
    int sizeSingleProcess = nodes / numprocs;

    // START for clique size = 3 :

    int i = (sizeSingleProcess*rank) + 1;
    int lim = (sizeSingleProcess*(rank+1)) + remainder;
    while(i <= lim)
    {
        for(int j=(i+1); j < (nodes+1); j++)
        {
            for(int k=(j+1); k < (nodes+1); k++)
            {
                if(graph[i][j] == 1 && graph[i][k] == 1 && graph[j][k] == 1)
                {
                    temp_ar1[edgeWeight[i][j] + edgeWeight[i][k] + edgeWeight[j][k]]++;
                }
            }
        }
        i++;
    }
    MPI_Reduce(&temp_ar1, &output_ar1, 4, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // END for clique size = 3

    // START for clique size = 4 :

    i = (sizeSingleProcess*rank) + 1;
    lim = (sizeSingleProcess*(rank+1)) + remainder;

    while(i<lim)
    {
        for(int j=(i+1); j<(nodes+1); j++)
        {
            for(int k=(j+1); k<(nodes+1); k++)
            {
                for (int l=(k+1); l<(nodes+1); l++)
                {
                    if (graph[i][j] && graph[i][k] && graph[j][k] && graph[i][l] && graph[k][l] && graph[j][l])
                    {
                        temp_ar2[edgeWeight[i][j] + edgeWeight[i][k] + edgeWeight[j][k] + edgeWeight[i][l] + edgeWeight[k][l] + edgeWeight[j][l]]++;
                    }
                }
            }
        }
        i++;
    }
    MPI_Reduce(&temp_ar2, &output_ar2, 7, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // END for clique size = 4

    // PRINT the result
    if (rank == 0)
    {
        ofstream outp(argv[2]);
        for(int i=0; i<7; i++)
        {
            outp<<"3 "<<i<<" "<<output_ar1[i]<<"\n";
        }

        for(int i=0; i<7; i++)
        {
            outp<<"4 "<<i<<" "<<output_ar2[i]<<"\n";
        }
    }

    MPI_Barrier( MPI_COMM_WORLD );
    double end_time = MPI_Wtime() - start_time;
    double maxTime;
    // get max program run time for all processes
    MPI_Reduce( &end_time, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        cout<<"Total time (s): "<<maxTime<<"\n";
    }

    // shut down MPI and close
    MPI_Finalize();
    return 0;

}