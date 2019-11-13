#include "RegionQueryService.hpp"
#include "rq_object.hpp"
#include "debug_helper.hpp"

//Fake RQ by just working directly to working set.
#ifdef NO_RQ
    RegionQuery::RegionQuery() {}
    void RegionQuery::insertObject(IShape*) {}
    void RegionQuery::removeObject(IShape*) {}
    void RegionQuery::clear() {}
    void RegionQuery::shutDown() {}
    
    void RegionQuery::setWS(IObjectPoolPtr ws) { m_ws = ws; }

    IShape* RegionQuery::getShapeUnderPos(const QPoint& point) const
    {
        for(auto shape: m_ws->getObjects()) 
            if ( shape->contains(point) )
                return shape;
        
        return nullptr;
    }
    
    std::vector<IShape*> RegionQuery::getShapesUnderRect(const QRect& box) const
    {
        std::vector<IShape*> shapes;
            for(auto shape: m_ws->getObjects()) {
                bool contains = true;
                
                for(auto point: shape->getPoints())
                    if (!box.contains(point))
                        contains = false;
                
                if (contains)    
                    shapes.push_back(shape);
            }
        return shapes;
    }
#endif
    
#ifndef NO_RQ
RegionQuery::RegionQuery()
{
	m_tree = std::shared_ptr<rq::RQtree<IShape>> (new rq::RQtree<IShape>());
}

void RegionQuery::insertObject(IShape* object)
{
	rq::RQobjectPtr obj;
	switch (object->getType())
	{
	case LINE:
		obj = std::shared_ptr<rq::IRQobject>(new rq::RQline(object));
		break;
	case RECTANGLE:
		obj = std::shared_ptr<rq::IRQobject>(new rq::RQrect(object));
		break;
	case ELLIPSE:
		obj = std::shared_ptr<rq::IRQobject>(new rq::RQellipse(object));
		break;
	case POLYGON:
		obj = std::shared_ptr<rq::IRQobject>(new rq::RQpolygon(object));
		break;
	}
    DBG_RQ("insert",object);
	m_tree->insert(obj);
}
 
void RegionQuery::removeObject(IShape* object)
{
	rq::RQobjectPtr obj;
	switch (object->getType())
	{
	case LINE:
		obj = std::shared_ptr<rq::IRQobject>(new rq::RQline(object));
		break;
	case RECTANGLE:
		obj = std::shared_ptr<rq::IRQobject>(new rq::RQrect(object));
		break;
	case ELLIPSE:
		obj = std::shared_ptr<rq::IRQobject>(new rq::RQellipse(object));
		break;
	case POLYGON:
		obj = std::shared_ptr<rq::IRQobject>(new rq::RQpolygon(object));
		break;
	}
    DBG_RQ("remove",object);
	m_tree->remove(obj);
}

IShape* RegionQuery::getShapeUnderPos(const QPoint& p) const
{
	rq::RQobjectPtr obj = m_tree->getObject(rq::CPoint(p));

	if (obj != nullptr)
		return obj->getObject();

	return nullptr;
}

std::vector<IShape*> RegionQuery::getShapesUnderRect(const QRect& oRect) const
{
	std::vector<IShape*> shapes;

	std::vector<rq::RQobjectPtr> objects = m_tree->getObjects(oRect);

	for (auto it : objects)
		if (it != nullptr)
			shapes.push_back(it->getObject());

	return shapes;
}

void RegionQuery::clear()
{
	m_tree->clear();
}

void RegionQuery::shutDown()
{
	clear();
}
#endif
