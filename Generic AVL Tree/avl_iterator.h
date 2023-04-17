#ifndef AVL_ITERATOR_H_
#define AVL_ITERATOR_H_

#include <stack>
#include <cassert>
#include "avl_node.h"
#include "avl_excep.h"


/*   ***   Const-Iterator that traverses the tree inorder   ***   */

template <class T>
class avl_iterator{
    
    using node_ptr = const node<T>*;
    
protected:
    std::stack<node_ptr> path;
    node_ptr current;
    node_ptr avl_root;
    
    avl_iterator(node_ptr root);
    
public:
    avl_iterator& operator++();
    
    avl_iterator operator++(int);
    
    const T& operator*() const;
    
    bool operator==(const avl_iterator& iter) const;
    
    bool operator!=(const avl_iterator& iter) const;
    
    void init_for_begin();
};



template <class T>
avl_iterator<T>::avl_iterator(node_ptr root) : current(nullptr), avl_root(root){
}


template <class T>
avl_iterator<T>& avl_iterator<T>::operator++(){
    
    if(current == nullptr)
        throw null_iterator<T>(avl_root);
    
    if(current->right){
        
        current = current->right;
        
        while(current->left){
            
            path.push(current);
            
            current = current->left;
        }
    }
    else if(!path.empty()){
        
        current = path.top();
        
        path.pop();
    }
    else{
        current = nullptr;
    }
    
    return *this;
}


template <class T>
avl_iterator<T> avl_iterator<T>::operator++(int){
    
    avl_iterator ret_val = *this;
    
    this->operator++();
    
    return ret_val;
}


template <class T>
const T& avl_iterator<T>::operator*() const{
    
    if(current == nullptr)
        throw null_iterator<T>(avl_root);
    
    return current->key;
}


template <class T>
bool avl_iterator<T>::operator==(const avl_iterator& iter) const{
    
    return this->current == iter.current;
}


template <class T>
bool avl_iterator<T>::operator!=(const avl_iterator& iter) const{
    
    return !(*this == iter);
}


template <class T>
void avl_iterator<T>::init_for_begin(){
    
    while(!path.empty())
        path.pop();
            
    if((current = avl_root) == nullptr)
        return;
    
    while(current->left){
        
        path.push(current);
        
        current = current->left;
    }
}


#endif /* AVL_ITERATOR_H_ */
