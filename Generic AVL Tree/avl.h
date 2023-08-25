#ifndef AVL_H_
#define AVL_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdbool>
#include <cstdlib>
#include <iterator>
#include <vector>

#include "avl_iterator.h"
#include "avl_utils.h"


/* Type 'T' MUST SUPPORT Default C'tor */

template <typename T, typename Comp = std::less<T>>
class avl {
    struct node<T>* root;
    struct node<T>* min;
    struct node<T>* max;
    size_t tree_size;

public:
    const Comp& key_comp;

// Constractors:
    avl();
    explicit avl(const Comp& comp);
    avl(const avl& src);
    // TODO: Add Move C'tor

    /* Build a Tree in O(size) if sorted gets true */
    explicit avl(std::vector<T> elements, bool sorted = false);
    avl(std::vector<T> elements, const Comp& comp, bool sorted = false);
    avl(T* elements, size_t arr_size, bool sorted = false);
    avl(T* elements, size_t arr_size, const Comp& comp, bool sorted = false);

    avl<T, Comp>& operator=(const avl<T, Comp>& src);
    ~avl();

// Operations:
    void insert(T element);
    void remove(const T element);
    bool contains(const T& element) const;
    
    size_t rank(const T& key) const;
    const T& select(size_t index) const;
    
    T& getRef(const T& key);
    T getCopy(const T& key); // TODO
    const T& getMin() const;
    const T& getMax() const;
    T popMin(); // TODO
    T popMax(); // TODO
    size_t size() const;
    bool empty() const;
    std::vector<T> getAll() const;

// const-iterator:
    class iterator : public avl_iterator<T>{
    public:
        iterator();
        iterator(node<T>* root);
    };

    iterator begin() noexcept;
    iterator end();
    
// Tree Traversals:
    template <typename Functor>
    void inorder(Functor& func);
    
    template <typename Functor>
    void preorder(Functor& func);
    
    template <typename Functor>
    void postorder(Functor& func);

// Const Tree Traversals (for read-only use):
    template <typename Functor>
    void constInorder(Functor& func) const;
    
private:
    using vec_iter = typename std::vector<T>::iterator;

    /* Equality check of two keys. For internal use */
    bool keysEqual(const T& k1, const T& k2) const;
    
// Auxiliary Functions:
    node<T>* find(const T& key) const;
    AVL_STATUS insertAux(node<T>* iter, T& element);
    AVL_STATUS removeLeaf(node<T>* iter, T leaf);
    const T& selectAux(node<T>* iter, size_t index) const;
    void updateMinAndMax();

// Height balance:
    AVL_STATUS updateHeight(node<T>* iter);
    int balanceFactor(node<T>* iter);
    void genericRollingPart(node<T>* B);
    void swapSons(node<T>* father);

// Build almost-complete tree:
    void buildAlmostCompleteTree(size_t size);
    node<T>* buildCompleteTree(int height);
    void removeLeaves(node<T>** it_ptr, int& num_to_remove, int root_height);
    void initHeightAndWeight(node<T>* iter);
    
// Tree Traversals Auxiliary:
    template <typename Functor>
    void inorderAux(Functor& func, node<T>* iter);
    template <typename Functor>
    void preorderAux(Functor& func, node<T>* iter);
    template <typename Functor>
    void postorderAux(Functor& func, node<T>* iter);

// Const Tree Traversals (for read-only use):
    template <typename Functor>
    void constInorderAux(Functor& func, node<T>* iter) const;
};

#endif  /* AVL_H_ */
