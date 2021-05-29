#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function declaration
//  - add graph to read edges info and get sum of shortest paths: unsigned int * (number of vertices variable pointer: unsigned int*)
unsigned int *add_graph(unsigned int *vertices);
void get_best_graphs();

// Static constants
//  - max command string size: unsigned char
//  - add graph command string: char*
//  - get best graphs command string: char*
static const unsigned char MAX_COMMAND_SIZE = 13;
static const char *ADD_GRAPH_COMMAND = "AggiungiGrafo";
static const char *GET_BEST_GRAPHS_COMMAND = "TopK";

// Main
int main()
{
  // -------- PROFILING --------
  clock_t start = clock();
  // -------- PROFILING --------

  // Variable declaration
  //  - number of graph vertices: unsigned int
  //  - number of best graphs to track: unsigned int
  //  - string command: char*
  unsigned int vertices, best_graphs;
  char *command = malloc(MAX_COMMAND_SIZE * sizeof(char));

  // Read graph rank info
  scanf("%u %u", &vertices, &best_graphs);

  // Main loop
  while (scanf("%s", command) == 1) // Read command; if EOF, exit loop
  {
    // Parse command
    if (strcmp(command, ADD_GRAPH_COMMAND) == 0)
    {
      add_graph(&vertices);
    }
    else if (strcmp(command, GET_BEST_GRAPHS_COMMAND) == 0)
    {
    }
  }

  // -------- PROFILING --------
  clock_t end = clock();
  double elapsed_time = (end - start) / (double)CLOCKS_PER_SEC;
  fprintf(stderr, "%lf", elapsed_time);
  // -------- PROFILING --------

  // The end!
  return 0;
}

// add_graph implementation
unsigned int *add_graph(unsigned int *vertices)
{
  // Allocate adjacency matrix as a single array, usage: *(adjacency_matrix + row * *vertices + column)
  unsigned int *adjacency_matrix = (unsigned int *)malloc(*vertices * *vertices * sizeof(unsigned int));

  // Input reading loop
  for (unsigned int i = 0; i < *vertices; i++)
  {
    for (unsigned int j = 0; j < *vertices - 1; j++)
      scanf("%u,", (adjacency_matrix + i * *vertices + j));
    scanf("%u", (adjacency_matrix + i * *vertices + *vertices - 1));
  }

  // Graph fitness calculated by summation of all shortest paths
  unsigned int *graph_fitness = 0;

  // Free adjacency matrix memory
  free(adjacency_matrix);

  return graph_fitness;
}