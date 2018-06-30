#ifndef ECS_TEST

template<typename T>
struct Pool
{
    size_t size;
    size_t capacity;
    
    T *list;
    
    T &operator[](size_t index)
    {
        // do bounds checking?
        return this->list[index];
    }
};

#define BASE_CAPACITY 64

template<typename T>
static void add(Pool<T> &pool, T t)
{
    if(pool.capacity == 0)
    {
        pool.capacity = MAX(BASE_CAPACITY, sizeof(T));
        pool.list = (T*)malloc(sizeof(T) * pool.capacity);
    }
    
    if(pool.size + 1 > pool.capacity)
    {
        pool.capacity += MAX(BASE_CAPACITY, sizeof(T));
        pool.list = (T*)realloc(pool.list, pool.capacity);
    }
    
    pool.list[pool.size++] = t;
}

struct Transform
{
    math::Vec3 position;
    math::Vec3 scale;
    math::Quat orientation;
};

struct Rendering
{
    math::Rgba color;
};

struct _Component
{};

template<typename T>
struct Component : public _Component
{};

typedef Component<Transform> TransformComponent;
typedef Component<Rendering> RenderingComponent;


struct EntityCache
{
    Pool<_Component*> component_pools;
    
};

template<typename C>
static void add_component(EntityCache &cache, Entity &entity)
{
    
    
    
}

#endif

