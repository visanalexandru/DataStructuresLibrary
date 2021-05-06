//
// Created by gvisan on 06.05.2021.
//

#ifndef DSL_LIST_H
#define DSL_LIST_H

#include<iterator> //for std::forward_iterator tag

namespace dsl {
    template<class type>
    class list {
    private:
        /* This structure represents a single entry in the list */
        struct node {

            /* The value this node holds */
            type value;

            /* Pointers to the next and the previous node in the list */
            node *next, *previous;

            explicit node(const type &val) : value(val), next(nullptr), previous(nullptr) {

            }

            node() : value(), next(nullptr), previous(nullptr) {

            }
        };

        /* The iterator for the list, used to return elements in the order they were added in the list */
        struct iterator {
            friend class list;

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = type;
            using pointer = type *;  // or also value_type*
            using reference = type &;  // or also value_type&

            explicit iterator(node *position) : h_node(position) {

            }

            reference operator*() const { return h_node->value; }

            pointer operator->() { return &h_node->value; }


            /* Prefix increment, just move the node forward */
            iterator &operator++() {
                h_node = h_node->next;
                return *this;
            }

            /* Same as prefix increment, but return the value before the increment */
            iterator operator++(int) {
                iterator tmp = *this;
                h_node = h_node->next;
                return tmp;
            }

            /* Prefix decrement, just move the node backwards */
            iterator &operator--() {
                h_node = h_node->previous;
                return *this;
            }

            /* Same as prefix decrement, but return the value before the increment */
            iterator operator--(int) {
                iterator tmp = *this;
                h_node = h_node->previous;
                return tmp;
            }

            friend bool operator==(const iterator &a, const iterator &b) { return a.h_node == b.h_node; };

            friend bool operator!=(const iterator &a, const iterator &b) { return a.h_node != b.h_node; };

        private:
            /* The position of the iterator in the list */
            node *h_node;
        };

        /* This node is used to mark the end of the list*/
        node *last;

        /* The first node of the list */
        node *first;

        /* The number of elements in the list */
        size_t count;

        void destroy_list() {
            node *here = first;
            while (here != last) {
                node *next = here->next;
                delete here;
                here = next;
            }
            last->previous = nullptr;
            first = last;
        }

    public:

        list() : count(0) {
            last = new node();
            first = last;
        }

        /* Copy constructor, make a copy of the other list */
        list(const list &other) : count(other.count) {
            last = new node();
            node *here = last, *current = other.last;

            while (current != other.first) {
                node *copy = new node(current->previous->value);
                here->previous = copy;
                copy->next = here;
                here = copy;
                current = current->previous;
            }
            first = here;
        }

        /* Copy assignment operator */
        list &operator=(list other) {
            swap(other);
            return *this;
        }

        /* Move constructor, just swap the two lists */
        list(list &&other) noexcept: count(0) {
            swap(*this, other);
        }

        /* Swap the two lists by swapping their corresponding pointers*/
        void swap(list &other) {
            std::swap(first, other.first);
            std::swap(last, other.last);
            std::swap(count, other.count);
        }

        /* Destructor, don't forget to delete the last node*/
        ~list() {
            destroy_list();
            delete last;
        }

        /* Returns an iterator that points to the beginning of the list */
        iterator begin() {
            return iterator(first);
        }

        /* Returns an iterator that points to the end of the list */
        iterator end() {
            return iterator(last);
        }

        /* Inserts the given value before the element at the specified position*/
        /* Return an iterator to the newly created value */
        iterator insert(iterator position, const type &value) {
            node *to_add = new node(value);
            node *next = position.h_node;

            to_add->next = next;
            to_add->previous = next->previous;

            next->previous = to_add;

            if (next == first) {
                first = to_add;
            } else to_add->previous->next = to_add;

            count++;
            return iterator(to_add);
        }

        /* Erase the node at the specified position */
        /* Return an iterator to the element that followed the erased element */
        iterator erase(iterator position) {
            node *to_erase = position.h_node;
            node *next = to_erase->next;

            next->previous = to_erase->previous;

            if (to_erase == first) {
                first = next;
            } else to_erase->previous->next = next;

            delete to_erase;
            count--;
            return iterator(next);
        }

        /* Returns the number of elements in the list */
        size_t size() const {
            return count;
        }

        /* Checks if the list is empty */
        bool empty() const {
            return count == 0;
        }

        /* Removes all the elements from the list */
        void clear() {
            destroy_list();
            count = 0;
        }

        /* Returns a reference to the first element */
        /* Calling this function when the list is empty results in undefined behaviour */
        type &front() {
            return first->value;
        }

        /* Returns a reference to the element */
        /* It is undefined behaviour to call this function when the list is emtpy */
        type &back() {
            return last->previous->value;
        }

    };
}


#endif //DSL_LIST_H
