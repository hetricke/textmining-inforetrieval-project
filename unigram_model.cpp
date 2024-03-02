//
// Created by hetricke on 3/1/24.
//
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

    return 0;
}

void updateDictionary(string word){

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

