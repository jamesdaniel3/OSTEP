// TODO: 
    // implement b-search lookups
    // clean up code (are there any dead paths?)
    // remove helper functions that we do not need

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define M 25

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

int steal_key_from_neighbor(tree_node* node, size_t child_index) {
    // I might rather break this into steal from left and right
    if (
        child_index > 0 && 
        node->children[child_index - 1]->num_keys > M / 2
    ) {
        // steal from left neighbor
        tree_node* child_left_neighbor = node->children[child_index - 1];
        tree_node* child_node = node->children[child_index];
        
        // update child node
        memmove(
            child_node->keys + 1,
            child_node->keys,
            sizeof(int) * child_node->num_keys
        );
        child_node->keys[0] = node->keys[child_index - 1];
        child_node->num_keys++;

        if(child_left_neighbor->num_children != 0){
            memmove(
                child_node->children + 1,
                child_node->children,
                sizeof(tree_node *) * child_node->num_children
            );
            
            child_node->children[0] = child_left_neighbor->children[child_left_neighbor->num_children - 1];
            child_node->num_children++;
        } 

        // update parent node
        node->keys[child_index - 1] = child_left_neighbor->keys[child_left_neighbor->num_keys - 1];

        // update neighbor node
        child_left_neighbor->num_keys--;
        
        if (child_left_neighbor->num_children != 0){
            child_left_neighbor->num_children--;
        }

        return 1;
    }

    if (
        child_index < node->num_children - 2 && 
        node->children[child_index + 1]->num_keys > M / 2
    ){
        // steal from right neighbor
        tree_node* child_right_neighbor = node->children[child_index + 1];
        tree_node* child_node = node->children[child_index];
        
        // update child node
        child_node->keys[child_node->num_keys] = node->keys[child_index - 1];
        child_node->num_keys++;

        if(child_right_neighbor->num_children != 0){
            child_node->children[child_node->num_children] = child_right_neighbor->children[0];
            child_node->num_children++;
        } 

        // update parent node
        node->keys[child_index - 1] = child_right_neighbor->keys[0];

        // update neighbor node
        memmove(
            child_right_neighbor->keys,
            child_right_neighbor->keys + 1,
            sizeof(int) * (child_right_neighbor->num_keys - 1)
        );
        child_right_neighbor->num_keys--;
        
        if (child_right_neighbor->num_children != 0) {
            memmove(
                child_right_neighbor->children,
                child_right_neighbor->children + 1,
                sizeof(int) * (child_right_neighbor->num_children - 1)
            );
            child_right_neighbor->num_children--;
        }

        return 1;
    }

    return 0;
}

int merge_with_neighbor(tree_node* node, size_t child_index) {
    tree_node* child = node->children[child_index];

    if (
        child_index > 0 && 
        node->children[child_index + 1]->num_keys <= M / 2
    ){
        // merge with left neighbor
        // I think we never hit this case but written anyway
        tree_node* child_left_neighbor = node->children[child_index - 1]; 

        child_left_neighbor->keys[child_left_neighbor->num_keys] = node->keys[child_index - 1];
        child_left_neighbor->num_keys++;
        memcpy(
            child_left_neighbor->keys + child_left_neighbor->num_keys,
            child->keys,
            sizeof(int) * child->num_keys
        );
        child_left_neighbor->num_keys += child->num_keys;
        
        if (child->num_children != 0){
            memcpy(
                child_left_neighbor->children + child_left_neighbor->num_children,
                child->children,
                sizeof(tree_node *) * child->num_children
            );
            child_left_neighbor->num_keys += child->num_keys;
        }

        // update head
        memmove(
            node->keys + child_index - 1,
            node->keys + child_index,
            sizeof(int) * (node->num_keys - child_index)
        );
        node->num_keys--;

        memmove(
            node->children + child_index,
            node->children + child_index + 1,
            sizeof(tree_node *) * (node->num_children - (child_index + 1))
        );
        node->num_children--;
        
        return 1;
    }
        
    if (
        child_index < node->num_children - 2 && 
        node->children[child_index + 1]->num_keys <= M / 2
    ){
        // merge with right neighbor 
        tree_node* child_right_neighbor = node->children[child_index + 1]; 

        child->keys[child->num_keys] = node->keys[child_index - 1];
        child->num_keys++;
        memcpy(
            child->keys + child->num_keys,
            child_right_neighbor->keys,
            sizeof(int) * child_right_neighbor->num_keys
        );
        child->num_keys += child_right_neighbor->num_keys;
        
        if (child_right_neighbor->num_children != 0){
            memcpy(
                child->children + child->num_children,
                child_right_neighbor->children,
                sizeof(tree_node *) * child_right_neighbor->num_children
            );
            child->num_keys += child_right_neighbor->num_keys;
        }

        // update head
        memmove(
            node->keys + child_index,
            node->keys + child_index + 1,
            sizeof(int) * (node->num_keys - (child_index + 1))
        );
        node->num_keys--;

        memmove(
            node->children + child_index + 1,
            node->children + child_index + 2,
            sizeof(tree_node *) * (node->num_children - (child_index + 2))
        );
        node->num_children--;
        
        return 1;
    }

    return 0;
}

void tree_delete(tree_node* head, int target){
    for (size_t i = 0; i < head->num_keys; i++){
        if (
            head->num_children > 0 && 
            head->children[i]->num_keys < M / 2
        ){
            int key_successfully_stolen = steal_key_from_neighbor(head, i);

            if (!key_successfully_stolen) {
                merge_with_neighbor(head, i);
            }
        }

        if (head->keys[i] == target) {
            // handle delete
            if (head->num_children == 0){
                memmove(
                    head->keys + i,
                    head->keys + (i + 1),
                    sizeof(int) * (head->num_keys - (i + 1))
                );
                head->num_keys--;
                return;
            }

            // handle the recursive case
            tree_node* child_to_recurse = head->children[i];
            head->keys[i] = child_to_recurse->keys[child_to_recurse->num_keys - 1];
            return tree_delete(child_to_recurse, child_to_recurse->keys[child_to_recurse->num_keys - 1]);
        }

        if (head->keys[i] > target) {
            if (head->num_children == 0) {
                return;
            }

            tree_delete(head->children[i], target);
        }
    }

    if (
        head->num_children > 0 && 
        head->children[head->num_keys]->num_keys < M / 2
    ) {
        int key_successfully_stolen = steal_key_from_neighbor(head, head->num_keys);

        if (!key_successfully_stolen) {
            merge_with_neighbor(head, head->num_keys);
        }
    }
    if (head->num_children > 0){
        return tree_delete(head->children[head->num_children - 1], target);
    }
}

int tree_search(tree_node* head, int target){
    for(size_t i = 0; i < head->num_keys; i++){
        if(head->keys[i] == target){
            return 1;
        }

        if(head->keys[i] > target){
            if (head->num_children == 0){
                return 0;
            }

            return tree_search(head->children[i], target);
        }
    }
    
    if (head->num_children == 0){
        return 0;
    }

    return tree_search(head->children[head->num_children - 1], target);
}
