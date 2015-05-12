// SmartPointer.h
#include <cstdint>
#include <utility>
#include <type_traits>

class ControlBlock {
public:
	uint32_t ref_count = 0;
};


template <typename T>
class SmartPointer {
	static_assert(std::is_base_of<ControlBlock, T>::value, "You must use ControlBlock as a base class");
private:

public:
	T& operator*(void) const { return *ptr; }
	T* operator->(void) const { return ptr; }

	SmartPointer(const SmartPointer<T>& rhs) { copy(rhs); }

	SmartPointer<T>& operator=(const SmartPointer<T>& rhs) {
		if (this != &rhs) {
			destroy();
			copy(rhs);
		}
		return *this;
	}

	template <typename U>
	SmartPointer(const SmartPointer<U>& rhs) {
		//static_assert(std::is_base_of<T, U>::value, "cannot upcast smart pointers");

		if (rhs.ptr == nullptr) {
			this->ptr = nullptr;
			return;
		}

		ptr = dynamic_cast<T*>(rhs.ptr);
		ptr->ControlBlock::ref_count += 1;
	}

	SmartPointer(T* obj = nullptr) {
		ptr = obj;
		if (obj) {
			ptr->ControlBlock::ref_count += 1;
		}
	}

	~SmartPointer(void) { destroy(); }

	operator bool(void) const { return ptr; }

private:
	T* ptr = nullptr;
	template <typename U>
	friend class SmartPointer;

	void copy(const SmartPointer<T>& rhs) {
		this->ptr = rhs.ptr;
		if (ptr) {
			ptr->ControlBlock::ref_count += 1;
		}
	}

	void destroy(void) {
		if (ptr) {
			ptr->ControlBlock::ref_count -= 1;
			if (ptr->ControlBlock::ref_count == 0) {
				delete ptr;
			}
		}
	}
};

