#pragma once

#include "CoreMinimal.h"
#include "Chunks/VWChunk.h"
#include "GameFramework/Actor.h"
#include "VWChunkWorld.generated.h"

UCLASS()
class VOXELWORLD_API AVWChunkWorld : public AActor
{
	GENERATED_BODY()

public:
	AVWChunkWorld();

	UPROPERTY(EditAnywhere, Category="ChunkWorld|World")
	TSubclassOf<AVWChunkBase> ChunkClass;

	UPROPERTY(EditAnywhere, Category="ChunkWorld|World")
	int DrawDistance = 5;

	UPROPERTY(EditAnywhere, Category="ChunkWorld|Chunk")
	int ChunkSize = 32;

	UPROPERTY(EditInstanceOnly, Category="ChunkWorld|HeightMap")
	EGenerationType GenerationType;

	UPROPERTY(EditDefaultsOnly, Category="ChunkWorld|HeightMap")
	float Frequency = 0.03f;

	UPROPERTY(EditDefaultsOnly, Category="ChunkWorld|World")
	bool GenerateCollision = false;

protected:
	virtual void BeginPlay() override;

private:
	int ChunkCount;

	void Generate3DWorld();
	void Generate2DWorld();
};
