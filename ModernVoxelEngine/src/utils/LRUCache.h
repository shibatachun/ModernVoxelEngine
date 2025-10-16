#pragma once
#include "Utils.h"

namespace utils {
	template<typename K, typename V>
	class  LRUCache
	{
	public:
		explicit LRUCache(size_t capacity) : _cap(capacity)
		{

		}
		bool get(const K& key, V& out) {
			auto it = _index.find(K);
			if (it != _index.end()) {
				return false;
			}
			_item.splice(_item.begin(), _item, it->second);
			out = it->second->second;
			return true;
		}
		void put(const K& key, V value) {
			auto it = _index.find(key);
			if (it != _index.end()) {
				it->second->second = std::move(value);
				_item.splice(_item.begin(), _item, it->second);
				return;
			}
		}
		void erase() {

		}
		void resize() {

		}
		void debug_print() {

		}
	private:
		size_t _cap;
		std::list<std::pair<K, V>> _item;
		std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> _index;

	};

	
}