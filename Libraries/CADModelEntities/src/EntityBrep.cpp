// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityBrep.h"
#include "DataBase.h"
#include "Types.h"

#include "Bnd_Box.hxx"
#include "BRepBndLib.hxx"
#include "BRepTools_ShapeSet.hxx"
#include "BRepTools.hxx"

EntityBrep::EntityBrep(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner)
{
	transformMatrix = gp_Trsf();
}

EntityBrep::~EntityBrep()
{

}

bool EntityBrep::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	// If the shape is empty, an exception may be thrown here.

	Bnd_Box Box;

	BRepBndLib::Add(brep, Box);
	Box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

	return true;
}

void EntityBrep::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now check whether the geometry is modified and we need to create a new entry

	std::stringstream brepStream;
	BRepTools::Write(brep, brepStream);		
		
	storage.append(bsoncxx::builder::basic::kvp("Brep", brepStream.str()));
	
	auto matrix = bsoncxx::builder::basic::array();

	for (int j = 1; j <= 3; j++)
	{
		for (int i = 1; i <= 4; i++)
		{
			matrix.append(transformMatrix.Value(j, i));
		}
	}

	storage.append(bsoncxx::builder::basic::kvp("Transform", matrix));
}

void EntityBrep::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the Brep and Facet objects
	std::string brepData = doc_view["Brep"].get_utf8().value.data();

	std::istringstream brepStream(brepData);

	BRep_Builder brbp;
	BRepTools::Read(brep, brepStream, brbp);

	transformMatrix = gp_Trsf();

	try
	{
		auto matrix = doc_view["Transform"].get_array().value;
		assert(std::distance(matrix.begin(), matrix.end()) == 12);

		auto pm = matrix.begin();

		double m[12] = { 0.0 };

		for (unsigned long index = 0; index < 12; index++)
		{
			m[index] = pm->get_double();
			pm++;
		}

		transformMatrix.SetValues(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11]);
	}
	catch (std::exception)
	{
	}

	resetModified();
}

gp_Trsf EntityBrep::getTransform(void)
{
	return transformMatrix;
}

void EntityBrep::setTransform(gp_Trsf transform)
{
	transformMatrix = transform;
}
