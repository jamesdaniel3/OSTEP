typedef int compare_function(void const*, void const*);
void quicksort_generic(void* base, size_t element_size, size_t num_elements, compare_function* comparator);