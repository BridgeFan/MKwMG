//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_OBJECTARRAY_H
#define MG1_ZAD2_OBJECTARRAY_H


#include <memory>
#include <vector>
#include <glm/vec3.hpp>

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

namespace bf {
	class Object;
	class Settings;
	class ObjectArray {
	private:
		std::vector<std::pair<std::unique_ptr<bf::Object>, bool> > objects;
	public:
		[[nodiscard]] std::size_t size() const {return objects.size();}
		bf::Object& operator[](std::size_t index);
		const bf::Object& operator[](std::size_t index) const;
		[[nodiscard]] bool isCorrect(std::size_t index) const;
		bf::Object* getObjectPtr(std::size_t index);
		[[nodiscard]] const bf::Object* getObjectPtr(std::size_t index) const;
		void add(bf::Object* object);
		bool remove(std::size_t index);
		void removeActive();

		template<Derived<bf::Object> T>
		void add() {
			std::unique_ptr<bf::Object> ptr(new T());
			objects.emplace_back(std::move(ptr), false);
		}
		template<Derived<bf::Object> T, typename... Args>
		void add(Args... args) {
			std::unique_ptr<bf::Object> ptr(new T(std::forward<Args>(args)...));
			objects.emplace_back(std::move(ptr), false);
		}
		bool toggleActive(std::size_t index);
		bool isActive(std::size_t index);
		bool isAnyActive();
		void clearSelection(std::size_t index, Settings& settings);
		void clearSelection(Settings& settings);
		glm::vec3 getCentre();
	};
}


#endif //MG1_ZAD2_OBJECTARRAY_H
