/* PROPERTIES OF A B-TREE
- All leaf nodes are at the same level 
- The keys of each node are stored in ascending order 
- Excluding the root node, all non-leaf nodes should have at least m/2 children
- Excluding the root node, all nodes should have at least m/2 - 1 keys 
- If the root node is a leaf node, it will have at least one key. 
- IF the root node is a non-leaf node, than it will have at least 2 children and 
  at least 1 key
- A non-leaf node should have 1 more children than it does key values (excluding NULL values)

-  b trees are self balanced (bubble up operation I think)
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define M 3

typedef struct tree_node tree_node;
struct tree_node {
    int* keys;
    size_t num_keys;
    tree_node** children;
    size_t num_children;
};

void print_node_info(tree_node* node) {
    printf("--------- Node Display---------\n");
    printf("%zu Keys: [ ", node->num_keys);
    for (size_t i = 0; i < node->num_keys; i++){
        printf("%d ", node->keys[i]);
    }
    printf("]\n");
    printf("%zu Children: [ ", node->num_children);
    for (size_t i = 0; i < node->num_children; i++){
        printf("%p ", node->children[i]);
    }
    printf("]\n");
}

void walk_tree(tree_node* head){
    print_node_info(head);
    for (size_t i = 0; i < head->num_children; i++){
        walk_tree(head->children[i]);
    }
}

tree_node* init_node(){
    tree_node* new_node = malloc(sizeof(tree_node));
    assert(new_node != NULL);

    int* keys = calloc(M, sizeof(int));
    assert(keys != NULL);

    tree_node** children = calloc(M + 1, sizeof(tree_node*));
    assert(children != NULL);

    new_node->keys = keys;
    new_node->num_keys = 0;
    new_node->children = children;
    new_node->num_children = 0;

    return new_node;
}

void destroy_node(tree_node* node){
    free(node->keys);
    free(node->children);
    free(node);
}

void destroy_tree(tree_node* head){
    for(size_t i = 0; i < head->num_children; i++) {
        destroy_tree(head->children[i]);
    }
    destroy_node(head);
}

size_t add_key_to_keylist(tree_node* node, int val) {           
    for (size_t i = 0; i < node->num_keys; i++){
        if (node->keys[i] > val) {
            memmove(
                node->keys + i + 1,
                node->keys + i,
                sizeof(int) * (node->num_keys - i)
            );
            node->keys[i] = val;
            node->num_keys++;
            return i;
        }
    }
    
    node->keys[node->num_keys] = val;
    node->num_keys++;
    return node->num_keys - 1;
}

void split_non_root_node(tree_node* parent, tree_node* full_child){
    int midpoint = M / 2;

    // create sibling node
    tree_node* new_node = init_node();
    new_node->num_keys = M / 2;
    if (full_child->num_children != 0){
       new_node->num_children = M / 2 + 1;
       memcpy(
           new_node->children,
           full_child->children + (midpoint + 1),
           sizeof(tree_node *) * (full_child->num_children - (midpoint + 1))
       );
    }
    
    memcpy(
       new_node->keys,
       full_child->keys + (midpoint + 1),
       sizeof(int) * (full_child->num_keys - (midpoint + 1))
    );

    size_t location = add_key_to_keylist(parent, full_child->keys[midpoint]);
    memmove(
       parent->children + location + 2,
       parent->children + location + 1,
       sizeof(tree_node *) * (parent->num_children - (location + 1))  
    );
    parent->children[location + 1] = new_node;
    parent->num_children++;

    // clear old node
    full_child->num_keys = M / 2;
    if (full_child->num_children != 0){
        full_child->num_children = M / 2 + 1;
    }
}

void tree_insert(tree_node* head, int new_val){
    // check if full and grow if so, only catches the root 
    if (head->num_keys == M) {
        int midpoint = M / 2;

        // create sibling node and new head
        tree_node* new_left_sibling = init_node();
        tree_node* new_right_sibling = init_node();
        
        // copy children
        if (head->num_children != 0){
           new_left_sibling->num_children = M / 2 + 1;
           memcpy(
               new_left_sibling->children,
               head->children,
               sizeof(tree_node *) * (head->num_children - (midpoint + 1))
           );

           new_right_sibling->num_children = M / 2 + 1;
           memcpy(
               new_right_sibling->children,
               head->children + (midpoint + 1),
               sizeof(tree_node *) * (head->num_children - (midpoint + 1))
           );
        }
        
        head->children[0] = new_left_sibling;
        head->children[1] = new_right_sibling;
        head->num_children = 2;

        // copy keys
        new_right_sibling->num_keys = M / 2;
        memcpy(
           new_right_sibling->keys,
           head->keys + (midpoint + 1),
           sizeof(int) * (head->num_keys - (midpoint + 1))
        );

        new_left_sibling->num_keys = M / 2;
        memcpy(
           new_left_sibling->keys,
           head->keys,
           sizeof(int) * (head->num_keys - (midpoint + 1))
        );

        head->num_keys = 0;
        add_key_to_keylist(head, head->keys[midpoint]);
    }

    if (head->num_children > 0) {
        // find the proper node for insertion
        for (size_t i = 0; i < head->num_keys; i++) {
            if (head->keys[i] < new_val) {  // should this be <= ? 
                continue;
            }

            tree_insert(head->children[i], new_val);
            
            // if the child is full, add a sibling
            if (head->children[i]->num_keys == M) {
               split_non_root_node(head, head->children[i]); 
            }

            return;
        }

        tree_insert(head->children[head->num_keys], new_val);
        
        if(head->children[head->num_keys]->num_keys == M) {
            split_non_root_node(head, head->children[head->num_keys]);
        }

        return;
    }
    
    add_key_to_keylist(head, new_val);
}


void tree_delete(tree_node* head, int target){
    // also a recursive algo
    // find the target
    // once found, delete
    // if the node now has fewer than the min number of nodes (M // 2)
        // try to steal a node from one of the sibling nodes 
        // if both sibling nodes are at min size
            // you can join sibling nodes (make sure to bring a val down from parent)
        // if the node you are deleting from has children, take a val from one of the children to replace 
}



int search_tree(tree_node* head, int target){
    for(size_t i = 0; i < head->num_keys; i++){
        if(head->keys[i] == target){
            return 1;
        }

        if(head->keys[i] > target){
            if (head->num_children == 0){
                return 0;
            }

            return search_tree(head->children[i], target);
        }
    }
    
    if (head->num_children == 0){
        return 0;
    }

    return search_tree(head->children[head->num_children - 1], target);
}


int main(){
    tree_node* head = init_node();
    int test_values[8] = {10, 20, 5, 6, 12, 30, 7, 17};

    for(size_t i = 0; i < 8; i++){
        tree_insert(head, test_values[i]);
    }
    walk_tree();
    destroy_tree(head);
}
