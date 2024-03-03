//
// Created by hetricke on 3/1/24.
//
#include <iostream>
#include <fstream>
using namespace std;

struct Unigram{
    int word_code;
    string word;
    int doc_count;
    int global_count;
};

vector<int> alphabet = {};
vector<int> alphabet_count = {};

vector<vector<string>> dictionary = {};

vector<Unigram> unigram_word_code = {};
vector<Unigram> unigram_global_count = {};


string stemWord(string word){

    return "";
}

int updateAlphabetCount(string letter){

    auto search_result = lower_bound(alphabet.begin(), alphabet.end(), letter);

    if(search_result = alphabet.end()){
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
        alphabet_count.insert(index, 1)

        vector<string> new_dict = {};
        dictionary.insert(index, new_dict);

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

    int index = distance(dictionary[letter_index], search_result);

    if (dictionary[letter_index][index] == word){
        return;
    }
    else{
        dictionary[letter_index].insert(index, word);
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

    dictionary_file.close()
    return;
}

void updateUnigram(string word, bool new_doc){


    return;
}

void writeOutUnigrams(){
    ofstream unigrams_file ("unigrams.txt");

    for(int i = 0; i<unigrams_global_count.size(); i++){
        string unigram_output = unigrams_global_count[i].word_code + " ";
        unigram_output += unigrams_global_count[i].word + " ";
        unigram_output += unigrams_global_count[i].doc_count + " ";
        unigram_output += unigrams_global_count[i].global_count + " ";

        unigrams_file << unigram_output << endl;
    }

    unigrams_file.close()
    return;
}

int main(){
    return 0;
}

