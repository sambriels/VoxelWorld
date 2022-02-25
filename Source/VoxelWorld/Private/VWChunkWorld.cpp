#include "VWChunkWorld.h"

#include "Kismet/GameplayStatics.h"

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
			auto Transform = FTransform(
				FRotator::ZeroRotator,
				FVector(x * ChunkSize * 100, y * ChunkSize * 100, 1 * ChunkSize * 100),
				FVector::OneVector
			);
			const auto Chunk = GetWorld()->SpawnActorDeferred<AVWChunkBase>(
				ChunkClass,
				Transform,
				this
			);

			// Chunk->GenerationType = EGenerationType::GT_3D;
			Chunk->Frequency = Frequency;
			// Chunk->Material = Material;
			Chunk->Size = ChunkSize;

			UGameplayStatics::FinishSpawningActor(Chunk, Transform);
		}
	}
}
