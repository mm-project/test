#ifndef WORKING_SET_HPP
#define WORKING_SET_HPP

#include "basic_shape.hpp"
#include "iobject_pool.hpp"

#include <vector>
#include <QObject>

class WorkingSet : public IObjectPool
{
public:
	//
	//	Interface
	//
	virtual void clear() override;
	virtual std::vector<IShape*> getObjects() const override;
	virtual void addObject(IShape*) override;
	virtual ~WorkingSet() {}

private:
	//
	//	Methods
	// 
	std::vector<IShape*> m_shapes;
};

#endif
