#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ---------------------------------------------------------------- Functions, constants, structs ----------------------------------------------------------------

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
  // Current position in the array: unsigned int
  unsigned int heapSize;

  // Maximum size of the heap: unsigned int
  unsigned int size;

  // Array of keys: VDTuple *
  VDTuple *keys;

} MinHeap;

// GFElement struct
typedef struct s_GFElement GFElement;

struct s_GFElement
{
  // Graph index: unsigned int
  unsigned int graphIndex;

  // Graph fitness: unsigned int *
  unsigned int *graphFitness;

  // Next list element: GFElement *
  GFElement *next;
};

// List struct
typedef struct
{
  // List length: unsigned int
  unsigned int length;

  // Maximum list length: unsigned int *
  unsigned int *maxLength;

  // First element: GFElement *
  GFElement *first;

} List;

// Functions declaration

// Get graph fitness after reading edges info.
//
// Return type: unsigned int *
// Arguments
// - number of vertices variable pointer: unsigned int*
unsigned int *GetGraphFitness(unsigned int *vertices);

// Dijkstra's algorithm to calculate shortest paths from source to all other vertices and get the total sum.
//
// Return type: unsigned int *
// Arguments
// - number of vertices variable pointer: unsigned int *
// - adjacency matrix pointer: unsigned int *
// - source vertex: unsigned int
unsigned int *DijkstraSum(unsigned int *vertices, unsigned int *adjacencyMatrix, unsigned int source);

// Min-heap init function to allocate memory for the heap and return pointer.
//
// Return type: MinHeap *
// Arguments
// - size of the heap: unsigned int *
MinHeap *MinHeapInit(unsigned int *size);

// Min-heap insert function for new distance.
//
// Return type: void
// Arguments
// - min-heap structure pointer: MinHeap *
// - vertex to insert: unsigned int
// - distance to insert: unsigned int *
void MinHeapInsert(MinHeap *minHeap, unsigned int vertex, unsigned int *distance);

// Min-heap decrease distance function for decreasing distances (maintaining properties).
//
// Return type: void
// Arguments
// - min-heap structure pointer: MinHeap *
// - index of distance to decrease: unsigned int
// - vertex of distance to decrease: unsigned int
// - new distance value: unsigned int
void MinHeapDecreaseDistance(MinHeap *minHeap, unsigned int index, unsigned int vertex, unsigned int *distance);

// Min-heap extract min function to get the minimum distance (and its vertex) inside the array.
//
// Return type: VDTuple *
// Arguments
// - min-heap structure pointer: MinHeap *
VDTuple *MinHeapPopMin(MinHeap *minHeap);

// Min-heap heapify function that fixes the min-heap to make sure that it still respects the properties.
//
// Return type: void
// Arguments
// - min-heap structure pointer: MinHeap *
// - root index from which the function starts: unsigned int
void MinHeapify(MinHeap *minHeap, unsigned int root);

// GFElement init function to allocate memory for the element and return pointer.
//
// Return type: GFElement *
// Arguments
// - graph index since the start of the program: unsigned int
// - graph fitness value obtained through shortest paths sum: unsigned int *
GFElement *GFElementInit(unsigned int graphIndex, unsigned int *graphFitness);

// List init function to allocate memory for the list and return pointer.
//
// Return type: List *
// Arguments
// - max length of the list: unsigned int *
List *ListInit(unsigned int *maxLength);

// List insert function to check if graph goes in the ranking and pop worst graph if needed.
//
// Return type: void
// Arguments
// - best graphs list structure pointer: List *
// - graph index since the start of the program: unsigned int
// - graph fitness value obtained through shortest paths sum: unsigned int *
void ListInsertGraph(List *bestGraphsList, unsigned int graphIndex, unsigned int *graphFitness);

// List print function to get the best graphs.
//
// Return type: void
// Arguments
// - best graphs list structure pointer: List *
void ListPrint(List *bestGraphsList);

// Static constants

// Max command string size: unsigned char
static const unsigned char MAX_COMMAND_SIZE = 13;

// Add graph command string: char*
static const char *ADD_GRAPH_COMMAND = "AggiungiGrafo";

// Get best graphs command string: char*
static const char *GET_BEST_GRAPHS_COMMAND = "TopK";

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

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
  unsigned int graphIndex = UINT32_MAX;

  // Read graph rank info
  scanf("%u %u", &vertices, &bestGraphs);

  // Best graphs list: List *
  List *bestGraphsList = ListInit(&bestGraphs);

  // String command: char*
  char *command = malloc(MAX_COMMAND_SIZE * sizeof *command);

  // Main loop
  while (1)
  {
    // Read command
    scanf("%s", command);

    // After failed read, if eof exit the loop
    if (feof(stdin))
      break;

    // Parse command
    if (strcmp(command, ADD_GRAPH_COMMAND) == 0)
    {
      // Increment graph index and insert graph in list (if it should be there)
      graphIndex++;
      ListInsertGraph(bestGraphsList, graphIndex, GetGraphFitness(&vertices));
    }
    else if (strcmp(command, GET_BEST_GRAPHS_COMMAND) == 0)
      // Print best graphs
      ListPrint(bestGraphsList);
  }

  // Free memory galore
  free(bestGraphsList);
  free(command);

  // -------- PROFILING --------

  clock_t end = clock();
  double elapsed_time = (end - start) / (double)CLOCKS_PER_SEC;
  fprintf(stderr, "\n%lf", elapsed_time);

  // -------- PROFILING --------

  // The end!
  return 0;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------- Functions implementation ------------------------------------------------------------------

// GetGraphFitness implementation

unsigned int *GetGraphFitness(unsigned int *vertices)
{
  // Allocate adjacency matrix as a single array, usage: *(adjacencyMatrix + row * *vertices + column)
  unsigned int *adjacencyMatrix = malloc(*vertices * *vertices * sizeof *adjacencyMatrix);

  // Input reading loop
  for (unsigned int i = 0; i < *vertices; i++)
  {
    for (unsigned int j = 0; j < *vertices - 1; j++)
      scanf("%u,", adjacencyMatrix + i * *vertices + j);
    scanf("%u", adjacencyMatrix + i * *vertices + *vertices - 1);
  }

  // Graph fitness calculated by summation of all shortest paths
  unsigned int *graphFitness = DijkstraSum(vertices, adjacencyMatrix, 0);

  // Free adjacency matrix memory
  free(adjacencyMatrix);

  // Return graph fitness for ranking
  return graphFitness;
}

// DijkstraSum implementation

unsigned int *DijkstraSum(unsigned int *vertices, unsigned int *adjacencyMatrix, unsigned int source)
{
  // Initialize min-heap priority queue
  MinHeap *minHeap = MinHeapInit(vertices);

  // Allocate array of vertices distances
  unsigned int *distances = malloc(*vertices * sizeof *distances);

  // Set values for vertex 0
  distances[0] = 0;
  MinHeapInsert(minHeap, 0, distances);

  // Initialize values for other vertices
  for (unsigned int i = 1; i < *vertices; i++)
  {
    distances[i] = UINT32_MAX;
    MinHeapInsert(minHeap, i, distances + i);
  }

  // Distances sum
  unsigned int *distancesSum = malloc(sizeof *distancesSum);
  *distancesSum = UINT32_MAX;

  VDTuple *vdTuple = NULL;

  // Calculate shortest path for every node
  while (minHeap->heapSize + 1 > 1)
  {
    // Get next vertex in min-heap
    vdTuple = MinHeapPopMin(minHeap);

    for (unsigned int i = 0; i < *vertices; i++)
    {
      // Loop only adjacent vertices (ignores itself and not connected)
      if (i != vdTuple->vertex && *(adjacencyMatrix + vdTuple->vertex * *vertices + i) != 0)
      {
        // If total distance is better, update min-heap
        if (distances[i] == UINT32_MAX)
        {
          distances[i] = distances[vdTuple->vertex] + *(adjacencyMatrix + vdTuple->vertex * *vertices + i);
          MinHeapDecreaseDistance(minHeap, i, i, distances + i);
        }
        else if (distances[i] > distances[vdTuple->vertex] + *(adjacencyMatrix + vdTuple->vertex * *vertices + i))
        {
          distances[i] = distances[vdTuple->vertex] + *(adjacencyMatrix + vdTuple->vertex * *vertices + i);
          MinHeapDecreaseDistance(minHeap, i, i, distances + i);
        }
      }
    }

    // Add distance to total
    if (*distancesSum == UINT32_MAX)
      *distancesSum = distances[vdTuple->vertex];
    else if (distances[vdTuple->vertex] != UINT32_MAX)
      (*distancesSum) += distances[vdTuple->vertex];
  }

  // Free 'em all
  free(minHeap);
  free(distances);
  free(vdTuple);

  // Return distances sum
  return distancesSum;
}

// MinHeapInit implementation

MinHeap *MinHeapInit(unsigned int *size)
{
  // Allocate min-heap memory and its array, initialize size and heap size
  MinHeap *minHeap = malloc(sizeof *minHeap);

  minHeap->size = *size;
  minHeap->heapSize = 0;
  minHeap->keys = malloc(*size * sizeof minHeap->keys);

  // Return pointer to struct
  return minHeap;
}

// MinHeapInsert implementation

void MinHeapInsert(MinHeap *minHeap, unsigned int vertex, unsigned int *distance)
{
  (minHeap->heapSize)++;
  (minHeap->keys)[minHeap->heapSize - 1].vertex = vertex;
  (minHeap->keys)[minHeap->heapSize - 1].distance = *distance;

  MinHeapDecreaseDistance(minHeap, (minHeap->heapSize) - 1, vertex, distance);
}

// MinHeapDecreaseDistance implementation

void MinHeapDecreaseDistance(MinHeap *minHeap, unsigned int index, unsigned int vertex, unsigned int *distance)
{
  // Set new key value
  (minHeap->keys)[index].vertex = vertex;
  (minHeap->keys)[index].distance = *distance;

  // Swap to correct order
  while (index > 0 && (minHeap->keys)[index / 2].distance > (minHeap->keys)[index].distance)
  {
    // Swap
    VDTuple tempKey = (minHeap->keys)[index / 2];
    (minHeap->keys)[index / 2] = (minHeap->keys)[index];
    (minHeap->keys)[index] = tempKey;

    // Next parent
    index /= 2;
  }
}

// MinHeapPopMin implementation

VDTuple *MinHeapPopMin(MinHeap *minHeap)
{
  // Get min
  VDTuple *min = malloc(sizeof *min);
  min->vertex = (minHeap->keys)[0].vertex;
  min->distance = (minHeap->keys)[0].distance;

  // Assign last item to first and fix
  (minHeap->keys)[0] = (minHeap->keys)[minHeap->heapSize - 1];
  (minHeap->heapSize)--;
  MinHeapify(minHeap, 0);

  // Return popped min
  return min;
}

// MinHeapify implementation

void MinHeapify(MinHeap *minHeap, unsigned int root)
{
  // Get index of left and right children
  unsigned int left = 2 * root + 1;
  unsigned int right = 2 * root + 2;

  // Min key index
  unsigned int min = 0;

  // Find min key index
  if (left < minHeap->heapSize && (minHeap->keys)[left].distance < (minHeap->keys)[root].distance)
    min = left;
  else
    min = root;

  if (right < minHeap->heapSize && (minHeap->keys)[right].distance < (minHeap->keys)[min].distance)
    min = right;

  // If min-heap is still not fixed
  if (min != root)
  {
    // Swap
    VDTuple tempKey = (minHeap->keys)[min];
    (minHeap->keys)[min] = (minHeap->keys)[root];
    (minHeap->keys)[root] = tempKey;

    // Climb up
    MinHeapify(minHeap, min);
  }
}

// GFElementInit implementation

GFElement *GFElementInit(unsigned int graphIndex, unsigned int *graphFitness)
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

List *ListInit(unsigned int *maxLength)
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

void ListInsertGraph(List *bestGraphsList, unsigned int graphIndex, unsigned int *graphFitness)
{
  // Well...
  if (*(bestGraphsList->maxLength) > 0)
  {
    // Allocate new graph
    GFElement *newGraph = GFElementInit(graphIndex, graphFitness);

    // Discard new graph case
    if (bestGraphsList->first != NULL && *((bestGraphsList->first)->graphFitness) == *(newGraph->graphFitness))
    {
      free(newGraph);
      return;

    } // Empty list or first element case
    else if (bestGraphsList->first == NULL || *((bestGraphsList->first)->graphFitness) >= *(newGraph->graphFitness))
    {
      // Insert it as first element, increase list size
      newGraph->next = bestGraphsList->first;
      bestGraphsList->first = newGraph;
      (bestGraphsList->length)++;
    }
    // Non-empty list case
    else
    {
      // Get first element
      GFElement *currentGraph = bestGraphsList->first;

      // Find correct position
      while (currentGraph->next != NULL && *((currentGraph->next)->graphFitness) < *(newGraph->graphFitness))
        currentGraph = currentGraph->next;

      // Discard new graph case
      if (currentGraph->next != NULL && *((currentGraph->next)->graphFitness) == *(newGraph->graphFitness))
      {
        free(newGraph);
        return;
      }

      // Insert new graph
      newGraph->next = currentGraph->next;
      currentGraph->next = newGraph;

      // Pop worst graph if needed
      if (bestGraphsList->length == *(bestGraphsList->maxLength))
      {
        currentGraph = bestGraphsList->first;

        while ((currentGraph->next)->next != NULL)
          currentGraph = currentGraph->next;

        free(currentGraph->next);

        currentGraph->next = NULL;
      }
      else
        // Increase list size
        (bestGraphsList->length)++;
    }
  }
}

// ListPrint implementation

void ListPrint(List *bestGraphsList)
{
  GFElement *currentGraph = bestGraphsList->first;

  // Just loop
  while (currentGraph != NULL)
  {
    printf("%u ", currentGraph->graphIndex);
    currentGraph = currentGraph->next;
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
