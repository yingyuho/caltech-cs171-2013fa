#include "material.h"

// Material

Material::Material(const Vec3& a, const Vec3& d, const Vec3& s, double shininess)\
: aColor(a), dColor(d), sColor(s), shininess(shininess) {}