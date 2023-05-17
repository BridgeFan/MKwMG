#pragma once
//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_OBJECTARRAY_H
#define MG1_ZAD2_OBJECTARRAY_H


#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include "ObjectArrayListener.h"
#include "Object.h"
#include <map>

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

namespace bf {
	struct ConfigState;
	struct ShaderArray;
	class MultiCursor;
	class ObjectArray {
	private:
        int addToIndex = -1;
        int activeIndex = -1;
		int countActive = 0;
        int activeRedirector = -1;
		std::vector<std::pair<std::unique_ptr<bf::Object>, bool> > objects;
		std::map<intptr_t, bf::ObjectArrayListener*> listeners;
        glm::vec3 centre;
        void updateCentre();
		friend bool loadFromFile(bf::ObjectArray& objectArray, const std::string& path);
	public:
		~ObjectArray();
        bool isForcedActive=false;
		[[nodiscard]] unsigned size() const {return static_cast<unsigned>(objects.size());}
		bf::Object& operator[](unsigned index);
		const bf::Object& operator[](unsigned index) const;
		bf::Object* getPtr(unsigned index) {return objects[index].first.get();}
		[[nodiscard]] bool isCorrect(unsigned index) const;
		void add(bf::Object* object);
		bool remove(unsigned index);
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
            clearSelection(-1);
            activeIndex=static_cast<int>(objects.size());
			objects.emplace_back(std::move(ptr), true);
		}
        void setActiveRedirector(bf::Object const* redirector=nullptr);
        int getActiveRedirector() const {return activeRedirector;}
		bool toggleActive(unsigned index);
		bool isActive(unsigned index);
		bool setActive(unsigned index);
		bool setUnactive(unsigned index);
		[[nodiscard]] bool isAnyActive() const;
		[[nodiscard]] bool isMultipleActive() const;
		void clearSelection(int index=-1);
		glm::vec3 getCentre();
		void removeAll();
		void onMove(unsigned index);
		bool isMovable(unsigned index);
        int getAddToIndex() const;
        void setAddToIndex(int addToIndex);
		bool imGuiCheckChanged(unsigned index, MultiCursor& multiCursor);
		[[nodiscard]] int getActiveIndex() const;
		void draw(bf::ShaderArray& shaderArray, const bf::ConfigState& configState);
        bool onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit mods);
        bool onKeyReleased(bf::event::Key key, bf::event::ModifierKeyBit mods);
        bool onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit mods);
        bool onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit mods);
		void onMouseMove(const glm::vec2& oldPos, const glm::vec2& newPos, const bf::ConfigState& configState,
                         const glm::mat4& view, const glm::mat4& projection); //return if event should not be checked after
	};
}


#endif //MG1_ZAD2_OBJECTARRAY_H
