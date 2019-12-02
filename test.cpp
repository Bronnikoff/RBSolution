#include "HashTable.hpp"
#include <iostream>
#include <string>

using namespace std;

typedef struct functor{
    // Should be >= 0
    size_t operator()(const int i){
        return i + 4;
    }
}functor;

// The next notes on russian. Not important information


// Этот тест приложен лишь для демонстрации функций таблицы

// В будущем необходимо продумать более экономичный способ хранения объектов и перехеширования таблицы
// Также следует добавить обработку исключений, например получения отрицательного хэша
// Следует лучше протестировать и отладить данный код

int main(){
    custom_map<int, int, functor> table;
    cout << "Table is empty: " << table.empty() << endl;
    for(int i = 0; i < 1000000; ++i){
        table.insert(i*2, i*3);
    }
    for(int i = 0; i <= 1000000; i+=200000){
        auto it = table.search(i*2);
        if(it == table.end()){
            cout << "not exist" << endl;
        }else{
            cout << it->first << " : " << it->second << endl;
        }

    }
    for(int i = 1000001; i >= 0; --i){
        table.erase(i);
    }
    return 0;
}