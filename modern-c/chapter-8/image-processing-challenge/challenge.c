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

const char* IMAGE_TO_SEGMENT = "elephants.jpeg";
const char* OUTPUT_FILENAME = "segemented.jpg";
const int MAX_DIFFERERNCE_BETWEEN_GROUPS = 75;
typedef struct statistic statistic;
struct statistic {
    double mean_red;
    double mean_green;
    double mean_blue;
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
    while (neighbor_candidate_pixel >= 0){
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
    while (neighbor_candidate_pixel >= 0 && neighbor_candidate_pixel % num_cols != 0){
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
    double combined_mean_red = ((current_group_stats.mean_red * current_group_stats.num_values) + (neighbor_group_stats.mean_red * neighbor_group_stats.num_values)) / (combined_count);
    double combined_mean_blue = ((current_group_stats.mean_blue * current_group_stats.num_values) + (neighbor_group_stats.mean_blue * neighbor_group_stats.num_values)) / (combined_count);
    double combined_mean_green = ((current_group_stats.mean_green * current_group_stats.num_values) + (neighbor_group_stats.mean_green * neighbor_group_stats.num_values)) / (combined_count);
    
    stats[current_neighbor_parent_location].mean_red = combined_mean_red;
    stats[current_neighbor_parent_location].mean_blue = combined_mean_blue;
    stats[current_neighbor_parent_location].mean_green = combined_mean_green;
    stats[current_neighbor_parent_location].num_values = combined_count;

    // add current group to top group 
    parents[current_parent_location] = current_neighbor_parent_location;
}

double calculate_difference(size_t first_pixel, size_t second_pixel, statistic stats[]){
    double red_diff = (stats[second_pixel].mean_red - stats[first_pixel].mean_red);
    double blue_diff = (stats[second_pixel].mean_blue - stats[first_pixel].mean_blue);
    double green_diff = (stats[second_pixel].mean_green - stats[first_pixel].mean_green);

    return sqrt((red_diff * red_diff) + (blue_diff * blue_diff) + (green_diff * green_diff));
}


int main(){
    int num_cols, num_rows, n;
    unsigned char *image_data = stbi_load(IMAGE_TO_SEGMENT, &num_cols, &num_rows, &n, 3); // array where pixels are stored sequentially row by row
    if (!image_data) {
        fprintf(stderr, "Failed to load image\n");
        return 1;
    }

    size_t total = (size_t)num_cols * num_rows;
    size_t *parents = malloc(total * sizeof(size_t));
    statistic *stats = malloc(total * sizeof(statistic));

    for (int current_value = 0; current_value < num_cols * num_rows * 3; current_value += 3) {
        int current_pixel = (int) current_value / 3;

        statistic current_stats = {
            .mean_red = image_data[current_value],
            .mean_green = image_data[current_value + 1],
            .mean_blue = image_data[current_value + 2],
            .num_values = 1
        };

        stats[current_pixel] = current_stats;
        parents[current_pixel] = SIZE_MAX;
    }

    for (int current_value = (num_rows * num_cols * 3) - 1; current_value >= 0 ; current_value -= 3) {
        int current_pixel = (int) current_value / 3;
        int top_neighbor_location = find_top_neighbor(current_pixel, parents, num_cols);
        int left_neighbor_location = find_left_neighbor(current_pixel, parents, num_cols);

        double top_difference = top_neighbor_location ==  -1 ? SIZE_MAX : calculate_difference(find_parent(current_pixel, parents), find_parent(top_neighbor_location, parents), stats);
        double left_difference = left_neighbor_location ==  -1 ? SIZE_MAX : calculate_difference(find_parent(current_pixel, parents), find_parent(left_neighbor_location, parents), stats);

        if (top_difference > MAX_DIFFERERNCE_BETWEEN_GROUPS && left_difference > MAX_DIFFERERNCE_BETWEEN_GROUPS) {
            continue;
        }

        complete_merge(current_pixel, (top_difference < left_difference ? top_neighbor_location : left_neighbor_location), parents, stats);
    }

    unsigned char *new_image_data = malloc(total * sizeof(unsigned char) * 3); 
    for (int current_value = 0; current_value < num_rows * num_cols * 3 ; current_value += 3) {
        int current_pixel = (int) current_value / 3;
        size_t parent_index = find_parent(current_pixel, parents);
        statistic group_stats = stats[parent_index];

        new_image_data[current_value] = group_stats.mean_red;
        new_image_data[current_value + 1] = group_stats.mean_green;
        new_image_data[current_value + 2] = group_stats.mean_blue;
    }

    stbi_write_jpg(OUTPUT_FILENAME, num_cols, num_rows, 3, new_image_data, 100);
    free(parents);
    free(new_image_data);
    free(stats);

}