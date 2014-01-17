#include "Keyframe.h"

std::ostream& operator<<( std::ostream& os, const Keyframe& k )
{
	return os << "Frame " << k.index << std::endl << k.data << std::endl;
}