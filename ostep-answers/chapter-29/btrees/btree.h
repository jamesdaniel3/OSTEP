#include <stdlib.h>

typedef struct tree_node tree_node;
struct tree_node {
    int* keys;
    size_t num_keys;
    tree_node** children;
    size_t num_children;
};

tree_node* init_node();
void destroy_tree(tree_node* head);

void tree_insert(tree_node* head, int new_val);
void tree_delete(tree_node* head, int target);
int tree_search(tree_node* head, int target);
