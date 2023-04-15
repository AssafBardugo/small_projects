
#include <iostream>
#include <string>
#include "avl.h"
#include "avl_impl.h"

void print_iter(avl<int>& tree, std::string lable);

int main() {
    
    int arr[5] = {5,4,3,2,1};
    
    std::vector<int> vec = {2,1,3,5,4};
    
    avl<int> empty_tree;
    avl<int> vec_tree(vec);
    avl<int> arr_tree(arr, 5);
    
    
    // USING INSERT:
    
    empty_tree.insert(7);
    empty_tree.insert(8);
    
    arr_tree.insert(7);
    arr_tree.insert(8);
    
    vec_tree.insert(7);
    vec_tree.insert(8);
    
    print_iter(arr_tree, "arr_tree:");
    print_iter(vec_tree, "vec_tree:");
    print_iter(empty_tree, "empty_tree:");
    
    assert(arr_tree.insert(4) == AVL_FAILURE);
    assert(vec_tree.insert(7) == AVL_FAILURE);
    assert(empty_tree.insert(4) == AVL_SUCCESS);
    
    
    // USING REMOVE:
    
    empty_tree.remove(8);
    arr_tree.remove(4);
    vec_tree.remove(3);
    
    print_iter(arr_tree, "arr_tree:");
    print_iter(vec_tree, "vec_tree:");
    print_iter(empty_tree, "empty_tree:");
    
    empty_tree.remove(7);
    empty_tree.remove(4);
    
    print_iter(empty_tree, "empty_tree:");
    
    std::cout << '\n' << "SUCCEED!" << '\n' << std::endl;
    
    return 0;
}



void print_iter(avl<int>& tree, std::string lable){
    
    std::cout << lable << std::endl;
    for(avl<int>::iterator it = tree.begin(); it != tree.end(); ++it){
        
        std::cout << *it << '\t';
    }
    std::cout << "\n\n";
}
