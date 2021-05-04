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

        /* The tree structure */
        struct tree {
        public:
            /* The root of the tree */
            node *root;

            /* A dummy node, used to mark the end of the container */
            node *nil;

            /*The comparator, used to compare keys */
            compare comparator;

            /* Left-rotation in the tree */
            void rotate_left(node *&here) {
                node *left = here->left;
                left->parent = here->parent;
                here->left = left->right;

                if (left->right != nil)
                    left->right->parent = here;

                left->right = here;
                here->parent = left;

                here = left;
            }

            /*Right-rotation in the tree */
            void rotate_right(node *&here) {
                node *right = here->right;
                right->parent = here->parent;
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

            /* Returns the successor of the node. If no such node exists, return nil */
            node *tree_successor(node *x) {
                if (x->right != nil) { //Its successor is the minimum node in its right subtree
                    return tree_minimum(x->right);
                }

                //Search up the tree
                node *y = x->parent;
                while (y != nil && x == y->right) {
                    x = y;
                    y = y->parent;
                }
                return y;
            }

            /* Same as tree_successor but return the node that precedes the given node*/
            node *tree_predecessor(node *x) {
                if (x->left != nil) { //Its predecessor is the maximum node in its right subtree
                    return tree_maximum(x->left);
                }

                //Search up the tree
                node *y = x->parent;
                while (y != nil && x == y->left) {
                    x = y;
                    y = y->parent;
                }
                return y;
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
                    return;
                }

                /* Check for equality first */
                if (!comparator(key_value, here->key_value) && !comparator(here->key_value, key_value))
                    return;

                if (comparator(key_value, here->key_value)) {
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

            /* Find the node with the given key. It there is none return nil */
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

            tree() {
                nil = new node(0, 0, nullptr, nullptr, nullptr);
                root = nil; //The root of the tree is currently nil
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

            explicit iterator(node *here, tree *structure) : h_node(here), h_structure(structure) {

            }

            reference operator*() const {
                return h_node->key_value;
            }

            pointer operator->() {
                return &h_node->key_value;
            }

            // Prefix increment
            iterator &operator++() {
                h_node = h_structure->tree_successor(h_node);
                return *this;
            }

            // Postfix increment
            iterator operator++(int) {
                iterator tmp = *this;
                h_node = h_structure->tree_successor(h_node);
                return tmp;
            }

            // Prefix decrement
            iterator &operator--() {

                if (h_node == h_structure->nil) { //If its the end iterator, jump to the greatest value in the tree
                    h_node = h_structure->tree_maximum(h_structure->root);
                } else {
                    h_node = h_structure->tree_predecessor(h_node);
                }
                return *this;
            }

            // Postfix decrement
            iterator operator--(int) {
                iterator tmp = *this;

                if (h_node == h_structure->nil) { //If its the end iterator, jump to the greatest value in the tree
                    h_node = h_structure->tree_maximum(h_structure->root);
                } else {
                    h_node = h_structure->tree_predecessor(h_node);
                }
                return tmp;
            }

            friend bool operator==(const iterator &a, const iterator &b) { return a.h_node == b.h_node; };

            friend bool operator!=(const iterator &a, const iterator &b) { return a.h_node != b.h_node; };

        private:
            node *h_node;

            tree *h_structure;//a reference to the tree structure
        };

        tree structure;

    public:
        /* The smallest node of the tree */
        iterator begin() {
            return iterator(structure.tree_minimum(structure.root), &structure);
        }

        /* Returns an iterator with a nil handle */
        iterator end() {
            return iterator(structure.nil, &structure);
        }

        /*Insert a new node with the given key by calling the recursive insert method */
        void insert(const key &key_value) {
            structure.insert(structure.root, structure.nil, key_value);
        }

        /* Returns an iterator that points to the node with the given key */
        iterator find(const key &key_value) {
            return iterator(structure.find(structure.root, key_value), &structure);
        }

        /* Erase node by iterator */
        void erase(iterator to_erase) {
            structure.erase(to_erase.h_node);
        }

    };
}


#endif //DSL_SET_H
