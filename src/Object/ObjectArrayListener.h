//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_OBJECTARRAYLISTENER_H
#define MG1_ZAD2_OBJECTARRAYLISTENER_H
#include <cstddef>

namespace bf {
	class ObjectArray;
	class ObjectArrayListener {
	protected:
		ObjectArray& objectArray;
	public:
		virtual ~ObjectArrayListener();
		explicit ObjectArrayListener(ObjectArray& array);
		virtual void onRemoveObject(std::size_t index)=0;
	};
}


#endif //MG1_ZAD2_OBJECTARRAYLISTENER_H
