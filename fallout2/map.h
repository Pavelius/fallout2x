#include "crt.h"

#pragma once

template<typename T>
class uniquelist : vector<T> {
	bool find(T v) const {
		for(auto& e : *this) {
			if(e == v)
				return true;
		}
		return false;
	}
	void add(const T v) {
		if(!find(v))
			vector<T>::add(v);
	}
};

template<typename K, typename V>
struct keyvalue {
	K key;
	V value;
};

template<typename K, typename V>
class umap : vector<keyvalue<K, V>> {
	typedef keyvalue<K, V> T;
	bool find(K k) const {
		for(auto& e : *this) {
			if(e.k == k)
				return true;
		}
		return false;
	}
	void add(const K k, const V v) {
		if(!find(k))
			vector<T>::add(T(k, v));
	}
};
