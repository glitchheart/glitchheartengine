#ifndef LIST_H
#define LIST_H

namespace list
{
    template<typename T, size_t SIZE>
    struct List
    {
        T items[SIZE];
        i32 count;

        T& operator[](i32 index)
        {
            return items[index];
        }

        void add(T &value)
        {
            assert(count < SIZE);
                
            items[count++] = value;
        }
    };

    template<typename T>
    struct SwapList
    {
        T *items;
        i32 count;
            
        i32 *_internal_handles;
        i32 _current_internal_handle;
        size_t max_size;

        T& operator[](i32 index)
        {
            return items[index];
        }
    };

    template<typename T>
    static bool has_value(SwapList<T> *list, i32 index)
    {
        if(index == 0)
            return false;
        return list->_internal_handles[index - 1] != -1;
    }
    
    static i32 _find_handle_in_range(i32 start, i32 end, i32 *handles)
    {
        i32 handle = -1;
        for(i32 i = start; i < end; i++)
        {
            if(handles[i] == -1)
            {
                handle = i;
                break;
            }
        }
        return handle;
    }

    template<typename T>
    static i32 _find_next_free_internal_handle(SwapList<T>* list)
    {
        i32 handle = -1;

        if(list->_internal_handles[list->_current_internal_handle] == -1)
        {
            handle = list->_current_internal_handle;
        }
        else
        {
            i32 found_handle = _find_handle_in_range(list->_current_internal_handle, list->max_size, list->_internal_handles);

            if(found_handle == -1)
            {
                found_handle = _find_handle_in_range(0, list->_current_internal_handle, list->_internal_handles);
            }
            handle = found_handle;
        }

        list->_current_internal_handle = handle + 1;
        if(list->_current_internal_handle == list->max_size)
        {
            list->_current_internal_handle = 0;
        }
        return handle;
    }

    template<typename T>
    static T& get(SwapList<T>* list, i32 index)
    {
        assert(list->max_size > index);
        i32 _internal_handle = list->_internal_handles[index - 1];
        return list->items[_internal_handle];
    }

    template<typename T>
    static i32 get_internal_handle(SwapList<T> *list, i32 index)
    {
        return list->_internal_handles[index - 1];
    }

    template<typename T>
    static i32 add(SwapList<T>* list, T value)
    {
        i32 new_handle = _find_next_free_internal_handle<T>(list) + 1;
        list->_internal_handles[new_handle - 1] = list->count++;
        list->items[list->_internal_handles[new_handle - 1]] = value;
        return new_handle;
    }

    template<typename T>
    static void remove(SwapList<T>* list, i32 index)
    {
        if(index == 0)
            return;
        
        i32 _internal_handle = list->_internal_handles[index] - 1;
        list->items[_internal_handle] = list->items[list->count - 1];
        list->count--;
    }

    template<typename T>
    static void clear(SwapList<T>* list)
    {
        list->count = 0;
        list->_current_internal_handle = 0;
        for(i32 i = 0; i < list->max_size; i++)
        {
            list->_internal_handles[i] = -1;
        }
    }

    template<typename T>
    static void free(SwapList<T>* list)
    {
        clear(list);
        ::free(list->_internal_handles);
        ::free(list->items);
    }
    
    template<typename T>
    static void _init(SwapList<T>* list, i32 count)
    {
        list->count = 0;
        list->_current_internal_handle = 0;
        list->max_size = count;

        for(i32 i = 0; i < list->max_size; i++)
        {
            list->_internal_handles[i] = -1;
        }
    }

    template<typename T>
    static void init(SwapList<T>* list, i32 count)
    {
        list->_internal_handles = (i32*)malloc(sizeof(i32) * count);
        list->items = (T*)malloc(sizeof(T) * count);
        _init(list, count);
    }

    template<typename T>
    static void init(SwapList<T>* list, i32 count, MemoryArena *arena)
    {
        list->internal_handles = push_array(arena, count, i32);
        list->items = push_array(arena, count, T);
        _init(list, count);
    }
}

#endif
