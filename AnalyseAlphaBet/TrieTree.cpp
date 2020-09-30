#include <iostream>
#include <functional>
#include <utility>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <queue>
#include "stack"

using namespace std;

template <class T, class S, class C>
S& Container(priority_queue<T, S, C>& q) {
    struct HackedQueue : private priority_queue<T, S, C> {
        static S& Container(priority_queue<T, S, C>& q) {
            return q.*&HackedQueue::c;
        }
    };
    return HackedQueue::Container(q);
}

// A Trie node
struct Trie
{
    // true when node is a leaf node
    bool isLeaf;
    vector<int> files{0,0,0,0,0};
    string code_word;
    int level;
    int score;

    // each node stores a map to its child nodes
    unordered_map<char, Trie*> map;
    Trie* parent;

};

static int returnScore(const vector<int>& files, int level){
    int sum = 0;
    for(int i : files){
        if (i == 0)
            return 0;
        sum += i;
    }
    int avg = sum/files.size();
    return avg * level;
}


struct OrderByScore
{
    bool operator() (Trie const &a, Trie const &b) {
        return a.score < b.score;
    }
};



// Function that returns a new Trie node
Trie* getNewTrieNode()
{
    Trie* node = new Trie;
    node->isLeaf = false;

    return node;
}

// Iterative function to insert a string in Trie.
void insert(Trie*& head, char* str, const int& file_number)
{
    if (head == nullptr)
        head = getNewTrieNode();

    // start from root node
    Trie* curr = head;

    while (*str)
    {
        // create a new node if path doesn't exists
        if (curr->map.find(*str) == curr->map.end())
            curr->map[*str] = getNewTrieNode();

        // go to next node
        curr = curr->map[*str];
        curr->files[file_number] ++;
        // move to next character
        str++;
    }

    // mark current node as leaf
    curr->isLeaf = true;
}

// returns true if given node has any children
bool haveChildren(Trie const* curr)
{
    // don't use (curr->map).size() to check for children

    for (auto it : curr->map)
        if (it.second != nullptr)
            return true;

    return false;
}

// Recursive function to delete a string in Trie.
bool deletion(Trie*& curr, char* str)
{
    // return if Trie is empty
    if (curr == nullptr)
        return false;

    // if we have not reached the end of the string
    if (*str)
    {
        // recur for the node corresponding to next character in
        // the string and if it returns true, delete current node
        // (if it is non-leaf)
        if (curr != nullptr &&  curr->map.find(*str) != curr->map.end() &&
            deletion(curr->map[*str], str + 1) && curr->isLeaf == false)
        {
            if (!haveChildren(curr))
            {
                delete curr;;
                curr = nullptr;
                return true;
            }
            else {
                return false;
            }
        }
    }

    // if we have reached the end of the string
    if (*str == '\0' && curr->isLeaf)
    {
        // if current node is a leaf node and don't have any children
        if (!haveChildren(curr))
        {
            delete curr;; // delete current node
            curr = nullptr;
            return true; // delete non-leaf parent nodes
        }

            // if current node is a leaf node and have children
        else
        {
            // mark current node as non-leaf node (DON'T DELETE IT)
            curr->isLeaf = false;
            return false;	   // don't delete its parent nodes
        }
    }

    return false;
}

// Iterative function to search a string in Trie. It returns true
// if the string is found in the Trie, else it returns false
bool search(Trie* head, char* str)
{
    // return false if Trie is empty
    if (head == nullptr)
        return false;

    Trie* curr = head;
    while (*str)
    {
        // go to next node
        curr = curr->map[*str];

        // if string is invalid (reached end of path in Trie)
        if (curr == nullptr)
            return false;

        // move to next character
        str++;
    }

    // if current node is a leaf and we have reached the
    // end of the string, return true
    return curr->isLeaf;
}

void traverseTree(const Trie &node){
        stack<Trie> trie_stack;
        priority_queue<Trie, vector<Trie>, OrderByScore> trie_pq;
        Trie head(node);
        trie_stack.push(head);
        head.code_word = "";
        head.level = 0;
        head.parent = nullptr;
        while (!trie_stack.empty()) {
            Trie tmp = trie_stack.top();
            trie_stack.pop();
            for (auto child : tmp.map) {
                child.second->parent = &tmp;
                child.second->level = child.second->parent->level + 1;
                child.second->code_word = tmp.code_word + child.first;
                child.second->score = returnScore(child.second->files, child.second->level);
                trie_stack.push(*child.second);
            }
            if (tmp.level > 1 && tmp.score != 0)
                trie_pq.push(tmp);
        }

/*        vector<Trie> &top_results = Container(trie_pq);
        int i = 20;
        for(const auto& trie : top_results){
            if(i == 0)
                break;
            cout << "level: " << trie.level << " score: " << trie.score << endl;
            cout << "string: " << trie.code_word << endl;
            for(auto i : trie.files)
                cout << i << " ";
            cout << "\n" << endl;
            i--;
        }*/
    }

// Memory efficient Trie Implementation in C++ using Map
    int main()
    {
        string files [] = {"binarySerach.out", "BST.out", "bubbleSort.out", "matrixMultiply.out", "palindrom.out"};
        Trie* head = nullptr;
        std :: vector<int> data;
        int number_file = 0;
        for (const auto &file : files){
            try {
                string input = "/Users/itaytu/CLionProjects/Data-Compression/files/" + file;
                ifstream in(input, ios::binary);
                while(true) {
                    int b = in.get();
                    if(b == EOF)
                        break;
                    data.push_back(b);
                }
                in.close();
            }
            catch (const std :: exception& e){
                std :: cout << e.what() << std :: endl;
            }

            for (int i = 0 ; i < data.size() - 16 ; i ++){
                string s = "";
                for (int j = i; j < 16 + i ; j ++){
                    s += data[j];
                }
                insert(head, const_cast<char*>(s.c_str()), number_file);
            }
            data.clear();
            number_file ++;
        }

        traverseTree(*head);

        return 0;
    }