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

vector<Unigram> unigram_word_code = {};
vector<Unigram> unigram_global_count = {};


string stemWord(string word){

    return "";
}

void updateAlphabetCount(string letter, int count){

    auto search_result = lower_bound(alphabet.begin(), alphabet.end(), letter);

    if (search_result != alphabet.end()){
        int index = distance(alphabet.begin(), search_result);
        alphabet_count[index] = count;
    }
    else{
        auto insertion_point = upper_bound(alphabet.begin(), alphabet.end(), letter);
        alphabet.insert(insertion_point, letter);

        int index = distance(alphabet.begin(), insertion_point);
        alphabet_count.insert(index, count)
    }

    return;
}

int getWordCode(string word){
    string file_name = word.substr(0,1) + "_dictionary.txt";
    ifstream dictionary_file;
    dictionary_file.open(file_name);

    int start = 0;

    auto search_result = lower_bound(alphabet.begin(), alphabet.end(), word.substr(0,1));
    int end = distance(alphabet.begin(), search_result);

    int mid = (start+end)/2;

    while(start<=end){
        string file_word;
        getline(dictionary_file, mid)
        if(file_word == word){
            dictionary_file.close()
            return mid;
        }
        else if (file_word < word){
            start = mid+1;
        }
        else{
            end = mid-1;
        }
        mid = (start+end)/2;

    }

    dictionary_file.close()

    return -1;
}

void updateDictionary(string word){
    string file_name = word.substr(0,1) + "_dictionary.txt";
    ifstream dictionary_file;
    dictionary_file.open(file_name);

    int start = 0;

    auto search_result = lower_bound(alphabet.begin(), alphabet.end(), word.substr(0,1));
    int end = distance(alphabet.begin(), search_result);

    int mid = (start+end)/2;

    while(start<=end){
        string file_word;
        getline(dictionary_file, mid)
        if(file_word == word){
            dictionary_file.close()
            return;
        }

        //TODO: check if this will exceed the end of the file
        //TODO: read in all the file after this point into a vector and make the changes to the txt file
        string next_file_word;
        getline(dictionary_file, mid+1)

        if ()
        else if (file_word < word){
            start = mid+1;
        }
        else{
            end = mid-1;
        }
        mid = (start+end)/2;

    }

    dictionary_file.close()

    return -1;

    return;
}

void combineDictionary(){
    return;
}

void updateUnigram(string word, bool new_doc){
    return;
}

void writeOutUnigrams(){

    return;
}

int main(){


    return 0;
}

