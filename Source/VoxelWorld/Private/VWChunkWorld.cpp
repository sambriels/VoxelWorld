#include "VWChunkWorld.h"
#include "Utils/Enums.h"
#include "Kismet/GameplayStatics.h"

AVWChunkWorld::AVWChunkWorld()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVWChunkWorld::BeginPlay()
{
	Super::BeginPlay();

	switch (GenerationType)
	{
	case EGenerationType::GT_3D:
		Generate3DWorld();
		break;
	case EGenerationType::GT_2D:
		Generate2DWorld();
		break;
	default:
		throw std::invalid_argument("Invalid Generation Type");
	}

	UE_LOG(LogTemp, Warning, TEXT("%d Chunks Created"), ChunkCount);
}

void AVWChunkWorld::Generate3DWorld()
{
	for (int x = -DrawDistance; x <= DrawDistance; x++)
	{
		for (int y = -DrawDistance; y <= DrawDistance; ++y)
		{
			for (int z = -DrawDistance; z <= DrawDistance; ++z)
			{
				auto transform = FTransform(
					FRotator::ZeroRotator,
					FVector(x * ChunkSize * 100, y * ChunkSize * 100, z * ChunkSize * 100),
					FVector::OneVector
				);

				const auto chunk = GetWorld()->SpawnActorDeferred<AVWChunkBase>(
					ChunkClass,
					transform,
					this
				);

				chunk->GenerationType = EGenerationType::GT_3D;
				chunk->Frequency = Frequency;
				chunk->GenerateCollision = GenerateCollision;
				// chunk->Material = Material;
				chunk->Size = ChunkSize;

				UGameplayStatics::FinishSpawningActor(chunk, transform);

				ChunkCount++;
			}
		}
	}
}

void AVWChunkWorld::Generate2DWorld()
{
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

			Chunk->GenerationType = EGenerationType::GT_2D;
			Chunk->Frequency = Frequency;
			// Chunk->Material = Material;
			Chunk->Size = ChunkSize;
			Chunk->GenerateCollision = GenerateCollision;

			UGameplayStatics::FinishSpawningActor(Chunk, Transform);
		}
	}
}
