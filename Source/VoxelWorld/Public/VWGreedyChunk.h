#pragma once

#include "CoreMinimal.h"
#include "VWChunkBase.h"
#include "VWGreedyChunk.generated.h"

enum class EBlock;
class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class VOXELWORLD_API AVWGreedyChunk : public AVWChunkBase
{
	GENERATED_BODY()

	struct FMask
	{
		EBlock Block;
		int Normal;
	};

	virtual void Setup() override;
	virtual void GenerateHeightMap() override;
	virtual void GenerateMesh() override;

	TArray<EBlock> Blocks;

	void CreateQuad(FMask Mask, FIntVector AxisMask, FIntVector V1, FIntVector V2, FIntVector V3, FIntVector V4);
	int GetBlockIndex(int X, int Y, int Z) const;
	EBlock GetBlock(FIntVector Index) const;
	bool CompareMask(FMask M1, FMask M2);
};
