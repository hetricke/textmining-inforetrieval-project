import string

#hash the dictionary by having different txt files for different starting letters
#gonna have to cycle through the document twice b/c you won't know the word code until you've tabulated all of them

#returns stemmed word
def stemWord(word):
    return ""

#updates the dictionary with a new word if needed; either way returns the word code
def updateDict(word):
    first_letter = word[0]
    file_name = first_letter + "_dictionary.txt"

    df = open(file_name, 'r')
    data = df.read()
    word_list = data.split("\n")

    start = 0;
    end = len(word_list)
    mid = start+end

    loc_found = False

    while not loc_found:

        if word_list[mid] == word:
            return
        
        elif (word_list[mid] < word) and (word_list[mid+1]):
            loc_found = True

        elif (word_list[mid] < word_list):
            start = mid
            mid = start + end

    return 0


#update unigrams.txt
#gotta figure out how to track which doc (line) we're on 
def updateUnigrams(word, word_code, new_doc = False):
    return

updateUnigrams.counter = 0

def combineDict():
    return

def combineUnigrams():
    return



with open('tiny_wikipedia.txt', 'r') as data:
    for line in data:
        new_line = True
        words = line.split()
        for w in words:
            w = w.lower().translate(None, string.punctuation)
            stem_word = stemWord(w)
            w_code = updateDict(stem_word)
            updateUnigrams(stem_word, w_code, True) if new_line else updateUnigrams(stem_word, w_code)

combineDict()
combineUnigrams()
