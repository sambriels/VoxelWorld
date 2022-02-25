#include "VWMarchingChunk.h"

#include "FastNoiseLite.h"

void AVWMarchingChunk::Setup()
{
	Voxels.SetNum((Size + 1) * (Size + 1) * (Size + 1));
}

void AVWMarchingChunk::GenerateMesh()
{
	Super::GenerateMesh();
	if (SurfaceLevel > 0.0f)
	{
		TriangleOrder[0] = 0;
		TriangleOrder[1] = 1;
		TriangleOrder[2] = 2;
	}
	else
	{
		TriangleOrder[0] = 2;
		TriangleOrder[1] = 1;
		TriangleOrder[2] = 0;
	}

	float Cube[8];

	for (int X = 0; X < Size; ++X)
	{
		for (int Y = 0; Y < Size; ++Y)
		{
			for (int Z = 0; Z < Size; ++Z)
			{
				for (int i = 0; i < 8; ++i)
				{
					Cube[i] = Voxels[GetVoxelIndex(X + VertexOffset[i][0], Y + VertexOffset[i][1], Z + VertexOffset[i][2])];
				}
				March(X, Y, Z, Cube);
			}
		}	
	}
}

void AVWMarchingChunk::GenerateHeightMap()
{
	Super::GenerateHeightMap();
	const auto Position = GetActorLocation() / 100;

	for (int X = 0; X <= Size; ++X)
	{
		for (int Y = 0; Y <= Size; ++Y)
		{
			for (int Z = 0; Z <= Size; ++Z)
			{
				Voxels[GetVoxelIndex(X, Y, Z)] = Noise->GetNoise(X + Position.X, Y + Position.Y, Z + Position.Z);
			}
		}	
	}
}

void AVWMarchingChunk::March(int X, int Y, int Z, const float Cube[8])
{
	int VertexMask = 0;
	FVector EdgeVertex[12];

	for (int i = 0; i < 8; ++i)
	{
		if (Cube[i] <= SurfaceLevel)
		{
			VertexMask |= 1 << i;
		}
	}

	const int EdgeMask = CubeEdgeFlags[VertexMask];

	if (EdgeMask == 0)
	{
		return;
	}

	for (int i = 0; i < 12; ++i)
	{
		if ((EdgeMask & 1 << i) != 0)
		{
			const float Offset = bUseInterpolation ? GetInterpolationOffset(Cube[EdgeConnection[i][0]], Cube[EdgeConnection[i][1]]) : 0.5f;
			EdgeVertex[i].X = X + (VertexOffset[EdgeConnection[i][0]][0] + Offset * EdgeDirection[i][0]);
			EdgeVertex[i].Y = Y + (VertexOffset[EdgeConnection[i][0]][1] + Offset * EdgeDirection[i][1]);
			EdgeVertex[i].Z = Z + (VertexOffset[EdgeConnection[i][0]][2] + Offset * EdgeDirection[i][2]);
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		if (TriangleConnectionTable[VertexMask][3 * i] < 0)
		{
			break;
		}

		auto V1 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i]] * 100;
		auto V2 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i + 1]] * 100;
		auto V3 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i + 2]] * 100;

		auto Normal = FVector::CrossProduct(V2 - V1, V3 - V1);
		Normal.Normalize();

		MeshData.Vertices.Add(V1);
		MeshData.Vertices.Add(V2);
		MeshData.Vertices.Add(V3);

		MeshData.Triangles.Add(VertexCount + TriangleOrder[0]);
		MeshData.Triangles.Add(VertexCount + TriangleOrder[1]);
		MeshData.Triangles.Add(VertexCount + TriangleOrder[2]);

		MeshData.Normals.Add(Normal);
		MeshData.Normals.Add(Normal);
		MeshData.Normals.Add(Normal);

		VertexCount += 3;
	}
}

int AVWMarchingChunk::GetVoxelIndex(int X, int Y, int Z) const
{
	return Z * (Size + 1) * (Size + 1) + Y * (Size + 1) + X;
}

float AVWMarchingChunk::GetInterpolationOffset(float V1, float V2) const
{
	const float Delta = V2 - V1;
	return Delta == 0.0f ? SurfaceLevel : (SurfaceLevel - V1) / Delta;
}
