# Closest Points

## Dependencies

- A C compiler, like [GCC](https://gcc.gnu.org/)
- Any MPI implementation, for example: [Open MPI](https://www.open-mpi.org/)

## Executing

You can run the code using the command:

```$ sbatch job.sh```

## Input

The input expected by the program has the following format:
```
<n>
<x1> <y1>
<x2> <y2>
...
<xn> <yn>
```

Where `n` is the number of points and the following `n` lines contain two
decimal values representing the coordinates of the points.
