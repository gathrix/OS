
#ifndef PROJECT_BASE_LIST_HPP
#define PROJECT_BASE_LIST_HPP

template<typename T>
class List{

private:
    struct Elem{
        T* data;
        Elem* next;
        Elem(T* data, Elem* next): data(data), next(next) {}
    };
    Elem* head, *tail;


public:
    List() : head(nullptr), tail(nullptr){}
    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T* data){
        Elem* item = new Elem(data, head);
        head = item;
        if(!tail) tail = head;
    }

    void addLast(T* data){
        Elem* item = new Elem(data, 0);
        if(tail){
            tail->next = item;
            tail = item;
        }else{
            head = tail = item;
        }
    }
    T* removeFirst(){
        if(!head) {return 0;}
        Elem* tmp = head;
        head = head->next;
        if(!head) tail = 0;
        T* r = tmp->data;
        delete tmp;
        return r;

    }

    T* peekFirst(){
        if(!head) {return 0;}
        return head->data;
    }

    T* removeLast() {
        if(!head) {return 0;}
        Elem* prev = 0;
        for(Elem* cur = head; cur && cur != tail; cur = cur->next){
            prev = cur;

        }
        Elem* tmp = tail;
        if(prev) {prev->next = 0;}
        else{
            head = 0;
        }
        tail = prev;

        T*ret = tmp->data;
        delete tmp;
        return ret;



    }

    T* peekLast(){
        if(!tail) {return 0;}
        return tail->data;
    }

};





#endif