// SIMMTransform.cpp
// Authors: Ayman Habib, Peter Loan
/*
 * Copyright (c)  2005, Stanford University. All rights reserved. 
* Use of the OpenSim software in source form is permitted provided that the following
* conditions are met:
* 	1. The software is used only for non-commercial research and education. It may not
*     be used in relation to any commercial activity.
* 	2. The software is not distributed or redistributed.  Software distribution is allowed 
*     only through https://simtk.org/home/opensim.
* 	3. Use of the OpenSim software or derivatives must be acknowledged in all publications,
*      presentations, or documents describing work in which OpenSim or derivatives are used.
* 	4. Credits to developers may not be removed from executables
*     created from modifications of the source.
* 	5. Modifications of source code must retain the above copyright notice, this list of
*     conditions and the following disclaimer. 
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR BUSINESS INTERRUPTION) OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
*  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>
#include "Mtx.h"
#include "SIMMTransform.h"
#include "PropertyDblArray.h"

//=============================================================================
// STATIC VARIABLES
//=============================================================================


using namespace OpenSim;
using namespace std;

//=============================================================================
// CONSTRUCTOR(S) AND DESTRUCTOR
//=============================================================================
//_____________________________________________________________________________
/**
 * Destructor.
 */
SIMMTransform::~SIMMTransform()
{
}

//_____________________________________________________________________________
/**
 * Construct an identity SIMMTransform.
 *
 */
SIMMTransform::SIMMTransform()
{
	for(int i=0; i < 4; i++)
		for(int j=0; j < 4; j++)
			_matrix4[i][j] = (i==j)?1.0:0.0;
	_translationOnly = true;

}

//_____________________________________________________________________________
/**
 * Copy constructor
 *
 */
SIMMTransform::SIMMTransform(const SIMMTransform &aTransform)
{
	//Assignment
	(*this) = aTransform;

}
//_____________________________________________________________________________
/**
 * Copy method to be used by ArrayPtrs if needed.
 *
 */
SIMMTransform* SIMMTransform::
copy() const
{
	return(new SIMMTransform(*this));
}

// SIMMTransform from a 4x4 Matrix
SIMMTransform::SIMMTransform(const double aMat44[4][4])
{
	_translationOnly=false;
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			_matrix4[i][j] = aMat44[i][j];
}

// Construct a transform to rotate around an arbitrary axis with specified angle
SIMMTransform::SIMMTransform(const double r, const AnglePreference preference, const SimTK::Vec3& axis)
{
	double aa = axis[X] * axis[X];
	double bb = axis[Y] * axis[Y];
	double cc = axis[Z] * axis[Z];

	double rInRadians = (preference==Radians)? r : (r * SimTK_DEGREE_TO_RADIAN);
	double sinTheta = sin(rInRadians);
	double cosTheta = cos(rInRadians);
	double omCos = 1.0 - cosTheta;

	_matrix4[X][X] = aa + (1.0 - aa) * cosTheta;
	_matrix4[Y][Y] = bb + (1.0 - bb) * cosTheta;
	_matrix4[Z][Z] = cc + (1.0 - cc) * cosTheta;
	_matrix4[X][Y] = axis[X] * axis[Y] * omCos + axis[Z] * sinTheta;
	_matrix4[X][Z] = axis[X] * axis[Z] * omCos - axis[Y] * sinTheta;
	_matrix4[Y][X] = axis[X] * axis[Y] * omCos - axis[Z] * sinTheta;
	_matrix4[Y][Z] = axis[Y] * axis[Z] * omCos + axis[X] * sinTheta;
	_matrix4[Z][X] = axis[X] * axis[Z] * omCos + axis[Y] * sinTheta;
	_matrix4[Z][Y] = axis[Y] * axis[Z] * omCos - axis[X] * sinTheta;

	_matrix4[X][W] = _matrix4[Y][W] = _matrix4[Z][W] = 0.0;
	_matrix4[W][X] = _matrix4[W][Y] = _matrix4[W][Z] = 0.0;
	_matrix4[W][W] = 1.0;

	_translationOnly = false;

}

//=============================================================================
// CONSTRUCTION
//=============================================================================
//=============================================================================
// OPERATORS
//=============================================================================
//-----------------------------------------------------------------------------
// ASSIGNMENT
//-----------------------------------------------------------------------------
SIMMTransform& SIMMTransform::operator=(const SIMMTransform &aTransform)
{
	int i;
	for(i=0; i < 4; i++)
		for(int j=0; j < 4; j++)
			_matrix4[i][j]= aTransform._matrix4[i][j];

	_translationOnly = aTransform._translationOnly;
	return (*this);

}
//=============================================================================
// SET / GET
//=============================================================================
//_____________________________________________________________________________
/**
 * Get the position vector.
 *
 */
void SIMMTransform::
getPosition(SimTK::Vec3& pos) const
{
	for(int i=0; i<3; i++)
		pos[i] = _matrix4[3][i];  //JPL 9/15/05: moved translation from last column to last row
}
//_____________________________________________________________________________
/**
 * Set the position vector.
 *
 */
void SIMMTransform::
setPosition(const SimTK::Vec3& pos)
{
	for(int i=0; i<3; i++)
		_matrix4[3][i]=pos[i];  //JPL 9/15/05: moved translation from last column to last row

}
//_____________________________________________________________________________
/**
 * Get 3x3 orientation matrix from a 4x4 transform matrix
 *
 */
void SIMMTransform::
getOrientation(double rOrientation[3][3]) const
{
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			rOrientation[i][j] = _matrix4[i][j];
}
// Set 3x3 orientation matrix in a 4x4 transform matrix
void SIMMTransform::
setOrientation(const double aOrientation[3][3])
{
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			_matrix4[i][j] = aOrientation[i][j];
}
//_____________________________________________________________________________
/**
 * Set transform matrix to identity
 *
 */
void SIMMTransform::
setIdentity()
{
	Mtx::Identity(4, (double *)_matrix4);
	_translationOnly = true;
}
//_____________________________________________________________________________
/**
 * Set transform matrix based on angles, rotation order and preference
 *
 */
void SIMMTransform::
rotate(const double r[3], const AnglePreference preference, const RotationOrder order)
{
	// Convert angle to radians as this's what low level functions use
	for(int rotAxisIndex=0; rotAxisIndex<3; rotAxisIndex++){
		SIMMTransform::AxisName nextAxis = getRotationAxis(rotAxisIndex, order);
		switch(nextAxis){
			case X:
				rotateX(r[rotAxisIndex], preference);
				break;
			case Y:
				rotateY(r[rotAxisIndex], preference);
				break;
			case Z:
				rotateZ(r[rotAxisIndex], preference);
				break;
			case W: // should we return an error in this case?
				break;
			case NoAxis:
				break;
		}
	}
	_translationOnly = false;
}
//_____________________________________________________________________________
/**
 * Rotate by r degrees or radians around X axis
 *
 */
void SIMMTransform::
rotateX(double r, const AnglePreference preference)
{
	// Convert angle to radians as this's what low level functions use
	double rInRadians = (preference==Radians)? r : (r * SimTK_DEGREE_TO_RADIAN);
	double RotationMatrix[4][4];
	Mtx::Identity(4, (double *)RotationMatrix);
		// COMPUTE SIN AND COS
	double c = cos(rInRadians);
	double s = sin(rInRadians);
	RotationMatrix[1][1] = RotationMatrix[2][2] = c;
	RotationMatrix[1][2] = s;
	RotationMatrix[2][1] = -s;
	Mtx::Multiply(4, 4, 4, (double *)_matrix4, (double *)RotationMatrix, (double *)_matrix4);
	_translationOnly = false;

}
//_____________________________________________________________________________
/**
 * Rotate by r degrees or radians around Y axis
 *
 */
void SIMMTransform::
rotateY(double r, const AnglePreference preference)
{
	// Convert angle to radians as this's what low level functions use
	double rInRadians = (preference==Radians)? r : (r * SimTK_DEGREE_TO_RADIAN);
	double RotationMatrix[4][4];
	Mtx::Identity(4, (double *)RotationMatrix);
		// COMPUTE SIN AND COS
	double c = cos(rInRadians);
	double s = sin(rInRadians);
	RotationMatrix[0][0] = RotationMatrix[2][2] = c;
	RotationMatrix[2][0] = s;
	RotationMatrix[0][2] = -s;

	Mtx::Multiply(4, 4, 4, (double *)_matrix4, (double *)RotationMatrix, (double *)_matrix4);

	_translationOnly = false;
}
//_____________________________________________________________________________
/**
 * Rotate by r degrees or radians around Z axis
 *
 */
void SIMMTransform::
rotateZ(double r, const AnglePreference preference)
{
	// Convert angle to radians as this's what low level functions use
	double rInRadians = (preference==Radians)? r : (r * SimTK_DEGREE_TO_RADIAN);
	double RotationMatrix[4][4];
	Mtx::Identity(4, (double *)RotationMatrix);
		// COMPUTE SIN AND COS
	double c = cos(rInRadians);
	double s = sin(rInRadians);
	RotationMatrix[0][0] = RotationMatrix[1][1] = c;
	RotationMatrix[0][1] = s;
	RotationMatrix[1][0] = -s;

	Mtx::Multiply(4, 4, 4, (double *)_matrix4, (double *)RotationMatrix, (double *)_matrix4);

	_translationOnly = false;
}
//_____________________________________________________________________________
/**
 * Rotate by r degrees or radians around arbitrary axis. The axis must be of
 * unit length.
 *
 */
void SIMMTransform::
rotateAxis(double r, const AnglePreference preference, const SimTK::Vec3& axis)
{
	SIMMTransform RotationMatrix(r, preference, axis);

	Mtx::Multiply(4, 4, 4, (double *)_matrix4, RotationMatrix.getMatrix(), (double *)_matrix4);

	_translationOnly = false;
}
//_____________________________________________________________________________
/**
 * Rotate by r degrees or radians around the local X axis
 *
 */
void SIMMTransform::
rotateXBodyFixed(double r, const AnglePreference preference)
{
	SimTK::Vec3 axis;
	// the local X axis is the first row in the matrix
	axis[0] = _matrix4[0][0];
	axis[1] = _matrix4[0][1];
	axis[2] = _matrix4[0][2];
	rotateAxis(r, preference, axis);
}
//_____________________________________________________________________________
/**
 * Rotate by r degrees or radians around the local Y axis
 *
 */
void SIMMTransform::
rotateYBodyFixed(double r, const AnglePreference preference)
{
	SimTK::Vec3 axis;
	// the local Y axis is the second row in the matrix
	axis[0] = _matrix4[1][0];
	axis[1] = _matrix4[1][1];
	axis[2] = _matrix4[1][2];
	rotateAxis(r, preference, axis);
}
//_____________________________________________________________________________
/**
 * Rotate by r degrees or radians around the local Z axis
 *
 */
void SIMMTransform::
rotateZBodyFixed(double r, const AnglePreference preference)
{
	SimTK::Vec3 axis;
	// the local Z axis is the third row in the matrix
	axis[0] = _matrix4[2][0];
	axis[1] = _matrix4[2][1];
	axis[2] = _matrix4[2][2];
	rotateAxis(r, preference, axis);
}
//_____________________________________________________________________________
/**
 * Translate by double in X direction
 *
 */
void SIMMTransform::
translateX(const double tX)
{
		_matrix4[3][0] += tX;  //JPL 9/15/05: moved translation from last column to last row
}
//_____________________________________________________________________________
/**
 * Translate by double in Y direction
 *
 */
void SIMMTransform::
translateY(const double tY)
{
		_matrix4[3][1] += tY;  //JPL 9/15/05: moved translation from last column to last row
}
//_____________________________________________________________________________
/**
 * Translate by double in Z direction
 *
 */
void SIMMTransform::
translateZ(const double tZ)
{
		_matrix4[3][2] += tZ;  //JPL 9/15/05: moved translation from last column to last row
}

//_____________________________________________________________________________
/**
 * Translate by vector t
 *
 */
void SIMMTransform::
translate(const SimTK::Vec3& t)
{
	for (int i=0; i < 3; i++)
		_matrix4[3][i] += t[i];  //JPL 9/15/05: moved translation from last column to last row
}
/**
 * Return 0 for X, 1 for Y 2 for Z index i starts at 0
 * A careful assignment of numbers to enums will make this much smaller 
 * but much harder to debug or understand!
 * where it can be achieved by bitwise operations
 * eg. XYZ=00.01.10 to indicate X then Y then Z
 * XZY=00.10.01 to indicate X then Z then Y
 * ....
 * then we can return ((order << (2*i))& 03)
 */
SIMMTransform::AxisName SIMMTransform::
getRotationAxis(const int i, RotationOrder order)
{
	if (i==0){
		switch(order){
			case XYZ:
			case XZY:
				return X;
			case YXZ:
			case YZX:
				return Y;
			case ZXY:
			case ZYX:
				return Z;
			default:
				return NoAxis;
		}
	} 
	else if (i==1) {
		switch(order){
			case YXZ:
			case ZXY:
				return X;
			case XYZ:
			case ZYX:
				return Y;
			case YZX:
			case XZY:
				return Z;
			default:
				return NoAxis;
		}

	} 
	else if (i==2) {
		switch(order){
			case ZYX:
			case YZX:
				return X;
			case XZY:
			case ZXY:
				return Y;
			case XYZ:
			case YXZ:
				return Z;
			default:
				return NoAxis;
		}

	} 
	else 
		return NoAxis;
}

void SIMMTransform::transformPoint(double pt[3]) const
{
	double tx = pt[X] * _matrix4[0][0] + pt[Y] * _matrix4[1][0] + pt[Z] * _matrix4[2][0] + _matrix4[3][0];
	double ty = pt[X] * _matrix4[0][1] + pt[Y] * _matrix4[1][1] + pt[Z] * _matrix4[2][1] + _matrix4[3][1];

	pt[Z] = pt[X] * _matrix4[0][2] + pt[Y] * _matrix4[1][2] + pt[Z] * _matrix4[2][2] + _matrix4[3][2];
	pt[X] = tx;
	pt[Y] = ty;
}

void SIMMTransform::transformPoint(SimTK::Vec3& pt) const
{
	double tx = pt[X] * _matrix4[0][0] + pt[Y] * _matrix4[1][0] + pt[Z] * _matrix4[2][0] + _matrix4[3][0];
	double ty = pt[X] * _matrix4[0][1] + pt[Y] * _matrix4[1][1] + pt[Z] * _matrix4[2][1] + _matrix4[3][1];

	pt[Z] = pt[X] * _matrix4[0][2] + pt[Y] * _matrix4[1][2] + pt[Z] * _matrix4[2][2] + _matrix4[3][2];
	pt[X] = tx;
	pt[Y] = ty;
}

void SIMMTransform::transformVector(double vec[3]) const
{
	double tx = vec[X] * _matrix4[0][0] + vec[Y] * _matrix4[1][0] + vec[Z] * _matrix4[2][0];
	double ty = vec[X] * _matrix4[0][1] + vec[Y] * _matrix4[1][1] + vec[Z] * _matrix4[2][1];

	vec[Z] = vec[X] * _matrix4[0][2] + vec[Y] * _matrix4[1][2] + vec[Z] * _matrix4[2][2];
	vec[X] = tx;
	vec[Y] = ty;
}

void SIMMTransform::transformVector(SimTK::Vec3& vec) const
{
	double tx = vec[X] * _matrix4[0][0] + vec[Y] * _matrix4[1][0] + vec[Z] * _matrix4[2][0];
	double ty = vec[X] * _matrix4[0][1] + vec[Y] * _matrix4[1][1] + vec[Z] * _matrix4[2][1];

	vec[Z] = vec[X] * _matrix4[0][2] + vec[Y] * _matrix4[1][2] + vec[Z] * _matrix4[2][2];
	vec[X] = tx;
	vec[Y] = ty;
}

void SIMMTransform::getMatrix(double aMat[]) const
{
	for(int i=0; i < 4; i++)
		for(int j=0; j < 4; j++)
			aMat[i*4+j] = _matrix4[i][j];
}

void SIMMTransform::
setRotationSubmatrix(double rDirCos[3][3])
{
	_translationOnly=false;
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			_matrix4[i][j] = rDirCos[i][j];
}

/**
 * Debugging 
 */
void SIMMTransform::printMatrix()
{
	cout << "Xform: " << endl;
	for(int i=0; i < 4; i++){
		for(int j=0; j < 4; j++){
			cout << _matrix4[i][j]<< " ";
		}
		cout << endl;
	}
	cout << endl << endl;
}