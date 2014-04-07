/*	This part is copied in full from the RPG_Game Plugin as it is
	a very useful way to do it and trying to rewrite this parts or
	reproduce it in my own code does not make any sense */
#pragma once
#include <Vision/Runtime/Engine/System/Vision.hpp>

namespace kSerializer
{
	//This function writes to the archive
	void WriteVariableList(VTypedObject const *typedObj, VArchive& ar);
	//This function read from the archive
	void ReadVariableList(VTypedObject *typedObj, VArchive & ar);
}
