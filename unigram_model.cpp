//
// Created by hetricke on 3/1/24.
//
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct Unigram{
    int word_code = -1;
    string word;
    int doc_count = 1;
    int global_count = 1;


};

struct CompareCodeUnigrams{

    bool operator()(const Unigram &left, const Unigram &right){
        return left.word_code <= right.word_code;
    }

    bool operator()(const Unigram &left, const int &right){
        return left.word_code <= right;
    }

    bool operator()(const int &left, const Unigram &right){
        return left <= right.word_code;
    }


};

struct CompareCountUnigrams{

    bool operator()(const Unigram &left, const Unigram &right){
        return left.global_count <= right.global_count;
    }

    bool operator()(const Unigram &left, const int &right){
        return left.global_count <= right;
    }

    bool operator()(const int &left, const Unigram &right){
        return left <= right.global_count;
    }
};

vector<string> alphabet;
vector<int> alphabet_count;

vector<vector<string>> dictionary;

vector<Unigram> unigram_word_code;
vector<Unigram> unigram_global_count;


string stemWord(string &word){

    return "";
}

int updateAlphabetCount(const string letter){

    auto search_result = lower_bound(alphabet.begin(), alphabet.end(), letter);

    if(search_result == alphabet.end()){
        alphabet.push_back(letter);
        alphabet_count.push_back(1);
        return alphabet.size()-1;
    }

    int index = distance(alphabet.begin(), search_result);
    if (alphabet[index] == letter){
        alphabet_count[index] +=1;
        return index;
    }
    else{
        alphabet.insert(search_result, letter);
        alphabet_count.insert(alphabet_count.begin()+index, 1);

        vector<string> new_dict;
        dictionary.insert(dictionary.begin()+index, new_dict);

        return index;
    }

}

int getWordCode(string word){
    auto letter_search = lower_bound(alphabet.begin(), alphabet.end(), word.substr(0,1));
    int letter_index = distance(alphabet.begin(), letter_search);

    auto word_search = lower_bound(dictionary[letter_index].begin(), dictionary[letter_index].end(), word);
    int word_index = distance(dictionary[letter_index].begin(), word_search);


    int word_code = 0;
    for (int i = 0; i<letter_index; i++){
        word_code += alphabet_count[i];
    }

    return word_code+word_index;
}

void updateDictionary(string word){

    int letter_index = updateAlphabetCount(word.substr(0,1));

    auto search_result = lower_bound(dictionary[letter_index].begin(), dictionary[letter_index].end(), word);

    if (search_result == dictionary[letter_index].end()){
        dictionary[letter_index].push_back(word);
        return;
    }

    int index = std::distance(dictionary[letter_index].begin(), search_result);

    if (dictionary[letter_index][index] == word){
        return;
    }
    else{
        dictionary[letter_index].insert(search_result, word);
        return;
    }
}

void writeOutDictionary(){
    ofstream dictionary_file ("dictionary.txt");

    for(int i = 0; i<dictionary.size(); i++){
        for(int j = 0; j<dictionary[i].size(); j++){
            dictionary_file << dictionary[i][j] << endl;
        }
    }

    dictionary_file.close();
}

void updateUnigram(const string word, bool new_doc){
    int word_code = getWordCode(word);

    auto word_code_search = lower_bound(unigram_word_code.begin(), unigram_word_code.end(), word_code, CompareCodeUnigrams());

    if (word_code_search == unigram_word_code.end()){
        Unigram u;
        u.word_code = word_code;
        u.word = word;
        u.doc_count = 1;
        u.global_count = 1;

        unigram_word_code.push_back(u);
        unigram_global_count.push_back(u);
        return;
    }

    int code_index = std::distance(unigram_word_code.begin(), word_code_search);

    if (unigram_word_code[code_index].word_code != word_code){
        Unigram u;
        u.word_code = word_code;
        u.word = word;
        u.doc_count = 1;
        u.global_count = 1;

        unigram_word_code.insert(word_code_search, u);
        unigram_global_count.push_back(u);
        return;
    }
    else{

        Unigram u = unigram_word_code[code_index];
        if (new_doc){
            u.doc_count++;
        }

        auto count_loc_search = lower_bound(unigram_global_count.begin(), unigram_global_count.end(), u.global_count, CompareCountUnigrams());
        auto count_new_loc_search = lower_bound(unigram_global_count.begin(), unigram_global_count.end(), u.global_count+1, CompareCountUnigrams());

        int count_index = distance(unigram_global_count.begin(), count_loc_search);
        while(unigram_global_count[count_index].global_count == u.global_count){

            if (unigram_global_count[count_index].word_code == u.word_code){
                unigram_global_count.erase(unigram_global_count.begin()+count_index);
                break;
            }

            count_index++;
        }

        u.global_count++;
        unigram_global_count.insert(count_new_loc_search, u);
        return;
    }
}

void writeOutUnigrams(){
    ofstream unigrams_file ("unigrams.txt");

    for(int i = 0; i<unigram_global_count.size(); i++){
        string unigram_output = to_string(unigram_global_count[i].word_code) + " ";
        unigram_output += unigram_global_count[i].word + " ";
        unigram_output += to_string(unigram_global_count[i].doc_count) + " ";
        unigram_output += to_string(unigram_global_count[i].global_count) + " ";

        unigrams_file << unigram_output << endl;
    }

    unigrams_file.close();
}

int main(){


    ifstream datafile("tiny_wikipedia.txt");
    string article;

    int breaker = 0;
    while(getline(datafile, article)){
        int word_tracker = 0;
        while (word_tracker != -1){

            int next_space = article.find(" ", word_tracker);
            int word_length = (next_space != -1) ? next_space-word_tracker : next_space;
            updateDictionary(article.substr(word_tracker+1, word_length));
            word_tracker = next_space;

        }

        breaker++;

        if(breaker > 3){
            break;
        }
    }

    datafile.close();
    writeOutDictionary();

    breaker = 0;

    datafile.open("tiny_wikipedia.txt");

    while(getline(datafile, article)){
        int word_tracker = 0;
        bool ndoc = true;

        while (word_tracker != -1){

            int next_space = article.find(" ", word_tracker);
            int word_length = (next_space != -1) ? next_space-word_tracker : next_space;
            updateUnigram(article.substr(word_tracker+1, word_length), ndoc);
            word_tracker = next_space;

            ndoc = false;

        }

        breaker++;

        if(breaker > 3){
            break;
        }
    }

    writeOutUnigrams();
    datafile.close();
    return 0;
}

