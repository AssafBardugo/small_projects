#ifndef AVL_H_
#define AVL_H_

#include <cstdbool>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <vector>
#include "avl_node.h"
#include "avl_iterator.h"
#include "avl_excep.h"

using std::vector;

namespace AVL {
    enum status {
        SUCCESS,
        FAILURE,
        ADD_HERE,
        REMOVE_HERE,
        WAS_ROLLING,
        WAS_HEIGHT_UPDATE
    };
}

/*
    THE CLASS 'T' MUST SUPPORT:
        - Default C'tor
        - Copy C'tor
        - Assuming operator
        - Operators: <, ==
*/

template <class T>
class avl {
    struct node<T>* root;
    
public:
    avl();
    ~avl();
    avl(const avl& src);
    avl<T>& operator=(const avl& src);
    
// Constractors:
    /* Build an AVL Tree in O(size) if is_sorted get true */
    avl(vector<T> elements, bool is_sorted = false);
    avl(T* elem_arr, int size, bool is_sorted = false);
    
// Operations:
    AVL::status insert(const T element);
    AVL::status remove(const T element);
    bool contains(const T& element) const;
    
    int rank(const T& key) const;
    const T& select(int index) const;
    
    const T& min() const;
    const T& max() const;
    vector<T> getAll() const;
    
// const-iterator:
    class iterator : public avl_iterator<T>{
    public:
        iterator();
        iterator(node<T>* root);
    };
    iterator begin() noexcept;
    iterator end();
    
// Tree Traversals (for read-only use):
    template <class FuncObj>
    void inorder(FuncObj& func) const;
    
    template <class FuncObj>
    void preorder(FuncObj& func) const;
    
    template <class FuncObj>
    void postorder(FuncObj& func) const;
    
private:
    using vec_iter = typename vector<T>::iterator;
    
// Set elements inorder:
    void setInorderVec(node<T>* root, vec_iter& it);
    void setInorderArr(node<T>* root, T* arr, int& index);
    
// Auxiliary Functions:
    node<T>* find(const T& key) const;
    AVL::status insertAux(node<T>* iter, const T& element);
    AVL::status removeLeaf(node<T>* iter, T leaf);
    const T& selectAux(node<T>* iter, int index) const;

// Height balance:
    AVL::status updateHeight(node<T>* iter);
    int balanceFactor(node<T>* iter);
    void genericRollingPart(node<T>* B);
    void swapSons(node<T>* father);

// Build almost-complete tree:
    void buildAlmostCompleteTree(size_t size);
    node<T>* buildCompleteTree(int height);
    void removeLeaves(node<T>** _root, int& num_to_remove, int root_height);
    void initHeightAndWeight(node<T>* _root);
    
// Tree Traversals Auxiliary:
    template <typename FuncObj>
    void inorderAux(FuncObj& func, node<T>* root) const;
    template <class FuncObj>
    void preorderAux(FuncObj& func, node<T>* root) const;
    template <class FuncObj>
    void postorderAux(FuncObj& func, node<T>* root) const;
};


template <class T>
avl<T>::avl() : root(nullptr){
}

template <class T>
avl<T>::~avl(){
    delete root;
}

template <class T>
avl<T>::avl(const avl& src) : avl(src.getAll(), true){
}

template <class T>
avl<T>& avl<T>::operator=(const avl& src){
    
    if(this == &src){
        return *this;
    }
    
    node<T>* to_delete = root;
    root = nullptr;
    delete to_delete;
    
    vector<T> copy_elem = src.getAll();
    this->buildAlmostCompleteTree(copy_elem.size());
    
    vec_iter iter = copy_elem.begin();
    this->setInorderVec(root, iter);
    
    return *this;
}


/*   ***   Constructors   ***   */

template <class T>
avl<T>::avl(vector<T> elements, bool is_sorted) : avl(){

    this->buildAlmostCompleteTree(elements.size());

    if(!is_sorted){
        std::sort(elements.begin(), elements.end());
    }

    vec_iter iter = elements.begin();
    this->setInorderVec(root, iter);
}


// compare func for qsort.
template <class T>
int FuncCompare(const void* a, const void* b){

    if(*(T*)a < *(T*)b)
        return -1;

    bool is_equal = (*(T*)a == *(T*)b);

    return 1 - is_equal;
}

template <class T>
avl<T>::avl(T* elem_arr, int size, bool is_sorted) : avl(){

    this->buildAlmostCompleteTree(size);

    if(!is_sorted){
        qsort(elem_arr, size, sizeof(T), FuncCompare<T>);
    }

    int index = 0;
    this->setInorderArr(root, elem_arr, index);
}


/*   ***   Operations   ***   */

template <class T>
AVL::status avl<T>::insert(const T element){

    // if the tree is empty:
    if(root == nullptr){
        
        root = new node<T>(element);
        return AVL::SUCCESS;
    }

    return insertAux(root, element);
}


template <class T>
AVL::status avl<T>::remove(const T element){
    
    node<T>* to_remove = find(element);
    
    if(to_remove == nullptr){
        throw element_not_exist<T>(element);
    }
    
    if(to_remove->left && to_remove->right){
        
        node<T>* following = to_remove->right;
        
        while(following->left){
            
            following = following->left;
        }
        
        T save_following_key = following->key;
        AVL::status ret_val = remove(following->key);
        
        node<T>* change_to_following = find(element);
        change_to_following->key = save_following_key;
        
        return ret_val;
    }
    
    if(to_remove->left){
        
        std::swap(to_remove->key, to_remove->left->key);
        std::swap(to_remove->left, to_remove->right);
    }
    else if(to_remove->right){
        
        std::swap(to_remove->key, to_remove->right->key);
        std::swap(to_remove->left, to_remove->right);
    }
    else if(root->key == element){
        
        node<T>* to_delete = root;
        root = nullptr;
        delete to_delete;
        return AVL::SUCCESS;
    }
    
    return removeLeaf(root, element);
}


template <class T>
bool avl<T>::contains(const T& element) const {
    
    return find(element) != nullptr;
}


template <class T>
int avl<T>::rank(const T& key) const {
    
    if(find(key) == nullptr){
        throw element_not_exist<T>(key, true);
    }
    
    int rank = 0;
    node<T>* iter = root;
    
    while(iter){
        
        if(key == iter->key)
            break;
        
        if(key < iter->key){
            iter = iter->left;
            continue;
        }
        
        iter = iter->right;
        rank += iter->w_left() + 1;
    }
    
    return rank + iter->w_left() + 1;
}


template <class T>
const T& avl<T>::select(int index) const {
    
    if(root == nullptr)
        throw tree_is_empty();
    
    return selectAux(root, index);
}


template <class T>
const T& avl<T>::min() const {
    
    if(root == nullptr)
        throw tree_is_empty();
    
    node<T>* iter = root;
    
    while(iter->left)
        iter = iter->left;
    
    return iter->key;
}


template <class T>
const T& avl<T>::max() const {
    
    if(root == nullptr)
        throw tree_is_empty();
    
    node<T>* iter = root;
    
    while(iter->right)
        iter = iter->right;
    
    return iter->key;
}


// Auxiliary struct for using inorderAux in getAll()
template <class T>
struct vec_push{
    
    vector<T> vec;
    
    void operator()(T key){
        vec.push_back(key);
    }
};

template <class T>
vector<T> avl<T>::getAll() const {
    
    vec_push<T> ret_val;
    
    inorderAux<vec_push<T>>(ret_val, root);
    
    return ret_val.vec;
}


/*   ***   iterator functions   ***   */

template <class T>
avl<T>::iterator::iterator() : avl_iterator<T>(nullptr){
}

template <class T>
avl<T>::iterator::iterator(node<T>* root) : avl_iterator<T>(root){
}

template <class T>
typename avl<T>::iterator avl<T>::begin() noexcept{
    
    iterator ret_val(this->root);
    
    ret_val.init_for_begin();
    
    return ret_val;
}

template <class T>
typename avl<T>::iterator avl<T>::end(){
    
    return iterator();
}


/*   ***   Tree Traversals   ***   */

template <class T>
template <class FuncObj>
void avl<T>::inorder(FuncObj& func) const {
    
    inorderAux(func, root);
}


template <class T>
template <class FuncObj>
void avl<T>::preorder(FuncObj& func) const {
    
    preorderAux(func, root);
}


template <class T>
template <class FuncObj>
void avl<T>::postorder(FuncObj& func) const {
    
    postorderAux(func, root);
}


/*   ***   Implementation of the private methods   ***   */


/*   ***   Set elements inorder   ***   */

template <class T>
void avl<T>::setInorderVec(node<T>* root, vec_iter& it){
    
    if(root == nullptr)
        return;
    
    vector<T> v;
    
    setInorderVec(root->left, it);
    
    root->key = *it++;
    
    if(it != v.end() && root->key == *it){
        
        throw non_unique_key<T>(*it);
    }
    
    setInorderVec(root->right, it);
}

template <class T>
void avl<T>::setInorderArr(node<T>* root, T* arr, int& index){
    
    if(root == nullptr)
        return;
    
    setInorderArr(root->left, arr, index);
    
    if(index > 0 && arr[index - 1] == arr[index]){
        
        throw non_unique_key<T>(arr[index], true);
    }
    
    root->key = arr[index++];
    
    setInorderArr(root->right, arr, index);
}


/*   ***   insert & remove Auxiliary Functions   ***   */

template <class T>
AVL::status avl<T>::insertAux(node<T>* iter, const T& element){

    // when we got to a leaf:
    if(iter == nullptr){
        return AVL::ADD_HERE;
    }

    if(element < iter->key){

        switch(insertAux(iter->left, element)){
                
            case AVL::SUCCESS:
            case AVL::WAS_ROLLING:
                iter->updateWeight();
                                        return AVL::SUCCESS;
            case AVL::ADD_HERE:
                iter->left = new node<T>(element);
                
            case AVL::WAS_HEIGHT_UPDATE:
                iter->updateWeight();
                                        return updateHeight(iter);
            default:
                                        return AVL::FAILURE;
        }
    }

    else if(!(element == iter->key)){

        switch(insertAux(iter->right, element)){

            case AVL::SUCCESS:
            case AVL::WAS_ROLLING:
                iter->updateWeight();
                                        return AVL::SUCCESS;
            case AVL::ADD_HERE:
                iter->right = new node<T>(element);
                
            case AVL::WAS_HEIGHT_UPDATE:
                iter->updateWeight();
                                        return updateHeight(iter);
            default:
                                        return AVL::FAILURE;
        }
    }
    
    // if 'element' already exist:
    return AVL::FAILURE;
}


template <class T>
AVL::status avl<T>::removeLeaf(node<T>* iter, T leaf){
    
    node<T>* to_delete = nullptr;
    
    if(leaf < iter->key){
        switch(removeLeaf(iter->left, leaf)){
                
            case AVL::REMOVE_HERE:
                to_delete = iter->left;
                iter->left = nullptr;
                delete to_delete;
                
            case AVL::WAS_HEIGHT_UPDATE:
            case AVL::WAS_ROLLING:
                iter->updateWeight();
                                        return updateHeight(iter);
            default:
                iter->updateWeight();
                                        return AVL::SUCCESS;
        }
    }
    
    else if(!(leaf == iter->key)){
        switch(removeLeaf(iter->right, leaf)){
                
            case AVL::REMOVE_HERE:
                to_delete = iter->right;
                iter->right = nullptr;
                delete to_delete;
                
            case AVL::WAS_HEIGHT_UPDATE:
            case AVL::WAS_ROLLING:
                iter->updateWeight();
                                        return updateHeight(iter);
            default:
                iter->updateWeight();
                                        return AVL::SUCCESS;
        }
    }
    
    return AVL::REMOVE_HERE;
}


/*   ***   select & contains Auxiliary Functions   ***   */

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
    return nullptr;
}


/*   ***   Height balance of AVL   ***   */

template <class T>
AVL::status avl<T>::updateHeight(node<T>* iter){
    
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
        return AVL::WAS_ROLLING;
    }
    
    if(iter->height != old_height){
        return AVL::WAS_HEIGHT_UPDATE;
    }
    
    return AVL::SUCCESS;
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
    
    node<T>* _root = new node<T>;
    
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
        *_root = nullptr;
        delete temp;
        num_to_remove--;
        return;
    }
    
    removeLeaves(&((*_root)->right), num_to_remove, root_height - 1);
    removeLeaves(&((*_root)->left), num_to_remove, root_height - 1);
}

template <class T>
void avl<T>::initHeightAndWeight(node<T>* _root){
    
    if(_root == nullptr)
        return;
    
    initHeightAndWeight(_root->left);
    initHeightAndWeight(_root->right);
    
    _root->height = 1 + maxHeight<T>(_root->left, _root->right);
    _root->updateWeight();
}



/*   ***   Tree Traversals Auxiliary   ***   */

template <class T>
template <typename FuncObj>
void avl<T>::inorderAux(FuncObj& func, node<T>* root) const {
    
    if(root == nullptr)
        return;
    
    inorderAux(func, root->left);
    
    func(root->key);
    
    inorderAux(func, root->right);
}


template <class T>
template <typename FuncObj>
void avl<T>::preorderAux(FuncObj& func, node<T>* root) const {
    
    if(root == nullptr)
        return;
    
    func(root->key);
    
    preorderAux(func, root->left);
    preorderAux(func, root->right);
}


template <class T>
template <typename FuncObj>
void avl<T>::postorderAux(FuncObj& func, node<T>* root) const {
    
    if(root == nullptr)
        return;
    
    postorderAux(func, root->left);
    postorderAux(func, root->right);
    
    func(root->key);
}


#endif  /* AVL_H_ */
