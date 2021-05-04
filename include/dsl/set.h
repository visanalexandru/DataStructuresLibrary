//
// Created by gvisan on 03.05.2021.
//

#ifndef DSL_SET_H
#define DSL_SET_H

#include<cstddef> //for size_t
#include<functional> //for less

namespace dsl {

    /*This class is an implementation of a ordered set using a treap data structure. It holds
     * the elements in the order given by the comparator */

    template<class key, class compare=std::less<key>>
    class set {
    private:

        /* A node in the tree structure */
        struct node {
            const key key_value;
            size_t priority;
            node *left, *right, *parent;

            node(key value, size_t pr, node *l, node *r, node *p) : key_value(value), priority(pr), left(l), right(r),
                                                                    parent(p) {

            }
        };

        /* This is the iterator for the set. It returns values in the order defined by the comparator.*/
        struct iterator {
            friend class set;

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = const key;
            using pointer = const key *;  // or also value_type*
            using reference = const key &;  // or also value_type&

            explicit iterator(node *here) : h_node(here) {


            }

            reference operator*() const {
                return h_node->key_value;
            }

            pointer operator->() {
                return &h_node->key_value;
            }

            friend bool operator==(const iterator &a, const iterator &b) { return a.h_node == b.h_node; };

            friend bool operator!=(const iterator &a, const iterator &b) { return a.h_node != b.h_node; };

        private:
            node *h_node;
        };

        /* The root of the tree */
        node *root;

        /* A dummy node, used to mark the end of the container */
        node *nil;

        /*The comparator, used to compare keys */
        compare comparator;

        /* Left-rotation in the treap */
        void rotate_left(node *&here) {
            node *left = here->left;
            here->left = left->right;

            if (left->right != nil)
                left->right->parent = here;

            left->right = here;
            here->parent = left;

            here = left;
        }

        /*Right-rotation in the treap */
        void rotate_right(node *&here) {
            node *right = here->right;
            here->right = right->left;

            if (right->left != nil) {
                right->left->parent = here;
            }

            right->left = here;
            here->parent = right;

            here = right;
        }

        /* This method returns the node with the minimum value in the given subtree */
        node *tree_minimum(node *here) {
            while (here->left != nil) {
                here = here->left;
            }
            return here;
        }

        /* This method returns the node with the maximum value in the given subtree */
        node *tree_maximum(node *here) {
            while (here->right != nil) {
                here = here->right;
            }
            return here;
        }

        /* Balance to keep the heap property of the priorities */
        void balance(node *&n) {
            if (n->left->priority > n->priority) {
                rotate_left(n);
            } else if (n->right->priority > n->priority) {
                rotate_right(n);
            }
        }

        /* Insert a new node with the given key */
        void insert(node *&here, node *parent, const key &key_value) {
            if (here == nil) {
                here = new node(key_value, rand() + 1, nil, nil, parent);
            } else if (comparator(key_value, here->key_value)) {
                insert(here->left, here, key_value);
            } else {
                insert(here->right, here, key_value);
            }
            balance(here);
        }

        /* Erase the given node from the tree */
        void erase(node *&here) {
            if (here == nil)
                return;
            if (here->left == nil && here->right == nil) {
                delete here;
                here = nil;
            } else {
                (here->left->priority > here->right->priority) ? rotate_left(here) : rotate_right(here);
                erase(here);
            }
        }

        node *find(node *here, const key &key_value) {
            if (here == nil)
                return nil;

            /* First checking for equality */
            if (!comparator(key_value, here->key_value) && !comparator(here->key_value, key_value)) {
                return here;
            } else if (comparator(key_value, here->key_value)) {
                return find(here->left, key_value);
            } else {
                return find(here->right, key_value);
            }
        }

    public:
        set() {
            nil = new node(0, 0, nullptr, nullptr, nullptr);
            root = nil;
        }

        /* The smallest node of the tree */
        iterator begin() {
            return iterator(tree_minimum(root));
        }

        /* Returns an iterator with a nil handle */
        iterator end() {
            return iterator(nil);
        }

        /*Insert a new node with the given key by calling the recursive insert method */
        void insert(const key &key_value) {
            insert(root, nil, key_value);
        }

        /* Returns an iterator that points to the node with the given key */
        iterator find(const key &key_value) {
            return iterator(find(root, key_value));
        }

        /* Erase node by iterator */
        void erase(iterator to_erase) {
            erase(to_erase.h_node);
        }

    };
}


#endif //DSL_SET_H
