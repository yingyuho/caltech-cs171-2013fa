#include "shading_data.h"

// ShadingData

ShadingData::ShadingData() : ws_coord(0.), nd_coord(0.), ws_normal(0.) {}

ShadingData::ShadingData(const Vec4& wsc, const Vec4& ndc, const NVec3& wsn)\
: ws_coord(homogenize(wsc)), nd_coord(homogenize(ndc)), ws_normal(wsn.normalize()) {}

std::ostream& operator<<(std::ostream& os, const ShadingData& x) {
    os << x.ws_coord.transpose() << x.nd_coord.transpose() << x.ws_normal << std::endl;
    return os;
}
