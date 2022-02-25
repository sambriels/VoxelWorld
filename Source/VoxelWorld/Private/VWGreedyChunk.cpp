#include "VWGreedyChunk.h"

#include "Enums.h"
#include "FastNoiseLite.h"
#include "ProceduralMeshComponent.h"

AVWGreedyChunk::AVWGreedyChunk()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	Mesh->SetCastShadow(false);

	Noise = new FastNoiseLite();

	Blocks.SetNum(Size.X * Size.Y * Size.Z);

	SetRootComponent(Mesh);
}

void AVWGreedyChunk::BeginPlay()
{
	Super::BeginPlay();

	Noise->SetFrequency(0.03f);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

	GenerateBlocks();
	GenerateMesh();
	ApplyMesh();
}

void AVWGreedyChunk::GenerateBlocks()
{
	const FVector Location = GetActorLocation();

	for (int x = 0; x < Size.X; ++x)
	{
		for (int y = 0; y < Size.Y; ++y)
		{
			const float Xpos = (x * 100 + Location.X) / 100;
			const float Ypos = (y * 100 + Location.Y) / 100;

			// Value between -1 and 1
			const float NoiseValue = Noise->GetNoise(Xpos, Ypos);
			// Add one so value is between 0 and 2, final value will be between 0 and Size
			const int ScaledNoiseValue = FMath::RoundToInt((NoiseValue + 1) * Size.Z / 2);
			// Clamp just to be safe
			const int Height = FMath::Clamp(ScaledNoiseValue, 0, Size.Z);

			for (int z = 0; z < Height; ++z)
			{
				Blocks[GetBlockIndex(x, y, z)] = EBlock::Stone;
			}

			for (int z = Height; z < Size.Z; ++z)
			{
				Blocks[GetBlockIndex(x, y, z)] = EBlock::Air;
			}
		}
	}
}

void AVWGreedyChunk::ApplyMesh()
{
	Mesh->CreateMeshSection(
		0,
		MeshData.Vertices,
		MeshData.Triangles,
		MeshData.Normals,
		MeshData.UV0,
		TArray<FColor>(),
		TArray<FProcMeshTangent>(),
		false
	);
}

void AVWGreedyChunk::GenerateMesh()
{
	for (int Axis = 0; Axis < 3; ++Axis)
	{
		const int Axis1 = (Axis +1 ) % 3;
		const int Axis2 = (Axis +2 ) % 3;

		const int MainAxisLimit = Size[Axis];
		int Axis1Limit = Size[Axis1];
		int Axis2Limit = Size[Axis2];

		FIntVector DeltaAxis1 = FIntVector::ZeroValue;
		FIntVector DeltaAxis2 = FIntVector::ZeroValue;

		FIntVector ChunkItr = FIntVector::ZeroValue;
		FIntVector AxisMask = FIntVector::ZeroValue;

		AxisMask[Axis] = 1;

		TArray<FMask> Mask;
		Mask.SetNum(Axis1Limit * Axis2Limit);

		// Check each slice
		for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit; )
		{
			int MaskIndex = 0;

			for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2])
			{
				for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1])
				{
					const EBlock CurrentBlock = GetBlock(ChunkItr);
					// Next block in the direction we are iterating
					const EBlock NextBlock = GetBlock(ChunkItr + AxisMask);

					const bool bIsCurrentBlockOpaque = CurrentBlock != EBlock::Air;
					const bool bIsNextBlockOpaque = NextBlock != EBlock::Air;

					// If they are both opaque or both not opaque, we don't need to create a face
					if (bIsCurrentBlockOpaque == bIsNextBlockOpaque)
					{
						Mask[MaskIndex++] = FMask{ EBlock::Null, 0};
					} else if (bIsCurrentBlockOpaque)
					{
						Mask[MaskIndex++] = FMask{CurrentBlock, 1};
					} else
					{
						Mask[MaskIndex++] = FMask{NextBlock, -1};
					}
				}	
			}
			++ChunkItr[Axis]; // Todo: get this inside for loop, should be doable
			MaskIndex = 0;
			
			// Generate Mesh from the mask
			for (int j = 0; j < Axis2Limit; ++j)
			{
				for (int i = 0; i < Axis1Limit;)
				{
					if (Mask[MaskIndex].Normal != 0)
					{
						const auto CurrentMask = Mask[MaskIndex];
						ChunkItr[Axis1] = i;
						ChunkItr[Axis2] = j;

						int Width;

						for (Width = 1; i + Width < Axis1Limit && CompareMask(CurrentMask, Mask[MaskIndex + Width]); ++Width)
						{
							
						}

						int Height;
						bool bNextBlockIsDifferent = false;

						for (Height = 1; j + Height < Axis2Limit; ++Height)
						{
							for (int k = 0; k < Width; ++k)
							{
								if (CompareMask(CurrentMask, Mask[MaskIndex + k + Height * Axis1Limit]))
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
								Mask[MaskIndex + k + l * Axis1Limit] = FMask {EBlock::Null, 0};
							}
						}

						i+= Width;
						MaskIndex += Width;
					} else
					{
						i++;
						MaskIndex ++;
					}
				}
			}
		}
	}
}

void AVWGreedyChunk::CreateQuad(FMask Mask, FIntVector AxisMask, FIntVector V1, FIntVector V2, FIntVector V3, FIntVector V4)
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
	return Z * Size.X * Size.Y + Y * Size.X + X;
}

EBlock AVWGreedyChunk::GetBlock(FIntVector Index) const
{
	if (Index.X >= Size.X ||
		Index.Y >= Size.Y ||
		Index.Z >= Size.Z ||
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
