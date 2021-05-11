//
// Created by gvisan on 06.05.2021.
//

#ifndef DSL_LIST_H
#define DSL_LIST_H

#include<iterator> //for std::forward_iterator tag
#include<utility> //for std::swap

namespace dsl {
    /**
     * Aceasta clasa este o implementare a unei liste dublu inlantuite.
     * @tparam type Tipul valorii unui element din lista.
     */
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

        /* This node is used to mark the end of the list*/
        node *last;

        /* The first node of the list */
        node *first;

        /* The number of elements in the list */
        size_t count;

        /* Delete the nodes of the list, don't delete the end node */
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
        /** Aceasta clasa este iteratorul listei dublu inlantuite.
         *  Parcurgerea listei returneaza elementele in ordinea in care au fost inserate in lista.
         */
        struct iterator {
            friend class list;

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = type;
            using pointer = type *;  // or also value_type*
            using reference = type &;  // or also value_type&

            explicit iterator(node *position) : h_node(position) {

            }

            /** Obtine o referinta la un element din lista. */
            reference operator*() const { return h_node->value; }

            /** Obtine o referinta la un element din lista. */
            pointer operator->() { return &h_node->value; }


            /** Muta iteratorul la urmatorul element din lista. */
            iterator &operator++() {
                h_node = h_node->next;
                return *this;
            }

            /** Muta iteratorul la urmatorul element din lista. */
            iterator operator++(int) {
                iterator tmp = *this;
                h_node = h_node->next;
                return tmp;
            }

            /** Muta iteratorul la elementul precedent din lista. */
            iterator &operator--() {
                h_node = h_node->previous;
                return *this;
            }

            /** Muta iteratorul la elementul precedent din lista. */
            iterator operator--(int) {
                iterator tmp = *this;
                h_node = h_node->previous;
                return tmp;
            }

            /** Verifica daca doi iteratori sunt egali. */
            friend bool operator==(const iterator &a, const iterator &b) { return a.h_node == b.h_node; };

            /** Verifica daca doi iteratori nu sunt egali. */
            friend bool operator!=(const iterator &a, const iterator &b) { return a.h_node != b.h_node; };

        private:
            /* The position of the iterator in the list */
            node *h_node;
        };

        list() : count(0) {
            last = new node();
            first = last;
        }

        /** Constructor de copiere, creeaza o copie a celeilalte liste. */
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

        /** Atribuie un nou continut listei, prin intermediul altei liste. Copiaza continutul celeilalte liste in lista curenta.*/
        list &operator=(list other) {
            swap(other);
            return *this;
        }

        /** Interschimba continutul acestei liste cu cel al listei de tip "rvalue". */
        list(list &&other) noexcept: count(0) {
            swap(other);
        }

        /** Interschimba continutul acestei liste cu cel al listei date.*/
        void swap(list &other) {
            std::swap(first, other.first);
            std::swap(last, other.last);
            std::swap(count, other.count);
        }

        /** Distruge obiectul lista.*/
        ~list() {
            destroy_list();
            delete last;
        }

        /** Returneaza un iterator ce reprezinta inceputul listei. */
        iterator begin() {
            return iterator(first);
        }

        /** Returneaza un iterator ce reprezinta sfarsitul listei. Acest iterator nu trebuie accesat deoarece nu are o referinta la vreun element din lista.*/
        iterator end() {
            return iterator(last);
        }

        /** Insereaza un nou element cu valoarea data inainte de elementul de la pozitia specificata de iterator.
         *
         * Returneaza un iterator la noul element inserat. */
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

        /** Sterge elementul de la pozitia specificata de iterator.
         *
         * Returneaza un iterator la elementul ce era succesorul elementului sters.
         */
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

        /** Returneaza numarul de elemente din lista. */
        size_t size() const {
            return count;
        }

        /** Verifica daca nu exista niciun element in lista. */
        bool empty() const {
            return count == 0;
        }

        /** Goleste lista. */
        void clear() {
            destroy_list();
            count = 0;
        }

        /** Returneaza o referinta la primul element al listei.
         *
         * Comportamentul acestei functii apelate cand lista este goala este indefinit.*/
        type &front() {
            return first->value;
        }

        /** Returneaza o referinta la ultimul element al listei.
         *
         * Comportamentul acestei functii apelate cand lista este goala este indefinit.*/

        type &back() {
            return last->previous->value;
        }

    };
}


#endif //DSL_LIST_H
