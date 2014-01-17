#include "FrameStruct.h"

FrameStruct::FrameStruct( void ) : \
translation( 0., 0., 0. ), rotation( 1. ), scale( 1., 1., 1. ) {}

FrameStruct FrameStruct::operator+( const FrameStruct& f ) const
{
    FrameStruct result = *this;
    result += f;
    return result;
}

FrameStruct FrameStruct::operator-( const FrameStruct& f ) const
{
    FrameStruct result = *this;
    result -= f;
    return result;
}

FrameStruct FrameStruct::operator-( void ) const
{
    FrameStruct result;
    result.translation = -translation;
    result.rotation = -rotation;
    result.scale = -scale;
    return result;
}

FrameStruct FrameStruct::operator*( double c ) const
{
    FrameStruct result = *this;
    result *= c;
    return result;
}

FrameStruct FrameStruct::operator/( double c ) const
{
    FrameStruct result = *this;
    result /= c;
    return result;
}

FrameStruct & FrameStruct::operator+=( const FrameStruct& f )
{
    translation += f.translation;
    rotation += f.rotation;
    scale += f.scale;
    return *this;
}

FrameStruct & FrameStruct::operator-=( const FrameStruct& f )
{
    translation -= f.translation;
    rotation -= f.rotation;
    scale -= f.scale;
    return *this;
}

FrameStruct & FrameStruct::operator*=( double c )
{
    translation *= c;
    rotation *= c;
    scale *= c;
    return *this;
}

FrameStruct & FrameStruct::operator/=( double c )
{
    translation /= c;
    rotation /= c;
    scale /= c;
    return *this;
}

FrameStruct FrameStruct::unit( void )
{
    FrameStruct result(*this);
    result.rotation.normalize();
    return result;
}

FrameStruct operator*( double c, const FrameStruct& f )
{
    return f * c;
}

std::ostream& operator<<( std::ostream& os, const FrameStruct& f )
{
    return os \
        << "T: " << f.translation   << std::endl \
        << "R: " << f.rotation      << std::endl \
        << "S: " << f.scale         << std::endl ;
}
