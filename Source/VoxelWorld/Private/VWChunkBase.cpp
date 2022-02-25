#include "VWChunkBase.h"

#include "FastNoiseLite.h"
#include "ProceduralMeshComponent.h"

AVWChunkBase::AVWChunkBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Mesh->SetCastShadow(false);
	
	Noise = new FastNoiseLite();

	SetRootComponent(Mesh);
}

void AVWChunkBase::BeginPlay()
{
	Super::BeginPlay();

	Noise->SetFrequency(Frequency);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

	Setup();
	GenerateHeightMap();
	GenerateMesh();

	UE_LOG(LogTemp, Warning, TEXT("Vertex Count : %d"), VertexCount);

	ApplyMesh();
}

void AVWChunkBase::ApplyMesh() const
{
	Mesh->CreateMeshSection(
		0,
		MeshData.Vertices,
		MeshData.Triangles,
		MeshData.Normals,
		MeshData.UV0,
		// MeshData.Colors,
		TArray<FColor>{},
		TArray<FProcMeshTangent>(),
		false
	);
}

