#include"ClosestPairUtilities.h"

int main (int argc, char* argv[])
{
  // Argument Managment 
  if (argc != 8)
  {
    printf("Definition:\n\tThis Function Generates Points For Closest Point Problem\n");
    printf("Usage:\n\tGeneratePoint filePath numPoints minX maxX minY maxY dimension\n");
    printf("Arguments:\n");
    printf("\t- filePath: Path to save the points\n");
    printf("\t- numPoints: Number of points to be printed\n");
    printf("\t- minX: Lower bound of x-direction\n");
    printf("\t- maxX: Upper bound of x-direction\n");
    printf("\t- minY: Lower bound of y-direction\n");
    printf("\t- maxY: Upper bound of y-direction\n");
    printf("\t- dimension: Dimensionality of the points\n");
    return 0;
  }
  
  // Error Checking
  const char* filePath = argv[1];
    char* end;
    size_t numPoints = strtol(argv[2], &end, 10);
    if (*end != '\0' || numPoints <= 0) {
        printf("Error: Invalid number of points.\n");
        return -1;
    }

    double minX = strtod(argv[3], &end);
    if (*end != '\0') {
        printf("Error: Invalid minimum X value.\n");
        return -1;
    }

    double maxX = strtod(argv[4], &end);
    if (*end != '\0' || minX >= maxX) {
        printf("Error: Invalid maximum X value or minX >= maxX.\n");
        return -1;
    }

    double minY = strtod(argv[5], &end);
    if (*end != '\0') {
        printf("Error: Invalid minimum Y value.\n");
        return -1;
    }

    double maxY = strtod(argv[6], &end);
    if (*end != '\0' || minY >= maxY) {
        printf("Error: Invalid maximum Y value or minY >= maxY.\n");
        return -1;
    }

    int dimension = strtol(argv[7], &end, 10);
    if (*end != '\0' || dimension <= 0) {
        printf("Error: Invalid dimension value.\n");
        return -1;
    }

  // Seed the random number generator
  srand(time(NULL));
  
  // Implementation
  Point *points = NULL;
  printf("Start Generating the Points...\n");
  if (generateRandomPoints(&points, numPoints, minX, maxX, minY, maxY)) 
  {
    printf("Random Point Generation Failed!\n");
    return -1;
  }
  printf("Points Generated!\n");

  printf("Writing Points to %s...\n", filePath);
  if (writePointsToFile(filePath, points, numPoints, minX, maxX, minY, maxY, dimension)) 
  {
    printf("Random Point Generation Failed!\n");
    return -1;
  }
  printf("Points Writed!\n");

  printf("Validating Written File...\n");
  points = NULL;
  int errcode = readPointsFromFile(filePath, &points, &numPoints, &minX, &maxX, &minY, &maxY, &dimension);
  if (errcode)
  {
    printf("Validation From File Failed with Error Code %d!\n", errcode);
    return -1;
  }
  printf("Written File is Valid!\n");

  // Evaluating the points
  // printPointsAndHeader(points, numPoints, minX, maxX, minY, maxY, dimension);

  free(points); // Clean up allocated memory
  printf("Done!\n");  
  return 0;
}