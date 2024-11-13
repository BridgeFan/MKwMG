//
// Created by kamil-hp on 27.03.23.
//

#include "ObjectArrayListener.h"
#include "ObjectArray.h"

bf::ObjectArrayListener::ObjectArrayListener(bf::ObjectArray &array): objectArray(array) {
	objectArray.addListener(*this);
}

bf::ObjectArrayListener::~ObjectArrayListener() {
	objectArray.removeListener(*this);
}
