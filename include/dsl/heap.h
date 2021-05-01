//
// Created by gvisan on 01.05.2021.
//

#ifndef DSL_HEAP_H
#define DSL_HEAP_H

#include<vector>

namespace dsl {
    template<class type>

    class heap {
    private:
        std::vector<type> data;
        size_t count;

        /* Returns the index of the left son of the node */
        size_t left_son(size_t node) {
            return node << 1u;
        }

        /* Returns the index of the right son of the node */
        size_t right_son(size_t node) {
            return (node << 1u) + 1;
        }

        /* Returns the index of the father of the node */
        size_t father(size_t node) {
            return node >> 1u;
        }

        /* This method shifts the node down the tree, comparing its value with the value of its children */
        void shift(size_t node) {
            size_t best, l = left_son(node), r = right_son(node);

            /* Check if the node is not a leaf node*/
            if (l <= count) {

                best = l;

                /* If it also has a right-node then we have to take the child node with the maximum value */

                if (r <= count) {
                    best = (data[l] > data[r]) ? l : r;
                }

                /* If the value of the best child node is greater than the value of this node, we swap the
                 * two nodes and continue the process*/

                if (data[best] > data[node]) {
                    std::swap(data[node], data[best]);
                    shift(best);
                }
            }
        }

        /* This method lifts the node up the tree, comparing its value with the value of its father */
        void percolate(size_t node) {
            size_t ft = father(node);

            /* If this is not the root node and its value is greater than the value of its parent,
             * swap the two values and continue the process */

            if (ft != 0 && data[node] > data[ft]) {
                std::swap(data[node], data[ft]);
                percolate(ft);
            }
        }


    public:
        heap() : data(1), count(0) {

        }

        /* Constructs a heap from a container. First and last are iterators to the given container.
         * This method first inserts its contents and then sorts the heap */

        template<class Iter>
        heap(Iter first, Iter last) {
            data.resize(1);
            data.insert(data.begin() + 1, first, last);

            count = data.size() - 1;

            /* Now we build the heap */
            for (size_t node = count / 2; node >= 1; node--) {
                shift(node);
            }
        }

        /* This method returns the size of the heap */
        size_t size() {
            return count;
        }

        /* This method is used to check if the heap is empty*/
        bool empty() {
            return count == 0;
        }

        /* This method adds a new value into the heap */
        void push(type value) {
            data.push_back(value);
            count++;
            percolate(count);
        }

        /* This method removes the root of the heap */
        void pop() {
            std::swap(data[1], data[count]);
            data.pop_back();
            count--;
            shift(1);
        }

        /* This method returns the value of the root */
        type top() {
            return data[1];
        }

    };
}

#endif //DSL_HEAP_H
