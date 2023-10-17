#pragma once
#include "OpenTwinCore/Owner.h"

namespace ot
{
	//! @brief Manage objects that have an owner
	template<class K, class V> 
	class __declspec(dllexport) OwnerManagerTemplate {
	public:
		OwnerManagerTemplate() {};
		virtual ~OwnerManagerTemplate() {};

		//! @brief Store the provided object for the given owner
		//! The manager takes ownership of the object
		void store(const ot::Owner<K>& _owner, V* _obj) { this->objectList(_owner).push_back(_obj); };

		//! @brief Will clean up the memory while removing all entries
		void free(void) {
			for (auto it : this->m_data) {
				for (auto obj : *it.second) {
					if (obj) delete obj;
				}
				delete it.second;
			}
			this->m_data.clear();
		}

		//! @brief Clean up the memory for the given owner and remove its entries
		//! @param _owner The object owner
		void free(const ot::Owner<K>& _owner) {
			auto it = this->m_data.find(_owner);
			if (it != this->m_data.end()) {
				for (auto obj : *it->second) delete obj;
				delete it->second;

				this->remove(_owner);
			}
		}

		//! @brief Remove all entries for the given owner
		//! Callee takes ownership of affected objects
		//! @param _owner The object owner
		void remove(const ot::Owner<K>& _owner) { this->m_data.erase(_owner); };

		//! @brief Remove all entries for all owners
		void removeAll(void) {
			for (auto it : this->m_data) { delete it.second; }
			this->m_data.clear();
		}

		//! @brief Returns true if there exists an entry for the provided owner
		bool contains(const ot::Owner<K>& _owner) { return this->m_data.count(_owner) > 0; };

		//! @brief Return the owner data
		std::list<V*>& operator[](const ot::Owner<K>& _owner) { return this->objectList(_owner); };

		ot::Owner<K> findOwner(V* _obj) {
			for (auto it : this->m_data) {
				for (auto e : *it.second) {
					if (e == _obj) return it.first;
				}
			}
			OT_LOG_WA("Owner not found");
			throw std::exception("Owner not found");
		}

	private:

		//! @brief Return object list for the given owner
		std::list<V*>& objectList(const ot::Owner<K>& _owner) {
			auto it = this->m_data.find(_owner);
			if (it == this->m_data.end()) {
				std::list<V*>* newList = new std::list<V*>;
				this->m_data.insert_or_assign(_owner, newList);
				return *newList;
			}
			else {
				return *it->second;
			}
		};

		//! @brief Owner -> ObjectList map
		std::map<ot::Owner<K>, std::list<V*>*> m_data;

	};

	template <class V> class ServiceOwnerManagerTemplate : public OwnerManagerTemplate<ot::serviceID_t, V> {};
}
