/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "filter_embree.h"
#include <wrap/embree/EmbreeAdaptor.h>

/**
 * @brief Constructor usually performs only two simple tasks of filling the two lists
 *  - typeList: with all the possible id of the filtering actions
 *  - actionList with the corresponding actions. If you want to add icons to
 *  your filtering actions you can do here by construction the QActions accordingly
 */
FilterEmbreePlugin::FilterEmbreePlugin() 
{ 
	typeList = {		 
		FP_OBSCURANCE, 
		FP_AMBIENT_OCCLUSION,
		FP_SDF, 
		FP_SELECT_VISIBLE_FACES, 
		FP_ANALYZE_NORMALS
		};

	for(ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
	
}

FilterEmbreePlugin::~FilterEmbreePlugin()
{
}

QString FilterEmbreePlugin::pluginName() const
{
    return "FilterEmbree";
}

/**
 * @brief ST() must return the very short string describing each filtering action
 * (this string is used also to define the menu entry)
 * @param filterId: the id of the filter
 * @return the name of the filter
 */
QString FilterEmbreePlugin::filterName(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_OBSCURANCE :
		return "Compute Obscurance (embree)";
	case FP_AMBIENT_OCCLUSION:
		return "Compute Ambient occlusion (Embree)";
	case FP_SDF:
		return "Compute Shape-Diameter Function (embree)";
	case FP_SELECT_VISIBLE_FACES:
		return "Compute Visible Faces Select (embree)";
	case FP_ANALYZE_NORMALS:
		return "Compute Face Normal Analysis (embree)";
	default :
		assert(0);
		return QString();
	}
}

/**
 * @brief FilterSamplePlugin::pythonFilterName if you want that your filter should have a different
 * name on pymeshlab, use this function to return its python name.
 * @param f
 * @return
 */
QString FilterEmbreePlugin::pythonFilterName(ActionIDType f) const
{
	switch(f) {
	case FP_OBSCURANCE :
		return "Compute Obscurance (embree)";
	case FP_AMBIENT_OCCLUSION:
		return "Compute Ambient occlusion (embree)";
	case FP_SDF:
		return "Compute Shape-Diameter Function (embree)";
	case FP_SELECT_VISIBLE_FACES:
		return "Compute Visible Faces Select (embree)";
	case FP_ANALYZE_NORMALS:
		return "Compute Face Normal Analysis (embree)";
	default :
		assert(0);
		return QString();
	}
}


/**
 * @brief // Info() must return the longer string describing each filtering action
 * (this string is used in the About plugin dialog)
 * @param filterId: the id of the filter
 * @return an info string of the filter
 */
 QString FilterEmbreePlugin::filterInfo(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_OBSCURANCE:
		return "Compute Obscurance (embree); requires the number of rays used to calculate the obscurance and a tau value used. The Obscurance value is saved into face quality and mapped into a gray shade.";
	case FP_AMBIENT_OCCLUSION:
		return "Compute Ambient occlusion (embree):  requires the number of rays used to calculate the occlusion; The ambient occlusion value is saved into face quality and mapped into a gray shade. ";
	case FP_SDF:
		return "Compute Shape-Diameter Function (embree):  requires the number of rays used to calculate the SDF; The SDF value is saved into face quality and mapped into a color ramp. ";
	case FP_SELECT_VISIBLE_FACES:
		return "Compute visible face select (embree): given the number of rays and a point3f direction, computes a simple shadow; Shadow value is saved into face quality and mapped into a gray shade. ";
	case FP_ANALYZE_NORMALS:
		return "Compute Face Normal Analysis (embree): given a mesh this filter analyzes the mesh and corrects the normals pointing inwards";
	default :
		assert(0);
		return "Unknown Filter";
	}
}

 /**
 * @brief The FilterClass describes in which generic class of filters it fits.
 * This choice affect the submenu in which each filter will be placed
 * More than a single class can be chosen.
 * @param a: the action of the filter
 * @return the class od the filter
 */
FilterEmbreePlugin::FilterClass FilterEmbreePlugin::getClass(const QAction *a) const
{
	return FilterPlugin::Generic;
	/*
	switch(ID(a)) {	
	default :
		return FilterPlugin::Generic;
	}
	*/
}

/**
 * @brief FilterSamplePlugin::filterArity
 * @return
 */
FilterEmbreePlugin::FilterArity FilterEmbreePlugin::filterArity(const QAction*) const
{
	return SINGLE_MESH;
}

/**
 * @brief FilterSamplePlugin::getPreConditions
 * @return
 */
int FilterEmbreePlugin::getPreConditions(const QAction*) const
{
	return MeshModel::MM_NONE;
}

/**
 * @brief FilterSamplePlugin::postCondition
 * @return
 */
int FilterEmbreePlugin::postCondition(const QAction*) const
{
	return MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEQUALITY;
}

/**
 * @brief This function define the needed parameters for each filter. Return true if the filter has some parameters
 * it is called every time, so you can set the default value of parameters according to the mesh
 * For each parameter you need to define,
 * - the name of the parameter,
 * - the default value
 * - the string shown in the dialog
 * - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
 * @param action
 * @param m
 * @param parlst
 */
RichParameterList FilterEmbreePlugin::initParameterList(const QAction *action,const MeshModel &m)
{
	RichParameterList parlst;
	switch(ID(action)) {
		case FP_OBSCURANCE :
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face."));
			parlst.addParam(RichBool ("Parallelize", true, "Parallelize", "If toggled openMP threads are used to speedup the process, the threads used are 4."));
			parlst.addParam(RichFloat("TAU",0.1f,"Tau value", "The value to control spatial decay"));
			break;
		case FP_AMBIENT_OCCLUSION:
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face."));
			parlst.addParam(RichBool ("Parallelize", true, "Parallelize", "If toggled openMP threads are used to speedup the process, the threads used are 4."));
			break;
		case FP_SDF:
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face."));
			parlst.addParam(RichBool ("Parallelize", true, "Parallelize", "If toggled openMP threads are used to speedup the process, the threads used are 4."));
			parlst.addParam(RichFloat("degree",0.1f,"Degree ", "The value for the angle of the cone for which we consider a point as a valid direction"));
			
			break;
		case FP_SELECT_VISIBLE_FACES:
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face."));
			parlst.addParam(RichPosition("dir", Point3f(1.0f, 0.0f, 0.0f), "Direction", "This values indicates the direction of the shadows"));
			break;
		case FP_ANALYZE_NORMALS:
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face."));
			parlst.addParam(RichBool ("Parallelize", true, "Parallelize", "If toggled openMP threads are used to speedup the process, the threads used are 4."));
			break;
		default :
			assert(0);
	}
	return parlst;
}

/**
 * @brief The Real Core Function doing the actual mesh processing.
 * @param action
 * @param md: an object containing all the meshes and rasters of MeshLab
 * @param par: the set of parameters of each filter
 * @param cb: callback object to tell MeshLab the percentage of execution of the filter
 * @return true if the filter has been applied correctly, false otherwise
 */
std::map<std::string, QVariant> FilterEmbreePlugin::applyFilter(const QAction * action, const RichParameterList & parameters, MeshDocument &md, unsigned int& /*postConditionMask*/, vcg::CallBackPos *cb)
{
	
	MeshModel *m = md.mm();
	EmbreeAdaptor<CMeshO> adaptor = EmbreeAdaptor<CMeshO>(m->cm,1);
	
	if(parameters.getBool("Parallelize")){
		adaptor = EmbreeAdaptor<CMeshO>(m->cm,4);			
	}			
	
	switch(ID(action)) {
	case FP_OBSCURANCE:
		adaptor.computeObscurance(m->cm, parameters.getInt("Rays"), parameters.getFloat("TAU"));
		tri::UpdateQuality<CMeshO>::VertexFromFace(m->cm);
  		tri::UpdateColor<CMeshO>::PerVertexQualityGray(m->cm);
		break;
	case FP_AMBIENT_OCCLUSION:			
		adaptor.computeAmbientOcclusion(m->cm,parameters.getInt("Rays"));
		tri::UpdateQuality<CMeshO>::VertexFromFace(m->cm);
  		tri::UpdateColor<CMeshO>::PerVertexQualityGray(m->cm);
		break;
	case FP_SDF:
		adaptor.computeSDF(m->cm,parameters.getInt("Rays"),parameters.getFloat("degree"));
		tri::UpdateQuality<CMeshO>::VertexFromFace(m->cm);
  		tri::UpdateColor<CMeshO>::PerVertexQualityGray(m->cm);
		break;
	case FP_SELECT_VISIBLE_FACES:
		adaptor.selectVisibleFaces(m->cm,parameters.getPoint3m("dir"));
		tri::UpdateQuality<CMeshO>::VertexFromFace(m->cm);
  		tri::UpdateColor<CMeshO>::PerVertexQualityGray(m->cm);
		break;	
	case FP_ANALYZE_NORMALS:
		adaptor.computeNormalAnalysis(m->cm,parameters.getInt("Rays"));
		break;
	default :
		wrongActionCalled(action);
	}
	
	return std::map<std::string, QVariant>();
	
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterEmbreePlugin)
