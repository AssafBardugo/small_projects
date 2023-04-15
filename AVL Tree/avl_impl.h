#ifndef AVL_IMPL_H_
#define AVL_IMPL_H_

#include "avl.h"


/*   ***   Implementation of the private methods of class 'avl'   ***   */


/*   ***   Set elements inorder   ***   */

template <class T>
void avl<T>::setInorderVec(node<T>* root, vec_it& it){
    
    if(root == nullptr)
        return;
    
    setInorderVec(root->left, it);
    
    root->key = *it++;
    
    if(root->key == *it)
        throw vec_contain_a_not_unique_key();
    
    setInorderVec(root->right, it);
}

template <class T>
void avl<T>::setInorderArr(node<T>* root, T* arr, int& index){
    
    if(root == nullptr)
        return;
    
    setInorderArr(root->left, arr, index);
    
    root->key = arr[index++];
    
    if(root->key == arr[index])
        throw arr_contain_a_not_unique_key();
    
    setInorderArr(root->right, arr, index);
}


/*   ***   insert & remove Auxiliary Functions   ***   */

template <class T>
AVLstatus avl<T>::insertAux(node<T>* iter, const T& element){

    // when we got to a leaf:
    if(iter == nullptr){
        return AVL_ADD_HERE;
    }

    if(element < iter->key){

        switch(insertAux(iter->left, element)){
                
            case AVL_SUCCESS:
            case AVL_WAS_ROLLING:
                iter->updateWeight();
                                        return AVL_SUCCESS;
            case AVL_ADD_HERE:
                try{
                    iter->left = new node<T>(element);
                }
                catch(std::bad_alloc&){
                    throw AVL_Allocation_error();
                }
            case AVL_WAS_HEIGHT_UPDATE:
                iter->updateWeight();
                                        return updateHeight(iter);
            default:
                                        return AVL_FAILURE;
        }
    }

    else if(!(element == iter->key)){

        switch(insertAux(iter->right, element)){

            case AVL_SUCCESS:
            case AVL_WAS_ROLLING:
                iter->updateWeight();
                                        return AVL_SUCCESS;
            case AVL_ADD_HERE:
                try{
                    iter->right = new node<T>(element);
                }
                catch(std::bad_alloc&){
                    throw AVL_Allocation_error();
                }
            case AVL_WAS_HEIGHT_UPDATE:
                iter->updateWeight();
                                        return updateHeight(iter);
            default:
                                        return AVL_FAILURE;
        }
    }
    
    // if 'element' already exist:
    return AVL_FAILURE;
}


template <class T>
AVLstatus avl<T>::removeLeaf(node<T>* iter, T leaf){
    
    node<T>* to_delete = NULL;
    
    if(leaf < iter->key){
        switch(removeLeaf(iter->left, leaf)){
                
            case AVL_REMOVE_HERE:
                to_delete = iter->left;
                iter->left = NULL;
                delete to_delete;
                
            case AVL_WAS_HEIGHT_UPDATE:
            case AVL_WAS_ROLLING:
                iter->updateWeight();
                                        return updateHeight(iter);
            default:
                iter->updateWeight();
                                        return AVL_SUCCESS;
        }
    }
    
    else if(!(leaf == iter->key)){
        switch(removeLeaf(iter->right, leaf)){
                
            case AVL_REMOVE_HERE:
                to_delete = iter->right;
                iter->right = NULL;
                delete to_delete;
                
            case AVL_WAS_HEIGHT_UPDATE:
            case AVL_WAS_ROLLING:
                iter->updateWeight();
                                        return updateHeight(iter);
            default:
                iter->updateWeight();
                                        return AVL_SUCCESS;
        }
    }
    
    return AVL_REMOVE_HERE;
}


template <class T>
const T& avl<T>::selectAux(node<T>* iter, int index) const {
    
    if(iter->w_left() > index - 1){
        
        if(iter->left == nullptr)
            return min();
        
        return selectAux(iter->left, index);
    }
    
    if(iter->w_left() < index - 1){
        
        if(iter->right == nullptr)
            return max();
        
        return selectAux(iter->right, index - iter->w_left() - 1);
    }
    
    return iter->key;
}


template <class T>
node<T>* avl<T>::find(const T& key) const {
    
    node<T>* iter = root;
    
    while(iter){
        if(key == iter->key)
            return iter;
        
        if(key < iter->key)
            iter = iter->left;
        else
            iter = iter->right;
    }
    return NULL;
}


/*   ***   Height balance of AVL   ***   */

template <class T>
AVLstatus avl<T>::updateHeight(node<T>* iter){
    
    int old_height = iter->height;
    int balance_f = balanceFactor(iter);
    
    if(balance_f == 2){
        switch(balanceFactor(iter->left)){
                
        case -1:    // LR-rolling. which is RR(left son) + LL
            genericRollingPart(iter->left);
            swapSons(iter->left->right);
            swapSons(iter->left);
                
        default:    // LL-rolling
            swapSons(iter);
            swapSons(iter->right);
            genericRollingPart(iter);
        }
    }
    
    if(balance_f == -2){
        switch(balanceFactor(iter->right)){
                
        case 1:     // RL-rolling. which is LL(right son) + RR
            swapSons(iter->right);
            swapSons(iter->right->right);
            genericRollingPart(iter->right);
                
        default:    // RR-rolling
            genericRollingPart(iter);
            swapSons(iter->right);
            swapSons(iter);
        }
    }
    
    iter->height = 1 + maxHeight<T>(iter->left, iter->right);
    
    if(std::abs(balance_f) > 1){
        return AVL_WAS_ROLLING;
    }
    
    if(iter->height != old_height){
        return AVL_WAS_HEIGHT_UPDATE;
    }
    
    return AVL_SUCCESS;
}


template <class T>
int avl<T>::balanceFactor(node<T>* iter){
    
    int left_height = 0, right_height = 0;
    
    if(iter->left)
        left_height = iter->left->height + 1;
    
    if(iter->right)
        right_height = iter->right->height + 1;
    
    return left_height - right_height;
}


template <class T>
void avl<T>::genericRollingPart(node<T>* B){
    
    std::swap(B->key, B->right->key);
    std::swap(B->left, B->right->right);
    
    B->right->height = 1 + maxHeight<T>(B->right->left, B->right->right);
    B->height = 1 + maxHeight<T>(B->left, B->right);
    
    B->right->updateWeight();
    B->updateWeight();
}


template <class T>
void avl<T>::swapSons(node<T>* father){
    std::swap(father->left, father->right);
}


/*   ***   Build almost-complete tree   ***   */

template <class T>
void avl<T>::buildAlmostCompleteTree(size_t size){
    
    assert(this->root == nullptr);
    
    int tree_height = 0;
    
    while(std::pow(2, tree_height + 1) < size + 1){
        
        tree_height++;
    }
    
    this->root = buildCompleteTree(tree_height);

    int num_to_remove = std::pow(2, tree_height + 1) - (size + 1);
    
    removeLeaves(&(this->root), num_to_remove, tree_height);
    
    initHeightAndWeight(this->root);
}

template <class T>
node<T>* avl<T>::buildCompleteTree(int height){
    
    if(height == -1)
        return nullptr;
    
    node<T>* _root = nullptr;
    try{
        _root = new node<T>;
    }
    catch(std::bad_alloc&){
        throw AVL_Allocation_error();
    }
    
    _root->left = buildCompleteTree(height - 1);
    _root->right = buildCompleteTree(height - 1);
    
    return _root;
}

template <class T>
void avl<T>::removeLeaves(node<T>** _root, int& num_to_remove, int root_height){
    
    if(num_to_remove == 0){
        return;
    }
    
    if(root_height == 0){
        
        node<T>* temp = *_root;
        *_root = NULL;
        delete temp;
        num_to_remove--;
        return;
    }
    
    removeLeaves(&((*_root)->right), num_to_remove, root_height - 1);
    removeLeaves(&((*_root)->left), num_to_remove, root_height - 1);
}

template <class T>
void avl<T>::initHeightAndWeight(node<T>* _root){
    
    if(_root == NULL)
        return;
    
    initHeightAndWeight(_root->left);
    initHeightAndWeight(_root->right);
    
    _root->height = 1 + maxHeight<T>(_root->left, _root->right);
    _root->updateWeight();
}


#endif /* AVL_IMPL_H_ */
