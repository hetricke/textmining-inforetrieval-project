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
    
    f = open(file_name, 'a+')
    f.close()

    rf = open(file_name, 'r')
    data = rf.read()
    rf.close()

    df = open(file_name, 'w')

    word_list = data.split("\n")
    
    word_list.pop(len(word_list)-1)

    if(len(word_list)==0):
        word_list.append(word)
        df.writelines([i+'\n' for i in word_list])
        df.close()
        return 1
    

    if(len(word_list)==1 and word_list[0] != word):
        word_list.append(word) if word > word_list[0] else word_list.insert(0,word)
        df.writelines([i+'\n' for i in word_list])
        df.close()
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
        
        if (mid < end):
            if (word_list[mid] < word) and (word_list[mid+1] > word):
                word_list.insert(mid+1, word)
                df.writelines([i+'\n' for i in word_list])
                df.close()
                return mid+1

        elif (word_list[mid] < word):
            start = mid+1
            mid = int((start+end)/2)

        elif (word_list[mid] > word):
            end = mid-1
            mid = int((start+end)/2)


    if (word_list[len(word_list)-1] < word):
            word_list.append(word)
            df.writelines([i+'\n' for i in word_list])
            df.close()
            return len(word_list)-1

    elif (word_list[0] > word):
        word_list.insert(0, word)
        df.writelines([i+'\n' for i in word_list])
        df.close()
        return 0

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
    count = 0
    for lines in data:
        words = lines.split()
        for w in words:
            w_altered = w.lower()
            print(w)
            w_code = updateDict(w_altered)
        count+=1
        if(count>=1):
            break
