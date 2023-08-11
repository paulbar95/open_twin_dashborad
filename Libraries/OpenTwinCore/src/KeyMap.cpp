//! @file KeyMap.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OpenTwinCore/Logger.h"

ot::KeyMap& ot::KeyMap::operator = (const KeyMap& _other) {
	m_data = _other.m_data;
	return *this;
}

bool ot::KeyMap::contains(const std::string& _key) {
	return m_data.count(_key) > 0;
}

void ot::KeyMap::set(const std::string& _key, const std::string& _value) {
	m_mutex.lock();
	m_data.insert_or_assign(_key, _value);
	m_mutex.unlock();
}

std::string ot::KeyMap::get(const std::string& _key) {
	return m_data[_key];
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GlobalKeyMap& ot::GlobalKeyMap::instance(void) {
	static GlobalKeyMap g_instance;
	return g_instance;
}

ot::GlobalKeyMapRegistrar::GlobalKeyMapRegistrar(const std::string& _key, const std::string& _value) {
#ifdef _DEBUG
	if (ot::GlobalKeyMap::instance().contains(_key)) {
		OT_LOG_WA("Key already registered at global key map");
	}
#endif // _DEBUG
	ot::GlobalKeyMap::instance().set(_key, _value);
}