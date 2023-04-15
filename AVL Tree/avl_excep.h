#ifndef AVL_EXCEP_H_
#define AVL_EXCEP_H_

#include <exception>

class AVL_Failure :                     public std::exception{};
class AVL_Allocation_error :            public std::exception{};

class element_not_exist :               public std::exception {};
class tree_is_empty :                   public std::exception {};

class vec_contain_a_not_unique_key :    public std::exception {};
class arr_contain_a_not_unique_key :    public std::exception {};

class weight_incorrect :                public std::exception {};
class height_incorrect :                public std::exception {};

class tree_is_not_balanced :            public std::exception {};


#endif /* AVL_EXCEP_H_ */
