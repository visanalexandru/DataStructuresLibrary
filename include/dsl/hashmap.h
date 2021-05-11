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
     * Aceasta clasa este o implementare a unei tabele de dispersie ce foloseste o cautare liniara pentru a rezolva coliziuni.
     *
     * Un element reprezinta o pereche de tip cheie-valoare.
     *
     * Elementele sunt organizate in "bucket"-uri pentru a se stoca valorile in functie de "valoarea hash" a cheii.
     *
     * Se foloseste clasa std::vector pentru obiectul de tip "bucket".
     *
     * Un articol ce explica sumar conceptul de tabela hash se poate gasi la https://infoarena.ro/tabele-hash-scurta-prezentare.
     *
     * @tparam key Tipul cheii unui element in tabela de dispersie.
     * @tparam value Tipul valorii unui element in tabela de dispersie.
     * @tparam hash O clasa ce reprezinta o functie unara ce accepta un parametru de tip "key", folosita pentru a genera "valoarea hash" a unei chei.
     * @tparam equal O clasa ce reprezinta o functie binara ce accepta doi parametrii de tip "key", folosita pentru a verifica daca doua chei sunt egale.
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
        /** Aceasta clasa este iteratorul tabelei de dispersie. Parcurgerea tabelei de dispersie returneaza elementele
         * intr-o ordine aparent aleatorie. **/
        struct iterator {
            friend class hashmap;

            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            using value_type = std::pair<key, value>;
            using pointer = std::pair<key, value> *;
            using reference = std::pair<key, value> &;

            explicit iterator(node here) : h_node(here) {

            }

            /** Obtine o referinta la un element din tabela de dispersie. Valoarea cheii este expusa doar ca nu trebuie modificata. **/
            reference operator*() const {
                return (*h_node.bucket_iterator)[h_node.element_index];
            };

            /** Obtine o referinta la un element din tabela de dispersie. Valoarea cheii este expusa doar ca nu trebuie modificata. **/
            pointer operator->() {
                return &((*h_node.bucket_iterator)[h_node.element_index]);
            }

            /** Gaseste urmatoarea valoare in tabela de dispersie, sarind peste "bucket"-urile goale. **/
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

            /** Gaseste urmatoarea valoare in tabela de dispersie, sarind peste "bucket"-urile goale. **/
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

            /** Verifica daca doi iteratori sunt egali. */
            friend bool operator==(const iterator &a, const iterator &b) {
                return a.h_node.bucket_iterator == b.h_node.bucket_iterator &&
                       a.h_node.element_index == b.h_node.element_index;
            };

            /** Verifica daca doi iteratori nu sunt egali. */
            friend bool operator!=(const iterator &a, const iterator &b) {
                return a.h_node.bucket_iterator != b.h_node.bucket_iterator ||
                       a.h_node.element_index != b.h_node.element_index;
            };

        private:
            node h_node;
        };

        explicit hashmap(size_t bucket_count) : buckets(bucket_count), num_buckets(bucket_count), count(0) {

        }

        /** Returneaza un iterator ce reprezinta inceputul tabelei de dispersie, adica o referinta la primul element din tabela.**/
        iterator begin() {
            for (auto iter = buckets.begin(); iter != buckets.end(); iter++) {
                if (iter->size() != 0) {
                    return iterator({iter, 0, buckets.end()});
                }
            }
            return iterator({buckets.end(), 0, buckets.end()});
        }

        /** Returneaza un iterator ce reprezinta sfarsitul tabelei de dispersie. Acest iterator nu trebuie accesat deoarece nu are o referinta la vreun element din tabela.**/
        iterator end() {
            return iterator({buckets.end(), 0, buckets.end()});
        }

        /** Returneaza un iterator la elementul ce are cheia data.
        Daca niciun element nu are cheia data, returneaza iteratorul ce marcheaza sfarsitul tabelei. */
        iterator find(key id) {
            size_t h = hasher(id) % num_buckets; //The index of the bucket

            for (size_t i = 0; i < buckets[h].size(); i++) {
                if (comparator(id, buckets[h][i].first))
                    return iterator({buckets.begin() + h, i, buckets.end()});
            }
            return end();
        }

        /** Insereaza un nou element in tabela de dispersie.
        Daca exista deja un element cu cheia data, valoarea sa nu este modificata.*/

        void insert(const std::pair<key, value> &element) {
            size_t h = hasher(element.first) % num_buckets; // The index of the bucket

            for (size_t i = 0; i < buckets[h].size(); i++) {
                if (comparator(element.first, buckets[h][i].first))// We found an element with the same hash
                    return;
            }
            count++;
            buckets[h].push_back(element);
        }

        /** Sterge elementul dat de iteratorul luat ca parametru.
        Daca iteratorul nu este valid, comportamentul acestei metode nu este definit. */
        void erase(iterator it) {
            auto bucket = it.h_node.bucket_iterator;
            std::vector<std::pair<key, value>> &ref = *bucket;
            ref[it.h_node.element_index] = ref[ref.size() - 1];
            ref.pop_back();
            count--;
        }

        /** Returneaza numarul de elemente din tabela de dispersie.*/
        size_t size() const {
            return count;
        }

        /** Verifica daca tabela de dispersie este goala. */
        bool empty() const {
            return count == 0;
        }

        /**Sterge toate elementele din tabela de dispersie. */
        void clear() {
            for (size_t i = 0; i < num_buckets; i++)
                buckets[i].clear();
            count = 0;
        }

    };
}

#endif //DSL_HASHMAP_H
