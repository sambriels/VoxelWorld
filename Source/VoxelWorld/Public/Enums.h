#pragma once

enum class EDirection
{
	Forward,
	Right,
	Back,
	Left,
	Up,
	Down
};

const auto DirectionOrder = {
	EDirection::Forward,
	EDirection::Right,
	EDirection::Back,
	EDirection::Left,
	EDirection::Up,
	EDirection::Down
};

enum class EBlock
{
	Null,
	Air,
	Stone,
	Dirt,
	Grass
};
