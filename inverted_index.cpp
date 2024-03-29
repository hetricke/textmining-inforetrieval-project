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
#include <array>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

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

    bool operator()(const array<int, 2> left, const array<int, 2> right){
        return left[0] < right[0];

    }

    bool operator()(const int left, const array<int, 2> right){
        return left < right[0];

    }

    bool operator()(const array<int, 2> left, const int right){
        return left[0] < right;

    }
};

vector<string> alphabet;

vector<vector<string>> dictionary;

vector<Entry*> entries;


//returns a stemmed version of a given string
//TODO: make this actually work without a library
string stemWord(const string &w, const bool &new_doc = false){

    //checks for urls and returns an empty string if true
    regex url("(http|ftp|https):\\/\\/([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])");
    smatch m;
    if (regex_search(w.begin(), w.end(), m, url)){

        if (new_doc){

            //TODO: search for "curid=" and return the numerical value that follows
        }
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

void updateEntries(const string &word, const int &doc_id){

    //searches for the word in the current index
    auto search_result = lower_bound(entries.begin(), entries.end(), word, CompareWordEntries());

    if (search_result != entries.end()){

        int index = std::distance(entries.begin(), search_result);

        //if the word is already in the inverted index, finds the doc id in the list and updates the frequency
        if(entries[index]->word == word){
            Entry *v = entries[index];

            auto search_for_doc = lower_bound(v->doc_id_freq.begin(), v->doc_id_freq.end(), doc_id, CompareDocID());

            if (search_for_doc == v->doc_id_freq.end()){
                array<int, 2> df{ {doc_id, 1} };
                v->doc_id_freq.push_back(df);
                v->doc_count++;
                return;
            }

            int doc_index = std::distance(v->doc_id_freq.begin(), search_for_doc);

            if(search_for_doc[doc_index][0] == doc_id){
                search_for_doc[doc_index][1]++;
                return;
            }

            else{
                array<int, 2> df{ {doc_id, 1} };
                v->doc_id_freq.insert(search_for_doc, df);
                v->doc_count++;
                return;
            }
        }

        //If the word isn't already in the inverted index but doesn't belong at the end, inserts the word
        else{
            Entry *e = new Entry;
            e->word = word;
            array<int, 2> df{{doc_id, 1}};
            e->doc_id_freq.push_back(df);
            entries.insert(search_result, e);
            return;
        }

    }

    //Inserts a new word to the end of the inverted index
    else{
        Entry *e = new Entry;
        e->word = word;
        array<int, 2> df{{doc_id, 1}};
        e->doc_id_freq.push_back(df);
        entries.push_back(e);
        return;
    }


}

void updateDictionary(const string &word){

    int letter_index = updateAlphabetCount(word.substr(0,1));

    if (dictionary[letter_index].empty()){
        dictionary[letter_index].push_back(word);
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

//WARNING: calling this function wipes the current entries list
void writeOutEntries(string id){

    string output_file_name = "inverted_index_"+id;
    ofstream entries_file (output_file_name);

    for(Entry *i : entries){
        string entry_output = to_string(i->word_code) + " ";
        entry_output += i->word + " ";
        entry_output += to_string(i->doc_count) + " ";

        for(auto doc_freq : i->doc_id_freq){
            entry_output += "(" + to_string(doc_freq[0]) + ", " + to_string(doc_freq[1]) + ")";
        }
        entries_file << entry_output << endl;
    }

    entries.clear();

    entries_file.close();
}


//TODO: Update
//remember to call update Entries here
int main(){




    string collection_name = "wiki2022_small.";
    string collection_id = "000000";

    //progress bar
    cout<<"Assembling inverted index for article collection "<< i <<"...." <<endl;
    float progress = 0;
    int bar_width = 70;
    int tracker = 0;
    string s_progress;

    //iterates through all article collections to create inverted indexes
    for(int i = 0; i<32; i++){

        string current_col = to_string(i);
        collection_id = collection_id.substr(0, collection_id.length()-current_col.length());
        collection_id +=current_col;
        assertm(collection_id.length()==6, "Something wrong with filename string");

        ifstream datafile(collection_name+collection_id);
        string article;

        //iterates through each article
        while(getline(datafile, article)){
            int word_tracker = 0;
            int doc_id = -1;

            //iterates through each word in the article
            while (word_tracker != -1){

                //slices word out of article and stems it
                int next_space = article.find(" ", word_tracker+1);
                int word_length = (next_space != -1) ? next_space-word_tracker : next_space;
                string word = article.substr(word_tracker, word_length);

                //assumes the first 'word' in an article is a link containing the article id and extracts the id
                if(word_tracker == 0){
                    doc_id = std::stoi(stemWord(word, true));
                    word_tracker = next_space;
                    continue;
                }

                word = stemWord(word);

                if(word.empty()){
                    word_tracker = next_space;
                    continue;
                }

                updateDictionary(word);
                updateEntries(word, doc_id);

                word_tracker = next_space;

            }

        }

        datafile.close();

        //prints out the current inverted index
        writeOutEntries(collection_id);

        tracker++;

        int pos = bar_width * progress;
        s_progress ="[";
        for (int p = 0; p < bar_width; p++) {
            if (i < pos) s_progress += "=";
            else if (i == pos) s_progress +=  ">";
            else s_progress += " ";
        }
        s_progress += "] " + to_string(int(progress * 100)) + " % " + to_string(tracker) + "/32\r";
        cout<<s_progress;
        cout.flush();

        progress = float(tracker)/32;

    }

    cout<<endl;
    cout<<"Printing dictionary..."<<endl;
    writeOutDictionary();




    return 0;
}