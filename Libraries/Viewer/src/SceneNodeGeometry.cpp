#include "stdafx.h"

#include "SceneNodeGeometry.h"
#include "SceneNodeMaterial.h"
#include "Geometry.h"

#include "DataBase.h"
#include "Model.h"
#include "TextureMapManager.h"
#include "ViewerSettings.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Depth>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/LightSource>

#include <osg/Image>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/TextureCubeMap>
#include <osgUtil/HighlightMapGenerator>
#include <osg/TexGen>
#include <osg/TexEnvCombine>
#include <osg/MatrixTransform>

SceneNodeGeometry::SceneNodeGeometry() :
	triangles(nullptr), 
	edges(nullptr), 
	edgesHighlighted(nullptr), 
	faceEdgesHighlightNode(nullptr),
	surfaceColorRGB{ 0.0, 0.0, 0.0 },
	edgeColorRGB{ 0.0, 0.0, 0.0 },
	backFaceCulling(false),
	offsetFactor(1.0),
	entityID(0),
	entityVersion(0),
	needsInitialization(false),
	model(nullptr),
	materialType("Rough"),
	textureType("None"),
	reflective(false),
	showWhenSelected(false),
	textureAttribute(nullptr),
	textureAttributeGen(nullptr),
	textureAttributeEnv(nullptr),
	edgeTranspacency(0.35)
{
}

SceneNodeGeometry::~SceneNodeGeometry()
{
	// Remove the OSG objects 
	// This geometry node will always have a parent group or switch node

	deleteShapeNode();
}

void SceneNodeGeometry::deleteShapeNode(void)
{
	// loop through all parent nodes
	if (getShapeNode() != nullptr)
	{
		unsigned int numParents = getShapeNode()->getNumParents();

		for (unsigned int i = 0; i < numParents; i++)
		{
			osg::Group *parent = getShapeNode()->getParent(i);

			// Remove the child node from the parent (the node itself will then be deleted by reference counting automatically)
			parent->removeChild(getShapeNode());
		}

		// Now the shape node is invalid, since it might have been deleted by removing it from its parent
		shapeNode				= nullptr;
		triangles				= nullptr;
		edges					= nullptr;
		edgesHighlighted		= nullptr;
		faceEdgesHighlightNode	= nullptr;

		faceEdgesHighlight.clear();
		triangleToFaceId.clear();
	}
}

void SceneNodeGeometry::setTransformation(std::vector<double> &transformation)
{
	transformationMatrix.set(transformation.data());
	transformationMatrix.transpose(transformationMatrix);
}

osg::Matrix SceneNodeGeometry::getParentTransformation(void) 
{ 
	// Check whether we have a SceneNodeGeometry as parent. If so, we need to combine the transforms
	SceneNodeGeometry *geometryParent = dynamic_cast<SceneNodeGeometry *>(getParent());

	if (geometryParent != nullptr)
	{
		osg::Matrix parentMatrix = geometryParent->getTransformation();

		return parentMatrix;
	}

	return osg::Matrix(); 
}

osg::Matrix SceneNodeGeometry::getTransformation(void) 
{ 
	// Check whether we have a SceneNodeGeometry as parent. If so, we need to combine the transforms
	SceneNodeGeometry *geometryParent = dynamic_cast<SceneNodeGeometry *>(getParent());

	if (geometryParent != nullptr)
	{
		osg::Matrix parentMatrix = geometryParent->getTransformation();

		return  transformationMatrix * parentMatrix;
	}

	return transformationMatrix; 
}

void SceneNodeGeometry::setTransparent(bool t)
{
	if (getShapeNode() == nullptr) return;

	if (isTransparent() == t) return;  // No change necessary

	SceneNodeBase::setTransparent(t);

	bool visible = isVisible();
	bool transparent = isTransparent();
	bool wireframe = isWireframe();

	if (showWhenSelected && isSelected())
	{
		visible = true;
		transparent = false;
		wireframe = false;
	}

	updateTransparentState(visible, transparent, wireframe);
	updateWireframeState(false, wireframe, transparent);
}

void SceneNodeGeometry::updateTransparentState(bool visible, bool transparent, bool wireframe)
{
	if (transparent)
	{
		// Set node to transparent state
		osg::StateSet *stateset = getTriangles()->getOrCreateStateSet();
		stateset->clear();

		osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
		blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		stateset->setAttributeAndModes(blendFunc);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(false);
		stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
		cull->setMode(osg::CullFace::BACK);
		stateset->setAttributeAndModes(cull, osg::StateAttribute::ON);

		osg::StateSet *statesetEdge = getEdges()->getOrCreateStateSet();

		statesetEdge->setAttributeAndModes(blendFunc);
		statesetEdge->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::Depth> depthEdge = new osg::Depth;
		depthEdge->setWriteMask(false);
		statesetEdge->setAttributeAndModes(depthEdge, osg::StateAttribute::ON);
		statesetEdge->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		// Turn on triangle display (is shape is not hidden)
		if (visible)
		{
			if (!wireframe)
			{
				getShapeNode()->setChildValue(getTriangles(), true);
				getShapeNode()->setChildValue(getEdges(), false);
			}
			else
			{
				getShapeNode()->setChildValue(getTriangles(), false);
				getShapeNode()->setChildValue(getEdges(), true);
			}
		}


		getShapeNode()->setNodeMask(getShapeNode()->getNodeMask() & ~1);  // Reset last bit of node mask
	}
	else
	{
		// Set node to opaque state
		osg::StateSet *stateset = getTriangles()->getOrCreateStateSet();
		stateset->clear();

		osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
		cull->setMode(osg::CullFace::BACK);
		stateset->setAttributeAndModes(cull, osg::StateAttribute::ON);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		osg::StateSet *statesetEdge = getEdges()->getOrCreateStateSet();

		osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
		statesetEdge->setAttributeAndModes(blendFunc, osg::StateAttribute::OFF);

		if (backFaceCulling)
		{
			statesetEdge->setRenderingHint(osg::StateSet::OPAQUE_BIN);

			osg::ref_ptr<osg::Depth> depth = new osg::Depth;
			depth->setWriteMask(true);
			statesetEdge->setAttributeAndModes(depth, osg::StateAttribute::ON);
			statesetEdge->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		}
		else
		{ 
			statesetEdge->setRenderBinDetails(-2, "RenderBin");
		}

		// Turn on triangle display (is shape is not hidden and we are not in wireframe state)
		getShapeNode()->setChildValue(getTriangles(), visible && !wireframe);

		// Turn on edge display (if visible)
		if (visible)
		{
			getShapeNode()->setChildValue(getEdges(), true);
		}

		getShapeNode()->setNodeMask(getShapeNode()->getNodeMask() | 1);  // Set last bit of node mask
	}
}

void SceneNodeGeometry::setWireframe(bool w)
{
	if (isWireframe() == w) return;  // No change necessary

	SceneNodeBase::setWireframe(w);

	if (getShapeNode() != nullptr)
	{
		updateWireframeState(isVisible(), isWireframe(), isTransparent());
	}
}

void SceneNodeGeometry::updateWireframeState(bool visible, bool wireframe, bool transparent)
{
	double lineWidth = 0.0;	
	double lineWidthHighlight = 0.0;	

	if (isWireframe())
	{
		// Set node to wireframe state
		if (visible)
		{
			getShapeNode()->setChildValue(getTriangles(), false);
			getShapeNode()->setChildValue(getEdges(), true);
		}

		if (isTransparent())
		{
			lineWidth = 1.0;
		}
		else
		{
			lineWidth = 2.5;
		}

		lineWidthHighlight = 2.5;
	}
	else
	{
		// Set node to solid state
		if (visible)
		{
			getShapeNode()->setChildValue(getTriangles(), true);
			getShapeNode()->setChildValue(getEdges(), !isTransparent());
		}

		lineWidth = 1.0;
		lineWidthHighlight = 1.0;
	}

	// Find the line width attribute of the edges and set the width
	osg::StateSet *ss = getEdges()->getOrCreateStateSet();

	// find the line with attribute of the state set
	osg::LineWidth *lineWidthAttribute = dynamic_cast<osg::LineWidth*>(ss->getAttribute(osg::StateAttribute::Type::LINEWIDTH));

	if (lineWidthAttribute != nullptr)
	{
		lineWidthAttribute->setWidth(lineWidth);
	}

	// Find the line width attribute of the edges and set the width
	osg::StateSet *ssHighlight = getEdgesHighlighted()->getOrCreateStateSet();

	// find the line with attribute of the state set
	osg::LineWidth *lineWidthAttributeHighlight = dynamic_cast<osg::LineWidth*>(ssHighlight->getAttribute(osg::StateAttribute::Type::LINEWIDTH));

	if (lineWidthAttributeHighlight != nullptr)
	{
		lineWidthAttributeHighlight->setWidth(lineWidthHighlight);
	}
}

void SceneNodeGeometry::setVisible(bool v)
{
	if (isVisible() == v) return;  // No change necessary

	if (v && needsInitialization)
	{
		// This item has not yet been loaded from the storage 
		initializeFromDataStorage();
	}

	SceneNodeBase::setVisible(v);

	setVisibleState(isVisible(), isTransparent(), isWireframe());
}

void SceneNodeGeometry::setVisibleState(bool visible, bool transparent, bool wireframe)
{
	if (getShapeNode() != nullptr)
	{
		if (visible)
		{
			// Set node to visible state

			if (!transparent)
			{
				getShapeNode()->setChildValue(getEdges(), true);
			}

			if (!wireframe)
			{
				getShapeNode()->setChildValue(getTriangles(), true);
			}
		}
		else
		{
			// Set node to hidden state
			getShapeNode()->setChildValue(getEdges(), false);
			getShapeNode()->setChildValue(getTriangles(), false);
		}
	}
}


void SceneNodeGeometry::setHighlighted(bool h)
{
	if (getShapeNode() == nullptr) return;

	//if (!isVisible())
	//{
	//	h = false;
	//}

	if (isHighlighted() == h) return;  // No change necessary

	getShapeNode()->setChildValue(getEdgesHighlighted(), h);
	getFaceEdgesHighlight()->setAllChildrenOff();

	SceneNodeBase::setHighlighted(h);
}

void SceneNodeGeometry::setEdgeHighlight(unsigned long long faceId, bool h, double thickness)
{
//	getFaceEdgesHighlight()->setAllChildrenOff();
	getFaceEdgesHighlight()->setChildValue(faceEdgesHighlight[faceId], h);
	if (h)
	{
		osg::StateSet *ss = faceEdgesHighlight[faceId]->getOrCreateStateSet();

		osg::LineWidth *lineWidthAttribute = dynamic_cast<osg::LineWidth*>(ss->getAttribute(osg::StateAttribute::Type::LINEWIDTH));

		if (lineWidthAttribute != nullptr)
		{
			lineWidthAttribute->setWidth(thickness);
		}
	}
}

void SceneNodeGeometry::setSelected(bool selected)
{
	SceneNodeBase::setSelected(selected);

	// Handle visibility state
	if (showWhenSelected)
	{
		if (selected)
		{
			setVisibleState(true, false, false);
			updateTransparentState(true, false, false);
		}
		else
		{
			setVisibleState(isVisible(), isTransparent(), isWireframe());
			updateTransparentState(isVisible(), isTransparent(), isWireframe());
		}
	}
}

void SceneNodeGeometry::applyParentTransform(void)
{
	// Check whether we have a SceneNodeGeometry as parent. If so, we need to combine the transforms
	SceneNodeGeometry *geometryParent = dynamic_cast<SceneNodeGeometry *>(getParent());

	if (geometryParent != nullptr)
	{
		osg::Matrix parentMatrix = geometryParent->getTransformation();

		applyTransform(parentMatrix);
	}
}

void SceneNodeGeometry::applyTransform(osg::Matrix matrix)
{
	osg::MatrixTransform *triangles = dynamic_cast<osg::MatrixTransform *>(getTriangles());
	osg::MatrixTransform *edges = dynamic_cast<osg::MatrixTransform *>(getEdges());
	osg::MatrixTransform *edgesHighlighted = dynamic_cast<osg::MatrixTransform *>(getEdgesHighlighted());
	osg::MatrixTransform *faceEdgesHighlightNode = dynamic_cast<osg::MatrixTransform *>(getFaceEdgesHighlight());

	if (triangles != nullptr             ) triangles->setMatrix(matrix);
	if (edges != nullptr                 ) edges->setMatrix(matrix);
	if (edgesHighlighted != nullptr      ) edgesHighlighted->setMatrix(matrix);
	if (faceEdgesHighlightNode != nullptr) faceEdgesHighlightNode->setMatrix(matrix);
}

void SceneNodeGeometry::initializeFromFacetData(std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges)
{
	// Add a switch (group) node for the shape
	if (shapeNode == nullptr)
	{
		// Create an new shapeNode
		shapeNode = new osg::Switch;

		// Now add the current nodes osg node to the parent's osg node
		getParent()->getShapeNode()->addChild(getShapeNode());

		getModel()->addSceneNode(this);
	}
	else
	{
		// Delete the children of the shape node
		if (getTriangles() != nullptr) shapeNode->removeChild(getTriangles());
		if (getEdges() != nullptr) shapeNode->removeChild(getEdges());
		if (getEdgesHighlighted() != nullptr) shapeNode->removeChild(getEdgesHighlighted());
		if (getFaceEdgesHighlight() != nullptr) shapeNode->removeChild(getFaceEdgesHighlight());
	}

	// Store the triangle / face information

	for (auto t : triangles)
	{
		triangleToFaceId.push_back(t.getFaceId());
	}

	// Create the triangle node
	osg::Node *triangleNode = createOSGNodeFromTriangles(surfaceColorRGB, materialType, textureType, reflective, backFaceCulling, offsetFactor, nodes, triangles);

	// Create the edges node
	osg::Node *edgeNode(nullptr), *edgeHighlightedNode(nullptr);
	osg::Switch *faceEdgesHighlightNode(nullptr);

	createOSGNodeFromEdges(edgeColorRGB, edges, edgeNode, edgeHighlightedNode, faceEdgesHighlightNode);

	// Create a new entry in the shape map for this object

	setTriangles(triangleNode);
	setEdges(edgeNode);
	setEdgesHighlighted(edgeHighlightedNode);
	setFaceEdgesHighlighted(faceEdgesHighlightNode);

	// Add the triangle and edge nodes to the group node and add the group node to the root
	shapeNode->addChild(triangleNode);
	shapeNode->addChild(edgeNode);
	shapeNode->addChild(edgeHighlightedNode);
	shapeNode->addChild(faceEdgesHighlightNode);

	// Apply the parent transformation (if any)
	applyParentTransform();

	// Ensure that the triangles are drawn opaquely
	setTransparent(false);

	// Turn off highlighting
	shapeNode->setChildValue(edgeHighlightedNode, false);
	((osg::Switch *) faceEdgesHighlightNode)->setAllChildrenOff();

	if (!isVisible())
	{
		// Set node to hidden state
		getShapeNode()->setChildValue(getEdges(), false);
		getShapeNode()->setChildValue(getTriangles(), false);
	}

	updateWireframeState(isVisible(), isWireframe(), isTransparent());

	// Initialization is completed
	needsInitialization = false;
}

//void SceneNodeGeometry::assignMaterial(const std::string& materialType) {
//	this->materialType = materialType;
//}

osg::Node * SceneNodeGeometry::createOSGNodeFromTriangles(double colorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, bool backFaceCulling, double offsetFactor, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles)
{
	osg::ref_ptr<osg::Material> material = new osg::Material;

	SceneNodeMaterial sceneNodeMaterial;
	auto materialSet = sceneNodeMaterial.setMaterial(material, materialType, colorRGB[0], colorRGB[1], colorRGB[2], transparency);

	bool applyTexture = (textureType != "None");

	// Allocate and dimensions arrays for nodes and normals (each triangle has three nodes - the nodes are not shared between adjacent triangles)
	unsigned long long nTriangles = triangles.size();
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nTriangles * 3);
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(nTriangles * 3);
	osg::ref_ptr<osg::Vec2Array> textures = new osg::Vec2Array(nTriangles * 3);

	// Now store the triangle vertices in the nodes and normals array

	unsigned long long nVertex = 0;
	unsigned long long nNormal = 0;
	unsigned long long nTexture = 0;

	for (auto triangle : triangles)
	{
		unsigned long long n1 = triangle.getNode(0);
		unsigned long long n2 = triangle.getNode(1);
		unsigned long long n3 = triangle.getNode(2);

		vertices->at(nVertex).set(nodes[n1].getCoord(0), nodes[n1].getCoord(1), nodes[n1].getCoord(2));
		vertices->at(nVertex + 1).set(nodes[n2].getCoord(0), nodes[n2].getCoord(1), nodes[n2].getCoord(2));
		vertices->at(nVertex + 2).set(nodes[n3].getCoord(0), nodes[n3].getCoord(1), nodes[n3].getCoord(2));

		nVertex += 3;

		normals->at(nNormal).set(nodes[n1].getNormal(0), nodes[n1].getNormal(1), nodes[n1].getNormal(2));
		normals->at(nNormal + 1).set(nodes[n2].getNormal(0), nodes[n2].getNormal(1), nodes[n2].getNormal(2));
		normals->at(nNormal + 2).set(nodes[n3].getNormal(0), nodes[n3].getNormal(1), nodes[n3].getNormal(2));

		nNormal += 3;

		textures->at(nTexture).set(nodes[n1].getUVpar(0), nodes[n1].getUVpar(1));
		textures->at(nTexture + 1).set(nodes[n2].getUVpar(0), nodes[n2].getUVpar(1));
		textures->at(nTexture + 2).set(nodes[n3].getUVpar(0), nodes[n3].getUVpar(1));

		nTexture += 3;
	}

	// Store the color in a color array (the color will be shared among all nodes, so only one entry is needed)
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(colorRGB[0], colorRGB[1], colorRGB[2], transparency));

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	if (!backFaceCulling)
	{
		newGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		newGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
	}

	osg::LightModel *p = new osg::LightModel;
	p->setTwoSided(true);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(p);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	setOffset(offsetFactor);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(2.0f * offsetFactor, 2.0f * offsetFactor));

	newGeometry->setVertexArray(vertices.get());
	newGeometry->setTexCoordArray(0, textures.get());

	newGeometry->setNormalArray(normals.get());
	newGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	newGeometry->setColorArray(colors.get());
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, nTriangles * 3));

	newGeometry->getOrCreateStateSet()->setAttribute(material.get());

	// Now create the geometry node and assign the drawable
	osg::Geode *triangleNode = new osg::Geode;

	if (applyTexture) 
	{
		assignTexture(newGeometry, textureType, reflective, colorRGB[0], colorRGB[1], colorRGB[2]);
	}

	delete materialSet;
	materialSet = nullptr;

	triangleNode->addDrawable(newGeometry);

	osg::MatrixTransform *transformNode = new osg::MatrixTransform;
	transformNode->addChild(triangleNode);

	return transformNode;
}

void SceneNodeGeometry::createOSGNodeFromEdges(double colorRGB[3], std::list<Geometry::Edge> &edges, osg::Node *&edgesNode, osg::Node *&edgesHighlightedNode, osg::Switch *&faceEdgesHighlightNode)
{
	// Allocate and dimensions the array for nodes (each edge has two nodes - the nodes are not shared between adjacent edges)

	// Count the number of simple edge segments
	unsigned long long nEdges = 0;

	std::map<unsigned long long, std::list<Geometry::Edge *>> faceIdToEdgesList;

	for (auto &edge : edges)
	{
		nEdges += edge.getNpoints() - 1;
		faceIdToEdgesList[edge.getFaceId()].push_back(&edge);
	}

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nEdges * 2);

	// Now store the edge vertices in the array
	unsigned long long nVertex = 0;
	for (auto edge : edges)
	{
		for (int i = 0; i < edge.getNpoints() - 1; i++)
		{
			Geometry::Point p1 = edge.getPoint(i);
			Geometry::Point p2 = edge.getPoint(i + 1);

			vertices->at(nVertex).set(p1.getX(), p1.getY(), p1.getZ());
			vertices->at(nVertex + 1).set(p2.getX(), p2.getY(), p2.getZ());

			nVertex += 2;
		}
	}

	// Now build the osg nodes for the edge and the selected edge
	edgesNode = buildEdgesOSGNode(nEdges, vertices.get(), colorRGB[0], colorRGB[1], colorRGB[2], edgeTranspacency, true);
	edgesHighlightedNode = buildEdgesOSGNode(nEdges, vertices.get(), 1.0, 0.0, 0.0, 1.0, false);

	if (!backFaceCulling)
	{
		osg::StateSet *ss = edgesNode->getOrCreateStateSet();
		ss->setRenderBinDetails(-2, "RenderBin");
	}
	
	// In the next step, we build a edge highlight node for each face
	faceEdgesHighlightNode = new osg::Switch;

	for (auto faceEdges : faceIdToEdgesList)
	{
		unsigned long long nEdges = 0;

		for (auto edge : faceEdges.second)
		{
			nEdges += edge->getNpoints() - 1;
		}

		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nEdges * 2);

		// Now store the edge vertices in the array
		unsigned long long nVertex = 0;
		for (auto edge : faceEdges.second)
		{
			for (int i = 0; i < edge->getNpoints() - 1; i++)
			{
				Geometry::Point p1 = edge->getPoint(i);
				Geometry::Point p2 = edge->getPoint(i + 1);

				vertices->at(nVertex).set(p1.getX(), p1.getY(), p1.getZ());
				vertices->at(nVertex + 1).set(p2.getX(), p2.getY(), p2.getZ());

				nVertex += 2;
			}
		}

		osg::Node *faceEdgesNode = buildEdgesOSGNode(nEdges, vertices.get(), 1.0, 0.0, 0.0, 1.0, false);

		faceEdgesHighlight[faceEdges.first] = faceEdgesNode;
		faceEdgesHighlightNode->addChild(faceEdgesNode);
	}
}

osg::Node *SceneNodeGeometry::buildEdgesOSGNode(unsigned long long nEdges, osg::Vec3Array *vertices, double r, double g, double b, double transp, bool depthTest)
{
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(r, g, b, transp));

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	newGeometry->setVertexArray(vertices);

	newGeometry->setColorArray(colors.get());
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, nEdges * 2));

//	if (depthTest) {
//		osg::ref_ptr<osg::Material> matEdges = new osg::Material;
//		this->setMaterialProperties(matEdges, r, g, b);
//		newGeometry->getOrCreateStateSet()->setAttribute(matEdges.get());
//	}

	// Now create the geometry node and assign the drawable
	osg::Geode *edgesNode = new osg::Geode;
	edgesNode->addDrawable(newGeometry);

	// Set the display attributes for the edges geometry

	osg::MatrixTransform *transformNode = new osg::MatrixTransform;
	transformNode->addChild(edgesNode);

	osg::StateSet *ss = transformNode->getOrCreateStateSet();

	//osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	//blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//ss->setAttributeAndModes(blendFunc);

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);

	if (!depthTest)
	{
		ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(false);
		ss->setAttributeAndModes(depth, osg::StateAttribute::ON);
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	}

	return transformNode;
}

void SceneNodeGeometry::updateObjectColor(double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective)
{
	// First, update the color of the triangles
	osg::Transform *triangleTransform = dynamic_cast<osg::Transform *>(getTriangles());
	assert(triangleTransform != nullptr);

	assert(triangleTransform->getNumChildren() == 1);
	osg::Geode *triangleNode = dynamic_cast<osg::Geode *>(triangleTransform->getChild(0));
	assert(triangleNode != nullptr);

	if (triangleNode != nullptr)
	{
		for (int i = 0; i < triangleNode->getNumDrawables(); i++)
		{
			osg::Geometry *geometry = dynamic_cast<osg::Geometry *>(triangleNode->getDrawable(i));

			if (geometry != nullptr)
			{
				osg::ref_ptr<osg::Material> material = new osg::Material;

				SceneNodeMaterial sceneNodeMaterial;
				sceneNodeMaterial.setMaterial(material, materialType, surfaceColorRGB[0], surfaceColorRGB[1], surfaceColorRGB[2], transparency);

				geometry->getOrCreateStateSet()->setAttribute(material.get());

				if (textureAttribute != nullptr) geometry->getOrCreateStateSet()->removeTextureAttribute(0, textureAttribute);
				if (textureAttributeGen != nullptr) geometry->getOrCreateStateSet()->removeTextureAttribute(0, textureAttributeGen);
				if (textureAttributeEnv != nullptr) geometry->getOrCreateStateSet()->removeTextureAttribute(0, textureAttributeEnv);

				textureAttribute = nullptr;
				textureAttributeGen = nullptr;
				textureAttributeEnv = nullptr;

				if (textureType != "None")
				{
					assignTexture(geometry, textureType, reflective, surfaceColorRGB[0], surfaceColorRGB[1], surfaceColorRGB[2]);
				}

				geometry->dirtyGLObjects();
			}
		}
	}

	// Now update the edges color
	osg::Transform *edgeTransform = dynamic_cast<osg::Transform *>(getEdges());
	assert(edgeTransform != nullptr);

	assert(edgeTransform->getNumChildren() == 1);
	osg::Geode *edgesNode = dynamic_cast<osg::Geode *>(edgeTransform->getChild(0));
	assert(edgesNode != nullptr);

	if (edgesNode != nullptr)
	{
		for (int i = 0; i < edgesNode->getNumDrawables(); i++)
		{
			osg::Geometry *geometry = dynamic_cast<osg::Geometry *>(edgesNode->getDrawable(i));

			if (geometry != nullptr)
			{
				osg::Array *dataArray = geometry->getColorArray();
				osg::Vec4Array *colorArray = dynamic_cast<osg::Vec4Array *>(dataArray);

				if (colorArray != nullptr)
				{
					(*colorArray)[0] = osg::Vec4(edgeColorRGB[0], edgeColorRGB[1], edgeColorRGB[2], edgeTranspacency);
				}

				geometry->dirtyGLObjects();
			}
		}
	}
}

void SceneNodeGeometry::updateObjectFacetsFromDataBase(unsigned long long entityID, unsigned long long entityVersion)
{
	deleteShapeNode();

	setStorage(projectName, entityID, entityVersion);

	if (!isVisible())
	{
		setNeedsInitialization();
	}
	else
	{
		initializeFromDataStorage();
	}
}

void SceneNodeGeometry::initializeFromDataStorage(void)
{
	// Here we need to read the facets from the data base entry first

	std::vector<Geometry::Node> nodes;
	std::list<Geometry::Triangle> triangles;
	std::list<Geometry::Edge> edges;
	std::string errors;

	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(entityID, entityVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityFacetData")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityFacetData");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	bsoncxx::document::view nodesObj = doc_view["Nodes"].get_document().view();
	bsoncxx::document::view  trianglesObj = doc_view["Triangles"].get_document().view();
	bsoncxx::document::view  edgesObj = doc_view["Edges"].get_document().view();

	errors = doc_view["Errors"].get_utf8().value.data();

	DataBase::readBSON(nodesObj, nodes);
	DataBase::readBSON(trianglesObj, triangles);
	DataBase::readBSON(edgesObj, edges);

	initializeFromFacetData(nodes, triangles, edges);
}

void SceneNodeGeometry::setMaterialProperties(osg::ref_ptr<osg::Material>& mat, double r, double g, double b) 
{
	mat->setAmbient(osg::Material::FRONT_AND_BACK,
		osg::Vec4(r, g, b, 1.f));
	mat->setDiffuse(osg::Material::FRONT_AND_BACK,
		osg::Vec4(r, g, b, 1.f));
	mat->setSpecular(osg::Material::FRONT_AND_BACK,
		osg::Vec4(r, g, b, 1.f));
	mat->setShininess(osg::Material::FRONT_AND_BACK, 100.f);
	mat->setAlpha(osg::Material::FRONT_AND_BACK, 0.1);

	mat->setColorMode(osg::Material::OFF);
}

void SceneNodeGeometry::assignTexture(osg::Geometry *geometry, const std::string &textureType, bool reflective, double r, double g, double b)
{
	osg::ref_ptr<osg::Texture2D> texture2D = TextureMapManager::getTexture(textureType);

	if (texture2D != nullptr)
	{
		if (reflective)
		{
			geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture2D, osg::StateAttribute::ON);
			textureAttribute = texture2D;

			//Set automatically generated coordinates
			osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen;
			texgen->setMode(osg::TexGen::SPHERE_MAP);
			textureAttributeGen = texgen;

			//Set the arts and science environment, the mode is BLEND
			osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
			texenv->setMode(osg::TexEnv::BLEND);
			//Set the color of BLEND operation
			texenv->setColor(osg::Vec4((float) r, (float) g, (float) b, 0.0f));
			textureAttributeEnv = texenv;

			geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texgen.get(), osg::StateAttribute::ON);

			geometry->getOrCreateStateSet()->setTextureAttribute(0, texenv.get());
		}
		else
		{
			texture2D->setDataVariance(osg::Object::DYNAMIC);

			texture2D->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
			texture2D->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);

			texture2D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
			texture2D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

			geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture2D, osg::StateAttribute::ON);
			textureAttribute = texture2D;
		}
	}
	else
	{
		assert(0); // Texture missing
	}
}


