#include "runtime_pool.hpp"
#include "core.hpp"

#include "../gui/controller.hpp"

#include "shapes.hpp"
#include "design.hpp"

RuntimePool::RuntimePool(RuntimePoolManagerPtr p)
	: m_parent(p)
{
}

void RuntimePool::addPoint(QPoint p)
{
	for (auto it : m_shapes)
		it->addPoint(p);
}

void RuntimePool::movePoint(QPoint p)
{
	for (auto it : m_shapes)
		it->movePoint(p);
}

void RuntimePool::changeBasicProperties(const ShapeProperties& b)
{
	for (auto it : m_shapes)
		it->updateProperties(b);
}

std::string RuntimePool::getName() const noexcept
{
	return std::move("Runtime");
}

void RuntimePool::dumpToFile(const std::string&) const 
{

}

RuntimePoolManager::RuntimePoolManager()
{
}

void RuntimePoolManager::addChild(RuntimePoolPtr p, const std::string& name)
{
	ASSERT_RETURN(p != nullptr);
	m_children[name] = p;
}

RuntimePoolPtr RuntimePoolManager::getChild(const std::string& name) const
{
	// throw erroe if not exists
	auto it = m_children.find(name);
	if (it != m_children.end())
		return (*it).second;
	return nullptr;
}

std::map<std::string, RuntimePoolPtr> RuntimePoolManager::getChildren() const noexcept
{
	return m_children;
}