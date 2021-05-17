#!/bin/bash

ITERS=300

    for (( iter=1; iter<=$ITERS; iter++ ))
    do
        ompi ./mpi-tile-io --nr_files 14 --nr_tiles_x 14 --nr_tiles_y 1 --sz_tile_x 1 --sz_tile_y 67108864 --sz_element 1 --filename valera14 --write_file
        ompi ./mpi-tile-io --nr_files 14 --nr_tiles_x 14 --nr_tiles_y 1 --sz_tile_x 1 --sz_tile_y 67108864 --sz_element 1 --filename valera14
    done
