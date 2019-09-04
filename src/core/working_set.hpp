#ifndef WORKING_SET_HPP
#define WORKING_SET_HPP

#include "ishape.hpp"
#include "iobject_pool.hpp"

#include <vector>
#include <map>
#include <QObject>

class WSCallbackData;
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
	virtual std::string getName() override;
	virtual void dumpToFile(const std::string&);
    IShape* getClonee(IShape*);
	void removeObject(IShape*);

private:
	//
	//	Methods
	// 
	std::vector<IShape*> m_shapes;
    std::map<IShape*,IShape*> m_clone2original;
    WSCallbackData* cb;
};

using WorkingSetPtr = std::shared_ptr<WorkingSet>;

#endif
