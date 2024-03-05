//
// Created by hetricke on 3/1/24.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <regex>
#include "OleanderStemmingLibrary-master/src/english_stem.h"
#include <codecvt>
#include <locale>

using namespace std;

struct Unigram{
    int word_code = -1;
    string word;
    int doc_count = 1;
    int global_count = 1;


};

struct CompareWordUnigrams{

    bool operator()(const Unigram *left, const Unigram *right){
        return left->word < right->word;
    }

    bool operator()(const Unigram *left, const string &right){
        return left->word < right;
    }

    bool operator()(const string &left, const Unigram *right){
        return left < right->word;
    }


};

struct CompareCountUnigrams{

    bool operator()(const Unigram *left, const Unigram *right){
        if(left->global_count != right->global_count){
            return left->global_count > right->global_count;
        }
        else{
            return left->word > right->word;
        }

    }
};

vector<string> alphabet;

vector<vector<Unigram *>> dictionary;

vector<Unigram *> unigrams;


//returns a stemmed version of a given string
string stemWord(const string &w){

    //checks for urls and returns an empty string if true
    regex url("(http|ftp|https):\\/\\/([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])");
    smatch m;
    if (regex_search(w.begin(), w.end(), m, url)){

        return "";
    }

    //checks for formatting tags
    if(w.substr(0,1) == "<"
    && w.substr(w.length()-1) == ">"){
        return "";
    }

    string stemmed_word = w;

    //transforms the word into lowercase
    transform(stemmed_word.begin(), stemmed_word.end(), stemmed_word.begin(), ::tolower);

    //removes all non-alphanumeric characters
    stemmed_word = regex_replace(stemmed_word, regex(R"([^a-z0-9])"), "");

    stemming::english_stem<> StemEnglish;
    wstring word = wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stemmed_word);
    StemEnglish(word);
    stemmed_word =  std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(word);

    return stemmed_word;
}

int updateAlphabetCount(const string &letter){

    if (alphabet.empty()){
        alphabet.push_back(letter);
        vector<Unigram *> new_dict;
        dictionary.push_back(new_dict);
        return 0;

    }

    auto search_result = lower_bound(alphabet.begin(), alphabet.end(), letter);

    if(search_result == alphabet.end()){
        alphabet.push_back(letter);
        vector<Unigram *> new_dict;
        dictionary.push_back(new_dict);

        return alphabet.size()-1;
    }

    int index = distance(alphabet.begin(), search_result);
    if (alphabet[index] == letter){
        return index;
    }
    else{
        alphabet.insert(search_result, letter);

        vector<Unigram *> new_dict;
        dictionary.insert(dictionary.begin()+index, new_dict);

        return index;
    }

}

void updateUnigram(Unigram *u){

    auto search_result = lower_bound(unigrams.begin(), unigrams.end(), u, CompareCountUnigrams());

    if(search_result == unigrams.end()){
        unigrams.push_back(u);
        return;
    }

    int index = std::distance(unigrams.begin(), search_result);

    if(unigrams[index]->word == u->word){
        Unigram *v = unigrams[index];
        v->global_count++;
        unigrams.erase(search_result);

        auto new_loc = lower_bound(unigrams.begin(), unigrams.end(), v, CompareCountUnigrams());
        unigrams.insert(new_loc, v);
    }

    else{
        unigrams.insert(search_result, u);
    }

}

void updateDictionary(const string &word, bool new_doc){

    int letter_index = updateAlphabetCount(word.substr(0,1));

    auto *u = new Unigram;
    u->word = word;
    u->global_count = 1;
    u->doc_count = 1;

    if (dictionary[letter_index].empty()){
        dictionary[letter_index].push_back(u);
        updateUnigram(u);
        return;
    }

    auto search_result = lower_bound(dictionary[letter_index].begin(), dictionary[letter_index].end(), u, CompareWordUnigrams());

    if (search_result == dictionary[letter_index].end()){

        dictionary[letter_index].push_back(u);
        updateUnigram(u);
        return;
    }

    int index = std::distance(dictionary[letter_index].begin(), search_result);

    if (dictionary[letter_index][index]->word == word){

        if(new_doc){
            dictionary[letter_index][index]->doc_count++;
        }
        updateUnigram(dictionary[letter_index][index]);

        return;
    }
    else{
        dictionary[letter_index].insert(search_result, u);
        updateUnigram(u);
        return;
    }

}

void writeOutDictionary(){
    ofstream dictionary_file ("dictionary.txt");
    int word_code = 0;
    for(vector<Unigram *> &i : dictionary){
        for(Unigram * &j : i){

            dictionary_file << j->word << endl;
            j->word_code = word_code;
            word_code++;
        }
    }

    dictionary_file.close();
}

void writeOutUnigrams(){
    ofstream unigrams_file ("unigrams.txt");

    for(Unigram *i : unigrams){
        string unigram_output = to_string(i->word_code) + " ";
        unigram_output += i->word + " ";
        unigram_output += to_string(i->doc_count) + " ";
        unigram_output += to_string(i->global_count) + " ";

        unigrams_file << unigram_output << endl;
    }

    unigrams_file.close();
}



int main(){
    ifstream datafile("tiny_wikipedia.txt");
    string article;

    cout<<"Assembling dictionary..."<<endl;
    float progress = 0;
    int bar_width = 70;
    int tracker = 0;
    string s_progress;

    while(getline(datafile, article)){
        int word_tracker = 0;
        vector<string> article_words;
        while (word_tracker != -1){

            //slices word out of article and stems it
            int next_space = article.find(" ", word_tracker+1);
            int word_length = (next_space != -1) ? next_space-word_tracker : next_space;
            string word = article.substr(word_tracker, word_length);
            word = stemWord(word);

            if(word.empty()){
                word_tracker = next_space;
                continue;
            }

            //checks to see if word is new in article
            auto search = lower_bound(article_words.begin(), article_words.end(), word);

            if(search == article_words.end()){
                article_words.push_back(word);
                updateDictionary(word, true);

            }
            else if (*(search) != word){
                article_words.insert(search, word);
                updateDictionary(word, true);
            }

            else{
                updateDictionary(word, false);
            }

            word_tracker = next_space;

        }
        tracker++;
        int pos = bar_width * progress;
        s_progress ="[";
        for (int i = 0; i < bar_width; ++i) {
            if (i < pos) s_progress += "=";
            else if (i == pos) s_progress +=  ">";
            else s_progress += " ";
        }
        s_progress += "] " + to_string(int(progress * 100)) + " % " + to_string(tracker) + "/50000\r";
        cout<<s_progress;
        cout.flush();

        progress = float(tracker)/50000;
    }

    datafile.close();
    cout<<endl;
    cout<<"Printing dictionary..."<<endl;
    writeOutDictionary();

    cout<<"Printing unigrams..."<<endl;
    writeOutUnigrams();
    return 0;
}

