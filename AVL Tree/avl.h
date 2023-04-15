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
typedef enum{
    AVL_SUCCESS,
    AVL_FAILURE,
    AVL_ADD_HERE,
    AVL_REMOVE_HERE, 
    AVL_WAS_ROLLING, 
    AVL_WAS_HEIGHT_UPDATE
} AVLstatus;


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
    ~avl();

// Constractors:
    avl();
    
    avl(std::vector<T> elements);
    
    /* Build an AVL Tree in O(size) if is_sorted == true */
    avl(T* elem_arr, int size, bool is_sorted = false);
    
    
// Operations:
    AVLstatus insert(const T element);
    
    AVLstatus remove(const T element);

    bool contains(const T& element) const;
    
    int rank(const T& key) const;
    const T& select(int index) const;

    const T& min() const;
    const T& max() const;

    std::vector<T> getAll() const;

    template <class FuncObj>
    void inorder(FuncObj& func) const;
    
    template <class FuncObj>
    void preorder(FuncObj& func) const;
    
    template <class FuncObj>
    void postorder(FuncObj& func) const;

    
// Iterator:
    class iterator : public avl_iterator<T>{
    public:
        iterator();
        iterator(node<T>* root);
    };
    
    iterator begin() noexcept;
    iterator end();
    
    
private:
    avl(const avl& src);                     // deleted
    avl<T>& operator=(const avl& src);       // deleted

    node<T>* find(const T& key) const;
    AVLstatus insertAux(node<T>* iter, const T& element);
    AVLstatus removeLeaf(node<T>* iter, T leaf);
    const T& selectAux(node<T>* iter, int index) const;

    AVLstatus updateHeight(node<T>* iter);
    int balanceFactor(node<T>* iter);
    void genericRollingPart(node<T>* B);
    void swapSons(node<T>* father);

    using vec_it = typename std::vector<T>::iterator;
    void setInorderVec(node<T>* root, vec_it& it);
    void setInorderArr(node<T>* root, T* arr, int& index);

    void buildAlmostCompleteTree(size_t size);
    node<T>* buildCompleteTree(int height);
    void removeLeaves(node<T>** _root, int& num_to_remove, int root_height);
    void initHeightAndWeight(node<T>* _root);
    
    // void removeAllNodes();
    // bool isTheTreeLegal();
    // void postorderTest(node* p);
};



template <class T>
avl<T>::~avl(){
    delete root;
}

/*   ***   Constructors   ***   */

template <class T>
avl<T>::avl() : root(nullptr){
}

template <class T>
avl<T>::avl(std::vector<T> elements) : avl(){

    this->buildAlmostCompleteTree(elements.size());

    std::sort(elements.begin(), elements.end());

    vec_it iter = elements.begin();
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



/*   ***   Operations   ***   */

template <class T>
AVLstatus avl<T>::insert(const T element){

    // if the tree is empty:
    if(root == nullptr){
        try{
            root = new node<T>(element);
        }
        catch(std::bad_alloc&){
            throw AVL_Allocation_error();
        }
        return AVL_SUCCESS;
    }

    return insertAux(root, element);
}


template <class T>
AVLstatus avl<T>::remove(const T element){
    
    node<T>* to_remove = find(element);
    
    if(to_remove == nullptr){
        throw element_not_exist();
    }
    
    if(to_remove->left && to_remove->right){
        
        node<T>* following = to_remove->right;
        
        while(following->left){
            
            following = following->left;
        }
        
        T save_following_key = following->key;
        remove(following->key);
        
        node<T>* change_to_following = find(element);
        change_to_following->key = save_following_key;
        
        return AVL_SUCCESS;
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
        return AVL_SUCCESS;
    }
    
    return removeLeaf(root, element);
}


template <class T>
bool avl<T>::contains(const T& element) const {
    
    return find(element) != nullptr;
}


template <class T>
int avl<T>::rank(const T& key) const {
    
    if(contains(key) == false){
        throw element_not_exist();
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


template <class T>
void getAllAux(std::vector<T>& vec, node<T>* iter){
    
    if(iter == nullptr)
        return;
    
    getAllAux(vec, iter->left);
    
    vec.push_back(iter->key);
    
    getAllAux(vec, iter->right);
}

template <class T>
std::vector<T> avl<T>::getAll() const {
    
    std::vector<T> ret_val;
    
    getAllAux<T>(ret_val, root);
    
    return ret_val;
}

template <class T>
template <class FuncObj>
void avl<T>::inorder(FuncObj& func) const {
    
    
}


template <class T>
template <class FuncObj>
void avl<T>::preorder(FuncObj& func) const {
    
}


template <class T>
template <class FuncObj>
void avl<T>::postorder(FuncObj& func) const {
    
}




#endif  /* AVL_H_ */
