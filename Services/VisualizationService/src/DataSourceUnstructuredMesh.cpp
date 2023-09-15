
#include "stdafx.h"

#include "DataSourceUnstructuredMesh.h"
#include "EntityBase.h"
#include "EntityResultUnstructuredMesh.h"
#include "EntityResultUnstructuredMeshData.h"
#include "ClassFactory.h"

#include <vtkNew.h>
#include <vtkStructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkTetra.h>

DataSourceUnstructuredMesh::DataSourceUnstructuredMesh()
{

}

DataSourceUnstructuredMesh::~DataSourceUnstructuredMesh()
{
	FreeMemory();
}

bool DataSourceUnstructuredMesh::loadMeshData(EntityBase* meshEntity)
{
	FreeMemory();
	auto meshData = dynamic_cast<EntityResultUnstructuredMesh*>(meshEntity);
	assert(meshData != nullptr);

	ClassFactory classFactory;

	size_t numberPoints = 0;
	double* pointsX = nullptr, * pointsY = nullptr, * pointsZ = nullptr;
	meshData->GetPointCoordData(numberPoints, pointsX, pointsY, pointsZ, &classFactory);

	if (pointsX == nullptr || pointsY == nullptr || pointsZ == nullptr) return false;

	vtkPoints* points = vtkPoints::New();

	//points->SetNumberOfPoints(numberPoints);
	for (size_t index = 0; index < numberPoints; index++)
	{
		points->InsertNextPoint(pointsX[index], pointsY[index], pointsZ[index]);
	}

	delete[] pointsX; pointsX = nullptr;
	delete[] pointsY; pointsY = nullptr;
	delete[] pointsZ; pointsZ = nullptr;

	size_t numberCells = 0;
	size_t sizeCellData = 0;
	int* cellData = nullptr;
	meshData->GetCellData(numberCells, sizeCellData, cellData, &classFactory);

	if (cellData == nullptr) return false;

	vtkCellArray* cellArray = vtkCellArray::New();
	size_t index = 0;

	for (size_t cell = 0; cell < numberCells; cell++)
	{
		vtkSmartPointer<vtkTetra> tetra = vtkSmartPointer<vtkTetra>::New();
		assert(cellData[index] == 4); // Ensure that we have four nodes
		tetra->GetPointIds()->SetId(0, cellData[index + 1]);
		tetra->GetPointIds()->SetId(1, cellData[index + 2]);
		tetra->GetPointIds()->SetId(2, cellData[index + 3]);
		tetra->GetPointIds()->SetId(3, cellData[index + 4]);
		assert(cellData[index + 5] == 10); // Ensure that we have a tetrahedron type

		index += 6;
		cellArray->InsertNextCell(tetra);
	}

	vtkGrid->SetPoints(points);
	vtkGrid->SetCells(VTK_TETRA, cellArray);

	//size_t testP = vtkGrid->GetNumberOfPoints();
	//size_t testC = vtkGrid->GetNumberOfCells();

	return true;
}


bool DataSourceUnstructuredMesh::loadResultData(EntityBase* resultEntity)
{
	auto resultData = dynamic_cast<EntityResultUnstructuredMeshData*>(resultEntity);
	assert(resultData != nullptr);

	ClassFactory classFactory;

	size_t lenPointScalar = 0;
	size_t lenPointVector = 0;
	size_t lenCellScalar  = 0;
	size_t lenCellVector  = 0;

	float* pointScalar = nullptr;
	float* pointVector = nullptr;
	float* cellScalar  = nullptr;
	float* cellVector  = nullptr;

	resultData->getData(lenPointScalar, pointScalar, lenPointVector, pointVector, lenCellScalar, cellScalar, lenCellVector, cellVector, &classFactory);

	if (pointScalar == nullptr && pointVector == nullptr && cellScalar == nullptr && cellVector == nullptr) return false;

	vtkNew<vtkDoubleArray> pointScalarData, pointVectorData, cellScalarData, cellVectorData;

	pointScalarData->SetName("PointScalar");
	pointVectorData->SetName("PointVector");
	cellScalarData->SetName("CellScalar");
	cellVectorData->SetName("CellVector");

	buildScalarArray(lenPointScalar, pointScalar, pointScalarData);
	buildScalarArray(lenCellScalar, cellScalar, cellScalarData);

	buildVectorArray(lenPointVector, pointVector, pointVectorData);
	buildVectorArray(lenCellVector, cellVector, cellVectorData);

	if (pointScalar != nullptr)
	{
		vtkGrid->GetPointData()->SetScalars(pointScalarData);
		vtkGrid->GetPointData()->Update();
	}

	if (cellScalar != nullptr)
	{
		vtkGrid->GetCellData()->SetScalars(cellScalarData);
		vtkGrid->GetCellData()->Update();
	}

	if (pointVector != nullptr)
	{
		vtkGrid->GetPointData()->AddArray(pointVectorData);
	 	vtkGrid->GetPointData()->SetActiveVectors("PointVector");
		vtkGrid->GetPointData()->Update();
	}

	if (cellVector != nullptr)
	{
		vtkGrid->GetCellData()->AddArray(cellVectorData);
	 	vtkGrid->GetCellData()->SetActiveVectors("CellVector");
		vtkGrid->GetCellData()->Update();
	}

	return true;
}

void DataSourceUnstructuredMesh::buildScalarArray(size_t length, float *data, vtkNew<vtkDoubleArray> &dataArray)
{
	if (length == 0) return;
	assert(data != nullptr);

	dataArray->SetNumberOfComponents(1);
	dataArray->SetNumberOfTuples(length);

	size_t index = 0;
	for (int i = 0; i < length; i++)
	{
		dataArray->SetTuple1(i, data[index]);
		index++;
	}
}

void DataSourceUnstructuredMesh::buildVectorArray(size_t length, float* data, vtkNew<vtkDoubleArray>& dataArray)
{
	if (length == 0) return;
	assert(data != nullptr);

	dataArray->SetNumberOfComponents(3);
	dataArray->SetNumberOfTuples(length / 3);

	size_t index = 0;
	for (int i = 0; i < length / 3; i++)
	{
		dataArray->SetTuple3(i, data[index], data[index + 1], data[index + 2]);
		index += 3;
	}
}

bool DataSourceUnstructuredMesh::loadData(EntityBase *resultEntity, EntityBase *meshEntity)
{
	// First, load the mesh data
	EntityResultUnstructuredMesh *meshData = dynamic_cast<EntityResultUnstructuredMesh*>(meshEntity);

	if (meshData == nullptr)
	{
		assert(0); // Unable to read the mesh data
		return false;
	}

	// Now build the vtk grid from the mesh data
	bool success = loadMeshData(meshEntity);

	delete meshData;
	meshData = nullptr;

	if (!success) return false;

	// Now, load the result data from the data base
	EntityResultUnstructuredMeshData *resultData = dynamic_cast<EntityResultUnstructuredMeshData*>(resultEntity);
	if (resultData == nullptr)
	{
		assert(0); // Wrong data type
		return false;
	}

	success = loadResultData(resultEntity);

	delete resultData;
	resultData = nullptr;

	return success;
}

void DataSourceUnstructuredMesh::FreeMemory(void)
{
	//if (vtkGrid != nullptr)
	//{
	//	vtkGrid->Delete();
	//	vtkGrid = nullptr;
	//}
}

double DataSourceUnstructuredMesh::GetXMinCoordinate()
{
	return vtkGrid->GetBounds()[0];
}

double DataSourceUnstructuredMesh::GetYMinCoordinate()
{
	return vtkGrid->GetBounds()[2];
}

double DataSourceUnstructuredMesh::GetZMinCoordinate()
{
	return vtkGrid->GetBounds()[4];
}

double DataSourceUnstructuredMesh::GetXMaxCoordinate()
{
	return vtkGrid->GetBounds()[1];
}

double DataSourceUnstructuredMesh::GetYMaxCoordinate()
{
	return vtkGrid->GetBounds()[3];
}

double DataSourceUnstructuredMesh::GetZMaxCoordinate()
{
	return vtkGrid->GetBounds()[5];
}
