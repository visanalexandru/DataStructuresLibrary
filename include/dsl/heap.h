//
// Created by gvisan on 10.02.2021.
//

#ifndef DSL_HEAP_H
#define DSL_HEAP_H

#include<vector>    //for std::vector
#include<algorithm> //for std::swap

namespace dsl {
    template<class type>
    class heap {
    private:
        std::vector<type> data;

        void shift(unsigned node);

        unsigned count;

    public:
        void build(const std::vector<type> &p);//builds the heap from a vector

        type top() const; //returns the minimum element

        unsigned size() const;

        heap();
    };


    inline unsigned father(unsigned node) {
        return node >> 1u;
    }

    inline unsigned left(unsigned node) {
        return node << 1u;
    }

    inline unsigned right(unsigned node) {
        return (node << 1u) + 1;
    }

    template <class type>
    inline unsigned int heap<type>::size() const {
        return count;
    }

    template<class type>
    inline heap<type>::heap():count(0) {

    }

    template<class type>
    inline type heap<type>::top() const {
        return data[1];
    }

    template<class type>
    inline void heap<type>::shift(unsigned int node) {
        unsigned l = left(node), r = right(node);

        if (l <= count) {
            if (r <= count) {
                unsigned next = (data[l] > data[r]) ? l : r;

                if (data[next] > data[node]) {
                    std::swap(data[node], data[next]);
                    shift(next);
                }

            } else {
                if (data[l] > data[node]) {
                    std::swap(data[node], data[l]);
                    shift(l);
                }
            }
        }
    }

    template<class type>
    inline void heap<type>::build(const std::vector<type> &p) {
        data.resize(p.size() + 1);
        std::copy(p.begin(), p.end(), data.begin() + 1);
        count= p.size();

        for (unsigned i = count/2; i >= 1; i--) {
            shift(i);
        }
    }
}


#endif //DSL_HEAP_H
