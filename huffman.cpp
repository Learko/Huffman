#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp>
#include <boost/serialization/unordered_map.hpp>
#include "serialize.hpp"

#include "huffman.hpp"

using namespace std;
using namespace boost::archive;
using boost::dynamic_bitset;


struct node {
    char c;
    uint32_t count;
    
    node *lhs, *rhs;

    ~node() {
        delete lhs;
        delete rhs;
    }
};


using huffman_tree = priority_queue<node *, vector<node *>, function<bool(const node *, const node *)>>;


auto count(ifstream &ifs) {
    unordered_map<char, uint32_t> freq_map;
    char c;

    while (ifs.get(c)) {
        ++freq_map[c];
    }

    return freq_map;
}


bool node_cmp(const node *lhs, const node *rhs) {
    return lhs->count == rhs->count ? lhs->c < rhs->c : lhs->count > rhs->count;
}


void _codes(const node *node,
            dynamic_bitset<> &code,
            unordered_map<char, dynamic_bitset<>> &encode_map) {
    if (node->c == '\0') {
        code.push_back(0); _codes(node->lhs, code, encode_map); code.pop_back();
        code.push_back(1); _codes(node->rhs, code, encode_map); code.pop_back();
    }
    else {
        encode_map[node->c] = { code };
    }
}

auto codes(const huffman_tree &pq) {
    dynamic_bitset<> code;
    unordered_map<char, dynamic_bitset<>> encode_map;

    _codes(pq.top(), code, encode_map);

    return encode_map;
}


auto build_tree(const unordered_map<char, uint32_t> &freq_map) {
    huffman_tree pq(node_cmp);

    for (auto && [ key, value ] : freq_map) {
        pq.push(new node({ key, value, nullptr, nullptr }));
    }

    while (pq.size() > 1) {
        auto lhs = pq.top(); pq.pop();
        auto rhs = pq.top(); pq.pop();

        auto root = new node({ '\0', lhs->count + rhs->count, lhs, rhs });
        pq.push(root);
    }

    return pq;
}


void encode(const string &file) {
    ifstream ifs(file);

    auto freq_map = count(ifs);
    auto pq = build_tree(freq_map);

    ifs.clear();
    ifs.seekg(0, ios::beg);

    auto encode_map = codes(pq);
    
    dynamic_bitset<> data;
    char c;

    while (ifs.get(c)) {
        for (auto i = 0u; i < encode_map[c].size(); ++i) {
            data.push_back(encode_map[c][i]);
        }
    }

    ofstream ofs(file + ".huf"s);
    binary_oarchive oa(ofs);
    oa << freq_map;
    oa << data;
}


void decode(const string &file) {
    unordered_map<char, uint32_t> freq_map;
    dynamic_bitset<> data;
    ifstream ifs(file);

    binary_iarchive ia(ifs);
    ia >> freq_map;
    ia >> data;

    auto pq = build_tree(freq_map);

    ofstream ofs(file.substr(0, file.length() - 4) + ".dec"s);

    dynamic_bitset<> code;
    node *curr = pq.top();

    for (auto i = 0u; i < data.size(); ++i) {
        curr = data[i] ? curr->rhs : curr->lhs;

        if (curr->c != '\0') {
            ofs << curr->c;
            curr = pq.top();
        }
    }
}