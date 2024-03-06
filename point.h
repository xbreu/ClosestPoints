#ifndef MPI_POINT_H
#define MPI_POINT_H

#define POINT_SIZE (sizeof(struct point))

struct point {
    double x;
    double y;
};

struct point_array {
    struct point* points;
    int size;
};

/** Calculates the euclidean distance between two points */
double distance(struct point *, struct point *);

/** Read file and creates the point array */
struct point_array *read_file(char* file_path);

/** Frees the point array and the points inside it */
void free_point_array(struct point_array*);

#endif //MPI_POINT_H
