#include "Chunks/VWGreedyChunk.h"

#include "Utils/Enums.h"
#include "Utils/FastNoiseLite.h"

void AVWGreedyChunk::Setup()
{
	Blocks.SetNum(Size * Size * Size);
}

void AVWGreedyChunk::Generate3DHeightMap(FVector Position)
{
	for (int x = 0; x < Size; ++x)
	{
		for (int y = 0; y < Size; ++y)
		{
			for (int z = 0; z < Size; ++z)
			{
				const auto NoiseValue = Noise->GetNoise(x + Position.X, y + Position.Y, z + Position.Z);

				if (NoiseValue >= 0)
				{
					Blocks[GetBlockIndex(x, y, z)] = EBlock::Air;
				}
				else
				{
					Blocks[GetBlockIndex(x, y, z)] = EBlock::Stone;
				}
			}
		}
	}
}

void AVWGreedyChunk::Generate2DHeightMap(FVector Position)
{
	for (int x = 0; x < Size; x++)
	{
		for (int y = 0; y < Size; y++)
		{
			const float Xpos = x + Position.X;
			const float Ypos = y + Position.Y;

			const int Height = FMath::Clamp(FMath::RoundToInt((Noise->GetNoise(Xpos, Ypos) + 1) * Size / 2), 0, Size);

			for (int z = 0; z < Height; z++)
			{
				Blocks[GetBlockIndex(x, y, z)] = EBlock::Stone;
			}

			for (int z = Height; z < Size; z++)
			{
				Blocks[GetBlockIndex(x, y, z)] = EBlock::Air;
			}
		}
	}
}

void AVWGreedyChunk::GenerateMesh()
{
	for (int Axis = 0; Axis < 3; ++Axis)
	{
		const int Axis1 = (Axis + 1) % 3;
		const int Axis2 = (Axis + 2) % 3;

		FIntVector DeltaAxis1 = FIntVector::ZeroValue;
		FIntVector DeltaAxis2 = FIntVector::ZeroValue;

		FIntVector ChunkItr = FIntVector::ZeroValue;
		FIntVector AxisMask = FIntVector::ZeroValue;

		AxisMask[Axis] = 1;

		TArray<FMask> Mask;
		Mask.SetNum(Size * Size);

		// Check each slice
		for (ChunkItr[Axis] = -1; ChunkItr[Axis] < Size;)
		{
			int MaskIndex = 0;

			for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Size; ++ChunkItr[Axis2])
			{
				for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Size; ++ChunkItr[Axis1])
				{
					const EBlock CurrentBlock = GetBlock(ChunkItr);
					// Next block in the direction we are iterating
					const EBlock NextBlock = GetBlock(ChunkItr + AxisMask);

					const bool bIsCurrentBlockOpaque = CurrentBlock != EBlock::Air;
					const bool bIsNextBlockOpaque = NextBlock != EBlock::Air;

					// If they are both opaque or both not opaque, we don't need to create a face
					if (bIsCurrentBlockOpaque == bIsNextBlockOpaque)
					{
						Mask[MaskIndex++] = FMask{EBlock::Null, 0};
					}
					else if (bIsCurrentBlockOpaque)
					{
						Mask[MaskIndex++] = FMask{CurrentBlock, 1};
					}
					else
					{
						Mask[MaskIndex++] = FMask{NextBlock, -1};
					}
				}
			}
			++ChunkItr[Axis]; // Todo: get this inside for loop, should be doable
			MaskIndex = 0;

			// Generate Mesh from the mask
			for (int j = 0; j < Size; ++j)
			{
				for (int i = 0; i < Size;)
				{
					if (Mask[MaskIndex].Normal != 0)
					{
						const auto CurrentMask = Mask[MaskIndex];
						ChunkItr[Axis1] = i;
						ChunkItr[Axis2] = j;

						int Width;

						for (Width = 1; i + Width < Size && CompareMask(CurrentMask, Mask[MaskIndex + Width]); ++Width)
						{
						}

						int Height;
						bool bNextBlockIsDifferent = false;

						for (Height = 1; j + Height < Size; ++Height)
						{
							for (int k = 0; k < Width; ++k)
							{
								if (CompareMask(CurrentMask, Mask[MaskIndex + k + Height * Size]))
								{
									continue;
								}
								bNextBlockIsDifferent = true;
								break;
							}
							if (bNextBlockIsDifferent)
							{
								break;
							}
						}

						DeltaAxis1[Axis1] = Width;
						DeltaAxis2[Axis2] = Height;

						CreateQuad(
							CurrentMask,
							AxisMask,
							ChunkItr,
							ChunkItr + DeltaAxis1,
							ChunkItr + DeltaAxis2,
							ChunkItr + DeltaAxis1 + DeltaAxis2
						);

						// Reset
						DeltaAxis1 = FIntVector::ZeroValue;
						DeltaAxis2 = FIntVector::ZeroValue;

						for (int l = 0; l < Height; ++l)
						{
							for (int k = 0; k < Width; ++k)
							{
								Mask[MaskIndex + k + l * Size] = FMask{EBlock::Null, 0};
							}
						}

						i += Width;
						MaskIndex += Width;
					}
					else
					{
						i++;
						MaskIndex ++;
					}
				}
			}
		}
	}
}

void AVWGreedyChunk::CreateQuad(FMask Mask, FIntVector AxisMask, FIntVector V1, FIntVector V2, FIntVector V3,
                                FIntVector V4)
{
	const auto Normal = FVector(AxisMask * Mask.Normal);

	MeshData.Vertices.Add(FVector(V1) * 100);
	MeshData.Vertices.Add(FVector(V2) * 100);
	MeshData.Vertices.Add(FVector(V3) * 100);
	MeshData.Vertices.Add(FVector(V4) * 100);

	MeshData.Triangles.Add(VertexCount);
	MeshData.Triangles.Add(VertexCount + 2 + Mask.Normal);
	MeshData.Triangles.Add(VertexCount + 2 - Mask.Normal);

	MeshData.Triangles.Add(VertexCount + 3);
	MeshData.Triangles.Add(VertexCount + 1 - Mask.Normal);
	MeshData.Triangles.Add(VertexCount + 1 + Mask.Normal);

	MeshData.UV0.Add(FVector2D(0, 0));
	MeshData.UV0.Add(FVector2D(0, 1));
	MeshData.UV0.Add(FVector2D(1, 0));
	MeshData.UV0.Add(FVector2D(1, 1));

	MeshData.Normals.Add(Normal);
	MeshData.Normals.Add(Normal);
	MeshData.Normals.Add(Normal);
	MeshData.Normals.Add(Normal);

	VertexCount += 4;
}

int AVWGreedyChunk::GetBlockIndex(int X, int Y, int Z) const
{
	return Z * Size * Size + Y * Size + X;
}

EBlock AVWGreedyChunk::GetBlock(FIntVector Index) const
{
	if (Index.X >= Size ||
		Index.Y >= Size ||
		Index.Z >= Size ||
		Index.X < 0 ||
		Index.Y < 0 ||
		Index.Z < 0)
	{
		return EBlock::Air;
	}
	return Blocks[GetBlockIndex(Index.X, Index.Y, Index.Z)];
}

bool AVWGreedyChunk::CompareMask(FMask M1, FMask M2)
{
	return M1.Block == M2.Block && M1.Normal == M2.Normal;
}
