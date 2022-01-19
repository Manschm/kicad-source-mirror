/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2021-2022 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EDA_ANGLE_H
#define EDA_ANGLE_H

#include <cassert>
#include <cmath>
#include <math/vector2d.h>  // for VECTOR2I


enum EDA_ANGLE_T
{
    TENTHS_OF_A_DEGREE_T,
    DEGREES_T,
    RADIANS_T
};


class EDA_ANGLE
{
public:
    /**
     * Angles can be created in degrees, 1/10ths of a degree, or radians, and read as any of
     * the angle types.
     *
     * Angle type must be explicitly specified at creation, because there is no other way of
     * knowing what an int or a double represents.
     */
    EDA_ANGLE( double aValue, EDA_ANGLE_T aAngleType )
    {
        switch( aAngleType )
        {
        case RADIANS_T:
            m_value = aValue / DEGREES_TO_RADIANS;
            break;

        case TENTHS_OF_A_DEGREE_T:
            m_value = aValue / 10.0;
            break;

        default:
            m_value = aValue;
        }
    }

    explicit EDA_ANGLE( const VECTOR2D& aVector )
    {
        if( aVector.x == 0.0 && aVector.y == 0.0 )
        {
            m_value = 0.0;
        }
        else if( aVector.y == 0.0 )
        {
            if( aVector.x >= 0 )
                m_value = 0.0;
            else
                m_value = -180.0;
        }
        else if( aVector.x == 0.0 )
        {
            if( aVector.y >= 0.0 )
                m_value = 90.0;
            else
                m_value = -90.0;
        }
        else if( aVector.x == aVector.y )
        {
            if( aVector.x >= 0.0 )
                m_value = 45.0;
            else
                m_value = -180.0 + 45.0;
        }
        else if( aVector.x == -aVector.y )
        {
            if( aVector.x >= 0.0 )
                m_value = -45.0;
            else
                m_value = 180.0 - 45.0;
        }
        else
        {
            *this = EDA_ANGLE( atan2( aVector.y, aVector.x ), RADIANS_T );
        }
    }

    explicit EDA_ANGLE( const VECTOR2I& aVector )
    {
        /* gcc is surprisingly smart in optimizing these conditions in a tree! */
    
        if( aVector.x == 0 && aVector.y == 0 )
        {
            m_value = 0;
        }
        else if( aVector.y == 0 )
        {
            if( aVector.x >= 0 )
                m_value = 0.0;
            else
                m_value = -180.0;
        }
        else if( aVector.x == 0 )
        {
            if( aVector.y >= 0 )
                m_value = 90.0;
            else
                m_value = -90.0;
        }
        else if( aVector.x == aVector.y )
        {
            if( aVector.x >= 0 )
                m_value = 45.0;
            else
                m_value = -180.0 + 45.0;
        }
        else if( aVector.x == -aVector.y )
        {
            if( aVector.x >= 0 )
                m_value = -45.0;
            else
                m_value = 180.0 - 45.0;
        }
        else
        {
            *this = EDA_ANGLE( atan2( (double) aVector.y, (double) aVector.x ), RADIANS_T );
        }
    }

    EDA_ANGLE() :
            m_value( 0.0 )
    {}

    inline double AsDegrees() const { return m_value; }

    inline int AsTenthsOfADegree() const { return KiROUND( m_value * 10.0 ); }

    inline double AsRadians() const { return m_value * DEGREES_TO_RADIANS; }

    inline double AsAngleType( EDA_ANGLE_T aAngleType ) const
    {
        switch( aAngleType )
        {
        case TENTHS_OF_A_DEGREE_T: return AsTenthsOfADegree();
        case DEGREES_T:            return AsDegrees();
        case RADIANS_T:            return AsRadians();
        default: assert( false );
        }
    }

    static constexpr double DEGREES_TO_RADIANS = M_PI / 180.0;

    /**
     * @return true if angle is one of the four cardinal directions (0/90/180/270 degrees),
     *         otherwise false
     */
    bool IsCardinal() const
    {
        double test = m_value;

        while( test < 0.0 )
            test += 90.0;

        while( test > 90.0 )
            test -= 90.0;

        return test == 0.0;
    }

    bool IsZero() const
    {
        return m_value == 0.0;
    }

    bool IsHorizontal() const
    {
        return m_value == 0.0 || m_value == 180.0;
    }

    bool IsVertical() const
    {
        return m_value == 90.0 || m_value == 270.0;
    }

    EDA_ANGLE Invert() const
    {
        return EDA_ANGLE( -AsDegrees(), DEGREES_T );
    }

    double Sin() const
    {
        EDA_ANGLE test = *this;
        test.Normalize();

        if( test.m_value == 0.0 || test.m_value == 180.0 )
            return 0.0;
        else if( test.m_value == 90.0 )
            return 1.0;
        else if( test.m_value == 270.0 )
            return -1.0;
        else
            return sin( AsRadians() );
    }

    double Cos() const
    {
        EDA_ANGLE test = *this;
        test.Normalize();

        if( test.m_value == 0.0 )
            return 1.0;
        else if( test.m_value == 180.0 )
            return -1.0;
        else if( test.m_value == 90.0 || test.m_value == 270.0 )
            return 0.0;
        else
            return cos( AsRadians() );
    }

    double Tan() const { return tan( AsRadians() ); }

    static EDA_ANGLE Arccos( double x ) { return EDA_ANGLE( acos( x ), RADIANS_T ); }

    static EDA_ANGLE Arcsin( double x ) { return EDA_ANGLE( asin( x ), RADIANS_T ); }

    static EDA_ANGLE Arctan( double x ) { return EDA_ANGLE( atan( x ), RADIANS_T ); }

    static EDA_ANGLE Arctan2( double y, double x )
    {
        return EDA_ANGLE( atan2( y, x ), RADIANS_T );
    }

    inline EDA_ANGLE Normalize()
    {
        while( m_value < -0.0 )
            m_value += 360.0;

        while( m_value >= 360.0 )
            m_value -= 360.0;

        return *this;
    }

    inline EDA_ANGLE Normalize90()
    {
        while( m_value < -90.0 )
            m_value += 180.0;

        while( m_value > 90.0 )
            m_value -= 180.0;

        return *this;
    }

    inline EDA_ANGLE Normalize180()
    {
        while( m_value <= -180.0 )
            m_value += 360.0;

        while( m_value > 180.0 )
            m_value -= 360.0;

        return *this;
    }

    inline EDA_ANGLE Normalize720()
    {
        while( m_value < -360.0 )
            m_value += 360.0;

        while( m_value >= 360.0 )
            m_value -= 360.0;

        return *this;
    }

    EDA_ANGLE KeepUpright() const;

    EDA_ANGLE& operator+=( const EDA_ANGLE& aAngle )
    {
        *this = EDA_ANGLE( AsDegrees() + aAngle.AsDegrees(), DEGREES_T );
        return *this;
    }

    EDA_ANGLE& operator-=( const EDA_ANGLE& aAngle )
    {
        *this = EDA_ANGLE( AsDegrees() - aAngle.AsDegrees(), DEGREES_T );
        return *this;
    }

private:
    double  m_value;           ///< value in degrees

public:
    static EDA_ANGLE m_Angle0;
    static EDA_ANGLE m_Angle45;
    static EDA_ANGLE m_Angle90;
    static EDA_ANGLE m_Angle135;
    static EDA_ANGLE m_Angle180;
    static EDA_ANGLE m_Angle270;
    static EDA_ANGLE m_Angle360;
};


inline EDA_ANGLE operator-( const EDA_ANGLE& aAngle )
{
    return aAngle.Invert();
}


inline EDA_ANGLE operator-( const EDA_ANGLE& aAngleA, const EDA_ANGLE& aAngleB )
{
    return EDA_ANGLE( aAngleA.AsDegrees() - aAngleB.AsDegrees(), DEGREES_T );
}


inline EDA_ANGLE operator+( const EDA_ANGLE& aAngleA, const EDA_ANGLE& aAngleB )
{
    return EDA_ANGLE( aAngleA.AsDegrees() + aAngleB.AsDegrees(), DEGREES_T );
}


inline EDA_ANGLE operator*( const EDA_ANGLE& aAngleA, double aOperator )
{
    return EDA_ANGLE( aAngleA.AsDegrees() * aOperator, DEGREES_T );
}


inline EDA_ANGLE operator/( const EDA_ANGLE& aAngleA, double aOperator )
{
    return EDA_ANGLE( aAngleA.AsDegrees() / aOperator, DEGREES_T );
}


inline bool operator==( const EDA_ANGLE& aAngleA, const EDA_ANGLE& aAngleB )
{
    return aAngleA.AsDegrees() == aAngleB.AsDegrees();
}


inline bool operator!=( const EDA_ANGLE& aAngleA, const EDA_ANGLE& aAngleB )
{
    return aAngleA.AsDegrees() != aAngleB.AsDegrees();
}


inline bool operator>( const EDA_ANGLE& aAngleA, const EDA_ANGLE& aAngleB )
{
    return aAngleA.AsDegrees() > aAngleB.AsDegrees();
}


inline bool operator<( const EDA_ANGLE& aAngleA, const EDA_ANGLE& aAngleB )
{
    return aAngleA.AsDegrees() < aAngleB.AsDegrees();
}


inline bool operator<=( const EDA_ANGLE& aAngleA, const EDA_ANGLE& aAngleB )
{
    return aAngleA.AsDegrees() <= aAngleB.AsDegrees();
}


inline bool operator>=( const EDA_ANGLE& aAngleA, const EDA_ANGLE& aAngleB )
{
    return aAngleA.AsDegrees() >= aAngleB.AsDegrees();
}


static constexpr EDA_ANGLE& ANGLE_HORIZONTAL  = EDA_ANGLE::m_Angle0;
static constexpr EDA_ANGLE& ANGLE_VERTICAL    = EDA_ANGLE::m_Angle90;
static constexpr EDA_ANGLE& FULL_CIRCLE       = EDA_ANGLE::m_Angle360;

static constexpr EDA_ANGLE& ANGLE_0   = EDA_ANGLE::m_Angle0;
static constexpr EDA_ANGLE& ANGLE_45  = EDA_ANGLE::m_Angle45;
static constexpr EDA_ANGLE& ANGLE_90  = EDA_ANGLE::m_Angle90;
static constexpr EDA_ANGLE& ANGLE_135 = EDA_ANGLE::m_Angle135;
static constexpr EDA_ANGLE& ANGLE_180 = EDA_ANGLE::m_Angle180;
static constexpr EDA_ANGLE& ANGLE_270 = EDA_ANGLE::m_Angle270;
static constexpr EDA_ANGLE& ANGLE_360 = EDA_ANGLE::m_Angle360;


#endif // EDA_ANGLE_H
