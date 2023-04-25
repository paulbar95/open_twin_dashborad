#pragma once

#include "OpenTwinCore/CoreTypes.h"

#include <string>
#include <list>
#include <cassert>

#include "Types.h"

namespace osg { class Switch; };

class SceneNodeBase
{
public:
	SceneNodeBase() : shapeNode(nullptr), treeItemID(0), modelEntityID(0), editable(false), visible(true), selected(false), transparent(false), 
					  wireframe(false), highlighted(false), offset(1.0), selectChildren(true), manageVisibilityOfParent(true), manageVisibilityOfChildren(true), parent(nullptr) {};
	virtual ~SceneNodeBase() 
	{ 
		if (parent != nullptr)
		{
			parent->removeChild(this);
		}
		std::list<SceneNodeBase*> currentChilds = children;  
		for (auto child : currentChilds)
		{
			delete child;
		}
	};

	void setName(const std::string &n) { name = n; };
	const std::string& getName(void) { return name; };

	void setShapeNode(osg::Switch *node) { shapeNode = node; };
	osg::Switch *getShapeNode(void) { return shapeNode; };

	void setTreeItemID(ot::UID iD) { treeItemID = iD; };
	ot::UID getTreeItemID(void) { return treeItemID; };

	void setModelEntityID(unsigned long long id) { modelEntityID = id; };
	unsigned long long getModelEntityID(void) { return modelEntityID; };

	bool isEditable(void) { return editable; };
	virtual void setEditable(bool v) { editable = v; };

	bool isVisible(void) { return visible; };
	virtual void setVisible(bool v) { visible = v; };

	bool isSelected(void) { return selected; };
	virtual void setSelected(bool s) { selected = s; };

	bool isTransparent(void) { return transparent; };
	virtual void setTransparent(bool t) { transparent = t; };

	bool isWireframe(void) { return wireframe; };
	virtual void setWireframe(bool w) { wireframe = w; };

	bool isHighlighted(void) { return highlighted; };
	virtual void setHighlighted(bool h) { if (highlighted != h) { highlighted = h; if (selectChildren) { for (auto child : children) child->setHighlighted(h); } } };

	void        setErrors(std::string &e) { errors = e; };
	bool        hasErrors(void) { return !errors.empty(); };
	std::string getErrors(void) { return errors; };

	void   setOffset(double value) { offset = value; };
	double getOffset(void) { return offset; };

	void setSelectChildren(bool flag) { selectChildren = flag; }
	bool getSelectChildren(void) { return selectChildren; }

	void setManageVisibilityOfParent(bool flag) { manageVisibilityOfParent = flag; }
	bool getManageVisibilityOfParent(void) { return manageVisibilityOfParent; }

	void setManageVisibilityOfChildren(bool flag) { manageVisibilityOfChildren = flag; }
	bool getManageVisibilityOfChildren(void) { return manageVisibilityOfChildren; }

	void setParent(SceneNodeBase *item) { parent = item; };
	SceneNodeBase *getParent(void) { return parent; };

	void addChild(SceneNodeBase *child) { assert(std::find(children.begin(), children.end(), child) == children.end()); children.push_back(child); child->setParent(this); };
	void removeChild(SceneNodeBase *child) { assert(std::find(children.begin(), children.end(), child) != children.end());  children.remove(child); };
	const std::list<SceneNodeBase*> &getChildren(void) { return children; };

	void setTreeIcons(const TreeIcon &icons) { treeIcons = icons; };
	const TreeIcon &getTreeIcons(void) { return treeIcons; };

	virtual void getPrefetch(std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs) {};

	virtual bool isItem1D(void) = 0;
	virtual bool isItem3D(void) = 0;

protected:
	osg::Switch *      shapeNode;
	const float transparency = 0.15;

private:
	std::string        name;
	ot::UID			   treeItemID;
	unsigned long long modelEntityID;
	bool			   editable;
	bool               visible;
	bool               selected;
	bool               transparent;
	bool               wireframe;
	bool			   highlighted;
	double			   offset;
	bool			   selectChildren;
	bool			   manageVisibilityOfParent;
	bool			   manageVisibilityOfChildren;
	std::string        errors;
	SceneNodeBase *    parent;
	std::list<SceneNodeBase*> children;
	TreeIcon		   treeIcons;
};

