//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_OBJECTARRAY_H
#define MG1_ZAD2_OBJECTARRAY_H


#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <unordered_set>
#include "src/Object/ObjectArrayListener.h"

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

namespace bf {
	class Object;
	class Settings;
	class Shader;
	class ObjectArray {
	private:
		int activeIndex = -1;
		int countActive = 0;
		std::vector<std::pair<std::unique_ptr<bf::Object>, bool> > objects;
		std::unordered_set<bf::ObjectArrayListener*> listeners;
	public:
		[[nodiscard]] std::size_t size() const {return objects.size();}
		bf::Object& operator[](std::size_t index);
		const bf::Object& operator[](std::size_t index) const;
		bf::Object* getPtr(std::size_t index) {return objects[index].first.get();}
		[[nodiscard]] bool isCorrect(std::size_t index) const;
		void add(bf::Object* object);
		bool remove(std::size_t index);
		void removeActive();
		void addListener(bf::ObjectArrayListener& listener);
		void removeListener(bf::ObjectArrayListener& listener);

		template<Derived<bf::Object> T>
		void add() {
			std::unique_ptr<bf::Object> ptr(new T());
			objects.emplace_back(std::move(ptr), false);
		}
		template<Derived<bf::Object> T>
		void add(T* ptr) {
			//pointer to the object CANNOT be owned by any smart pointer
			objects.emplace_back(std::move(ptr), false);
		}
		template<Derived<bf::Object> T, typename... Args>
		void add(Args&... args) {
			std::unique_ptr<bf::Object> ptr(new T(std::forward<Args>(args)...));
			objects.emplace_back(std::move(ptr), false);
		}
		template<Derived<bf::Object> T, typename... Args>
		void addRef(Args&... args) {
			std::unique_ptr<bf::Object> ptr(new T(*this, std::forward<Args>(args)...));
			objects.emplace_back(std::move(ptr), false);
		}
		bool toggleActive(std::size_t index);
		bool isActive(std::size_t index);
		bool setActive(std::size_t index);
		bool setUnactive(std::size_t index);
		bool isAnyActive();
		bool isMultipleActive();
		void clearSelection(std::size_t index=-1);
		glm::vec3 getCentre();
		void onMove(std::size_t index);
		bool isMovable(std::size_t index);
		bool imGuiCheckChanged(std::size_t index);
		[[nodiscard]] int getActiveIndex() const;
		void draw(bf::Shader& shader);
	};
}


#endif //MG1_ZAD2_OBJECTARRAY_H
