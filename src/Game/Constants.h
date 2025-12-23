#pragma once

enum CollisionLayer
{
	collision_solid = 1,
	collision_hitable = 2,
	collision_pickable = 4,
	collision_actor = 8,
	collision_entity = 16,
	collision_debris = 32,
	collision_character = 64,
	collision_raycast = 128,
	collision_ragdoll = 256,

	collision_map = collision_solid | collision_actor | collision_ragdoll | collision_hitable | collision_debris | collision_raycast
};

enum GameMaterial
{
	material_void = 0,
	material_stone = 1,
	material_metal = 2,
	material_wood = 3,
	material_glass = 4
};

enum GameObjectType
{
	object_unknown = 0,
	object_player = 1,
	object_item = 2,
	object_creature = 3,
	object_container = 4,
	object_debris = 5,
	object_slidedoor = 6,
	object_winch = 7,
	object_moving = 8,
	object_terminate = 0xFF
};