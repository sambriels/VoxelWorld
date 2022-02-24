#pragma once

#include "CoreMinimal.h"
#include "VWChunk.h"
#include "GameFramework/Actor.h"
#include "VWChunkWorld.generated.h"

UCLASS()
class VOXELWORLD_API AVWChunkWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	AVWChunkWorld();

	UPROPERTY(EditAnywhere, Category="ChunkWorld")
	TSubclassOf<AActor> Chunk;

	UPROPERTY(EditAnywhere, Category="ChunkWorld")
	int DrawDistance = 5;

	UPROPERTY(EditAnywhere, Category="ChunkWorld")
	int ChunkSize = 32;

protected:
	virtual void BeginPlay() override;
};