// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkMeshData.h"
#include "GameFramework/Actor.h"
#include "VWChunkBase.generated.h"

enum class EBlock;
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

	UPROPERTY(EditDefaultsOnly, Category="HeightMap")
	float Frequency = 0.03f;

protected:
	virtual void BeginPlay() override;
	virtual void Setup(){};
	virtual void GenerateHeightMap(){}
	virtual void GenerateMesh(){};

	TObjectPtr<UProceduralMeshComponent> Mesh;
	TObjectPtr<FastNoiseLite> Noise;

	FChunkMeshData MeshData;
	TArray<EBlock> Blocks;

	int VertexCount;
private:
	virtual void ApplyMesh() const;
};
