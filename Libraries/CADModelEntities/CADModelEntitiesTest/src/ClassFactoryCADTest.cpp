#include <gtest/gtest.h>
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"
#include "ClassFactoryHandler.h"

TEST(CADModelEntities, CADEntityFound) 
{
	ClassFactoryHandler* handler = new ClassFactoryCAD();
	EntityBase* entity = handler->CreateEntity("EntityGeometry");
	EXPECT_NE(entity, nullptr);
}

TEST(CADModelEntities, ModelEntityNotFound)
{
	ClassFactoryHandler* handler = new ClassFactoryCAD();
	EntityBase* entity = handler->CreateEntity("EntityAnnotation");
	EXPECT_EQ(entity, nullptr);
}

TEST(CADModelEntities, ModelEntityFound)
{
	ClassFactoryHandler* cadHandler = new ClassFactoryCAD();
	ClassFactoryHandler* modelHandler = new ClassFactory();
	cadHandler->SetNextHandler(modelHandler);
	EntityBase* entity = cadHandler->CreateEntity("EntityAnnotation");
	EXPECT_NE(entity, nullptr);
}