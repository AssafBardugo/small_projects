#ifndef AVL_EXCEP_H_
#define AVL_EXCEP_H_

#include <exception>
#include "avl_node.h"

enum KEY_ERROR{
    NON_UNIQUE_KEY,
    KEY_NOT_EXIST,
    KEY_ALREADY_EXISTS
};

class avl_exceptions : public std::exception {};


class tree_is_empty : public avl_exceptions {
/*
 Throw from:
        select(), min(), max()

 Can be thrown following a call to:
        select(), min(), max()
*/
};


template <class T>
class avl_key_error : public avl_exceptions {
    
    const T& key;
    KEY_ERROR error_type;

public:
    avl_key_error(const T& key, KEY_ERROR type)
            : key(key), error_type(type){}

    const char* what() const noexcept{

        switch(error_type){

        case NON_UNIQUE_KEY:
            return "The vector/array contains a non-unique key, which is illegal in AVL.";
        case KEY_NOT_EXIST:
            return "The rank/remove method received an element that is not in the tree.";
        case KEY_ALREADY_EXISTS:
            return "The insert method received an element that is already in the tree.";
        default:
            return "";
        }
    }
    
    const T& getKey(){
        return key;
    }
};


template <class T>
class non_unique_key : public avl_key_error<T> {
/*
 Throw from:
        avl::SetFunctor::operator()

 Can be thrown following a call to:
        avl<T>& operator=(const avl& src);
        avl(std::vector<T>& elements, bool sorted = false);
        avl(std::vector<T>& elements, const Comp& comp, bool sorted = false);
        avl(T* elements, size_t arr_size, bool sorted = false);
        avl(T* elements, size_t arr_size, const Comp& comp, bool sorted = false);
*/    
public:
    non_unique_key(const T& key) 
            : avl_key_error<T>(key, NON_UNIQUE_KEY){}
};


template <class T>
class key_not_exist : public avl_key_error<T> {
/*
 Throw from:
        remove(), rank(), getRef()

 Can be thrown following a call to:
        remove(), rank(), getRef()
*/
public:
    key_not_exist(const T& key)
            : avl_key_error<T>(key, KEY_NOT_EXIST){}
};


template <typename T>
class key_already_exists : public avl_key_error<T> {
/*
 Throw from:
        insert()

 Can be thrown following a call to:
        insert()
*/
public:
    key_already_exists(const T& key) 
            : avl_key_error<T>(key, KEY_ALREADY_EXISTS){}
};


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


#endif /* AVL_EXCEP_H_ */
