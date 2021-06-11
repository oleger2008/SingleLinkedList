#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <iostream>
#include <utility>

template <typename Type>
class SingleLinkedList {
	// Узел списка
	struct Node {
		Node() = default;
		Node(const Type& val, Node* next)
			: value(val)
			, next_node(next) {
		}
		Type value = Type();
		Node* next_node = nullptr;
	};

// Шаблон класса Базовый Итератор.
// Определяет поведение итератора на элементы односвязного списка
// ValueType - совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
template <typename ValueType>
class BasicIterator {
	// Класс списка объявляется дружественным, чтобы из методов списка
	// был доступ к приватной области итератора
	friend class SingleLinkedList;

	// Конвертирующий конструктор итератора из указателя на узел списка
	explicit BasicIterator(Node* node)
	: node_(node)
	{
	}

	public:
	// Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

	// Категория итератора - forward iterator
	// (итератор, который поддерживает операции инкремента и многократное разыменование)
	using iterator_category = std::forward_iterator_tag;
	// Тип элементов, по которым перемещается итератор
	using value_type = Type;
	// Тип, используемый для хранения смещения между итераторами
	using difference_type = std::ptrdiff_t;
	// Тип указателя на итерируемое значение
	using pointer = ValueType*;
	// Тип ссылки на итерируемое значение
	using reference = ValueType&;

	BasicIterator() = default;

	// Конвертирующий конструктор/конструктор копирования
	// При ValueType, совпадающем с Type, играет роль копирующего конструктора
	// При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
	BasicIterator(const BasicIterator<Type>& other) noexcept {
		node_ = other.node_;
	}

	// Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
	// пользовательского конструктора копирования, явно объявим оператор = и
	// попросим компилятор сгенерировать его за нас.
	BasicIterator& operator=(const BasicIterator& rhs) = default;

	// Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
	// Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
	[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
		return this->node_ == rhs.node_;
	}

	// Оператор, проверки итераторов на неравенство
	// Противоположен !=
	[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
		return !(*this == rhs);
	}

	// Оператор сравнения итераторов (в роли второго аргумента итератор)
	// Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
	[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
		return this->node_ == rhs.node_;
	}

	// Оператор, проверки итераторов на неравенство
	// Противоположен !=
	[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
		return !(*this == rhs);
	}

	// Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
	// Возвращает ссылку на самого себя
	// Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
	BasicIterator& operator++() noexcept {
		this->node_ = this->node_->next_node;
		return *this;
	}

	// Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка.
	// Возвращает прежнее значение итератора
	// Инкремент итератора, не указывающего на существующий элемент списка,
	// приводит к неопределённому поведению
	BasicIterator operator++(int) noexcept {
		auto old_value(*this);
		this->node_ = this->node_->next_node;
		return old_value;
	}

	// Операция разыменования. Возвращает ссылку на текущий элемент
	// Вызов этого оператора, у итератора, не указывающего на существующий элемент списка,
	// приводит к неопределённому поведению
	[[nodiscard]] reference operator*() const noexcept {
		 return node_->value;
	}

	// Операция доступа к члену класса. Возвращает указатель на текущий элемент списка.
	// Вызов этого оператора, у итератора, не указывающего на существующий элемент списка,
	// приводит к неопределённому поведению
	[[nodiscard]] pointer operator->() const noexcept {
		return &(node_->value);
	}

private:
	Node* node_ = nullptr;
};

public:
	SingleLinkedList() = default;

	~SingleLinkedList() {
		Clear();
	}

	template<typename Iter>
	void InitializationByRange(Iter begin, Iter end) {
		for (auto& iter = begin; iter != end; ++iter) {
			this->PushFront(*iter);
		}
	}

	SingleLinkedList(std::initializer_list<Type> values) {
		InitializationByRange(std::crbegin(values), std::crend(values));
	}

	SingleLinkedList(const SingleLinkedList& other) {
		// Реализуйте конструктор самостоятельно
		// Сначала надо удостовериться, что текущий список пуст
		assert(size_ == 0 && head_.next_node == nullptr);

		SingleLinkedList reversed_temp;
		reversed_temp.InitializationByRange(other.cbegin(), other.cend());
		SingleLinkedList temp;
		temp.InitializationByRange(reversed_temp.cbegin(), reversed_temp.cend());


		// После того, как элементы скопированы, обмениваем данные текущего списка и tmp
		swap(temp);
		// Теперь tmp пуст, а текущий список содержит копию элементов other
	}

	// Вставляет элемент value в начало списка за время O(1)
	void PushFront(const Type& value) {
		head_.next_node = new Node(value, head_.next_node);
		++size_;
	}

	void PopFront() noexcept {
		auto temp = head_.next_node -> next_node;
		delete head_.next_node;
		head_.next_node = temp;
		--size_;
	}

	// Очищает список за время O(N)
	void Clear() noexcept {
		while (head_.next_node != nullptr) {
			Node* temp = head_.next_node -> next_node;
			delete head_.next_node;
			head_.next_node = temp;
		}
		size_ = 0;
	}

	// Возвращает количество элементов в списке за время O(1)
	[[nodiscard]] size_t GetSize() const noexcept {
		return size_;
	}

	// Сообщает, пустой ли список за время O(1)
	[[nodiscard]] bool IsEmpty() const noexcept {
		return size_ == 0;
	}

	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;

	// Итератор, допускающий изменение элементов списка
	using Iterator = BasicIterator<Type>;
	// Константный итератор, предоставляющий доступ для чтения к элементам списка
	using ConstIterator = BasicIterator<const Type>;

	// Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator before_begin() noexcept {
		return Iterator{&head_};
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		return ConstIterator{const_cast<Node*>(&head_)};
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator before_begin() const noexcept {
		return ConstIterator{const_cast<Node*>(&head_)};
	}

	// Возвращает итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	[[nodiscard]] Iterator begin() noexcept {
		return Iterator{head_.next_node};
	}

	// Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator end() noexcept {
		return Iterator{nullptr};
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	// Результат вызова эквивалентен вызову метода cbegin()
	[[nodiscard]] ConstIterator begin() const noexcept {
		return ConstIterator{head_.next_node};
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	// Результат вызова эквивалентен вызову метода cend()
	[[nodiscard]] ConstIterator end() const noexcept {
		return ConstIterator{nullptr};
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен cend()
	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return ConstIterator{head_.next_node};
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cend() const noexcept {
		return ConstIterator{nullptr};
	}

	/*
	 * Вставляет элемент value после элемента, на который указывает pos.
	 * Возвращает итератор на вставленный элемент
	 * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
	 */
	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		Node* insertable_element = new Node(value, pos.node_->next_node);
		pos.node_->next_node = insertable_element;
		++size_;
		return Iterator{insertable_element};
	}

	Iterator EraseAfter(ConstIterator pos) noexcept {
		auto temp = pos.node_->next_node->next_node;
		delete pos.node_->next_node;
		pos.node_->next_node = temp;
		--size_;
		return Iterator{pos.node_->next_node};
	}

	SingleLinkedList& operator=(const SingleLinkedList& rhs) {
		if (this != &rhs) {
			SingleLinkedList rhs_copy(rhs);
			this->swap(rhs_copy);
		}
		return *this;
	}

	// Обменивает содержимое списков за время O(1)
	void swap(SingleLinkedList& other) noexcept {
		std::swap(this->size_, other.size_);
		std::swap(this->head_.next_node, other.head_.next_node);
	}

private:
	// Фиктивный узел, используется для вставки "перед первым элементом"
	Node head_;
	size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(rhs > lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (rhs < lhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(rhs < lhs);
}
