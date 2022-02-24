#include "VWChunkWorld.h"

AVWChunkWorld::AVWChunkWorld()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AVWChunkWorld::BeginPlay()
{
	Super::BeginPlay();

	for (int x = -DrawDistance; x <= DrawDistance; ++x)
	{
		for (int y = -DrawDistance; y <= DrawDistance; ++y)
		{
			GetWorld()->SpawnActor<AActor>(Chunk, FVector(x * ChunkSize * 100, y * ChunkSize * 100, 0), FRotator::ZeroRotator);
		}	
	}
}	

