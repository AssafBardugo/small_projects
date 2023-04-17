#ifndef AVL_EXCEP_H_
#define AVL_EXCEP_H_

#include <exception>
#include <cstdbool>
#include "avl_node.h"


class avl_exceptions : public std::exception {};


template <class T>
class null_iterator : public avl_exceptions {
/*
 Throw from:
        class avl_iterator: operator*(), operator++()

 Can be thrown following a call to:
        avl::iterator::operator*()
        avl::iterator::operator*(int)
        avl::iterator::operator++()
*/
    const node<T>* iter_root;
    
public:
    null_iterator(const node<T>* avl_root) : iter_root(avl_root){}
    
    const char* what() const noexcept{
        
        if(iter_root == nullptr)
            return "The tree is empty or the iterator was not initialized with begin().";
        
        return "The iterator is equal to end().";
    }
};



template <class T>
class non_unique_key : public avl_exceptions {
/*
 Throw from:
        setInorderVec(), setInorderArr()

 Can be thrown following a call to:
        avl<T>& operator=(const avl& src);
        avl(vector<T> elements, bool is_sorted);
        avl(T* elem_arr, int size, bool is_sorted);
*/
    const T& key;
    bool from_arr;
    
public:

    non_unique_key(const T& key, bool is_arr = false) : key(key), from_arr(is_arr){}
        
    const char* what() const noexcept{
        
        if(!from_arr) return "The vector contains a non-unique key, which is illegal in AVL.";
        
        return "The array contains a non-unique key, which is illegal in AVL.";
    }
    
    const T& getKey(){
        return key;
    }
};



class tree_is_empty : public avl_exceptions {
/*
 Throw from:
        select(), min(), max()

 Can be thrown following a call to:
        select(), min(), max()
*/
};



template <class T>
class element_not_exist : public avl_exceptions {
/*
 Throw from:
        remove(), rank()

 Can be thrown following a call to:
        remove(), rank()
*/
    const T& element;
    bool rank_called;
    
public:

    element_not_exist(const T& element, bool is_rank = false)
            : element(element), rank_called(is_rank){}
        
    const char* what() const noexcept{
             
        if(!rank_called)
            return "The remove() method received an element that is not in the tree.";
        
        return "The rank() method received an element that is not in the tree and therefore has no rank.";
    }
    
    const T& getElement(){
        return element;
    }
};


#endif /* AVL_EXCEP_H_ */
