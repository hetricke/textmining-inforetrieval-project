This program was written on a Windows 10 Home OS, version 10.0.19045, build 19045.

Note: this program requires tiny_wikipedia.txt to be stored in the same directory as it.


In order to run the program double-click unigram_model.exe, potentially marked as an application file. This should open a window with a loading bar tracking the program's progress.

If for some reason the application fails to run or crashes, it is recommended that it is recompiled by using the terminal command g++ unigram_model.cpp -o unigram_model to generate a new exe file.
If g++ is not installed, follow the instructions here: https://code.visualstudio.com/docs/cpp/config-mingw to install it.

Otherwise, if the exe file fails, you can try running the unigram_model.cpp file in whatever your preferred IDE is. The cpp program expects to have the OleanderStemmingLibrary-master directory unzipped in the directory with it.

The program should generate two files in the directory where it is stored: dictionary.txt and unigrams.txt.