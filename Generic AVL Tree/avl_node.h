#ifndef AVL_NODE_H_
#define AVL_NODE_H_


template <class T>
struct node {
    T key;
    int height;
    int weight;
    node* left;
    node* right;

    node();
    explicit node(T key);
    ~node();
    node(const node&) = delete;
    node& operator=(const node&) = delete;

    void updateWeight();
    int w_left();
};


template <class T>
node<T>::node() : key(), left(nullptr), right(nullptr) {
    height = 0;
    weight = 1;
}

template <class T>
node<T>::node(T key) : node(){
    this->key = key;
}

template <class T>
node<T>::~node(){
    delete left;
    delete right;
}

template <class T>
void node<T>::updateWeight(){

    int w_left = 0, w_right = 0;

    if(left != nullptr) 
        w_left = left->weight;

    if(right != nullptr) 
        w_right = right->weight;

    weight = w_left + w_right + 1;
}

template <class T>
int node<T>::w_left(){

    if(left == nullptr)
        return 0;

    return left->weight;
}

template <class T>
int maxHeight(node<T>* a, node<T>* b){
    
    if(!a && !b) return -1;
    
    if(!a) return b->height;
    
    if(!b) return a->height;
    
    return (a->height > b->height) ? a->height : b->height;
}


#endif  /* AVL_NODE_H_ */
