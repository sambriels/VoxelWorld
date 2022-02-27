#include "Chunks/VWChunkBase.h"

#include "Utils/FastNoiseLite.h"
#include "Utils/Enums.h"
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

void AVWChunkBase::GenerateHeightMap()
{
	switch (GenerationType)
	{
	case EGenerationType::GT_3D:
		Generate3DHeightMap(GetActorLocation() / 100);
		break;
	case EGenerationType::GT_2D:
		Generate2DHeightMap(GetActorLocation() / 100);
		break;
	default:
		throw std::invalid_argument("Invalid Generation Type");
	}
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
		GenerateCollision
	);
}
