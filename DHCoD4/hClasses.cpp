#include <windows.h>
#include "hMath.h"


bool Sortable_t::operator==(Sortable_t in)
{
	return this->_a == in._a;
}

bool Sortable_t::operator<=(Sortable_t in)
{
	return this->_a <= in._a;
}

bool Sortable_t::operator<(Sortable_t in)
{
	return this->_a < in._a;
}

bool Sortable_t::operator>=(Sortable_t in)
{
	return this->_a >= in._a;
}

bool Sortable_t::operator>(Sortable_t in)
{
	return this->_a == in._a;
}
