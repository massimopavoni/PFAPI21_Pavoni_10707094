#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --------------------------------------------------------------------------- Structs ---------------------------------------------------------------------------

// VDTuple struct
typedef struct
{
  // Vertex index
  unsigned int vertex;

  // Distance of vertex
  unsigned int distance;

} VDTuple;

// MinHeap struct
typedef struct
{
  // Current size of the heap: unsigned int
  unsigned int heapSize;

  // Maximum size of the heap: unsigned int
  unsigned int size;

  // Positions of vertices in the heap: unsigned int *
  unsigned int *verticesPositions;

  // Keys array: VDTuple *
  VDTuple *keys;

} MinHeap;

// GFElement struct
typedef struct s_GFElement GFElement;

struct s_GFElement
{
  // Graph index: unsigned int
  unsigned int graphIndex;

  // Graph fitness: unsigned int
  unsigned int graphFitness;

  // Next list element: GFElement *
  GFElement *next;
};

// List struct
typedef struct
{
  // List length: unsigned int
  unsigned int length;

  // Maximum list length: unsigned int
  unsigned int maxLength;

  // First element: GFElement *
  GFElement *first;

} List;

// --------------------------------------------------------------------------- Structs ---------------------------------------------------------------------------

// -------------------------------------------------------------------- Functions declaration --------------------------------------------------------------------

// Get graph fitness after reading edges info.
//
// Return type: unsigned int
// Arguments
// - number of vertices variable pointer: unsigned int *
// - allocated adjacency matrix pointer: unsigned int *
// - allocated priority queue min-heap struct pointer: MinHeap *
// - allocated min-heap min element struct pointer: VDTuple *
// - allocated vertices' distances array pointer: unsigned int *
// - allocated processed vertices array pointer: bool *
unsigned int GetGraphFitness(unsigned int *vertices, unsigned int *adjacencyMatrix, MinHeap *minHeap,
                             VDTuple *minVDTuple, unsigned int *distances, bool *processed);

// Dijkstra's algorithm to calculate shortest paths from source to all other vertices and get the total sum.
//
// Return type: unsigned int
// Arguments
// - number of vertices variable pointer: unsigned int *
// - adjacency matrix pointer: unsigned int *
// - min-heap struct pointer: MinHeap *
// - min-heap min element struct pointer: VDTuple *
// - vertices' distances array pointer: unsigned int *
// - processed vertices array pointer: unsigned int *
// - source vertex: unsigned int
unsigned int DijkstraSum(unsigned int *vertices, unsigned int *adjacencyMatrix, MinHeap *minHeap,
                         VDTuple *minVDTuple, unsigned int *distances, bool *processed, unsigned int source);

// Min-heap insert function for new distance.
//
// Return type: void
// Arguments
// - min-heap struct pointer: MinHeap *
// - vertex to insert: unsigned int
// - distance to insert: unsigned int
void MinHeapInsert(MinHeap *minHeap, unsigned int vertex, unsigned int distance);

// Min-heap decrease distance function for decreasing distances (maintaining properties).
//
// Return type: void
// Arguments
// - min-heap struct pointer: MinHeap *
// - vertex of distance to decrease: unsigned int
// - new distance value: unsigned int
void MinHeapDecreaseDistance(MinHeap *minHeap, unsigned int vertex, unsigned int distance);

// Min-heap extract min function to get the minimum distance (and its vertex) inside the array.
//
// Return type: void
// Arguments
// - min-heap struct pointer: MinHeap *
// - min-heap min element struct pointer: VDTuple *
void MinHeapPopMin(MinHeap *minHeap, VDTuple *minVDTuple);

// Min-heap heapify function that fixes the min-heap to make sure that it still respects the properties.
//
// Return type: void
// Arguments
// - min-heap struct pointer: MinHeap *
// - index from which the function starts: unsigned int
void MinHeapify(MinHeap *minHeap, unsigned int index);

// GFElement init function to allocate memory for the element and return pointer.
//
// Return type: GFElement *
// Arguments
// - graph index since the start of the program: unsigned int
// - graph fitness value obtained through shortest paths sum: unsigned int
GFElement *GFElementInit(unsigned int graphIndex, unsigned int graphFitness);

// List init function to allocate memory for the list and return pointer.
//
// Return type: List *
// Arguments
// - max length of the list: unsigned int
List *ListInit(unsigned int maxLength);

// List insert function to check if graph goes in the ranking and pop worst graph if needed.
//
// Return type: void
// Arguments
// - best graphs list struct pointer: List *
// - graph index since the start of the program: unsigned int
// - graph fitness value obtained through shortest paths sum: unsigned int
void ListInsertGraph(List *bestGraphsList, unsigned int graphIndex, unsigned int graphFitness);

// List print function to get the best graphs.
//
// Return type: void
// Arguments
// - best graphs list struct pointer: List *
void ListPrint(List *bestGraphsList);

// -------------------------------------------------------------------- Functions declaration --------------------------------------------------------------------

// -------------------------------------------------------------------------- Constants --------------------------------------------------------------------------

// Max command string size: unsigned char
static const unsigned char MAX_COMMAND_SIZE = 13;

// Add graph command string: char*
static const char *ADD_GRAPH_COMMAND = "AggiungiGrafo";

// Get best graphs command string: char*
static const char *GET_BEST_GRAPHS_COMMAND = "TopK";

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

  // Allocate adjacency matrix just one time, as a single array; usage is *(adjacencyMatrix + row * *vertices + column)

  // Adjacency matrix for edge weight: unsigned int *
  unsigned int *adjacencyMatrix = malloc(vertices * vertices * sizeof *adjacencyMatrix);

  // Priority queue min-heap for Dijkstra: MinHeap *
  MinHeap *minHeap = malloc(sizeof *minHeap);
  minHeap->size = vertices;
  minHeap->verticesPositions = malloc(vertices * sizeof minHeap->verticesPositions);
  minHeap->keys = malloc(vertices * sizeof minHeap->keys);

  // Min-heap min element for Dijkstra: VDTuple *
  VDTuple *minVDTuple = malloc(sizeof *minVDTuple);

  // Vertices' distances array: unsigned int *
  unsigned int *distances = malloc(vertices * sizeof *distances);

  // Processed vertices array: unsigned int *
  bool *processed = malloc(vertices * sizeof *processed);

  // Best graphs list: List *
  List *bestGraphsList = ListInit(bestGraphs);

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
    if (strcmp(command, ADD_GRAPH_COMMAND) == 0)
    {
      // Increment graph index and insert graph in list (if it should be there)
      graphIndex++;
      ListInsertGraph(bestGraphsList, graphIndex, GetGraphFitness(&vertices, adjacencyMatrix, minHeap, minVDTuple, distances, processed));
    }
    else if (strcmp(command, GET_BEST_GRAPHS_COMMAND) == 0)
      // Print best graphs
      ListPrint(bestGraphsList);
  }

  // Free memory galore
  free(adjacencyMatrix);
  free(minHeap);
  free(distances);
  free(processed);
  free(bestGraphsList);
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

unsigned int GetGraphFitness(unsigned int *vertices, unsigned int *adjacencyMatrix, MinHeap *minHeap,
                             VDTuple *minVDTuple, unsigned int *distances, bool *processed)
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
  unsigned int graphFitness = DijkstraSum(vertices, adjacencyMatrix, minHeap, minVDTuple, distances, processed, 0);

  // Return graph fitness for ranking
  return graphFitness;
}

// DijkstraSum implementation

unsigned int DijkstraSum(unsigned int *vertices, unsigned int *adjacencyMatrix, MinHeap *minHeap,
                         VDTuple *minVDTuple, unsigned int *distances, bool *processed, unsigned int source)
{
  // Reset priority queue min-heap size
  minHeap->heapSize = 0;

  // Set values for vertex 0
  distances[0] = 0;
  processed[0] = false;
  MinHeapInsert(minHeap, 0, distances[0]);

  // Initialize values for other vertices
  for (unsigned int i = 1; i < *vertices; i++)
  {
    distances[i] = UINT_MAX;
    processed[i] = false;
    MinHeapInsert(minHeap, i, distances[i]);
  }

  // Distances sum
  unsigned int distancesSum = 0;

  // Calculate shortest path for every node
  while (minHeap->heapSize + 1 > 1)
  {
    // Get next vertex in min-heap
    MinHeapPopMin(minHeap, minVDTuple);

    // Mark it as processed
    processed[minVDTuple->vertex] = true;

    for (unsigned int i = 0; i < *vertices; i++)
    {
      // If total distance is better, update min-heap
      if (!processed[i] && *(adjacencyMatrix + minVDTuple->vertex * *vertices + i) && minVDTuple->distance != UINT_MAX &&
          distances[i] > minVDTuple->distance + *(adjacencyMatrix + minVDTuple->vertex * *vertices + i))
      {
        distances[i] = minVDTuple->distance + *(adjacencyMatrix + minVDTuple->vertex * *vertices + i);
        MinHeapDecreaseDistance(minHeap, i, distances[i]);
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

// MinHeapInsert implementation

void MinHeapInsert(MinHeap *minHeap, unsigned int vertex, unsigned int distance)
{
  (minHeap->heapSize)++;
  (minHeap->verticesPositions)[vertex] = vertex;
  (minHeap->keys)[minHeap->heapSize - 1].vertex = vertex;
  (minHeap->keys)[minHeap->heapSize - 1].distance = distance;
}

// MinHeapDecreaseDistance implementation

void MinHeapDecreaseDistance(MinHeap *minHeap, unsigned int vertex, unsigned int distance)
{
  // Get index of vertex
  unsigned int index = (minHeap->verticesPositions)[vertex];

  // Set new key value
  (minHeap->keys)[index].vertex = vertex;
  (minHeap->keys)[index].distance = distance;

  // Swap to correct order
  while (index && (minHeap->keys)[(index - 1) / 2].distance > (minHeap->keys)[index].distance)
  {
    // Swap vertices' positions
    (minHeap->verticesPositions)[(minHeap->keys)[index].vertex] = (index - 1) / 2;
    (minHeap->verticesPositions)[(minHeap->keys)[(index - 1) / 2].vertex] = index;

    // Swap tuples
    VDTuple tempKey = (minHeap->keys)[(index - 1) / 2];
    (minHeap->keys)[(index - 1) / 2] = (minHeap->keys)[index];
    (minHeap->keys)[index] = tempKey;

    // Climb up
    index = (index - 1) / 2;
  }
}

// MinHeapPopMin implementation

void MinHeapPopMin(MinHeap *minHeap, VDTuple *minVDTuple)
{
  // Get min
  minVDTuple->vertex = (minHeap->keys)[0].vertex;
  minVDTuple->distance = (minHeap->keys)[0].distance;

  // Assign last item to first, update vertex position and fix
  (minHeap->keys)[0] = (minHeap->keys)[minHeap->heapSize - 1];
  (minHeap->verticesPositions)[minVDTuple->vertex] = 0;
  (minHeap->heapSize)--;
  MinHeapify(minHeap, 0);
}

// MinHeapify implementation

void MinHeapify(MinHeap *minHeap, unsigned int index)
{
  // Get index of left and right children
  unsigned int left = 2 * index + 1;
  unsigned int right = 2 * index + 2;

  // Min key index
  unsigned int min = index;

  // Find min key index
  if (left < minHeap->heapSize && (minHeap->keys)[left].distance < (minHeap->keys)[min].distance)
    min = left;

  if (right < minHeap->heapSize && (minHeap->keys)[right].distance < (minHeap->keys)[min].distance)
    min = right;

  // If min-heap is still not fixed
  if (min != index)
  {
    // Swap vertices' positions
    (minHeap->verticesPositions)[(minHeap->keys)[min].vertex] = index;
    (minHeap->verticesPositions)[(minHeap->keys)[index].vertex] = min;

    // Swap tuples
    VDTuple tempKey = (minHeap->keys)[min];
    (minHeap->keys)[min] = (minHeap->keys)[index];
    (minHeap->keys)[index] = tempKey;

    // Climb up
    MinHeapify(minHeap, min);
  }
}

// GFElementInit implementation

GFElement *GFElementInit(unsigned int graphIndex, unsigned int graphFitness)
{
  // Allocate element memory, initialize graph index, graph fitness and next
  GFElement *element = malloc(sizeof *element);

  element->graphIndex = graphIndex;
  element->graphFitness = graphFitness;
  element->next = NULL;

  // Return pointer to struct
  return element;
}

// ListInit implementation

List *ListInit(unsigned int maxLength)
{
  // Allocate list memory, initialize length, max length and first element
  List *list = malloc(sizeof *list);

  list->length = 0;
  list->maxLength = maxLength;
  list->first = NULL;

  // Return pointer to struct
  return list;
}

// ListInsertGraph implementation

void ListInsertGraph(List *bestGraphsList, unsigned int graphIndex, unsigned int graphFitness)
{

  // Well...
  if (bestGraphsList->maxLength > 0)
  {
    // Empty list or first element case
    if (bestGraphsList->first == NULL || (bestGraphsList->first)->graphFitness > graphFitness)
    {
      // Allocate new graph
      GFElement *newGraph = GFElementInit(graphIndex, graphFitness);

      // Insert it as first element
      newGraph->next = bestGraphsList->first;
      bestGraphsList->first = newGraph;

    } // Non-empty list and not first element case
    else
    {
      // Get first element
      GFElement *currentGraph = bestGraphsList->first;

      // Find correct position
      while (currentGraph->next != NULL && (currentGraph->next)->graphFitness < graphFitness)
        currentGraph = currentGraph->next;

      // Allocate new graph
      GFElement *newGraph = GFElementInit(graphIndex, graphFitness);

      // Insert new graph
      newGraph->next = currentGraph->next;
      currentGraph->next = newGraph;
    }

    // Pop worst graph if needed
    if (bestGraphsList->length == bestGraphsList->maxLength)
    {
      GFElement *trashTheGuyNextToMeHeBad = bestGraphsList->first;

      while ((trashTheGuyNextToMeHeBad->next)->next != NULL)
        trashTheGuyNextToMeHeBad = trashTheGuyNextToMeHeBad->next;

      free(trashTheGuyNextToMeHeBad->next);

      trashTheGuyNextToMeHeBad->next = NULL;
    }
    else
      // Otherwise increase list size
      (bestGraphsList->length)++;
  }
}

// ListPrint implementation

void ListPrint(List *bestGraphsList)
{
  GFElement *currentGraph = bestGraphsList->first;

  // First graph
  if (currentGraph != NULL)
  {
    printf("%u", currentGraph->graphIndex);
    currentGraph = currentGraph->next;
  }

  // Just loop
  while (currentGraph != NULL)
  {
    printf(" %u", currentGraph->graphIndex);
    currentGraph = currentGraph->next;
  }

  printf("\n");
}

// ------------------------------------------------------------------- Functions implementation ------------------------------------------------------------------
