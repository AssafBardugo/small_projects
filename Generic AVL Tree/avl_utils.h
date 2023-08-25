#ifndef AVL_UTILITIES_H
#define AVL_UTILITIES_H

#include <vector>
#include "avl_excep.h"

enum AVL_STATUS {
    SUCCESS,
    FAILURE,
    ADD_HERE,
    REMOVE_HERE,
    WAS_ROLLING,
    WAS_HEIGHT_UPDATE
};


/*   ***   Functors for internal use of inorder   ***   */
    
template <typename T>
struct GetFunctor{
    std::vector<T> vec;
    
    void operator()(T key){
        
        vec.push_back(key);
    }
};


template <typename T, typename Comp, typename IterType>
struct SetFunctor{
    IterType first, last;
    const Comp& k_cmp;
    
    SetFunctor(IterType first, IterType last, const Comp& key_comp)
            : first(first), last(last), k_cmp(key_comp){
    }
    
    void operator()(T& key){
        
        key = *first++;

        if(first != last && (!k_cmp(key, *first)) && (!k_cmp(*first, key)))

            throw non_unique_key<T>(*first);
    }
};


#endif /* AVL_UTILITIES_H */
