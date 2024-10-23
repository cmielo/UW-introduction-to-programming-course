//Author: Gracjan Barski

#include <cstdlib>
#include "kol.h"

using namespace std;

namespace {

typedef struct list {
    interesant *head, *tail;
} list;

bool isempty(const list &l) {
    return l.head->l1 == l.tail;
}

list create_list() {
    list res;
    res.head = (interesant*) malloc(sizeof(interesant));
    res.tail = (interesant*) malloc(sizeof(interesant));
    res.head->l1 = res.tail;
    res.head->l2 = NULL;
    res.tail->l1 = res.head;
    res.tail->l2 = NULL;
    return res;
}

//If the list is empty, it gets destroyed
void destroy_list(list &l) {
    if(isempty(l)) {
        free(l.head);
        free(l.tail);
    }
}

interesant* front(const list &l) {
    return l.head->l1;
}

interesant* back(const list &l) {
    return l.tail->l1;
}

//Returns the "next" node in order prev-current-next
interesant* get_next(interesant* prev, interesant* current) {
    if(prev && current) {
        if(current->l1 == prev)
            return current->l2;
        else
            return current->l1;
    }
    return NULL;
}

void link(interesant *el, interesant *old, interesant *nw) {
    if(el) {
        if(el->l1 == old)
            el->l1 = nw;
        else
            el->l2 = nw;
    }
}

void push_back(list &l, interesant* i) {
    i->l1 = l.tail;
    i->l2 = back(l);
    link(back(l), l.tail, i);
    l.tail->l1 = i;
}

//Returns the pushed node
interesant* push_back(list &l, int x) {
    interesant *i = (interesant*) malloc(sizeof(interesant));
    i->val = x;
    push_back(l, i);
    return i;
}

void remove_node(interesant* el) {
    link(el->l1, el, el->l2);
    link(el->l2, el, el->l1);
    el->l1 = NULL;
    el->l2 = NULL;
}

//Returns the popped node
interesant* pop_front(list &l) {
    if (!isempty(l)) {
        interesant* el = front(l);
        remove_node(el);
        return el;
    }
    return NULL;
}

//Appends two lists, leaves l2 as an empty list
void append(list &l1, list &l2) {
    interesant *b = l1.tail;
    interesant *f = l2.head;
    link(back(l1), b, front(l2));
    link(front(l2), f, back(l1));
    
    //Reset l2
    b->l1 = f;
    f->l1 = b;
    b->l2 = NULL;
    f->l2 = NULL;
    l1.tail = l2.tail;
    l2.head = f;
    l2.tail = b;
}

//Connects nodes "left" and "right" and disconnects what lies between them 
//arguments are in order: ... - left - left_prev - ... - right_prev - right - ... (or reversed)
void connect(interesant* left, interesant* left_prev, interesant* right_prev, interesant* right) {
    if(left->l1 == left_prev)
        left->l1 = right;
    else
        left->l2 = right;
    
    if(right->l1 == right_prev)
        right->l1 = left;
    else
        right->l2 = left;

    //Disconnect the middle from the list
    if(left_prev->l1 == left)
        left_prev->l1 = NULL;
    else
        left_prev->l2 = NULL;
    
    if(right_prev->l1 == right)
        right_prev->l1 = NULL;
    else
        right_prev->l2 = NULL;
}

void reverse(list &l) {
    interesant *tmp = l.head;
    l.head = l.tail;
    l.tail = tmp;
}

vector<list> queues; 

int current_number;

}

void otwarcie_urzedu(int m) {
    queues.resize(m);
    for (int i = 0; i < m; ++i)
        queues[i] = create_list();
    current_number = 0;
}

interesant* nowy_interesant(int k) {
    return push_back(queues[k], current_number++);
}

int numerek(interesant *i) {
    return i->val;
}

interesant* obsluz(int k) {
    if (isempty(queues[k]))
        return NULL;
    return pop_front(queues[k]);
}

void zmiana_okienka(interesant *i, int k) {
    remove_node(i);
    push_back(queues[k], i);
}

void zamkniecie_okienka(int k1, int k2) {
    append(queues[k2], queues[k1]);
}

std::vector<interesant*> fast_track(interesant *i1, interesant *i2) {
    vector<interesant*> res;    
    if(i1 == i2) {
        remove_node(i1);
        res.push_back(i1);    
        return res;
    }

    //Find the proper nodes
    interesant* prev1 = i1;
    interesant* prev2 = i1;
    interesant* iterator1 = i1->l1; 
    interesant* iterator2 = i1->l2;
    interesant* temp;

    //Look for i2
    while(prev1 != i2 && prev2 != i2) {
        temp = iterator1;
        iterator1 = get_next(prev1, iterator1);
        prev1 = temp;

        temp = iterator2;
        iterator2 = get_next(prev2, iterator2);
        prev2 = temp;
    }

    //Now either of prev1 or prev2 is equal to i2    
    //Remove the subqueue from the original queue
    if(prev1 == i2)
        connect(i1->l2, i1, i2, iterator1);
    else
        connect(i1->l1, i1, i2, iterator2);

    prev1 = i1;
    iterator1 = (prev1->l1 ? prev1->l1 : prev1->l2);
    while(prev1) {
        res.push_back(prev1);
        temp = iterator1;
        iterator1 = get_next(prev1, iterator1);
        // Clear fields
        prev1->l1 = NULL;
        prev1->l2 = NULL;

        prev1 = temp;
    } 

    return res;
}

void naczelnik(int k) {
    reverse(queues[k]);
}

std::vector<interesant*> zamkniecie_urzedu() {
    vector<interesant*> res;
    interesant* current;

    for(size_t i = 0; i < queues.size(); i++) {    
        while(!isempty(queues[i])) {
            current = front(queues[i]);
            remove_node(current);
            res.push_back(current);
        }

        destroy_list(queues[i]); //destroy empty list
    }

    queues.resize(0);
    return res;
}
