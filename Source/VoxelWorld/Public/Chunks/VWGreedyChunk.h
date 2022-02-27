#pragma once

#include "CoreMinimal.h"
#include "VWChunkBase.h"
#include "VWGreedyChunk.generated.h"

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

protected:
	virtual void Setup() override;
	virtual void Generate2DHeightMap(FVector Position) override;
	virtual void Generate3DHeightMap(FVector Position) override;
	virtual void GenerateMesh() override;

private:
	TArray<EBlock> Blocks;

	void CreateQuad(FMask Mask, FIntVector AxisMask, FIntVector V1, FIntVector V2, FIntVector V3, FIntVector V4);
	int GetBlockIndex(int X, int Y, int Z) const;
	EBlock GetBlock(FIntVector Index) const;
	static bool CompareMask(FMask M1, FMask M2);
};
