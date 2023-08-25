#ifndef AVL_IMPLEMENTATION_H
#define AVL_IMPLEMENTATION_H

#include "avl.h"


/*   ***   Constructors   ***   */

template <typename T, typename Comp>
avl<T, Comp>::avl() 
        : avl(Comp()) {
}

template <typename T, typename Comp>
avl<T, Comp>::avl(const Comp& comp)
        : root(nullptr), min(nullptr), max(nullptr), tree_size(0), key_comp(comp){
}

template <typename T, typename Comp>
avl<T, Comp>::avl(const avl& src) 
        : avl(src.getAll(), src.key_comp, true){
}


template <typename T, typename Comp>
avl<T, Comp>::avl(std::vector<T> elements, bool sorted)
        : avl(elements, Comp(), sorted){
}


template <typename T, typename Comp>
avl<T, Comp>::avl(std::vector<T> elements, const Comp& comp, bool sorted)
        : avl(comp){

    buildAlmostCompleteTree(elements.size());

    if(!sorted)
        std::sort(elements.begin(), elements.end(), comp);

    SetFunctor<T, Comp, vec_iter> functor(elements.begin(), elements.end(), key_comp);
    inorderAux<SetFunctor<T, Comp, vec_iter>>(functor, root);

    updateMinAndMax();
    tree_size = elements.size();
}


template <typename T, typename Comp>
avl<T, Comp>::avl(T* elements, size_t arr_size, bool sorted) 
        : avl(elements, arr_size, Comp(), sorted){
}

template <typename T, typename Comp>
avl<T, Comp>::avl(T* elements, size_t arr_size, const Comp& comp, bool sorted) 
        : avl(comp){
    
    buildAlmostCompleteTree(arr_size);

    if(!sorted)
        std::sort(elements, elements + arr_size, comp);
    
    SetFunctor<T, Comp, T*> functor(elements, elements + arr_size, key_comp);
    inorderAux<SetFunctor<T, Comp, T*>>(functor, root);

    updateMinAndMax();
    tree_size = arr_size;
}


template <typename T, typename Comp>
avl<T, Comp>::~avl(){
    min = max = nullptr;
    delete root;
}


template <typename T, typename Comp>
avl<T, Comp>& 
avl<T, Comp>::operator=(const avl<T, Comp>& src){
    
    if(this == &src)
        return *this;
    
    min = max = nullptr;
    node<T>* to_delete = root;
    root = nullptr;
    delete to_delete;
    
    std::vector<T> copy_elem = src.getAll();
    buildAlmostCompleteTree(copy_elem.size());

    SetFunctor<T, Comp, vec_iter> functor(copy_elem.begin(), copy_elem.end(), key_comp);
    inorderAux<SetFunctor<T, Comp, vec_iter>>(functor, root);

    updateMinAndMax();
    tree_size = src.size();    
    return *this;
}


/*   ***   Operations   ***   */

template <typename T, typename Comp>
void 
avl<T, Comp>::insert(T element){

    // if the tree is empty:
    if(root == nullptr){
        
        root = new node<T>(element);
        min = max = root;
        tree_size++;
        return;
    }

    bool new_min_or_max = key_comp(element, min->key) || key_comp(max->key, element);

    if(insertAux(root, element) == FAILURE){

        throw key_already_exists<T>(element);
    }
    if(new_min_or_max)
        updateMinAndMax();

    tree_size++;
}


template <typename T, typename Comp>
void 
avl<T, Comp>::remove(const T element){
    
    node<T>* to_remove = find(element);
    
    if(to_remove == nullptr)
        throw key_not_exist<T>(element);
    
    if(to_remove->left && to_remove->right){
        
        node<T>* following = to_remove->right;
        
        while(following->left)
            following = following->left;
        
        T save_following_key = following->key;
        remove(following->key);
        
        node<T>* change_to_following = find(element);
        change_to_following->key = save_following_key;
        return;
    }

    // It checks pointers equality
    bool is_min_or_max = (to_remove == min) || (to_remove == max);
    
    if(to_remove->left){
        
        std::swap(to_remove->key, to_remove->left->key);
        std::swap(to_remove->left, to_remove->right);
    }
    else if(to_remove->right){
        
        std::swap(to_remove->key, to_remove->right->key);
        std::swap(to_remove->left, to_remove->right);
    }
    else if(keysEqual(root->key, element)){
        
        min = max = nullptr;
        tree_size--;
        node<T>* to_delete = root;
        root = nullptr;
        delete to_delete;
        return;
    }

    removeLeaf(root, element);

    if(is_min_or_max)
        updateMinAndMax();
        
    tree_size--;
}


template <typename T, typename Comp>
bool 
avl<T, Comp>::contains(const T& element) const {
    
    return find(element) != nullptr;
}


template <typename T, typename Comp>
size_t 
avl<T, Comp>::rank(const T& key) const {
    
    if(find(key) == nullptr)
        throw key_not_exist<T>(key);
    
    size_t rank = 0;
    node<T>* iter = root;
    
    while(iter){
        
        if(keysEqual(key, iter->key))
            break;
        
        if(key_comp(key, iter->key)){
            iter = iter->left;
            continue;
        }
        
        iter = iter->right;
        rank += iter->w_left() + 1;
    }
    
    return rank + iter->w_left() + 1;
}


template <typename T, typename Comp>
const T& 
avl<T, Comp>::select(size_t index) const {
    
    if(root == nullptr)
        throw tree_is_empty();
    
    return selectAux(root, index);
}


template <typename T, typename Comp>
T& 
avl<T, Comp>::getRef(const T& key){

/*
 *  When using this method, 
 *  be careful NOT to change the values that affect the 
 *  comparison between keys at this specific tree.
 */

    node<T>* iter = root;
    
    while(iter){
        if(keysEqual(key, iter->key))
            return iter->key;
        
        if(key_comp(key, iter->key))
            iter = iter->left;
        else
            iter = iter->right;
    }

    throw key_not_exist<T>(key);
}


template <typename T, typename Comp>
const T& 
avl<T, Comp>::getMin() const {
    return min->key;
}

template <typename T, typename Comp>
const T& 
avl<T, Comp>::getMax() const {
    return max->key;
}


template <typename T, typename Comp>
T 
avl<T, Comp>::popMin() {
    // TODO
    // Effective implementation is required here!
}

template <typename T, typename Comp>
T 
avl<T, Comp>::popMax() {
    // TODO
    // Effective implementation is required here!
}


template <typename T, typename Comp>
size_t 
avl<T, Comp>::size() const {
    return tree_size;
}

template <typename T, typename Comp>
bool 
avl<T, Comp>::empty() const {
    return tree_size == 0;
}


template <typename T, typename Comp>
std::vector<T> 
avl<T, Comp>::getAll() const {
    
    GetFunctor<T> ret_val;
    
    constInorderAux<GetFunctor<T>>(ret_val, root);
    
    return ret_val.vec;
}


/*   ***   iterator functions   ***   */

template <typename T, typename Comp>
avl<T, Comp>::iterator::iterator() 
        : avl_iterator<T>(nullptr){
}

template <typename T, typename Comp>
avl<T, Comp>::iterator::iterator(node<T>* root) 
        : avl_iterator<T>(root){
}

template <typename T, typename Comp>
typename avl<T, Comp>::iterator 
avl<T, Comp>::begin() noexcept{
    
    iterator ret_val(this->root);
    
    ret_val.init_for_begin();
    
    return ret_val;
}

template <typename T, typename Comp>
typename avl<T, Comp>::iterator 
avl<T, Comp>::end(){
    return iterator();
}


/*   ***   Tree Traversals   ***   */

template <typename T, typename Comp>
template <typename Functor>
void 
avl<T, Comp>::inorder(Functor& func) {
    
    inorderAux(func, root);
}


template <typename T, typename Comp>
template <typename Functor>
void 
avl<T, Comp>::preorder(Functor& func) {
    
    preorderAux(func, root);
}


template <typename T, typename Comp>
template <typename Functor>
void 
avl<T, Comp>::postorder(Functor& func) {
    
    postorderAux(func, root);
}


template <typename T, typename Comp>
template <typename Functor>
void 
avl<T, Comp>::constInorder(Functor& func) const{

    constInorderAux(func, root);
}


/*   ************   Implementation of the private methods   ************   */

template <typename T, typename Comp>
bool 
avl<T, Comp>::keysEqual(const T& k1, const T& k2) const {

    return (!key_comp(k1, k2)) && (!key_comp(k2, k1));
}

/*   ***   insert & remove Auxiliary Functions   ***   */

template <typename T, typename Comp>
AVL_STATUS
avl<T, Comp>::insertAux(node<T>* iter, T& element){

    // when we got to a leaf:
    if(iter == nullptr)
        return ADD_HERE;

    if(key_comp(element, iter->key))
        switch(insertAux(iter->left, element)){
                
        case SUCCESS:
        case WAS_ROLLING:
            iter->updateWeight();
            return SUCCESS;

        case ADD_HERE:
            iter->left = new node<T>(element);
            
        case WAS_HEIGHT_UPDATE:
            iter->updateWeight();
            return updateHeight(iter);

        default:
            return FAILURE;
        }

    if(key_comp(iter->key, element))
        switch(insertAux(iter->right, element)){

        case SUCCESS:
        case WAS_ROLLING:
            iter->updateWeight();
            return SUCCESS;

        case ADD_HERE:
            iter->right = new node<T>(element);
            
        case WAS_HEIGHT_UPDATE:
            iter->updateWeight();
            return updateHeight(iter);

        default:
            return FAILURE;
        }
    
    // if 'element' already exist:
    return FAILURE;
}


template <typename T, typename Comp>
AVL_STATUS
avl<T, Comp>::removeLeaf(node<T>* iter, T leaf){
    
    node<T>* to_delete = nullptr;
    
    if(key_comp(leaf, iter->key))
        switch(removeLeaf(iter->left, leaf)){
                
        case REMOVE_HERE:
            to_delete = iter->left;
            iter->left = nullptr;
            delete to_delete;

        case WAS_HEIGHT_UPDATE:
        case WAS_ROLLING:
            iter->updateWeight();
            return updateHeight(iter);

        default:
            iter->updateWeight();
            return SUCCESS;
        }
    
    if(key_comp(iter->key, leaf))
        switch(removeLeaf(iter->right, leaf)){
                
        case REMOVE_HERE:
            to_delete = iter->right;
            iter->right = nullptr;
            delete to_delete;
            
        case WAS_HEIGHT_UPDATE:
        case WAS_ROLLING:
            iter->updateWeight();
            return updateHeight(iter);

        default:
            iter->updateWeight();
            return SUCCESS;
        }
    
    return REMOVE_HERE;
}


/*   ***   select & contains Auxiliary Functions   ***   */

template <typename T, typename Comp>
const T& 
avl<T, Comp>::selectAux(node<T>* iter, size_t index) const {
    
    if(iter->w_left() > index - 1){
        
        if(iter->left == nullptr)
            return getMin();
        
        return selectAux(iter->left, index);
    }
    
    if(iter->w_left() < index - 1){
        
        if(iter->right == nullptr)
            return getMax();
        
        return selectAux(iter->right, index - iter->w_left() - 1);
    }
    
    return iter->key;
}


template <typename T, typename Comp>
node<T>* 
avl<T, Comp>::find(const T& key) const {
    
    node<T>* iter = root;
    
    while(iter){
        if(keysEqual(key, iter->key))
            return iter;
        
        if(key_comp(key, iter->key))
            iter = iter->left;
        else
            iter = iter->right;
    }
    return nullptr;
}


template <typename T, typename Comp>
void 
avl<T, Comp>::updateMinAndMax(){

    node<T>* iter = root;

    if(!iter){
        min = max = nullptr;
        return;
    }

    while(iter->left)
        iter = iter->left;
    min = iter;

    iter = root;
    while(iter->right)
        iter = iter->right;
    max = iter;
}


/*   ***   Height balance of AVL   ***   */

template <typename T, typename Comp>
AVL_STATUS
avl<T, Comp>::updateHeight(node<T>* iter){
    
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
    
    if(std::abs(balance_f) > 1)
        return WAS_ROLLING;
    
    if(iter->height != old_height)
        return WAS_HEIGHT_UPDATE;
    
    return SUCCESS;
}


template <typename T, typename Comp>
int 
avl<T, Comp>::balanceFactor(node<T>* iter){
    
    int left_height = 0, right_height = 0;
    
    if(iter->left)
        left_height = iter->left->height + 1;
    
    if(iter->right)
        right_height = iter->right->height + 1;
    
    return left_height - right_height;
}


template <typename T, typename Comp>
void 
avl<T, Comp>::genericRollingPart(node<T>* B){
    
    std::swap(B->key, B->right->key);
    std::swap(B->left, B->right->right);
    
    B->right->height = 1 + maxHeight<T>(B->right->left, B->right->right);
    B->height = 1 + maxHeight<T>(B->left, B->right);
    
    B->right->updateWeight();
    B->updateWeight();
}


template <typename T, typename Comp>
void 
avl<T, Comp>::swapSons(node<T>* father){
    std::swap(father->left, father->right);
}


/*   ***   Build almost-complete tree   ***   */

template <typename T, typename Comp>
void 
avl<T, Comp>::buildAlmostCompleteTree(size_t size){
    
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

template <typename T, typename Comp>
node<T>* 
avl<T, Comp>::buildCompleteTree(int height){
    
    if(height == -1)
        return nullptr;
    
    node<T>* _root = new node<T>;
    
    _root->left = buildCompleteTree(height - 1);
    _root->right = buildCompleteTree(height - 1);
    
    return _root;
}

template <typename T, typename Comp>
void 
avl<T, Comp>::removeLeaves(node<T>** it_ptr, int& num_to_remove, int root_height){
    
    if(num_to_remove == 0)
        return;
    
    if(root_height == 0){
        
        node<T>* temp = *it_ptr;
        *it_ptr = nullptr;
        delete temp;
        num_to_remove--;
        return;
    }
    
    removeLeaves(&((*it_ptr)->right), num_to_remove, root_height - 1);
    removeLeaves(&((*it_ptr)->left), num_to_remove, root_height - 1);
}

template <typename T, typename Comp>
void 
avl<T, Comp>::initHeightAndWeight(node<T>* iter){
    
    if(iter == nullptr)
        return;
    
    initHeightAndWeight(iter->left);
    initHeightAndWeight(iter->right);
    
    iter->height = 1 + maxHeight<T>(iter->left, iter->right);
    iter->updateWeight();
}


/*   ***   Tree Traversals Auxiliary   ***   */

template <typename T, typename Comp>
template <typename Functor>
void 
avl<T, Comp>::inorderAux(Functor& func, node<T>* iter) {
    
    if(iter == nullptr)
        return;
    
    inorderAux(func, iter->left);
    
    func(iter->key);
    
    inorderAux(func, iter->right);
}


template <typename T, typename Comp>
template <typename Functor>
void 
avl<T, Comp>::preorderAux(Functor& func, node<T>* iter) {
    
    if(iter == nullptr)
        return;
    
    func(iter->key);
    
    preorderAux(func, iter->left);
    preorderAux(func, iter->right);
}


template <typename T, typename Comp>
template <typename Functor>
void 
avl<T, Comp>::postorderAux(Functor& func, node<T>* iter) {
    
    if(iter == nullptr)
        return;
    
    postorderAux(func, iter->left);
    postorderAux(func, iter->right);
    
    func(iter->key);
}


template <typename T, typename Comp>
template <typename Functor>
void 
avl<T, Comp>::constInorderAux(Functor& func, node<T>* iter) const {

    if(iter == nullptr)
        return;
    
    constInorderAux(func, iter->left);
    
    func(iter->key);
    
    constInorderAux(func, iter->right);
}


#endif /* AVL_IMPLEMENTATION_H */
