/*
 * @file multiRobotsWorld.cpp
 *
 *  Created on: 14/07/2016
 *      Author: pthalamy
 */

#include "multiRobotsWorld.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include "multiRobotsBlock.h"
#include "events.h"
#include "configExporter.h"
#include "trace.h"

using namespace std;

namespace MultiRobots {

MultiRobotsWorld::MultiRobotsWorld(const Cell3DPosition &gridSize, const Vector3D &gridScale,
									 int argc, char *argv[]):World(argc, argv) {
	OUTPUT << "\033[1;31mMultiRobotsWorld constructor\033[0m" << endl;

    // PTHY: INCONSISTENCY
	if (GlutContext::GUIisEnabled) {
		objBlock = new ObjLoader::ObjLoader("../../simulatorCore/blinkyBlocksTextures",
                                            "blinkyBlockCentered.obj");
		objBlockForPicking = new ObjLoader::ObjLoader("../../simulatorCore/blinkyBlocksTextures",
													  "blinkyBlockPickingCentered.obj");
		objRepere = new ObjLoader::ObjLoader("../../simulatorCore/smartBlocksTextures",
                                             "repere25.obj");
	}

	lattice = new SLattice(gridSize, gridScale.hasZero() ? defaultBlockSize : gridScale);
}

MultiRobotsWorld::~MultiRobotsWorld() {
	OUTPUT << "MultiRobotsWorld destructor" << endl;
}

void MultiRobotsWorld::deleteWorld() {
	delete((MultiRobotsWorld*)world);
}

void MultiRobotsWorld::addBlock(int blockId, BlockCodeBuilder bcb,
								 const Cell3DPosition &pos, const Color &col,
								 short orientation, bool master) {
	if (blockId > maxBlockId)
		maxBlockId = blockId;
	else if (blockId == -1)
		blockId = incrementBlockId();
		
	MultiRobotsBlock *mrb = new MultiRobotsBlock(blockId, bcb);
	buildingBlocksMap.insert(std::pair<int,BaseSimulator::BuildingBlock*>
							 (mrb->blockId, (BaseSimulator::BuildingBlock*)mrb));
	getScheduler()->schedule(new CodeStartEvent(getScheduler()->now(), mrb));

	MultiRobotsGlBlock *glBlock = new MultiRobotsGlBlock(blockId);
	tabGlBlocks.push_back(glBlock);
	mrb->setGlBlock(glBlock);
	mrb->setPosition(pos);
	mrb->setColor(col);

	if (lattice->isInGrid(pos)) {
		lattice->insert(mrb, pos);
	} else {
		ERRPUT << "ERROR : BLOCK #" << blockId << " out of the grid !!!!!" << endl;
		exit(1);
	}
}

void MultiRobotsWorld::linkBlock(const Cell3DPosition &pos) {
	// MultiRobotsBlock *ptrNeighbor;
	// MultiRobotsBlock *ptrBlock = (MultiRobotsBlock*)lattice->getBlock(pos);
	// vector<Cell3DPosition> nCells = lattice->getNeighborhood(pos);

    // ptrBlock->addEdge(another);
    // ptrBlock->connected.push_back(another);
    
    // PTHY: TODO
	// // Check neighbors for each interface
	// for (int i = 0; i < 6; i++) {
	// 	ptrNeighbor = (MultiRobotsBlock*)lattice->getBlock(nCells[i]);
	// 	if (ptrNeighbor) {
	// 		(ptrBlock)->getInterface(i)->
	// 			connect(ptrNeighbor->getInterface(lattice->getOpposite(i)));

	// 		OUTPUT << "connection #" << (ptrBlock)->blockId <<
	// 			" to #" << ptrNeighbor->blockId << endl;
	// 	} else {
	// 		(ptrBlock)->getInterface(i)->connect(NULL);
	// 	}
	// }
}

void MultiRobotsWorld::glDraw() {
	static const GLfloat white[]={0.8f,0.8f,0.8f,1.0f},
		gray[]={0.2f,0.2f,0.2f,1.0};

		glPushMatrix();
		glTranslatef(0.5*lattice->gridScale[0],0.5*lattice->gridScale[1],0.5*lattice->gridScale[2]);
		// glTranslatef(0.5*lattice->gridScale[0],0.5*lattice->gridScale[1],0);
		glDisable(GL_TEXTURE_2D);
		vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
		lock();
		while (ic!=tabGlBlocks.end()) {
			((MultiRobotsGlBlock*)(*ic))->glDraw(objBlock);
			ic++;
		}
		unlock();

		glPopMatrix();
		glMaterialfv(GL_FRONT,GL_AMBIENT,gray);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
		glMaterialfv(GL_FRONT,GL_SPECULAR,gray);
		glMaterialf(GL_FRONT,GL_SHININESS,40.0);
		glPushMatrix();
		enableTexture(true);
		glBindTexture(GL_TEXTURE_2D,idTextureWall);
		glScalef(lattice->gridSize[0]*lattice->gridScale[0],
				 lattice->gridSize[1]*lattice->gridScale[1],
				 lattice->gridSize[2]*lattice->gridScale[2]);
		glBegin(GL_QUADS);
		// bottom
		glNormal3f(0,0,1.0f);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(lattice->gridSize[0]/4.0f,0);
		glVertex3f(1.0f,0.0f,0.0f);
		glTexCoord2f(lattice->gridSize[0]/4.0f,lattice->gridSize[1]/4.0f);
		glVertex3f(1.0,1.0,0.0f);
		glTexCoord2f(0,lattice->gridSize[1]/4.0f);
		glVertex3f(0.0,1.0,0.0f);
		// top
		glNormal3f(0,0,-1.0f);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0,lattice->gridSize[1]/4.0f);
		glVertex3f(0.0,1.0,1.0f);
		glTexCoord2f(lattice->gridSize[0]/4.0f,lattice->gridSize[1]/4.0f);
		glVertex3f(1.0,1.0,1.0f);
		glTexCoord2f(lattice->gridSize[0]/4.0f,0);
		glVertex3f(1.0f,0.0f,1.0f);
		// left
		glNormal3f(1.0,0,0);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(lattice->gridSize[1]/4.0f,0);
		glVertex3f(0.0f,1.0f,0.0f);
		glTexCoord2f(lattice->gridSize[1]/4.0f,lattice->gridSize[2]/4.0f);
		glVertex3f(0.0,1.0,1.0f);
		glTexCoord2f(0,lattice->gridSize[2]/4.0f);
		glVertex3f(0.0,0.0,1.0f);
		// right
		glNormal3f(-1.0,0,0);
		glTexCoord2f(0,0);
		glVertex3f(1.0f,0.0f,0.0f);
		glTexCoord2f(0,lattice->gridSize[2]/4.0f);
		glVertex3f(1.0,0.0,1.0f);
		glTexCoord2f(lattice->gridSize[1]/4.0f,lattice->gridSize[2]/4.0f);
		glVertex3f(1.0,1.0,1.0f);
		glTexCoord2f(lattice->gridSize[1]/4.0f,0);
		glVertex3f(1.0f,1.0f,0.0f);
		// back
		glNormal3f(0,-1.0,0);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,1.0f,0.0f);
		glTexCoord2f(lattice->gridSize[0]/4.0f,0);
		glVertex3f(1.0f,1.0f,0.0f);
		glTexCoord2f(lattice->gridSize[0]/4.0f,lattice->gridSize[2]/4.0f);
		glVertex3f(1.0f,1.0,1.0f);
		glTexCoord2f(0,lattice->gridSize[2]/4.0f);
		glVertex3f(0.0,1.0,1.0f);
		// front
		glNormal3f(0,1.0,0);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(0,lattice->gridSize[2]/4.0f);
		glVertex3f(0.0,0.0,1.0f);
		glTexCoord2f(lattice->gridSize[0]/4.0f,lattice->gridSize[2]/4.0f);
		glVertex3f(1.0f,0.0,1.0f);
		glTexCoord2f(lattice->gridSize[0]/4.0f,0);
		glVertex3f(1.0f,0.0f,0.0f);
		glEnd();
		glPopMatrix();
		// draw the axes
		objRepere->glDraw();
}

void MultiRobotsWorld::glDrawId() {
	glPushMatrix();
	glTranslatef(0.5*lattice->gridScale[0],0.5*lattice->gridScale[1],0);
	glDisable(GL_TEXTURE_2D);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	int n=1;
	lock();
	while (ic!=tabGlBlocks.end()) {
		((MultiRobotsGlBlock*)(*ic))->glDrawId(objBlock,n);
		ic++;
	}
	unlock();
	glPopMatrix();
}

void MultiRobotsWorld::glDrawIdByMaterial() {
	glPushMatrix();
	glTranslatef(0.5*lattice->gridScale[0],0.5*lattice->gridScale[1],0);

	glDisable(GL_TEXTURE_2D);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	int n=1;
	lock();
	while (ic!=tabGlBlocks.end()) {
		((MultiRobotsGlBlock*)(*ic))->glDrawIdByMaterial(objBlockForPicking,n);
		ic++;
	}
	unlock();
	glPopMatrix();
}


void MultiRobotsWorld::loadTextures(const string &str) {
	string path = str+"/texture_plane.tga";
	int lx,ly;
	idTextureWall = GlutWindow::loadTexture(path.c_str(),lx,ly);
}

void MultiRobotsWorld::setSelectedFace(int n) {
    //PTHY: INCONSISTENCY
    
	// numSelectedGlBlock=n/6;
	// string name = objBlockForPicking->getObjMtlName(n%6);

	// if (name=="_blinkyBlockPickingface_top") numSelectedFace=SCLattice::Top;
	// else if (name=="_blinkyBlockPickingface_bottom") numSelectedFace=SCLattice::Bottom;
	// else if (name=="_blinkyBlockPickingface_right") numSelectedFace=SCLattice::Right;
	// else if (name=="_blinkyBlockPickingface_left") numSelectedFace=SCLattice::Left;
	// else if (name=="_blinkyBlockPickingface_front") numSelectedFace=SCLattice::Front;
	// else if (name=="_blinkyBlockPickingface_back") numSelectedFace=SCLattice::Back;
	// else {
	// 	cerr << "warning: Unrecognized picking face" << endl;
	// 	numSelectedFace = 7;	// UNDEFINED
	// }
}

void MultiRobotsWorld::exportConfiguration() {
    // PTHY: INCONSISTENCY
	// MultiRobotsConfigExporter *exporter = new MultiRobotsConfigExporter(this);
	// exporter->exportConfiguration();
	// delete exporter;
}

} // MultiRobots namespace
