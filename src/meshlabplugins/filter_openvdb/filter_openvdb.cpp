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

#include "filter_openvdb.h"
#include <QElapsedTimer>

/*
 * The following symbols are defined in both QT and OpenVDB with conflicting definitions.
 * We need to undefine them to avoid compilation errors.
*/
#ifdef foreach
    #undef foreach
#endif
#ifdef emit
    #undef emit
#endif
#include <wrap/openvdb/OpenVDBAdapter.h>

/**
 * @brief Constructor usually performs only two simple tasks of filling the two lists
 *  - typeList: with all the possible id of the filtering actions
 *  - actionList with the corresponding actions. If you want to add icons to
 *  your filtering actions you can do here by construction the QActions accordingly
 */
FilterOpenVDBPlugin::FilterOpenVDBPlugin() 
{ 
	typeList = {FP_OPENVDB_VOLUME, FP_OPENVDB_LEVELSET};

	for(ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
}

FilterOpenVDBPlugin::~FilterOpenVDBPlugin()
{
}

QString FilterOpenVDBPlugin::pluginName() const
{
    return "FilterOpenVDB";
}

/**
 * @brief ST() must return the very short string describing each filtering action
 * (this string is used also to define the menu entry)
 * @param filterId: the id of the filter
 * @return the name of the filter
 */
QString FilterOpenVDBPlugin::filterName(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_OPENVDB_VOLUME :
		return "OpenVDB Volume Remeshing";
	case FP_OPENVDB_LEVELSET :
		return "OpenVDB Level Set Remeshing";
	default :
		assert(0);
		return QString();
	}
}

/**
#ifdef foreach
    #undef foreach
#endif
#ifdef emit
    #undef emit
#endif
 * @brief FilterOpenVDBPlugin::pythonFilterName if you want that your filter should have a different
 * name on pymeshlab, use this function to return its python name.
 * @param f
 * @return
 */
QString FilterOpenVDBPlugin::pythonFilterName(ActionIDType f) const
{
	switch(f) {
	case FP_OPENVDB_VOLUME :
		return "openvdb_volume_remesh";
	case FP_OPENVDB_LEVELSET :
		return "openvdb_levelset_remesh";
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
 QString FilterOpenVDBPlugin::filterInfo(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_OPENVDB_VOLUME :
		return "Remesh the current mesh using the function meshToVolume in the OpenVDB library. First converts the current mesh to a volumetric distance field representation, then discretizes the isosurface at a given isovalue into a triangle mesh.";
	case FP_OPENVDB_LEVELSET :
		return "Remesh the current mesh using the function meshToLevelSet in the OpenVDB library. First converts the current mesh to a volumetric distance field representation, then discretizes the isosurface at a given isovalue into a triangle mesh.";
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
FilterOpenVDBPlugin::FilterClass FilterOpenVDBPlugin::getClass(const QAction *a) const
{
	switch(ID(a)) {
	case FP_OPENVDB_VOLUME :
	case FP_OPENVDB_LEVELSET :
		return FilterPlugin::Remeshing;
	default :
		assert(0);
		return FilterPlugin::Generic;
	}
}

/**
 * @brief FilterOpenVDBPlugin::filterArity
 * @return
 */
FilterPlugin::FilterArity FilterOpenVDBPlugin::filterArity(const QAction*) const
{
	return SINGLE_MESH;
}

/**
 * @brief FilterOpenVDBPlugin::getPreConditions
 * @return
 */
int FilterOpenVDBPlugin::getPreConditions(const QAction*) const
{
	return MeshModel::MM_VERTCOORD | MeshModel::MM_FACEVERT;
}

/**
 * @brief FilterOpenVDBPlugin::postCondition
 * @return
 */
int FilterOpenVDBPlugin::postCondition(const QAction*) const
{
	return MeshModel::MM_NONE; // No postconditions as the filter creates a new mesh
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
RichParameterList FilterOpenVDBPlugin::initParameterList(const QAction *action,const MeshModel &m)
{
	RichParameterList parlst;
	switch(ID(action)) {
	case FP_OPENVDB_LEVELSET :
		parlst.addParam(RichPercentage("voxelSize", m.cm.bbox.Diag()/100.0f,0.0f,m.cm.bbox.Diag(), "Voxel Size", "Size of the voxels in the grid used to represent the distance field."));
		parlst.addParam(RichPercentage ("isovalue", 0,0.0f,m.cm.bbox.Diag(), "Isovalue", "Determines the isosurface used to recompute the mesh discretization."));
		parlst.addParam(RichPercentage("adaptivity", 0,0.0f,m.cm.bbox.Diag(), "Adaptivity", "The adaptivity threshold determines how closely\nthe isosurface is matched by the resulting mesh.\nHigher thresholds will allow more variation in\npolygon size, using fewer polygons to express the surface."));
		break;
	case FP_OPENVDB_VOLUME :
		parlst.addParam(RichPercentage("voxelSize", m.cm.bbox.Diag()/100.0f,0.0f,m.cm.bbox.Diag(), "Voxel Size", "Size of the voxels in the grid used to represent the distance field."));
		parlst.addParam(RichPercentage ("isovalue", 0,0.0f,m.cm.bbox.Diag(), "Isovalue", "Determines the isosurface used to recompute the mesh discretization."));
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
std::map<std::string, QVariant> FilterOpenVDBPlugin::applyFilter(const QAction * action, const RichParameterList & parameters, MeshDocument &md, unsigned int& /*postConditionMask*/, vcg::CallBackPos *cb)
{
	switch(ID(action)) {
	case FP_OPENVDB_VOLUME :
	case FP_OPENVDB_LEVELSET :
		remesh(md, cb, parameters.getAbsPerc("voxelSize"), parameters.getAbsPerc("isovalue"),
		ID(action) == FP_OPENVDB_LEVELSET ? parameters.getAbsPerc("adaptivity") : 0.0,
		ID(action) == FP_OPENVDB_LEVELSET);
		break;
	default :
		wrongActionCalled(action);
	}
	return std::map<std::string, QVariant>();
}

bool FilterOpenVDBPlugin::remesh(
	MeshDocument &md,
	vcg::CallBackPos *cb,
	Scalarm voxelSize,
	Scalarm isovalue,
	Scalarm adaptivity,
	bool isLevelSet)
{
	// Check that the voxel size is a number greater than 0
	if(voxelSize <= std::numeric_limits<Scalarm>::epsilon()){
		throw MLException("Voxel size must be a number greater than 0.");
		return false;
	}

	CMeshO &m = md.mm()->cm;

	// Check that the mesh is watertight
    // int boundaryEdgeNum, internalEdgeNum,nonManif;
    // vcg::tri::Clean<CMeshO>::CountEdgeNum(m,internalEdgeNum,boundaryEdgeNum,nonManif);
    // if(boundaryEdgeNum>0){
	// 	throw MLException("Unable to remesh non-watertight mesh.");
	// 	return false;
	// }
	
	log(" Input mesh %8i v %8i f\n",m.VN(),m.FN());

	cb(0, "Cleaning Mesh...");

	// Mesh cleaning
	vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(m);
	vcg::tri::Allocator<CMeshO>::CompactEveryVector(m);
	vcg::tri::UpdateBounding<CMeshO>::Box(m);

	vcg::tri::OpenVDBAdapter<CMeshO> adapter;
	adapter.setVoxelSize(voxelSize);
	adapter.setIsovalue(isovalue);
	adapter.setAdaptivity(adaptivity);
	
	QElapsedTimer timer;
	timer.start();
	cb(10, "Loading Mesh...");
	adapter.setMesh(&m);
	log("Loaded mesh in %i ms", timer.elapsed());
	timer.restart();
	cb(30, "Converting Mesh to Volume...");
	if(isLevelSet){
		adapter.meshToLevelSet();
	}else{
		adapter.meshToVolume();
	}
	log("Converted mesh to volume in %i ms", timer.elapsed());
	timer.restart();
	cb(70, "Converting Volume to Mesh...");
	CMeshO &m2 = md.addNewMesh("", "Offset mesh", true)->cm;
	// md.setCurrentMesh(0);
	adapter.volumeToMesh(m2);
	log("Converted volume to mesh in %i ms", timer.elapsed());
	cb(10, "Done.");

	log("Output mesh %8i v %8i f\n",m2.VN(),m2.FN());

	vcg::tri::UpdateBounding<CMeshO>::Box(m2);
	vcg::tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m2);

	return true;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterOpenVDBPlugin)
