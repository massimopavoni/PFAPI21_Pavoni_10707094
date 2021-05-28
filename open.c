#include <stdio.h>
#include <string.h>

// Function declaration
void add_graph();
void get_best_graphs();

// Statis constants for string commands
static const char *ADD_GRAPH_COMMAND = "AggiungiGrafo";
static const char *GET_BEST_GRAPHS_COMMAND = "TopK";

int main()
{
  // Variable declaration
  //  - number of graph nodes: int
  //  - number of best graphs to track: int
  //  - string command: char*
  int nodes, best_graphs;
  char *command;

  // Read graph rank info
  scanf("%d, %d", &nodes, &best_graphs);

  // Main cycle
  do
  {
    // Read command
    scanf("%s", &command);

    // Parse command
    if (strcmp(command, ADD_GRAPH_COMMAND))
    {
    }
    else if (strcmp(command, GET_BEST_GRAPHS_COMMAND))
    {
    }
  } while (strcmp(command, "") != 0); // If EOF, exit cycle

  // The end!
  return 0;
}