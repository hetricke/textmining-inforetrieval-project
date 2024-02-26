from dataclasses import dataclass
from pathlib import Path

#create a conda environment for this and export it as a yml to be sent with the code
#just need to grab an appropriate lemmatizer
#and do a regex to eliminate punctuation
#might not be a bad idea to create a stemmed version of the docs so that the unigram portion doesn't have to reprocess

#env name : info_retrieval_env
#add pathlib to env

@dataclass
class Unigram:
    word_code: int
    word: str
    doc_count: int
    global_count: int

alphabet = []
alphabet_count = []

unigram_dict = {}
unigram_list = []

#returns stemmed word
def stemWord(word):
    return ""

#updates the alphabet_count lists to track how many words belonging to each letter have been found
def updateAlphabetCount(letter, count):
    global alphabet_count
    global alphabet

    if letter in alphabet:
        index = alphabet.index(letter)
        alphabet_count[index] = count
        return

    if(len(alphabet)==0):
        alphabet.append(letter)
        alphabet_count.append(count)
        return 1

    if(len(alphabet)==1 and alphabet[0] != letter):
        alphabet.append(letter) if letter > alphabet[0] else alphabet.insert(0,letter)
        alphabet_count.append(1) if letter > alphabet[0] else alphabet_count.insert(0,count)
        return
    
    elif(alphabet[0] == letter):
        alphabet_count[0] = count
        return 0

    else:
        start = 0
        end = len(alphabet)-1
        mid = int((start+end)/2)

        while start<= end:
            
            if (mid < len(alphabet)-1):
                if (alphabet[mid] < letter) and (alphabet[mid+1] > letter):
                    alphabet.insert(mid+1, letter)
                    alphabet_count.insert(mid+1, count)
                    return

            if (alphabet[mid] < letter):
                start = mid+1
                mid = int((start+end)/2)

            elif (alphabet[mid] > letter):
                end = mid-1
                mid = int((start+end)/2)

        if (alphabet[len(alphabet)-1] < letter):
            alphabet.append(letter)
            alphabet_count.append(count)

            return

        elif (alphabet[0] > letter):
            alphabet.insert(0, letter)
            alphabet_count.append(count)
            return
        
#updates the correct sub-dictionary with a new word if needed; either way returns the word code
def updateDict(word):
    first_letter = word[0]
    file_name = first_letter + "_dictionary.txt"
    
    #Ensure the file exists and creates it if it doesn't
    f = open(file_name, 'a+')
    f.close()

    #Reads out the data
    rf = open(file_name, 'r')
    data = rf.read()
    rf.close()

    #Prepares to write to the file
    df = open(file_name, 'w')

    word_list = data.split("\n")   
    word_list.pop(len(word_list)-1)

    if(len(word_list)==0):
        word_list.append(word)
        df.writelines([i+'\n' for i in word_list])
        df.close()
        updateAlphabetCount(first_letter, len(word_list))
        return 1

    if(len(word_list)==1 and word_list[0] != word):
        word_list.append(word) if word > word_list[0] else word_list.insert(0,word)
        df.writelines([i+'\n' for i in word_list])
        df.close()
        updateAlphabetCount(first_letter, len(word_list))
        return 1 if word > word_list[0] else 0
    
    elif(word_list[0] == word):
        df.writelines([i+'\n' for i in word_list])
        df.close()
        return 0
    
    start = 0
    end = len(word_list)-1
    mid = int((start+end)/2)

    while start<= end:

        if word_list[mid] == word:
            df.writelines([i+'\n' for i in word_list])
            df.close()
            return mid
        
        if (mid < len(word_list)-1):
            if (word_list[mid] < word) and (word_list[mid+1] > word):
                word_list.insert(mid+1, word)
                df.writelines([i+'\n' for i in word_list])
                df.close()
                updateAlphabetCount(first_letter, len(word_list))
                return mid+1

        if (word_list[mid] < word):
            start = mid+1
            mid = int((start+end)/2)

        elif (word_list[mid] > word):
            end = mid-1
            mid = int((start+end)/2)


    if (word_list[len(word_list)-1] < word):
        word_list.append(word)
        df.writelines([i+'\n' for i in word_list])
        df.close()
        updateAlphabetCount(first_letter, len(word_list))
        return len(word_list)-1

    elif (word_list[0] > word):
        word_list.insert(0, word)
        df.writelines([i+'\n' for i in word_list])
        df.close()
        updateAlphabetCount(first_letter, len(word_list))
        return 0
        
    df.writelines([i+'\n' for i in word_list])
    df.close()

    return -1

#returns the word code of the given word
def getWordCode(word):
    global alphabet
    global alphabet_count

    first_letter = word[0]
    file_name = first_letter + "_dictionary.txt"
    first_letter_index = alphabet.index(first_letter)

    rf = open(file_name, 'r')
    data = rf.read()
    rf.close()

    word_list = data.split("\n")
    word_list.pop(len(word_list)-1)

    word_code = word_list.index(word)
    word_code += sum(alphabet_count[:first_letter_index])

    return word_code

#updates the unigrams list
def updateUnigrams(word, new_doc = False):
    global unigram_dict
    global unigram_list

    word_code = getWordCode(word)
    
    if word_code in unigram_dict:
        unigram = unigram_dict[word_code]
        unigram.global_count += 1

        if new_doc:
            unigram.doc_count += 1
        
        index = unigram_list.index(unigram)
        tracker = index-1
        unigram_list.pop(index)

        tgc = unigram_list[tracker].global_count
    
        while tracker > 0 and tgc <= unigram.global_count:
            tgc = unigram_list[tracker].global_count
            tracker -= 1

        unigram_list.insert(tracker, unigram)

    else:
        unigram = Unigram(word_code, word, 1, 1)
        unigram_dict.update({word_code : unigram})
        unigram_list.append(unigram)
    
    return

#saves the unigrams list to a txt file
def writeOutUnigrams():
    global unigram_list
    
    f = open("unigrams.txt", 'a+')
    f.close()

    wf = open("unigrams.txt", 'w')

    for unigram in unigram_list:
        line = str(unigram.word_code) +" "+ unigram.word +" "+ str(unigram.doc_count) +" "+ str(unigram.global_count) + "\n"
        wf.write(line)

    wf.close()
    unigram_dict.clear()
    unigram_list.clear()


def combineDict():
    global alphabet
    
    dictionary_list = []

    for letter in alphabet:
        file_name = letter + "_dictionary.txt"
        rf = open(file_name, 'r')
        data = rf.read()
        rf.close()

        word_list = data.split("\n")   
        word_list.pop(len(word_list)-1)

        dictionary_list += word_list

        p = Path(file_name)
        p.unlink()

    df = open("dictionary.txt", 'a+')
    df.writelines([i+'\n' for i in dictionary_list])
    df.close()


    return

#cycles through the whole doc list to create the dictionary
with open('tiny_wikipedia.txt', 'r') as data:
    count = 0
    for lines in data:
        words = lines.split()
        for w in words:
            w_altered = w.lower()
            w_code = updateDict(w_altered)
        count+=1
        if(count>=1):
            break

#cycles through the whole doc list to create the unigrams file
with open('tiny_wikipedia.txt', 'r') as data:
    for lines in data:
        n_doc = True
        words = lines.split()
        for w in words:
            w_altered = w.lower()
            w_code = updateUnigrams(w_altered, n_doc)
            n_doc = False

        count+=1
        if(count>=1):
            break

writeOutUnigrams()