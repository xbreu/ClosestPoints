#ifndef ClosestPairUtilities_h

#define ClosestPairUtilities_h
#include "PointSortUtilities.h" 
// Definition Closest Point
int IsSortingPointsXCorrect(Point array[], int arr_count, int* j);
int closestPairDACMPI(Point points[], const size_t numPoints, double* minDistance);
int writePointsToFile(const char* filename, Point points [], const size_t numPoints, const double minX, const double maxX, const double minY, const double maxY, const int dimension);
int readPointsFromFile(const char* filename, Point** points, size_t *numPoints, double *minX, double *maxX, double *minY, double *maxY, int *dimensions);
int generateRandomPoints(Point** points, const size_t numPoints, const double minX, const double maxX, const double minY, const double maxY);
int printPointsAndHeader(const Point points[], const size_t numPoints, const double minX, const double maxX, const double minY, const double maxY, const int dimension);
double calculateDistance(const Point* p1, const Point* p2);
int closestPairBruteForce(const Point points[], const size_t numPoints, double* minDistance);
int compareX(const void *a, const void *b);
int compareY(const void *a, const void *b);
double closestPairRecursive(const Point points[], const size_t numPoints);
double stripClosest(Point strip[], const size_t stripSize, const double min_lr);
int closestPairDAC(Point points[], const size_t numPoints, double* minDistance);

// Implementation
int writePointsToFile(const char* filename, Point points[], const size_t numPoints, const double minX, const double maxX, const double minY, const double maxY, const int dimension) {
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return -1;
    }

    // Write header information
    fprintf(file, "%ld\n", numPoints);
    fprintf(file, "%f %f %f %f\n", minX, maxX, minY, maxY);
    fprintf(file, "%d\n", dimension); // Number of dimensions

    // Write points
    size_t i;
    for (i = 0; i < numPoints; i++) {
        fprintf(file, "%15.10f %15.10f\n", points[i].x, points[i].y);
    }

    fclose(file);

    return 0;
}

int readPointsFromFile(const char* filename, Point** points, size_t *numPoints, double *minX, double *maxX, double *minY, double *maxY, int *dimensions) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return -1;
    }

    // Read the header information
    fscanf(file, "%ld", numPoints);
    fscanf(file, "%lf %lf %lf %lf", minX, maxX, minY, maxY);
    fscanf(file, "%d", dimensions);

    *points = (Point*) malloc((*numPoints) * sizeof(Point));
    if (*points == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return -2;
    }

    // Read points data
    size_t i;
    for (i = 0; i < *numPoints; i++) 
    {
        if (fscanf(file, "%lf %lf", &((*points)[i].x), &((*points)[i].y)) != *dimensions) 
        {
            fprintf(stderr, "Failed to read data for point %ld.\n", i);
            fclose(file);
            return -3; // Error code for reading failure
        }
    }

    fclose(file);
    return 0;
}

int generateRandomPoints(Point** points, const size_t numPoints, const double minX, const double maxX, const double minY, const double maxY) {

    *points = (Point*) malloc(numPoints * sizeof(Point));
    if (points == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return -1;
    }

    // Generate random points
    size_t i;
    for (i = 0; i < numPoints; i++) {
        (*points)[i].x = minX + (double)rand() / RAND_MAX * (maxX - minX);
        (*points)[i].y = minY + (double)rand() / RAND_MAX * (maxY - minY);
    }

    return 0;
}

int printPointsAndHeader(const Point points[], const size_t numPoints, const double minX, const double maxX, const double minY, const double maxY, const int dimension) {
    // Print header information
    printf("Number of Points: %ld\n", numPoints);
    printf("Domain Limits: X(%f, %f) Y(%f, %f)\n", minX, maxX, minY, maxY);
    printf("Dimensions: %d\n", dimension);

    // Check if points are NULL to avoid dereferencing NULL pointer
    if (points == NULL) {
        printf("No points to display.\n");
        return -1;
    }

    // Print point data
    size_t i;
    for (i = 0; i < numPoints; i++) {
        printf("Point %3ld: (%15.10f, %15.10f)\n", i + 1, points[i].x, points[i].y);
    }

    return 0;
}

double calculateDistance(const Point* p1, const Point* p2) {
    return sqrt((p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y));
}

int compareX(const void *a, const void *b) {
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

int compareY(const void *a, const void *b) {
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    return (p1->y > p2->y) - (p1->y < p2->y);
}

int closestPairBruteForce(const Point points[], const size_t numPoints, double* minDistance) {
    
    *minDistance = DBL_MAX;
    if (numPoints <= 1)
    {
        return 0;
    }
    if (numPoints == 2) {
        *minDistance = calculateDistance(&points[0], &points[1]);
        return 0;
    }
    
    size_t i, j;
    double distance;
    for (i = 0; i < numPoints; i++) 
    {
        for (j = i + 1; j < numPoints; j++) 
        {
            distance = calculateDistance(&points[i], &points[j]);
            if (distance < *minDistance) 
            {
                *minDistance = distance;
            }
        }
    }

    return 0;
}

// The main function that finds the smallest distance
int closestPairDAC(Point points[], const size_t numPoints, double* minDistance)
{
    qsort(points, numPoints, sizeof(Point), compareX);   
    // Use recursion to find the smallest distance
    *minDistance = closestPairRecursive(points, numPoints);

    return 0;
}

int closestPairDACMPI(Point points[], const size_t numPoints, double* minDistance)
{
    // Use recursion to find the smallest distance
    *minDistance = closestPairRecursive(points, numPoints);
    return 0;
}
 
double closestPairRecursive(const Point points[], const size_t numPoints)
{
    // If there are 1 or 2 points, then use brute force
    if (numPoints == 1){
        return DBL_MAX;
    }
    if (numPoints == 2){
        return calculateDistance(&points[0], &points[1]);
    }
    // Find the middle point
    int mid = numPoints/2;
    Point midPoint = points[mid];
    // Consider the vertical line passing through the middle point
    // calculate the smallest distance dl on left of middle point and
    // dr on right side
    double dl = closestPairRecursive(points, mid);
    double dr = closestPairRecursive(points + mid, numPoints-mid);
    // Find the smaller of two distances
    double minlr = (dl>dr) ? dr : dl; // minDouble(dl, dr);
    // Build an array strip[] that contains points close (closer than d)
    // to the line passing through the middle point
    Point* strip = (Point*) malloc(sizeof(Point) * numPoints);
    int i = 0, j = 0;
    for (i = 0; i < numPoints; i++){
        if (fabs(points[i].x - midPoint.x) < minlr)
        {
            strip[j] = points[i]; 
            j++;
        }   
    }
    // Find the closest points in strip. Return the minimum of d and closest
    // distance is strip[]
    double strpmin = stripClosest(strip, j, minlr);
    minlr = (minlr>strpmin) ? strpmin : minlr;
    free(strip);
    return minlr;
}

double stripClosest(Point strip[], const size_t stripSize, const double min_lr)
{
    double min_tot = min_lr;
    qsort(strip, stripSize, sizeof(Point), compareY); 
 
    // Pick all points one by one and try the next points till the difference
    // between y coordinates is smaller than d.
    // This is a proven fact that this loop runs at most 6 times
    int i, j;
    double dist;
    for (i = 0; i < stripSize; i++)
    {
        for (j = i+1; j < stripSize && (strip[j].y - strip[i].y) < min_tot; j++) // 
        {
            dist = calculateDistance(&strip[i], &strip[j]);
            if (dist < min_tot)
            {
                min_tot = dist;
            }
        }
    }
    return min_tot;
}

int IsSortingPointsXCorrect(Point array[], int arr_count, int* j)
{
    int i;
    *j = -1;
    for (i = 0; i < arr_count - 1; i++)
    {
        if (array[i].x > array[i + 1].x)
        {
            *j = i;
            return 0;
        }
    }
    return 1;
}

#endif