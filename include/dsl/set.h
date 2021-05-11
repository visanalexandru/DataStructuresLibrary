//
// Created by gvisan on 03.05.2021.
//

#ifndef DSL_SET_H
#define DSL_SET_H

#include<cstddef> //for size_t
#include<functional> //for less

namespace dsl {

    /**
     * This class is an implementation of an ordered set using a treap data structure.
     *
     * @tparam key The type of the value of an entry in the set.
     * @tparam compare A binary predicate that defines a strict weak ordering, used to order the elements.\n The expression compare(a,b) shall return true if a is considered to go before b.
     */

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

            /* The number of nodes in the tree structure */
            size_t count;

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
                    count++;
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
            void erase(node *&here, node *to_delete) {
                if (here == nil)
                    return;

                if (!comparator(to_delete->key_value, here->key_value) &&
                    !comparator(here->key_value, to_delete->key_value)) {
                    /* We found a match */
                    if (here->left == nil && here->right == nil) {//This is a leaf node, so delete it
                        delete here;
                        here = nil;
                        count--;
                    } else {//We rotate it while keeping the heap property and continue downwards
                        (here->left->priority > here->right->priority) ? rotate_left(here) : rotate_right(here);
                        erase(here, to_delete);
                    }
                    return;
                }

                if (comparator(to_delete->key_value, here->key_value)) {
                    erase(here->left, to_delete);
                } else {
                    erase(here->right, to_delete);
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

            /* Returns the first node that does not go before the value.*/
            /* Either it is equivalent, or goes after */
            /* If there is no such element, return nil */

            node *lower_bound(node *here, const key &key_value) {
                if (here == nil)
                    return nil;
                if (comparator(here->key_value, key_value)) {
                    return lower_bound(here->right, key_value);
                }
                node *ans = lower_bound(here->left, key_value);
                if (ans != nil)
                    return ans;
                return here;
            }

            /* Returns the first node that goes after the value */
            node *upper_bound(node *here, const key &key_value) {
                if (here == nil)
                    return nil;

                if (comparator(key_value, here->key_value)) {
                    node *ans = upper_bound(here->left, key_value);
                    if (ans != nil)
                        return ans;
                    return here;
                }

                return upper_bound(here->right, key_value);
            }

            tree() : count(0) {
                nil = new node(0, 0, nullptr, nullptr, nullptr);
                root = nil; //The root of the tree is currently nil
            }


            /* Deep-copy the tree, return the root of the tree*/
            /* We pass the nil value of the other node to identify nil nodes */
            node *copy(const node *here, const node *nil_marker) {

                if (here == nil_marker) {
                    return nil;
                }

                node *copy_node = new node(here->key_value, here->priority, nullptr, nullptr, nullptr);
                copy_node->left = nil;
                copy_node->right = nil;
                copy_node->parent = nil;

                /* Copy the left subtree */
                node *left = copy(here->left, nil_marker);
                copy_node->left = left;
                if (left != nil)
                    left->parent = copy_node;

                /*Copy the right subtree */
                node *right = copy(here->right, nil_marker);
                copy_node->right = right;
                if (right != nil)
                    right->parent = copy_node;

                return copy_node;
            }

            /* Swap the tree structure with another tree structure */
            void swap(tree &other) {
                std::swap(nil, other.nil);
                std::swap(root, other.root);
                std::swap(count, other.count);
            }

            /* Copy-construct the tree structure */
            tree(const tree &other) : count(other.count) {
                nil = new node(0, 0, nullptr, nullptr, nullptr);
                root = copy(other.root, other.nil);
            }

            /* Copy assignment operator */
            tree &operator=(const tree other) {
                swap(other);
                return *this;
            }


            /* Cleanup, recursively delete nodes */
            void destroy_tree(node *here) {
                if (here == nil)
                    return;
                destroy_tree(here->left);
                destroy_tree(here->right);

                delete here;
            }

            /* Clear the container by deleting all nodes ans resetting the root to the nil pointer */
            void clear() {
                destroy_tree(root);
                root = nil;
                count = 0;
            }

            ~tree() {
                clear();
                delete nil;// Don't forget to delete the nil node
            }
        };

        tree structure;
    public:

        /**
         * This is the iterator for the set.
         * Iterating through the set returns the elements in the order defined by the compare predicate
         */
        struct iterator {
            friend class set;

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = const key;
            using pointer = const key *;  // or also value_type*
            using reference = const key &;  // or also value_type&

            explicit iterator(node *here, tree *structure) : h_node(here), h_structure(structure) {

            }

            /** De-references the iterator. */
            reference operator*() const {
                return h_node->key_value;
            }

            /** De-references the iterator. */
            pointer operator->() {
                return &h_node->key_value;
            }

            /** Incrementing this iterator is finding the successor of the element the iterator points at. */
            iterator &operator++() {
                h_node = h_structure->tree_successor(h_node);
                return *this;
            }

            /** Post-increment, same as pre-increment, but return the value before the increment. */
            iterator operator++(int) {
                iterator tmp = *this;
                h_node = h_structure->tree_successor(h_node);
                return tmp;
            }

            /** Decrementing this iterator is finding the successor of the element the iterator points at. */
            iterator &operator--() {

                if (h_node == h_structure->nil) { //If its the end iterator, jump to the greatest value in the tree
                    h_node = h_structure->tree_maximum(h_structure->root);
                } else {
                    h_node = h_structure->tree_predecessor(h_node);
                }
                return *this;
            }

            /** Post-decrement, same as pre-decrement, but return the value before the decrement. */
            iterator operator--(int) {
                iterator tmp = *this;

                if (h_node == h_structure->nil) { //If its the end iterator, jump to the greatest value in the tree
                    h_node = h_structure->tree_maximum(h_structure->root);
                } else {
                    h_node = h_structure->tree_predecessor(h_node);
                }
                return tmp;
            }

            /** Checks if two iterators are equal. */
            friend bool operator==(const iterator &a, const iterator &b) { return a.h_node == b.h_node; };

            /** Checks if two iterators are not equal. */
            friend bool operator!=(const iterator &a, const iterator &b) { return a.h_node != b.h_node; };

        private:
            /* The position of the iterator in the tree */
            node *h_node;

            tree *h_structure;//a reference to the tree structure
        };

        /** Copy constructor, make a copy of the other set. */
        set(const set &other) : structure(other.structure) {

        }

        /** Assigns new contents to the set, replacing its current contents.*/
        set &operator=(set other) {
            swap(other);
            return *this;
        }

        /** Swaps the content of this set with another set.*/
        void swap(set &other) {
            structure.swap(other.structure);
        }

        set() = default;

        /** Returns an iterator to the first element in the set. */
        iterator begin() {
            if (structure.root == structure.nil) // If the root is nil, return nil
                return iterator(structure.nil, &structure);

            return iterator(structure.tree_minimum(structure.root), &structure);
        }

        /** Returns an iterator that represents the end of the set. */
        iterator end() {
            return iterator(structure.nil, &structure);
        }

        /** Insert a new entry with the given key value. **/
        void insert(const key &key_value) {
            structure.insert(structure.root, structure.nil, key_value);
        }

        /** Returns an iterator that points to the element with the given key. If no element with the given key is found in the set, return the end iterator. */
        iterator find(const key &key_value) {
            return iterator(structure.find(structure.root, key_value), &structure);
        }

        /** Returns an iterator to the first element which is not considered to go before the given value. */
        iterator lower_bound(const key &value) {
            return iterator(structure.lower_bound(structure.root, value), &structure);
        }

        /** Returns an iterator to the first element which is considered to go after the given value. */
        iterator upper_bound(const key &value) {
            return iterator(structure.upper_bound(structure.root, value), &structure);
        }

        /** Removes an element from the set by iterator. */
        void erase(iterator to_erase) {
            structure.erase(structure.root, to_erase.h_node);
        }

        /** Returns the number of elements in the set. */
        size_t size() const {
            return structure.count;
        }

        /** Checks if the container is empty. */
        bool empty() const {
            return structure.count == 0;
        }

        /** Removes all the elements from the set. */
        void clear() {
            structure.clear();
        }

    };
}


#endif //DSL_SET_H
