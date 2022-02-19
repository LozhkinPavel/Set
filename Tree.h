#pragma once

#include <set>
#include <cstddef>
#include <exception>
#include <algorithm>
#include <iostream>

template<class T>
class Set {
public:
    struct Node {
        T left_val, right_val;
        Node* edges[4];
        Node* parent = nullptr;
        size_t edge_cnt = 0;
        bool terminal = false;

        Node() = default;

        Node(Node* rs) {
            left_val = rs->left_val;
            right_val = rs->right_val;
            terminal = rs->terminal;
            edge_cnt = rs->edge_cnt;
            for (int i = 0; i < 4; ++i) {
                edges[i] = rs->edges[i];
            }
        }

        Node(T left, T right) : left_val(left), right_val(right) {}

        bool operator<(const Node& r) const {
            return left_val < r.left_val;
        }
    };

    class iterator {
    public:
        iterator() = default;

        explicit iterator(Node* t) : it(t) {}

        T operator*() const {
            if (it == nullptr) {
                throw std::exception();
            }
            return it->left_val;
        }

        bool operator==(const iterator& rv) const {
            return it == rv.it;
        }

        bool operator!=(const iterator& rv) const {
            return it != rv.it;
        }

        bool operator==(const Node* rv) const {
            return it == rv;
        }

        bool operator!=(const Node* rv) const {
            return it != rv;
        }

        iterator& operator=(Node* rv) {
            it = rv;
            return *this;
        }

        iterator& operator=(iterator rv) {
            it = rv.it;
            return *this;
        }

        T* operator->() {
            return &it->left_val;
        }

        iterator& operator++() {
            if (!it->terminal) {
                return *this;
            }
            Node* prev = it;
            Node* cur = it->parent;
            while (cur->edge_cnt != 0) {
                if (prev != cur->edges[cur->edge_cnt - 1]) {
                    break;
                }
                prev = cur;
                cur = cur->parent;
            }
            if (cur->edge_cnt == 0) {
                it = cur;
                return *this;
            }
            for (size_t i = 0; i < cur->edge_cnt; ++i) {
                if (cur->edges[i] == prev) {
                    cur = cur->edges[i + 1];
                    break;
                }
            }
            while (!cur->terminal) {
                cur = cur->edges[0];
            }
            it = cur;
            return *this;
        }

        iterator operator++(int) {
            ++(*this);
            return *this;
        }

        iterator& operator--() {
            if (!it->terminal) {
                Node* cur = it->edges[0];
                if (cur == nullptr) {
                    return *this;
                }
                while (!cur->terminal) {
                    cur = cur->edges[cur->edge_cnt - 1];
                }
                it = cur;
                return *this;
            }
            Node* prev = it;
            Node* cur = it->parent;
            while (cur->edge_cnt != 0) {
                if (prev != cur->edges[0]) {
                    break;
                }
                prev = cur;
                cur = cur->parent;
            }
            if (cur->edge_cnt == 0) {
                it = cur;
                return *this;
            }
            for (size_t i = 0; i < cur->edge_cnt; ++i) {
                if (cur->edges[i] == prev) {
                    cur = cur->edges[i - 1];
                    break;
                }
            }
            while (!cur->terminal) {
                cur = cur->edges[cur->edge_cnt - 1];
            }
            it = cur;
            return *this;
        }

        iterator operator--(int) {
            --(*this);
            return *this;
        }

        Node* it = nullptr;
    };

    void upd(Node* t) {
        if (t == nullptr) {
            return;
        }
        std::sort(t->edges, t->edges + t->edge_cnt, [](Node* l, Node* r) { return *l < *r; });
        t->left_val = t->edges[0]->left_val;
        t->right_val = t->edges[t->edge_cnt - 1]->right_val;
        for (int i = 0; i < t->edge_cnt; ++i) {
            t->edges[i]->parent = t;
        }
    }

    void set_new_root(Node *new_r) {
        root_ = new_r;
        if (root_ != nullptr) {
            root_->parent = end_.it;
        }
        end_.it->edges[0] = root_;
    }



    void insert(Node* t, Node* new_val) {
        if (t->terminal) {
            if (!(new_val->left_val < t->left_val) && !(t->left_val < new_val->left_val)) {
                delete new_val;
                return;
            }
            ++size_;
            if (end_ == t->parent) {
                Node* new_root = new Node;
                new_root->edge_cnt = 2;
                new_root->edges[0] = t;
                new_root->edges[1] = new_val;
                upd(new_root);
                set_new_root(new_root);
            }
            else {
                Node* p = t->parent;
                p->edges[p->edge_cnt++] = new_val;
                upd(p);
            }
            return;
        }
        if (!(t->edges[0]->left_val < new_val->left_val)) {
            insert(t->edges[0], new_val);
        }
        else {
            for (size_t i = 0; i < t->edge_cnt; ++i) {
                if (!(new_val->left_val < t->edges[t->edge_cnt - 1 - i]->left_val)) {
                    insert(t->edges[t->edge_cnt - 1 - i], new_val);
                    break;
                }
            }
        }
        upd(t);
        if (t->edge_cnt > 3) {
            Node* new_v = new Node;
            new_v->edge_cnt = 2;
            new_v->edges[0] = t->edges[2];
            new_v->edges[1] = t->edges[3];
            t->edge_cnt = 2;
            upd(t);
            upd(new_v);
            if (end_ != t->parent) {
                t->parent->edges[t->parent->edge_cnt++] = new_v;
                upd(t->parent);
            }
            else {
                Node* new_root = new Node;
                new_root->edge_cnt = 2;
                new_root->edges[0] = t;
                new_root->edges[1] = new_v;
                upd(new_root);
                set_new_root(new_root);
            }
        }
        return;
    }

    void remove_node(Node *t) {
        bool push = false;
        Node *p = t->parent;
        for (size_t i = 0; i < p->edge_cnt; ++i) {
            if (push) {
                p->edges[i - 1] = p->edges[i];
            }
            if (p->edges[i] == t) {
                push = true;
            }
        }
        --p->edge_cnt;
        delete t;
        upd(p);
    }

    void erase(Node* t, T elem) {
        if (t->terminal) {
            if ((elem < t->left_val) || (t->left_val < elem)) {
                return;
            }
            --size_;
            if (end_ == t->parent) {
                delete t;
                root_ = nullptr;
                end_.it->edges[0] = nullptr;
                begin_ = end_;
            }
            else {
                Node* p = t->parent;
                if (!(*begin_ < elem)) {
                    begin_ = p->edges[1];
                }
                remove_node(t);
            }
            return;
        }
        for (size_t i = 0; i < t->edge_cnt; ++i) {
            if (!(elem < t->edges[t->edge_cnt - 1 - i]->left_val)) {
                erase(t->edges[t->edge_cnt - 1 - i], elem);
                break;
            }
        }
        upd(t);
        if (t->edge_cnt >= 2) {
            return;
        }
        Node* p = t->parent;
        if (end_ != p) {
            size_t index;
            for (size_t i = 0; i < p->edge_cnt; ++i) {
                if (p->edges[i] == t) {
                    index = i;
                }
            }
            Node* nxt;
            if (index != 0) {
                nxt = p->edges[index - 1];
            }
            else {
                nxt = p->edges[index + 1];
                std::swap(nxt->edges[0], nxt->edges[nxt->edge_cnt - 1]);
            }

            if (nxt->edge_cnt == 3) {
                t->edges[t->edge_cnt++] = nxt->edges[2];
                nxt->edge_cnt--;
                upd(t);
                upd(nxt);
            }
            else {
                nxt->edges[nxt->edge_cnt++] = t->edges[0];
                upd(nxt);
                remove_node(t);
            }

        }
        else {
            set_new_root(t->edges[0]);
            delete t;
        }
        return;
    }

    void delet(Node* t) {
        if (t == nullptr) {
            return;
        }
        for (size_t i = 0; i < t->edge_cnt; ++i) {
            delet(t->edges[i]);
        }
        delete t;
    }

    Node* copy(Node* t, Node* begin) {
        Node* a = new Node(t);
        if (t == begin) {
            begin_ = a;
        }
        for (size_t i = 0; i < a->edge_cnt; ++i) {
            a->edges[i] = copy(t->edges[i], begin);
            a->edges[i]->parent = a;
        }
        return a;
    }

public:
    Set() : size_(0), root_(nullptr), end_(new Node) {
        begin_ = end_;
    }

    ~Set() {
        delet(root_);
        delete end_.it;
    }

    Set(const Set &rv) : size_(rv.size_), root_(nullptr), end_(new Node) {
        begin_ = end_;
        if (rv.root_ != nullptr) {
            root_ = copy(rv.root_, rv.begin_.it);
            end_.it->edges[0] = root_;
            root_->parent = end_.it;
        }
    }

    Set &operator=(const Set &rv) {
        if (rv.root_ == root_) {
            return *this;
        }
        size_ = rv.size_;
        delet(root_);
        delete end_.it;
        end_ = iterator(new Node);
        root_ = nullptr;
        begin_ = end_;
        if (rv.root_ != nullptr) {
            root_ = copy(rv.root_, rv.begin_.it);
            end_.it->edges[0] = root_;
            root_->parent = end_.it;
        }
        return *this;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) : size_(0), root_(nullptr), end_(new Node) {
        begin_ = end_;
        for (Iterator it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(std::initializer_list<T> elems) : size_(0), root_(nullptr), end_(new Node) {
        begin_ = end_;
        for (auto it = elems.begin(); it != elems.end(); ++it) {
            insert(*it);
        }
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return (size_ == 0);
    }

    void insert(const T &elem) {
        Node *new_val = new Node(elem, elem);
        new_val->terminal = true;
        if (root_ == nullptr || elem < root_->left_val) {
            begin_ = iterator(new_val);
        }
        if (root_ == nullptr) {
            root_ = new_val;
            end_.it->edges[0] = root_;
            root_->parent = end_.it;
            size_++;
            return;
        }
        insert(root_, new_val);
    }

    void erase(const T &elem) {
        if (root_ == nullptr || root_->right_val < elem || elem < root_->left_val) {
            return;
        }
        erase(root_, elem);
    }

    iterator begin() const {
        return begin_;
    }

    iterator end() const {
        return end_;
    }

    iterator find(const T &elem) const {
        iterator ans = lower_bound(elem);
        if (elem < *ans) {
            return end_;
        }
        return ans;
    }

    iterator lower_bound(const T &elem) const {
        Node *cur = root_;
        if (root_ == nullptr) {
            return end_;
        }
        if (cur->right_val < elem) {
            return end_;
        }
        while (!cur->terminal) {
            if (!(cur->edges[0]->right_val < elem)) {
                cur = cur = cur->edges[0];
                continue;
            }
            for (size_t i = 0; i < cur->edge_cnt; ++i) {
                if (cur->edges[cur->edge_cnt - 1 - i]->right_val < elem) {
                    cur = cur->edges[cur->edge_cnt - i];
                    break;
                }
            }
        }
        return iterator(cur);
    }

private:
    size_t size_ = 0;
    Node *root_ = nullptr;
    iterator begin_;
    iterator end_;
};
