//
// Created by gvisan on 02.05.2021.
//

#ifndef DSL_HASHMAP_H
#define DSL_HASHMAP_H

#include<vector>
#include<functional>
#include<iterator>
#include<cstddef>

namespace dsl {

    /**
     * This is an implementation of a hashmap that uses linear probing to solve collisions.
     *
     * It uses buckets of std::vector to store values.
     * @tparam key The type of the key value of an entry.
     * @tparam value The type of the mapped value of an entry.
     * @tparam hash A unary function object, used to retrieve the hash code of a key to order elements into buckets.
     * @tparam equal A binary predicate, used to compare two keys for equality.
     */
    template<class key, class value, class hash=std::hash<key>, class equal=std::equal_to<key>>
    class hashmap {
    private:

        /* The buckets */
        std::vector<std::vector<std::pair<key, value>>> buckets;

        /* The number of buckets */
        size_t num_buckets;

        /* The number of elements in the map */
        size_t count;

        /* The hasher */
        hash hasher;

        /* Comparator, used to check if two keys are equal */
        equal comparator;

        /* A node in the hashmap */
        struct node {
            /* Bucket iterator */
            typename std::vector<std::vector<std::pair<key, value>>>::iterator bucket_iterator;

            /* Element index */
            size_t element_index;

            /* End of the bucket list */
            typename std::vector<std::vector<std::pair<key, value>>>::iterator bucket_end;

        };

    public:
        /** This is the iterator for the hashmap.
        Iterating through the map returns the elements in a seemingly random order. **/
        struct iterator {
            friend class hashmap;

            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            using value_type = std::pair<key, value>;
            using pointer = std::pair<key, value> *;
            using reference = std::pair<key, value> &;

            explicit iterator(node here) : h_node(here) {

            }

            /** De-references the iterator. The key should not be modified. **/
            reference operator*() const {
                return (*h_node.bucket_iterator)[h_node.element_index];
            };

            /** De-references the iterator. The key should not be modified. **/
            pointer operator->() {
                return &((*h_node.bucket_iterator)[h_node.element_index]);
            }

            /** Incrementing this iterator is finding the next value, skipping empty buckets **/
            iterator &operator++() {
                h_node.element_index++;

                if (h_node.element_index == h_node.bucket_iterator->size()) {
                    h_node.element_index = 0;
                    h_node.bucket_iterator++;

                    while (h_node.bucket_iterator != h_node.bucket_end && h_node.bucket_iterator->size() == 0) {
                        h_node.bucket_iterator++;
                    }
                }
                return *this;
            }

            /** Post-increment, same as pre-increment, but return the value before the increment **/
            iterator operator++(int) {
                iterator tmp = *this;

                h_node.element_index++;
                if (h_node.element_index == h_node.bucket_iterator->size()) {
                    h_node.element_index = 0;
                    h_node.bucket_iterator++;

                    while (h_node.bucket_iterator != h_node.bucket_end && h_node.bucket_iterator->size() == 0) {
                        h_node.bucket_iterator++;
                    }
                }

                return tmp;
            }

            /** Checks if two iterators are equal. */
            friend bool operator==(const iterator &a, const iterator &b) {
                return a.h_node.bucket_iterator == b.h_node.bucket_iterator &&
                       a.h_node.element_index == b.h_node.element_index;
            };

            /** Checks if two iterators are not equal. */
            friend bool operator!=(const iterator &a, const iterator &b) {
                return a.h_node.bucket_iterator != b.h_node.bucket_iterator ||
                       a.h_node.element_index != b.h_node.element_index;
            };

        private:
            node h_node;
        };

        explicit hashmap(size_t bucket_count) : buckets(bucket_count), num_buckets(bucket_count), count(0) {

        }

        /** Finds the first element of the map, by iterating through the buckets,
         ** until a non-empty bucket is found. */
        iterator begin() {
            for (auto iter = buckets.begin(); iter != buckets.end(); iter++) {
                if (iter->size() != 0) {
                    return iterator({iter, 0, buckets.end()});
                }
            }
            return iterator({buckets.end(), 0, buckets.end()});
        }

        /** Returns an iterator to the end of the map. **/
        iterator end() {
            return iterator({buckets.end(), 0, buckets.end()});
        }

        /** Returns an iterator to the element identified by the key.
        If no element has the given key, return the end iterator. */
        iterator find(key id) {
            size_t h = hasher(id) % num_buckets; //The index of the bucket

            for (size_t i = 0; i < buckets[h].size(); i++) {
                if (comparator(id, buckets[h][i].first))
                    return iterator({buckets.begin() + h, i, buckets.end()});
            }
            return end();
        }

        /** Inserts a new element into the hashmap.
        If the key is already in the hashmap, don't modify its value.*/
        void insert(const std::pair<key, value> &element) {
            size_t h = hasher(element.first) % num_buckets; // The index of the bucket

            for (size_t i = 0; i < buckets[h].size(); i++) {
                if (comparator(element.first, buckets[h][i].first))// We found an element with the same hash
                    return;
            }
            count++;
            buckets[h].push_back(element);
        }

        /** Erases the element at the given iterator.
        If the iterator is not valid, the behaviour is undefined. */
        void erase(iterator it) {
            auto bucket = it.h_node.bucket_iterator;
            std::vector<std::pair<key, value>> &ref = *bucket;
            ref[it.h_node.element_index] = ref[ref.size() - 1];
            ref.pop_back();
            count--;
        }

        /** Returns the number of elements in the hashmap.*/
        size_t size() const {
            return count;
        }

        /** Checks if the hashmap is empty. */
        bool empty() const {
            return count == 0;
        }

        /**Clears the hashmap, by removing all the elements from all the buckets. */
        void clear() {
            for (size_t i = 0; i < num_buckets; i++)
                buckets[i].clear();
            count = 0;
        }

    };
}

#endif //DSL_HASHMAP_H
