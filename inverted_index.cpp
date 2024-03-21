//
// Created by hetricke on 3/1/24.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <regex>
#include <locale>

using namespace std;

//TODO: fix stemming operation
/*
 * Aight. Here's the plan.
 * The unigram struct becomes an entry struct recording the relevant inverted index data
 * Dictionary vector(s) store a string. The idea is on the first pass-thru you assemble all aspects of the inverted index entry except the word code
 * Then you iterate over just the local index collection again to insert the correct word code, binary searching the dictionary to find it
 * */

struct Entry{
    int word_code = -1;
    string word;
    int doc_count = 1;
    vector<array<int, 2>> doc_id_freq;
};



struct CompareWordEntries{

    bool operator()(const Entry* left, const Entry* right){
        return left->word < right->word;
    }

    bool operator()(const Entry *left, const string &right){
        return left->word < right;
    }

    bool operator()(const string &left, const Entry* right){
        return left < right->word;
    }


};

struct CompareDocID{

    bool operator()(const int* left, const int right){
        return left[0] < right;

    }

    bool operator()(const int left, const int* right){
        return left < right[0];

    }

    bool operator()(const int left, const int right){
        return left < right;

    }
};

vector<string> alphabet;

vector<vector<string>> dictionary;

vector<Entry*> entries;


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

    return stemmed_word;
}

int updateAlphabetCount(const string &letter){

    if (alphabet.empty()){
        alphabet.push_back(letter);
        vector<string> new_dict;
        dictionary.push_back(new_dict);
        return 0;

    }

    auto search_result = lower_bound(alphabet.begin(), alphabet.end(), letter);

    if(search_result == alphabet.end()){
        alphabet.push_back(letter);
        vector<string> new_dict;
        dictionary.push_back(new_dict);

        return alphabet.size()-1;
    }

    int index = distance(alphabet.begin(), search_result);
    if (alphabet[index] == letter){
        return index;
    }
    else{
        alphabet.insert(search_result, letter);

        vector<string> new_dict;
        dictionary.insert(dictionary.begin()+index, new_dict);

        return index;
    }

}

//TODO: Update
void updateEntries(string word, int doc_id){

    auto search_result = lower_bound(entries.begin(), entries.end(), word, CompareWordEntries());

    if (search_result != entries.end()){

        int index = std::distance(entries.begin(), search_result);

        if(entries[index]->word == word){
            Entry *v = entries[index];

            auto search_for_doc = lower_bound(v->doc_id_freq.begin(), v->doc_id_freq.end(), doc_id, CompareDocID());
            entries.erase(search_result);
        }

        else{
            entries.insert(search_result, u);
        }

    }

    else{
        entries.push_back(e);
        return;
    }


}

void updateDictionary(const string &word, bool new_doc){

    int letter_index = updateAlphabetCount(word.substr(0,1));

    if (dictionary[letter_index].empty()){
        dictionary[letter_index].push_back(word);
        updateEntries(word);
        return;
    }

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
    for(vector<string > &i : dictionary){
        for(string &word : i){
            dictionary_file << word << endl;
        }
    }

    dictionary_file.close();
}

void writeOutEntries(){
    ofstream entries_file ("entries.txt");

    for(Entry *i : entries){
        string entry_output = to_string(i->word_code) + " ";
        entry_output += i->word + " ";
        entry_output += to_string(i->doc_count) + " ";

        for(auto doc_freq : i->doc_id_freq){
            entry_output += "(" + to_string(doc_freq[0]) + ", " + to_string(doc_freq[1]) + ")";
        }
        entries_file << entry_output << endl;
    }

    entries_file.close();
}


//TODO: Update
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

    cout<<"Printing entries..."<<endl;
    writeOutentries();
    return 0;
}