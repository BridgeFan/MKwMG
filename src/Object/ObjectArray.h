//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_OBJECTARRAY_H
#define MG1_ZAD2_OBJECTARRAY_H


#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <unordered_set>
#include "ObjectArrayListener.h"
#include "Object.h"

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

namespace bf {
	class ConfigState;
	class ShaderArray;
    class MultiCursor;
	class ObjectArray {
		friend bool loadFromFile(bf::ObjectArray &objectArray, const std::string &path);
	private:
        int addToIndex = -1;
        int activeIndex = -1;
		int countActive = 0;
        int activeRedirector = -1;
		std::vector<std::pair<std::unique_ptr<bf::Object>, bool> > objects;
		std::unordered_set<bf::ObjectArrayListener*> listeners;
        glm::vec3 centre;
        void updateCentre();
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
			ptr->postInit();
			objects.emplace_back(std::move(ptr), false);
		}
		template<Derived<bf::Object> T, typename... Args>
		void addRef(Args&... args) {
			std::unique_ptr<bf::Object> ptr(new T(*this, std::forward<Args>(args)...));
			ptr->postInit();
			objects.emplace_back(std::move(ptr), false);
		}
        void setActiveRedirector(bf::Object const* redirector=nullptr);
        int getActiveRedirector() const {return activeRedirector;}
		bool toggleActive(std::size_t index);
		bool isActive(std::size_t index);
		bool setActive(std::size_t index);
		bool setUnactive(std::size_t index);
		bool isAnyActive();
		bool isMultipleActive();
		void clearSelection(std::size_t index=-1);
		glm::vec3 getCentre();
		void removeAll();
		void onMove(std::size_t index);
		bool isMovable(std::size_t index);
        int getAddToIndex() const;
        void setAddToIndex(int addToIndex);
		bool imGuiCheckChanged(std::size_t index, MultiCursor& multiCursor);
		[[nodiscard]] int getActiveIndex() const;
		void draw(bf::ShaderArray& shaderArray, const bf::ConfigState& configState);
        bool onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit mods);
        bool onKeyReleased(bf::event::Key key, bf::event::ModifierKeyBit mods);
        bool onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit mods);
        bool onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit mods);
		void onMouseMove(const glm::vec2& oldPos, const glm::vec2& newPos); //return if event should not be checked after
	};
}


#endif //MG1_ZAD2_OBJECTARRAY_H
