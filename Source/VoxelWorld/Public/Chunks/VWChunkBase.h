// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utils/ChunkMeshData.h"
#include "Utils/Enums.h"
#include "GameFramework/Actor.h"
#include "VWChunkBase.generated.h"

class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS(Abstract)
class VOXELWORLD_API AVWChunkBase : public AActor
{
	GENERATED_BODY()

public:
	AVWChunkBase();

	UPROPERTY(EditDefaultsOnly, Category="Chunk")
	int Size = 64;

	float Frequency = 0.03f;
	EGenerationType GenerationType;
	bool GenerateCollision = false;

protected:
	virtual void BeginPlay() override;

	virtual void Setup() PURE_VIRTUAL(AVWChunkBase::Setup);
	virtual void Generate2DHeightMap(const FVector Position) PURE_VIRTUAL(AChunkBase::Generate2DHeightMap);
	virtual void Generate3DHeightMap(const FVector Position) PURE_VIRTUAL(AChunkBase::Generate3DHeightMap);
	virtual void GenerateMesh() PURE_VIRTUAL(AVWChunkBase::GenerateMesh);

	TObjectPtr<UProceduralMeshComponent> Mesh;
	TObjectPtr<FastNoiseLite> Noise;

	FChunkMeshData MeshData;
	TArray<EBlock> Blocks;
	int VertexCount = 0;
private:
	virtual void ApplyMesh() const;
	virtual void GenerateHeightMap();
};
