#pragma once

#include <boost/operators.hpp>
class ShaderAdjacencyIterator :
	boost::forward_iterator_helper<ShaderAdjacencyIterator,
	unsigned, std::ptrdiff_t, unsigned*, unsigned>
{
public:
	ShaderAdjacencyIterator(void);
	~ShaderAdjacencyIterator(void);
};
