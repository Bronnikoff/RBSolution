// Made by Max Bronnikov
#ifndef HASH_TABLE
#define HASH_TABLE

#include <vector>
#include <iostream>
#include <utility>
#include <inttypes.h>

#define START_N 4 
#define LOAD_FACTOR 0.8

// We use hash table with open adressing because table with chains based on lists
// and in classical schema of table all inserts calls operator new for add element to list.


// Also i find interesting information about open adressing tables optimisation and use it in my realisation.
// Link: https://habr.com/ru/company/mailru/blog/323242/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////   ITERATOR ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Simple itterator(needs for incapsulate Hash Table search anwser)

template <class Key, class Value>
struct table_item{
    int8_t margin;
    std::pair<Key, Value> object;
};


template <class Key, class Value>
class Iterator{
  public:
    Iterator(std::vector<table_item<Key, Value>>& tab, size_t num) : table(tab), index(num){}

    std::pair<Key, Value>* operator *(){
        return &table[index].object;
    }

    std::pair<Key, Value>* operator ->(){
        return &table[index].object;
    }

    void operator ++(){
        do{
            ++index;
        }while(table[index].margin < 0 && index < table.size());
    }

    Iterator begin(){
        index = 0;
        while(table[index].margin < 0 && index < table.size()){
            ++index;
        }
        return *this;
    }

    Iterator end(){
        index = table.size();
        return *this;
    }

    Iterator operator ++(int){
        Iterator iter(*this);
        ++(*this);
        return iter;
    }

    bool operator ==(Iterator const& i){
        return (i.index == index);
    }

    bool operator !=(Iterator const& i){
        return !(*this == i);
    }


  private:
    std::vector<table_item<Key, Value>>& table;
    size_t index;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// HASH TABLE //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Minus of this realisation - we store a lot extra objects(good choice - store pointers to objects)
template <class Key, class Value, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
class custom_map{
    public:
        custom_map();
        void erase(const Key& key);
        void insert(const Key& key, const Value& value);
        bool empty();
        Iterator<Key, Value> search(const Key& key);
        Iterator<Key, Value> begin();
        Iterator<Key, Value> end();
        ~custom_map(){}



    private:

        int8_t log2;
        std::vector<table_item<Key, Value>> table_data; // Hash table with open adressing
        size_t table_capacity;
        size_t table_size;

        Hash hash_function; 
        KeyEqual equal_function;
        size_t get_index(const Key& key);
        void rehash();
};





// Returns itterrator on first elem(if empty returns end())
template <class Key, class Value, class Hash, class KeyEqual>
Iterator<Key, Value> custom_map<Key, Value, Hash, KeyEqual>::begin(){
    return Iterator<Key, Value>(table_data, 0).begin();
}

// Return iiterator to end of map. Naming this itterator gives segfault
template <class Key, class Value, class Hash, class KeyEqual>
Iterator<Key, Value> custom_map<Key, Value, Hash, KeyEqual>::end(){
    return Iterator<Key, Value>(table_data, 0).end();
}



// index of ideal position for insert in table;
template <class Key, class Value, class Hash, class KeyEqual>
size_t custom_map<Key, Value, Hash, KeyEqual>::get_index(const Key& key){
    return hash_function(key) & (table_capacity - 1); // because table_capacity === 2^n
}



// Insert will push back new element to allocator(work with memory will do vector) and insert object in table by Robin Hood hashing
template <class Key, class Value, class Hash, class KeyEqual>
void custom_map<Key, Value, Hash, KeyEqual>::insert(const Key& key, const Value& value){
    size_t ideal_position = get_index(key);
    int8_t dist = 0;

    // At first check existing element with this key in map and if find - change value
    for(size_t i = ideal_position; table_data[i].margin >= dist; ++i, ++dist){
        // term of existing item with key in table
        if(equal_function(table_data[i].object.first, key)){
            table_data[i].object.second = value;
            return;
        }
    }

    // We are here if we need create new item for table

    // if size of table more Load factor we shoud rehash table
    // if size of claster more log2(table_capacity) then rehash table for speed
    if(dist > log2 || static_cast<double>(table_size) >= static_cast<double>(table_capacity) * LOAD_FACTOR){
        rehash();
        ideal_position = get_index(key);
    }

    // create new object for insert
    std::pair<Key, Value> obj(key, value); 
    dist = 0;

    // inserting...
    size_t i = ideal_position;
    // while we not meet empty block
    for( ; table_data[i].margin >= 0; ++i, ++dist){
        // if element in pos i rich => insert obj to pos i and search next for rich element
        if(table_data[i].margin < dist){
            std::swap(obj, table_data[i].object);
            std::swap(dist, table_data[i].margin);
            continue;
        }
    }
    table_data[i].margin = dist;
    table_data[i].object = obj;

    ++table_size;
}



//Deletes element with key from map. Do nothing if element not exist
template <class Key, class Value, class Hash, class KeyEqual>
void custom_map<Key, Value, Hash, KeyEqual>::erase(const Key& key){
    size_t ideal_position = get_index(key);
    int8_t dist = 0;
    // At first search element in table
    size_t i = ideal_position;
    for( ; !equal_function(table_data[i].object.first, key); ++i, ++dist){
        // if distance more then object margin => element not exist 
        if(dist > table_data[i].margin){
            return;
        }
    }
    // i - index of removing elem in table

    // Remove:
    table_data[i].margin = -1;
    for(++i; table_data[i].margin > 0; ++i){
        --table_data[i].margin;
        std::swap(table_data[i - 1], table_data[i]);
    }


    --table_size;
}



// Create my custom map
template <class Key, class Value, class Hash, class KeyEqual>
custom_map<Key, Value, Hash, KeyEqual>::custom_map(){
    log2 = START_N;
    table_capacity = (1 << START_N);
    table_size = 0;
    table_data.resize(table_capacity + log2, 
            table_item<Key, Value>{-1, std::pair<Key, Value>()});
}



// Check map to emprty
template <class Key, class Value, class Hash, class KeyEqual>
bool custom_map<Key, Value, Hash, KeyEqual>::empty(){
    return table_size == 0;
}



// minus of this realisation: we need 2*n of extra memory(need more intellectual function)
template <class Key, class Value, class Hash, class KeyEqual>
void custom_map<Key, Value, Hash, KeyEqual>::rehash(){
    // incrementation of size
    ++log2;
    table_capacity <<= 1;

    // alloc new data
    std::vector<table_item<Key, Value>> new_data(log2 + table_capacity, 
            table_item<Key, Value>{-1, std::pair<Key, Value>()});

    // rehash old items
    for(size_t j = 0; j < table_data.size(); ++j){
        if(table_data[j].margin < 0){
            continue;
        }

        size_t ideal_position = hash_function(table_data[j].object.first);
        std::pair<Key, Value> obj(table_data[j].object.first, table_data[j].object.second); 
        int8_t dist = 0;

        // inserting...
        size_t i = ideal_position;
        // while we not meet empty block
        for( ; new_data[i].margin >= 0; ++i, ++dist){
            // if element in pos i rich => insert obj to pos i and search next for rich element
            if(new_data[i].margin < dist){
                std::swap(obj, new_data[i].object);
                std::swap(dist, new_data[i].margin);
                continue;
            }
        }
        new_data[i].margin = dist;
        new_data[i].object = obj;
    }
    table_data = new_data;
}



// Return iterator to element with key
template <class Key, class Value, class Hash, class KeyEqual>
Iterator<Key, Value> custom_map<Key, Value, Hash, KeyEqual>::search(const Key& key){
    size_t ideal_position = hash_function(key);
    int8_t dist = 0;
    for(size_t i = ideal_position; ;++i, ++dist){
        if(table_data[i].margin < dist){
            return end();
        }
        if(equal_function(table_data[i].object.first, key)){
            return Iterator<Key, Value>(table_data, i);
        }
    }
}


#endif