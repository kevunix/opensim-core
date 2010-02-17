#ifndef __OpenSimContext_h__
#define __OpenSimContext_h__

// OpenSimContext.h
// Authors: Jack Middleton, Ayman Habib
/*
 * Copyright (c)  2009, Stanford University. All rights reserved. 
 * Use of the OpenSim software in source form is permitted provided that the following
 * conditions are met:
 *   1. The software is used only for non-commercial research and education. It may not
 *     be used in relation to any commercial activity.
 *   2. The software is not distributed or redistributed.  Software distribution is allowed 
 *     only through https://simtk.org/home/opensim.
 *   3. Use of the OpenSim software or derivatives must be acknowledged in all publications,
 *      presentations, or documents describing work in which OpenSim or derivatives are used.
 *   4. Credits to developers may not be removed from executables
 *     created from modifications of the source.
 *   5. Modifications of source code must retain the above copyright notice, this list of
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

#include <OpenSim/Common/Object.h>
#include <OpenSim/Simulation/osimSimulationDLL.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Common/Array.h>
#include "SimTKsimbody.h"
#include <OpenSim/Tools/IKTool.h>

class SimTK::Transform;

namespace OpenSim {

class Body;
class Coordinate;
class TransformAxis;
class Function;
class Marker;
class MarkerSet;
class Model;
class MovingPathPoint;
class Muscle;
class GeometryPath;
class PathPoint;
class PathWrap;
class ConditionalPathPoint;
class WrapObject;
class Analysis;
class AnalyzeTool;
class ModelScaler;
class MarkerPlacer;
class MarkerData;
class Measurement;

#ifdef SWIG
    #ifdef OSIMSIMULATION_API
        #undef OSIMSIMULATION_API
        #define OSIMSIMULATION_API
    #endif
#endif


class /*OSIMSIMULATION_API*/ OpenSimContext : public Object {
//=============================================================================
// DATA
//=============================================================================

private:
    SimTK::State* _configState;
    Model* _model;

public:
    OpenSimContext(SimTK::State* s, Model* model);
    void setState( SimTK::State* s) { _configState = s; }
    void setModel( Model* m) { _model = m; }

	// States
	void setStates( Array<double>& states);
	void setStates( double statesBuffer[]);
	void computeConstrainedCoordinates( double statesBuffer[]);
	void getStates( double statesBuffer[]);
	// Transforms
    void transformPosition(const Body& body, double offset[], double gOffset[]);
    Transform getTransform(const Body& body);
	void transform(const Body& ground, double d[], Body& body, double dragVectorBody[]);
	// Coordinates
    double getValue(const Coordinate& coord);
	bool getLocked(const Coordinate& coord);
	void setValue(const Coordinate& coord, double d, bool enforceConstraints=true);
	void setClamped(const Coordinate& coord, bool newValue);
	bool getClamped(const Coordinate& coord);
	void setLocked(const Coordinate& coord, bool newValue);
	// Muscles
	double getActivation(Muscle& act);
	double getMuscleLength(Muscle& act);
	const Array<PathPoint*>& getCurrentPath(Muscle& act);
	const Array<PathPoint*>& getCurrentDisplayPath(Muscle& act);
	void updateDisplayer(Muscle& m);
    void copyMuscle(Muscle& from, Muscle& to);
	// Muscle Points
	void setXFunction(MovingPathPoint& mmp, Function& newFunction);
    void setYFunction(MovingPathPoint& mmp, Function& newFunction);
    void setZFunction(MovingPathPoint& mmp, Function& newFunction);
    void setXCoordinate(MovingPathPoint& mmp, Coordinate& newCoord);
    void setYCoordinate(MovingPathPoint& mmp, Coordinate& newCoord);
    void setZCoordinate(MovingPathPoint& mmp, Coordinate& newCoord);
    void setBody(PathPoint& pathPoint, Body& newBody);
    void setCoordinate(ConditionalPathPoint& via, Coordinate& newCoord);
    void setRangeMin(ConditionalPathPoint& via, double d);
    void setRangeMax(ConditionalPathPoint& via, double d);
    bool replacePathPoint(GeometryPath& p, PathPoint& mp, PathPoint& newPoint);
    void setLocation(PathPoint& mp, int i, double d);
    void setEndPoint(PathWrap& mw, int newEndPt);
	void addPathPoint(GeometryPath& p, int menuChoice, Body& body);
	bool deletePathPoint(GeometryPath& p, int menuChoice);
	bool isActivePathPoint(PathPoint& mp) ; 
	// Muscle Wrapping
	void setStartPoint(PathWrap& mw, int newStartPt);
	void addPathWrap(GeometryPath& p, WrapObject& awo);
    void moveUpPathWrap(GeometryPath& p, int num);
    void moveDownPathWrap(GeometryPath& p, int num);
    void deletePathWrap(GeometryPath& p, int num);
	// Markers
	void setBody(Marker& currentMarker, Body& newBody, bool  b);
	int replaceMarkerSet(Model& model, MarkerSet& aMarkerSet);
	// Analyses
	int step(Analysis& analysis);
	// Tools
	bool initializeTrial(IKTool& ikTool, int i);
	bool solveTrial( IKTool& ikTool, int i);
	void setStatesFromMotion(AnalyzeTool& analyzeTool, const Storage &aMotion, bool aInDegrees);
	void loadStatesFromFile(AnalyzeTool& analyzeTool);
	bool processModelScale(ModelScaler& modelScaler, 
		Model* aModel, const std::string& aPathToSubject="", double aFinalMass = -1.0);
	bool processModelMarkerPlacer( MarkerPlacer& markerPlacer, 
		Model* aModel, const std::string& aPathToSubject="");
	double computeMeasurementScaleFactor(ModelScaler& modelScaler, 
		const Model& aModel, const MarkerData& aMarkerData, const Measurement& aMeasurement) const;
   void replaceTransformAxisFunction(TransformAxis& aDof, OpenSim::Function& aFunction);

	// Utilities
    static bool isNaN( double v ) { return (SimTK::isNaN(v)); }

	double getTime() { 
		assert(_configState); 
		return (_configState->getTime()); 
	}

	static void getTransformAsDouble16(const Transform& aTransform, double flattened[]){
		 double* matStart = &aTransform.toMat44()[0][0];
		 for (int i=0; i<16; i++) flattened[i]=matStart[i];
	}

}; // class OpenSimContext

// Class used as base class for Java classes deriving from Analysis (used to be callback)
// It lives on the C++ side so that it gets access to SimTK::State.
class AnalysisWrapper : public Analysis {
	double* statesCache;
	int		statesCacheSize;
	double  simulationTime;
public:
	AnalysisWrapper(Model *aModel=0):
	  Analysis(aModel),
	  statesCacheSize(aModel->getNumStates()){
		statesCache = new double[statesCacheSize];
		simulationTime = -1.0;
	}
	virtual int step( const SimTK::State& s, int stepNumber) {
        const SimTK::Vector& y = s.getY(); 
		//std::string statedump= s.toString();
		memcpy(statesCache, &y[0], statesCacheSize*sizeof(double));
        //for(int i=0;i<statesCacheSize;i++ ) 
		//	*(statesCache+i) = y[i];
		simulationTime = s.getTime();
		return 0;
	}
	void getStates( double statesBuffer[]){
		for(int i=0;i<statesCacheSize;i++ ) 
			*(statesBuffer+i) = statesCache[i];
	}
	double getSimulationTime() {
		return simulationTime;
	}

}; // Class AnalysisWrapper


// Class to handle interrupts
class InterruptCallback : public Analysis {
	bool _throwException;
public:
	InterruptCallback(Model *aModel=0):
	  Analysis(aModel),
	  _throwException(false){};

	void interrupt() {
		_throwException=true;
	}
	virtual int step( const SimTK::State& s) {
		if (_throwException)
			throw Exception("Operation Aborted");
		return 0;
	}
	
};
}  // namespace OpenSimContext

#endif // _OpenSimContext_h__
