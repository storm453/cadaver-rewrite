enum EntityType
{
    entity_none = 0,
    entity_object,
    entity_player
};

struct Vec2
{
    float x;
    float y;
};

struct Entity
{
    EntityType type = entity_none;
    Vec2 position = { x: 0, y: 0 };
};