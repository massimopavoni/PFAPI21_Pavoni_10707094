#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --------------------------------------------------------------------------- Structs ---------------------------------------------------------------------------

// IDTuple struct
typedef struct
{
  // Index: unsigned int
  unsigned int index;

  // Distance: unsigned int
  unsigned int distance;

} IDTuple;

// MinHeap struct
typedef struct
{
  // Maximum size of the heap: unsigned int
  unsigned int size;

  // Current size of the heap: unsigned int
  unsigned int heapSize;

  // Positions of vertices in the heap: unsigned int *
  unsigned int *verticesPositions;

  // Keys array: IDTuple *
  IDTuple *keys;

} MinHeap;

// MaxHeap struct
typedef struct
{
  // Maximum size of the heap: unsigned int
  unsigned int size;

  // Current size of the heap: unsigned int
  unsigned int heapSize;

  // Keys array: IDTuple *
  IDTuple *keys;

} MaxHeap;

// --------------------------------------------------------------------------- Structs ---------------------------------------------------------------------------

// -------------------------------------------------------------------- Functions declaration --------------------------------------------------------------------

// Get graph fitness after reading edges info.
//
// Return type: unsigned int
// Arguments
// - number of vertices variable pointer: unsigned int *
// - allocated adjacency matrix pointer: unsigned int *
// - allocated priority queue Dijkstra min-heap struct pointer: MinHeap *
// - allocated min-heap min element struct pointer: IDTuple *
// - allocated vertices' distances array pointer: unsigned int *
// - allocated processed vertices array pointer: bool *
unsigned int GetGraphFitness(unsigned int *vertices, unsigned int *adjacencyMatrix, MinHeap *dijkstraMinHeap,
                             IDTuple *minIDTuple, unsigned int *distances, bool *processed);

// Dijkstra's algorithm to calculate shortest paths from source to all other vertices and get the total sum.
//
// Return type: unsigned int
// Arguments
// - number of vertices variable pointer: unsigned int *
// - adjacency matrix pointer: unsigned int *
// - Dijkstra min-heap struct pointer: MinHeap *
// - min-heap min element struct pointer: IDTuple *
// - vertices' distances array pointer: unsigned int *
// - processed vertices array pointer: unsigned int *
// - source vertex: unsigned int
unsigned int DijkstraSum(unsigned int *vertices, unsigned int *adjacencyMatrix, MinHeap *dijkstraMinHeap,
                         IDTuple *minIDTuple, unsigned int *distances, bool *processed, unsigned int source);

// Min-heap insert function for new vertex.
//
// Return type: void
// Arguments
// - Dijkstra min-heap struct pointer: MinHeap *
// - vertex to insert: unsigned int
// - distance to insert: unsigned int
void MinHeapInsertVertex(MinHeap *dijkstraMinHeap, unsigned int vertex, unsigned int distance);

// Min-heap decrease distance function for decreasing distances (maintaining properties).
//
// Return type: void
// Arguments
// - Dijkstra min-heap struct pointer: MinHeap *
// - vertex of distance to decrease: unsigned int
// - new distance value: unsigned int
void MinHeapDecreaseDistance(MinHeap *dijkstraMinHeap, unsigned int vertex, unsigned int distance);

// Min-heap extract min function to get the minimum distance (and its vertex) inside the array.
//
// Return type: void
// Arguments
// - Dijkstra min-heap struct pointer: MinHeap *
// - min-heap min element struct pointer: IDTuple *
void MinHeapPopMin(MinHeap *dijkstraMinHeap, IDTuple *minIDTuple);

// Min-heap heapify top down function that fixes the min-heap to make sure that it still respects the properties.
//
// Return type: void
// Arguments
// - Dijkstra min-heap struct pointer: MinHeap *
// - index from which the function starts: unsigned int
void MinHeapifyTopDown(MinHeap *dijkstraMinHeap, unsigned int index);

// Max heap insert function to check if graph goes in the ranking, realloc and pop worst graph if needed.
//
// Return type: void
// Arguments
// - best graphs max-heap struct pointer: MaxHeap *
// - number of best graphs to track: unsigned int *
// - graph index since the start of the program: unsigned int
// - graph fitness value obtained through shortest paths sum: unsigned int
void MaxHeapInsertGraph(MaxHeap *bestGraphsMaxHeap, unsigned int *bestGraphs, unsigned int index, unsigned int fitness);

// Max-heap extract max function to delete the worst graph inside the array.
//
// Return type: void
// Arguments
// - best graphs max-heap struct pointer: MaxHeap *
void MaxHeapPopMax(MaxHeap *bestGraphsMaxHeap);

// Max-heap heapify top down function that fixes the max-heap to make sure that it still respects the properties.
//
// Return type: void
// Arguments
// - best graphs max-heap struct pointer: MaxHeap *
// - index from which the function starts: unsigned int
void MaxHeapifyTopDown(MaxHeap *bestGraphsMaxHeap, unsigned int index);

// Max-heap heapify bottom up down function that fixes the max-heap to make sure that it still respects the properties.
//
// Return type: void
// Arguments
// - best graphs max-heap struct pointer: MaxHeap *
// - index from which the function starts: unsigned int
void MaxHeapifyBottomUp(MaxHeap *bestGraphsMaxHeap, unsigned int index);

// Max heap print function to get the best graphs.
//
// Return type: void
// Arguments
// - best graphs max-heap struct pointer: MaxHeap *
void MaxHeapPrint(MaxHeap *bestGraphsMaxHeap);

// -------------------------------------------------------------------- Functions declaration --------------------------------------------------------------------

// -------------------------------------------------------------------------- Constants --------------------------------------------------------------------------

// Max command string size: unsigned char
static const unsigned char MAX_COMMAND_SIZE = 13;

// Add graph command string: char*
static const char *ADD_GRAPH_COMMAND = "AggiungiGrafo";

// Get best graphs command string: char*
static const char *GET_BEST_GRAPHS_COMMAND = "TopK";

// Percentage of best graphs for ranking dynamic reallocation
static const float REALLOC_PERCENTAGE = 0.1;

// -------------------------------------------------------------------------- Constants --------------------------------------------------------------------------

// ----------------------------------------------------------------------------- Main ----------------------------------------------------------------------------

int main()
{
  // -------- PROFILING --------

  clock_t start = clock();

  // -------- PROFILING --------

  // Number of graph vertices: unsigned int
  unsigned int vertices;

  // Number of best graphs to track: unsigned int
  unsigned int bestGraphs;

  // Current graph index: unsigned int
  unsigned int graphIndex = UINT_MAX;

  // Read graph rank info
  if (!scanf("%u %u", &vertices, &bestGraphs))
  {
    fprintf(stderr, "input error: scanf vertices bestGraphs");
    exit(1);
  }

  // -------------------------------- Memory Allocation --------------------------------

  // Adjacency matrix usage is *(adjacencyMatrix + row * *vertices + column)

  // Adjacency matrix for edge weight: unsigned int *
  unsigned int *adjacencyMatrix = malloc(vertices * vertices * sizeof *adjacencyMatrix);

  // Priority queue min-heap for Dijkstra: MinHeap *
  MinHeap *dijkstraMinHeap = malloc(sizeof *dijkstraMinHeap);
  dijkstraMinHeap->size = vertices;
  dijkstraMinHeap->verticesPositions = malloc(vertices * sizeof dijkstraMinHeap->verticesPositions);
  dijkstraMinHeap->keys = malloc(vertices * sizeof dijkstraMinHeap->keys);

  // Min-heap min element for Dijkstra: IDTuple *
  IDTuple *minIDTuple = malloc(sizeof *minIDTuple);

  // Vertices' distances array: unsigned int *
  unsigned int *distances = malloc(vertices * sizeof *distances);

  // Processed vertices array: unsigned int *
  bool *processed = malloc(vertices * sizeof *processed);

  // Ranking max-heap for TopK: MaxHeap *
  MaxHeap *bestGraphsMaxHeap = malloc(sizeof *bestGraphsMaxHeap);
  bestGraphsMaxHeap->size = (unsigned int)(bestGraphs * REALLOC_PERCENTAGE);
  bestGraphsMaxHeap->heapSize = 0;
  bestGraphsMaxHeap->keys = malloc(bestGraphsMaxHeap->size * sizeof bestGraphsMaxHeap->keys);

  // String command: char*
  char *command = malloc(MAX_COMMAND_SIZE * sizeof *command);

  // -------------------------------- Memory Allocation --------------------------------

  // Main loop
  while (1)
  {
    // Read command
    if (!scanf("%s", command))
    {
      fprintf(stderr, "input error: scanf command");
      exit(1);
    }

    // After failed read, if eof exit the loop
    if (feof(stdin))
      break;

    // Parse command
    if (!strcmp(command, ADD_GRAPH_COMMAND))
    {
      // Increment graph index and insert graph in list (if it should be there)
      graphIndex++;
      MaxHeapInsertGraph(bestGraphsMaxHeap, &bestGraphs, graphIndex,
                         GetGraphFitness(&vertices, adjacencyMatrix, dijkstraMinHeap, minIDTuple, distances, processed));
    }
    else if (!strcmp(command, GET_BEST_GRAPHS_COMMAND))
      // Print best graphs
      MaxHeapPrint(bestGraphsMaxHeap);
  }

  // Free memory galore
  free(adjacencyMatrix);
  free(dijkstraMinHeap->keys);
  free(dijkstraMinHeap);
  free(distances);
  free(processed);
  free(bestGraphsMaxHeap->keys);
  free(bestGraphsMaxHeap);
  free(command);

  // -------- PROFILING --------

  clock_t end = clock();
  double elapsed_time = (end - start) / (double)CLOCKS_PER_SEC;
  fprintf(stderr, "\n%lf\n", elapsed_time);

  // -------- PROFILING --------

  // The end!
  return 0;
}

// ----------------------------------------------------------------------------- Main ----------------------------------------------------------------------------

// ------------------------------------------------------------------- Functions implementation ------------------------------------------------------------------

// GetGraphFitness implementation

unsigned int GetGraphFitness(unsigned int *vertices, unsigned int *adjacencyMatrix, MinHeap *dijkstraMinHeap,
                             IDTuple *minIDTuple, unsigned int *distances, bool *processed)
{
  // Input reading loop
  for (unsigned int i = 0; i < *vertices; i++)
  {
    for (unsigned int j = 0; j < *vertices - 1; j++)
    {
      if (!scanf("%u,", adjacencyMatrix + i * *vertices + j))
      {
        fprintf(stderr, "input error: scanf adjacencyMatrix");
        exit(1);
      }
    }
    if (!scanf("%u", adjacencyMatrix + i * *vertices + *vertices - 1))
    {
      fprintf(stderr, "input error: scanf adjacencyMatrix");
      exit(1);
    }
  }

  // Graph fitness calculated by summation of all shortest paths
  unsigned int graphFitness = DijkstraSum(vertices, adjacencyMatrix, dijkstraMinHeap, minIDTuple, distances, processed, 0);

  // Return graph fitness for ranking
  return graphFitness;
}

// DijkstraSum implementation

unsigned int DijkstraSum(unsigned int *vertices, unsigned int *adjacencyMatrix, MinHeap *dijkstraMinHeap,
                         IDTuple *minIDTuple, unsigned int *distances, bool *processed, unsigned int source)
{
  // Reset priority queue min-heap size
  dijkstraMinHeap->heapSize = 0;

  // Set values for vertex 0
  distances[0] = 0;
  processed[0] = false;
  MinHeapInsertVertex(dijkstraMinHeap, 0, distances[0]);

  // Initialize values for other vertices
  for (unsigned int i = 1; i < *vertices; i++)
  {
    distances[i] = UINT_MAX;
    processed[i] = false;
    MinHeapInsertVertex(dijkstraMinHeap, i, distances[i]);
  }

  // Distances sum
  unsigned int distancesSum = 0;

  // Calculate shortest path for every node
  while (dijkstraMinHeap->heapSize + 1 > 1)
  {
    // Get next vertex in min-heap
    MinHeapPopMin(dijkstraMinHeap, minIDTuple);

    // Mark it as processed
    processed[minIDTuple->index] = true;

    for (unsigned int i = 0; i < *vertices; i++)
    {
      // If total distance is better, update min-heap
      if (!processed[i] && *(adjacencyMatrix + minIDTuple->index * *vertices + i) && minIDTuple->distance != UINT_MAX &&
          distances[i] > minIDTuple->distance + *(adjacencyMatrix + minIDTuple->index * *vertices + i))
      {
        distances[i] = minIDTuple->distance + *(adjacencyMatrix + minIDTuple->index * *vertices + i);
        MinHeapDecreaseDistance(dijkstraMinHeap, i, distances[i]);
      }
    }
  }

  // Sum up all distances
  for (unsigned int i = 1; i < *vertices; i++)
  {
    if (processed[i] && distances[i] != UINT_MAX)
      distancesSum += distances[i];
  }

  // Return distances sum
  return distancesSum;
}

// MinHeapInsertVertex implementation

void MinHeapInsertVertex(MinHeap *dijkstraMinHeap, unsigned int vertex, unsigned int distance)
{
  (dijkstraMinHeap->heapSize)++;
  (dijkstraMinHeap->verticesPositions)[vertex] = vertex;
  (dijkstraMinHeap->keys)[dijkstraMinHeap->heapSize - 1].index = vertex;
  (dijkstraMinHeap->keys)[dijkstraMinHeap->heapSize - 1].distance = distance;
}

// MinHeapDecreaseDistance implementation

void MinHeapDecreaseDistance(MinHeap *dijkstraMinHeap, unsigned int vertex, unsigned int distance)
{
  // Get index of vertex
  unsigned int index = (dijkstraMinHeap->verticesPositions)[vertex];

  // Set new key value
  (dijkstraMinHeap->keys)[index].index = vertex;
  (dijkstraMinHeap->keys)[index].distance = distance;

  // Swap to correct order
  while (index && (dijkstraMinHeap->keys)[(index - 1) / 2].distance > (dijkstraMinHeap->keys)[index].distance)
  {
    // Swap vertices' positions
    (dijkstraMinHeap->verticesPositions)[(dijkstraMinHeap->keys)[index].index] = (index - 1) / 2;
    (dijkstraMinHeap->verticesPositions)[(dijkstraMinHeap->keys)[(index - 1) / 2].index] = index;

    // Swap tuples
    IDTuple tempKey = (dijkstraMinHeap->keys)[(index - 1) / 2];
    (dijkstraMinHeap->keys)[(index - 1) / 2] = (dijkstraMinHeap->keys)[index];
    (dijkstraMinHeap->keys)[index] = tempKey;

    // Climb up
    index = (index - 1) / 2;
  }
}

// MinHeapPopMin implementation

void MinHeapPopMin(MinHeap *dijkstraMinHeap, IDTuple *minIDTuple)
{
  // Get min
  minIDTuple->index = (dijkstraMinHeap->keys)[0].index;
  minIDTuple->distance = (dijkstraMinHeap->keys)[0].distance;

  // Assign last item to first, update vertex position and fix
  (dijkstraMinHeap->keys)[0] = (dijkstraMinHeap->keys)[dijkstraMinHeap->heapSize - 1];
  (dijkstraMinHeap->verticesPositions)[minIDTuple->index] = 0;
  (dijkstraMinHeap->heapSize)--;
  MinHeapifyTopDown(dijkstraMinHeap, 0);
}

// MinHeapifyTopDown implementation

void MinHeapifyTopDown(MinHeap *dijkstraMinHeap, unsigned int index)
{
  // Get index of left and right children
  unsigned int left = 2 * index + 1;
  unsigned int right = 2 * index + 2;

  // Min key index
  unsigned int min = index;

  // Find min key index
  if (left < dijkstraMinHeap->heapSize && (dijkstraMinHeap->keys)[left].distance < (dijkstraMinHeap->keys)[min].distance)
    min = left;

  if (right < dijkstraMinHeap->heapSize && (dijkstraMinHeap->keys)[right].distance < (dijkstraMinHeap->keys)[min].distance)
    min = right;

  // If min-heap is still not fixed
  if (min != index)
  {
    // Swap vertices' positions
    (dijkstraMinHeap->verticesPositions)[(dijkstraMinHeap->keys)[min].index] = index;
    (dijkstraMinHeap->verticesPositions)[(dijkstraMinHeap->keys)[index].index] = min;

    // Swap tuples
    IDTuple tempKey = (dijkstraMinHeap->keys)[min];
    (dijkstraMinHeap->keys)[min] = (dijkstraMinHeap->keys)[index];
    (dijkstraMinHeap->keys)[index] = tempKey;

    // Descend down
    MinHeapifyTopDown(dijkstraMinHeap, min);
  }
}

// MaxHeapInsertGraph implementation

void MaxHeapInsertGraph(MaxHeap *bestGraphsMaxHeap, unsigned int *bestGraphs, unsigned int index, unsigned int fitness)
{
  // Heap is full case
  if (bestGraphsMaxHeap->heapSize == bestGraphsMaxHeap->size)
  {
    // Is it really, tho?
    if (bestGraphsMaxHeap->size != *bestGraphs)
    {
      bestGraphsMaxHeap->size = (unsigned int)(bestGraphsMaxHeap->size + *bestGraphs * REALLOC_PERCENTAGE);
      IDTuple *tempKeys = realloc(bestGraphsMaxHeap->keys, bestGraphsMaxHeap->size * sizeof bestGraphsMaxHeap->keys);
      if (tempKeys == NULL)
      {
        fprintf(stderr, "allocation error: realloc keys MaxHeapInsertGraph");
        exit(1);
      }
      bestGraphsMaxHeap->keys = tempKeys;

      // Insert new graph and heapify bottom up
      (bestGraphsMaxHeap->heapSize)++;
      (bestGraphsMaxHeap->keys)[bestGraphsMaxHeap->heapSize - 1].index = index;
      (bestGraphsMaxHeap->keys)[bestGraphsMaxHeap->heapSize - 1].distance = fitness;
      MaxHeapifyBottomUp(bestGraphsMaxHeap, bestGraphsMaxHeap->heapSize - 1);
    }
    else
    {
      // Well then, let's pop the worst graph (substitute and heapify top down)
      (bestGraphsMaxHeap->keys)[0].index = index;
      (bestGraphsMaxHeap->keys)[0].distance = fitness;
      MaxHeapifyTopDown(bestGraphsMaxHeap, 0);
    }
  }
  else
  {
    // Insert new graph and heapify bottom up
    (bestGraphsMaxHeap->heapSize)++;
    (bestGraphsMaxHeap->keys)[bestGraphsMaxHeap->heapSize - 1].index = index;
    (bestGraphsMaxHeap->keys)[bestGraphsMaxHeap->heapSize - 1].distance = fitness;
    MaxHeapifyBottomUp(bestGraphsMaxHeap, bestGraphsMaxHeap->heapSize - 1);
  }
}

// MaxHeapPopMax implementation

void MaxHeapPopMax(MaxHeap *bestGraphsMaxHeap)
{
  // Assign last item to first and fix
  (bestGraphsMaxHeap->keys)[0] = (bestGraphsMaxHeap->keys)[bestGraphsMaxHeap->heapSize - 1];
  (bestGraphsMaxHeap->heapSize)--;
  MaxHeapifyTopDown(bestGraphsMaxHeap, 0);
}

// MaxHeapifyTopDown implementation

void MaxHeapifyTopDown(MaxHeap *bestGraphsMaxHeap, unsigned int index)
{
  // Get index of left and right children
  unsigned int left = 2 * index + 1;
  unsigned int right = 2 * index + 2;

  // Max key index
  unsigned int max = index;

  // Find max key index
  if (left < bestGraphsMaxHeap->heapSize && (bestGraphsMaxHeap->keys)[left].distance > (bestGraphsMaxHeap->keys)[max].distance)
    max = left;

  if (right < bestGraphsMaxHeap->heapSize && (bestGraphsMaxHeap->keys)[right].distance > (bestGraphsMaxHeap->keys)[max].distance)
    max = right;

  // If max-heap is still not fixed
  if (max != index)
  {
    // Swap tuples
    IDTuple tempKey = (bestGraphsMaxHeap->keys)[max];
    (bestGraphsMaxHeap->keys)[max] = (bestGraphsMaxHeap->keys)[index];
    (bestGraphsMaxHeap->keys)[index] = tempKey;

    // Descend down
    MaxHeapifyTopDown(bestGraphsMaxHeap, max);
  }
}

// MaxHeapifyBottomUp implementation

void MaxHeapifyBottomUp(MaxHeap *bestGraphsMaxHeap, unsigned int index)
{
  // Swap to correct order
  if (index && (bestGraphsMaxHeap->keys)[(index - 1) / 2].distance <= (bestGraphsMaxHeap->keys)[index].distance)
  {
    // Swap tuples
    IDTuple tempKey = (bestGraphsMaxHeap->keys)[(index - 1) / 2];
    (bestGraphsMaxHeap->keys)[(index - 1) / 2] = (bestGraphsMaxHeap->keys)[index];
    (bestGraphsMaxHeap->keys)[index] = tempKey;

    // Climb up
    MaxHeapifyBottomUp(bestGraphsMaxHeap, (index - 1) / 2);
  }
}

// MaxHeapPrint implementation

void MaxHeapPrint(MaxHeap *bestGraphsMaxHeap)
{
  // Just loop
  for (unsigned int i = 0; i < bestGraphsMaxHeap->heapSize - 1; i++)
    printf("%u ", (bestGraphsMaxHeap->keys)[i].index);

  // But remember no space at the end
  printf("%u\n", (bestGraphsMaxHeap->keys)[bestGraphsMaxHeap->heapSize - 1].index);
}

// ------------------------------------------------------------------- Functions implementation ------------------------------------------------------------------
