#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "point.h"

double square(double x) {
    return x * x;
}

double distance(struct point *p1, struct point *p2) {
    double delta_x = p1->x - p2->x;
    double delta_y = p1->y - p2->y;
    return sqrt(square(delta_x) + square(delta_y));
}

struct point_array *read_file(char *file_path) {
    // Opens the file to read
    FILE *file = fopen(file_path, "r");

    // Reserves memory for the array
    struct point_array *a = malloc(sizeof(struct point_array));

    // Reads the first line, which should have the number of points
    fscanf(file, "%d", &a->size);

    // Reserves memory for the points
    a->points = calloc(a->size, POINT_SIZE);

    struct point *point;
    // Read each line with x and y coordinates
    for (int i = 0; i < a->size; ++i) {
        point = &a->points[i];
        fscanf(file, "%lf %lf", &point->x, &point->y);
    }

    // Closes the file
    fclose(file);

    return a;
}

void free_point_array(struct point_array *a) {
    free(a->points);
    free(a);
}
