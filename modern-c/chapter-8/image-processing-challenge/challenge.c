#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#include "stb_image.h"
#include "stb_image_write.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// NEED COMPRESSION FUNCTIONS 
// kinda want to support colors as well 

const int MAX_DIFFERERNCE_BETWEEN_GROUPS = 5;
typedef struct statistic statistic;
struct statistic {
    double mean_value;
    size_t num_values;
};

size_t find_parent(size_t current_index, size_t parents[]) {
    while (parents[current_index] != SIZE_MAX) {
        current_index = parents[current_index];
    }

    return current_index;
}

int find_top_neighbor(size_t current_pixel, size_t parents[], int num_cols){
    int result = -1;
    size_t current_parent = find_parent(current_pixel, parents);
    int neighbor_candidate_pixel = current_pixel - num_cols;
    while (neighbor_candidate_pixel > 0){
        if (current_parent == find_parent(neighbor_candidate_pixel, parents)){
            neighbor_candidate_pixel -= num_cols;
            continue;
        }

        result = neighbor_candidate_pixel;
        break;
    }

    return result;
}

int find_left_neighbor(size_t current_pixel, size_t parents[], int num_cols){
    int result = -1;
    size_t current_parent = find_parent(current_pixel, parents);
    int neighbor_candidate_pixel = current_pixel - 1;
    while (neighbor_candidate_pixel >= 0 && neighbor_candidate_pixel % num_cols != num_cols){
        if (current_parent == find_parent(neighbor_candidate_pixel, parents)){
            neighbor_candidate_pixel -= 1;
            continue;
        }

        result = neighbor_candidate_pixel;
        break;
    }

    return result;
}

void complete_merge(size_t current_pixel, size_t neighbor, size_t parents[], statistic stats[]){
    size_t current_parent_location = find_parent(current_pixel, parents);
    size_t current_neighbor_parent_location = find_parent(neighbor, parents);

    statistic current_group_stats = stats[current_parent_location];
    statistic neighbor_group_stats = stats[current_neighbor_parent_location];

    // combine stats
    int combined_count = current_group_stats.num_values + neighbor_group_stats.num_values;
    double combined_mean = ((current_group_stats.mean_value * current_group_stats.num_values) + (neighbor_group_stats.mean_value * neighbor_group_stats.num_values)) / (combined_count);
    stats[current_neighbor_parent_location].mean_value = combined_mean;
    stats[current_neighbor_parent_location].num_values = combined_count;

    // add current group to top group 
    parents[current_parent_location] = current_neighbor_parent_location;
}


int main(){
    int num_cols, num_rows, n;
    unsigned char *image_data = stbi_load("image.jpg", &num_cols, &num_rows, &n, 0); // array where pixels are stored sequentially row by row
    if (!image_data) {
        fprintf(stderr, "Failed to load image\n");
        return 1;
    }

    size_t total = (size_t)num_cols * num_rows;
    size_t *parents = malloc(total * sizeof(size_t));
    statistic *stats = malloc(total * sizeof(statistic));

    for (int currentPixel = 0; currentPixel < num_cols * num_rows; currentPixel++) {
        statistic current_stats = {
            .mean_value = image_data[currentPixel],
            .num_values = 1
        };
        stats[currentPixel] = current_stats;
        parents[currentPixel] = SIZE_MAX;
    }

    for (int current_pixel = (num_rows * num_cols) - 1; current_pixel >= 0 ; current_pixel--) {
        int top_neighbor_location = find_top_neighbor(current_pixel, parents, num_cols);
        int left_neighbor_location = find_left_neighbor(current_pixel, parents, num_cols);

        double top_difference = top_neighbor_location ==  -1 ? SIZE_MAX : (stats[find_parent(current_pixel, parents)].mean_value - stats[find_parent(top_neighbor_location, parents)].mean_value);
        double left_difference = left_neighbor_location ==  -1 ? SIZE_MAX : (stats[find_parent(current_pixel, parents)].mean_value - stats[find_parent(left_neighbor_location, parents)].mean_value);

        if (top_difference > MAX_DIFFERERNCE_BETWEEN_GROUPS && left_difference > MAX_DIFFERERNCE_BETWEEN_GROUPS) {
            continue;
        }

        complete_merge(current_pixel, (top_difference < left_difference ? top_neighbor_location : left_neighbor_location), parents, stats);
    }

    free(stats);

    unsigned char *new_image_data = malloc(total * sizeof(unsigned char) * 3); 
    for (int current_pixel = (num_rows * num_cols) - 1; current_pixel >= 0 ; current_pixel--) {
        size_t parent_index = find_parent(current_pixel, parents);

        new_image_data[3*current_pixel + 0] = (parent_index * 53) % 256;
        new_image_data[3*current_pixel + 1] = (parent_index * 97) % 256;
        new_image_data[3*current_pixel + 2] = (parent_index * 193) % 256;
    }

    stbi_write_jpg("segmented.jpg", num_cols, num_rows, 3, new_image_data, 100);
    free(parents);
    free(new_image_data);

}