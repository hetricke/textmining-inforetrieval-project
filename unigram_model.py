import string

#hash the dictionary by having different txt files for different starting letters

#returns stemmed word
def stemWord(word):
    return ""

#update dictionary.txt

#updates the dictionary with a new word if needed; either way returns the word code
def updateDict(word):
    return 0

#update unigrams.txt
#gotta figure out how to track which doc (line) we're on 
def updateUnigrams(word, word_code,):
    return

def combineDict():
    return


with open('tiny_wikipedia.txt') as data:
    for line in data:
        words = line.split()
        for w in words:
            w = w.lower().translate(None, string.punctuation)
            stem_word = stemWord(w)
            w_code = updateDict(stem_word)
