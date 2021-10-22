#pragma once

template <class T>
struct ListItem {
    const char* id;
    T* item;
    ListItem<T>* next;
};

template <class T>
class ListIterator;

template <class T>
class List {
    protected:
        ListItem<T> *list = nullptr;

    public:
        friend class ListIterator<T>;

    public:
        void add(const char* id, T* item);
        T* get(const char* id);

};

template <class T>
void List<T>::add(const char* id, T* item){
    ListItem<T>* i = new ListItem<T>();
    i->id = id;
    i->item = item;
    i->next = nullptr;
    if (!list) list = i;
    else {
        ListItem<T>* l = list;
        while (l->next) l = l->next;
        l->next = i;
    }
};

template <class T>
T* List<T>::get(const char* id){
    ListItem<T>* j = list;
    while (j){
        if (strcmp(j->id,id) == 0) return j->item;
        j = j->next;
    }
    return nullptr;
};

template <class T>
class ListIterator {
    public:
        List<T> list;
        ListItem<T> *ptr;
    public:
        ListIterator(List<T> &list);
        T* get();
        bool next();
        void reset();
};

template <class T>
ListIterator<T>::ListIterator(List<T> &list){
    this->list = list;
};

template <class T>
T* ListIterator<T>::get(){
    T* i = nullptr;
    if (ptr) {
        i = ptr->item;
        ptr = ptr->next;
    }
    return i;
};

template <class T>
bool ListIterator<T>::next(){
    return ptr != nullptr;
};

template <class T>
void ListIterator<T>::reset(){
    ptr = list.list;
};