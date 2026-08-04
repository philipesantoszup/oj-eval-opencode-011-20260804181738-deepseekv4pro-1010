#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct Node {
		T value;
		Node *left;
		Node *right;
		size_t dist;

		Node(const T &v) : value(v), left(nullptr), right(nullptr), dist(1) {}
	};

	Node *root;
	size_t sz;
	Compare cmp;

	void clear(Node *node) {
		if (node) {
			clear(node->left);
			clear(node->right);
			delete node;
		}
	}

	Node* copy(Node *node) {
		if (!node) return nullptr;
		Node *new_node = new Node(node->value);
		new_node->left = copy(node->left);
		new_node->right = copy(node->right);
		new_node->dist = node->dist;
		return new_node;
	}

	Node* merge(Node *a, Node *b) {
		if (!a) return b;
		if (!b) return a;

		if (cmp(a->value, b->value)) {
			Node *tmp = a;
			a = b;
			b = tmp;
		}

		Node *old_right = a->right;
		Node *merged;
		try {
			merged = merge(a->right, b);
		} catch (...) {
			a->right = old_right;
			throw;
		}

		a->right = merged;

		if (!a->left || (a->right && a->left->dist < a->right->dist)) {
			Node *tmp = a->left;
			a->left = a->right;
			a->right = tmp;
		}
		a->dist = (a->right ? a->right->dist : 0) + 1;
		return a;
	}

public:
	priority_queue() : root(nullptr), sz(0) {}

	priority_queue(const priority_queue &other) : root(nullptr), sz(other.sz) {
		root = copy(other.root);
	}

	~priority_queue() {
		clear(root);
	}

	priority_queue &operator=(const priority_queue &other) {
		if (this == &other) return *this;
		clear(root);
		root = copy(other.root);
		sz = other.sz;
		return *this;
	}

	const T & top() const {
		if (empty()) throw container_is_empty();
		return root->value;
	}

	void push(const T &e) {
		Node *new_node = new Node(e);
		try {
			root = merge(root, new_node);
		} catch (...) {
			delete new_node;
			throw runtime_error();
		}
		++sz;
	}

	void pop() {
		if (empty()) throw container_is_empty();
		Node *left = root->left;
		Node *right = root->right;
		Node *old_root = root;
		try {
			root = merge(left, right);
		} catch (...) {
			root = old_root;
			root->left = left;
			root->right = right;
			throw runtime_error();
		}
		delete old_root;
		--sz;
	}

	size_t size() const {
		return sz;
	}

	bool empty() const {
		return sz == 0;
	}

	void merge(priority_queue &other) {
		if (this == &other) return;
		if (other.empty()) return;

		Node *saved_my_root = root;
		Node *saved_other_root = other.root;

		try {
			root = merge(root, other.root);
		} catch (...) {
			root = saved_my_root;
			other.root = saved_other_root;
			throw runtime_error();
		}

		sz += other.sz;
		other.root = nullptr;
		other.sz = 0;
	}
};

}

#endif
