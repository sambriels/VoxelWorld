#include "VoxelWorld/Public/VWChunk.h"

#include "FastNoiseLite.h"
#include "Enums.h"

AVWChunk::AVWChunk()
{
}

void AVWChunk::Setup()
{
	Blocks.SetNum(Size * Size * Size);
}

void AVWChunk::GenerateHeightMap()
{
	const FVector Location = GetActorLocation();

	for (int x = 0; x < Size; ++x)
	{
		for (int y = 0; y < Size; ++y)
		{
			const float Xpos = (x * 100 + Location.X) / 100;
			const float Ypos = (y * 100 + Location.Y) / 100;

			// Value between -1 and 1
			const float NoiseValue = Noise->GetNoise(Xpos, Ypos);
			// Add one so value is between 0 and 2, final value will be between 0 and Size
			const int ScaledNoiseValue = FMath::RoundToInt((NoiseValue + 1) * Size / 2);
			// Clamp just to be safe
			const int Height = FMath::Clamp(ScaledNoiseValue, 0, Size);

			for (int z = 0; z < Height; ++z)
			{
				Blocks[GetBlockIndex(x, y, z)] = EBlock::Stone;
			}

			for (int z = Height; z < Size; ++z)
			{
				Blocks[GetBlockIndex(x, y, z)] = EBlock::Air;
			}
		}
	}
}

void AVWChunk::GenerateMesh()
{
	for (int x = 0; x < Size; ++x)
	{
		for (int y = 0; y < Size; ++y)
		{
			for (int z = 0; z < Size; ++z)
			{
				if (Blocks[GetBlockIndex(x, y, z)] != EBlock::Air)
				{
					const FVector Position = FVector(x, y, z);

					for (const EDirection Direction : DirectionOrder)
					{
						if (Check(GetPositionInDirection(Direction, Position)))
						{
							CreateFace(Direction, Position * 100);
						}
					}
				}
			}
		}
	}
}

// Basically check if block in position is AirBlock
bool AVWChunk::Check(FVector Position) const
{
	if (Position.X >= Size ||
		Position.Y >= Size ||
		Position.Z >= Size ||
		Position.X < 0 ||
		Position.Y < 0 ||
		Position.Z < 0)
	{
		return true;
	}
	return Blocks[GetBlockIndex(Position.X, Position.Y, Position.Z)] == EBlock::Air;
}

void AVWChunk::CreateFace(EDirection Direction, FVector Position)
{
	MeshData.Vertices.Append(GetFaceVertices(Direction, Position));
	MeshData.UV0.Append({
		FVector2D(1, 1),
		FVector2D(1, 0),
		FVector2D(0, 0),
		FVector2D(0, 1)
	});
	MeshData.Triangles.Append({
		VertexCount + 3,
		VertexCount + 2,
		VertexCount,
		VertexCount + 2,
		VertexCount + 1,
		VertexCount
	});
	VertexCount += 4;
}

TArray<FVector> AVWChunk::GetFaceVertices(EDirection Direction, FVector Position) const
{
	TArray<FVector> Vertices;

	for (int i = 0; i < 4; ++i)
	{
		const int TriangleDataIndex = i + static_cast<int>(Direction) * 4;
		const int VertexDataIndex = BlockTriangleData[TriangleDataIndex];

		const FVector Vertex = BlockVertexData[VertexDataIndex];

		Vertices.Add(Vertex + Position);
	}

	return Vertices;
}

FVector AVWChunk::GetPositionInDirection(EDirection Direction, FVector Position) const
{
	switch (Direction)
	{
	case EDirection::Forward: return Position + FVector::ForwardVector;
	case EDirection::Right: return Position + FVector::RightVector;
	case EDirection::Back: return Position + FVector::BackwardVector;
	case EDirection::Left: return Position + FVector::LeftVector;
	case EDirection::Up: return Position + FVector::UpVector;
	case EDirection::Down: return Position + FVector::DownVector;
	default: throw std::invalid_argument("Invalid direction supplied");
	}
}

int AVWChunk::GetBlockIndex(int X, int Y, int Z) const
{
	return Z * Size * Size + Y * Size + X;
}
