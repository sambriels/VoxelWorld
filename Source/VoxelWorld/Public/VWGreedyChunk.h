#pragma once

#include "CoreMinimal.h"
#include "ChunkMeshData.h"
#include "GameFramework/Actor.h"
#include "VWGreedyChunk.generated.h"

enum class EBlock;
class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class VOXELWORLD_API AVWGreedyChunk : public AActor
{
	GENERATED_BODY()

	struct FMask
	{
		EBlock Block;
		int Normal;
	};
	
public:	
	AVWGreedyChunk();

	UPROPERTY(EditAnywhere, Category="Chunk")
	FIntVector Size = FIntVector(1, 1, 1) * 32;

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<UProceduralMeshComponent> Mesh;
	TObjectPtr<FastNoiseLite> Noise;

	FChunkMeshData MeshData;
	TArray<EBlock> Blocks;

	int VertexCount;

	void GenerateBlocks();
	void ApplyMesh();
	void GenerateMesh();

	void CreateQuad(FMask Mask, FIntVector AxisMask, FIntVector V1, FIntVector V2, FIntVector V3, FIntVector V4);
	int GetBlockIndex(int X, int Y, int Z) const;
	EBlock GetBlock(FIntVector Index) const;
	bool CompareMask(FMask M1, FMask M2);
};
