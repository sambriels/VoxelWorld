#pragma once

#include "CoreMinimal.h"
#include "VWChunkBase.h"
#include "VWChunk.generated.h"

enum class EBlock;
enum class EDirection;
class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class AVWChunk : public AVWChunkBase
{
	GENERATED_BODY()

public:
	AVWChunk();

private:
	virtual void Setup() override;
	virtual void GenerateMesh() override;
	virtual void GenerateHeightMap() override;

	bool Check(FVector Position) const;
	void CreateFace(EDirection Direction, FVector Position);
	TArray<FVector> GetFaceVertices(EDirection Direction, FVector Position) const;
	FVector GetPositionInDirection(EDirection Direction, FVector Position) const;
	int GetBlockIndex(int X, int Y, int Z) const;

	TArray<EBlock> Blocks;

	int VertexCount = 0;

	const FVector BlockVertexData[8] = {
		FVector(100, 100, 100),
		FVector(100, 0, 100),
		FVector(100, 0, 0),
		FVector(100, 100, 0),
		FVector(0, 0, 100),
		FVector(0, 100, 100),
		FVector(0, 100, 0),
		FVector(0, 0, 0)
	};

	const int BlockTriangleData[24] = {
		0, 1, 2, 3, // Forward
		5, 0, 3, 6, // Right
		4, 5, 6, 7, // Back
		1, 4, 7, 2, // Left
		5, 4, 1, 0, // Up
		3, 2, 7, 6 // Down
	};
};
