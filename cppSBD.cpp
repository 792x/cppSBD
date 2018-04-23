/*
 * Copyright (C) 2018 css
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   main.cpp
 * Author: css
 *
 * Created on April 22, 2018, 9:36 PM
 */

#include <cstdlib>
#include <string>
#include <algorithm>
#include <regex>
#include <vector>
#include <fstream>
#include <streambuf>
#include <iostream>


using namespace std;

//class declaration
class SentenceSplitter;
class StringBuilder;

//regex constants
regex REGEX_RETURN = regex("[\\n\\r]+");
regex REGEX_FORGOTTEN_SPACE = regex("(.)([\\.!?])([\\D&&\\S&&[^\\.\"'`\\)\\}\\]]])");
regex REGEX_SENTENCE = regex("(['\"`]*[\\(\\{\\[]?[a-zA-Z0-9]+.*?)([\\.!?:])(?:(?=([\\(\\[\\{\"'`\\)\\}\\]<]*[ \031]+)[\\(\\[\\{\"'`\\)\\}\\] ]*([A-Z0-9][a-z]*))|(?=([\\(\\)\"'`\\)\\}<\\] \031]+)\\s))");
regex REGEX_WHITESPACE = regex("\\s+");
regex REGEX_LAST_WORD = regex("\\b([\\w0-9\\.']+)$");

//global variables
vector<string> EnglishAbbreviations(0);
vector<string> EnglishDictionary(0);
string textfile;


//helper class to handle stringbuilding

class StringBuilder {
private:
    string main;
    string scratch;

    const string::size_type ScratchSize = 1024;
    // or some other arbitrary number

public:

    StringBuilder & append(const string & str) {
        scratch.append(str);
        if (scratch.size() > ScratchSize) {
            main.append(scratch);
            scratch.resize(0);
        }
        return *this;
    }

    const string & str() {
        if (scratch.size() > 0) {
            main.append(scratch);
            scratch.resize(0);
        }
        return main;
    }
};

//main class used to split sentences

class SentenceSplitter {
public:

    /*
     * Reads both included dictionary files
     * 
     * @params abbreviations is the list of english abbreviations
     * @params dictionary is the list of english words
     * @modifies vector EnglishAbbreviations and vector EnglishDictionary
     * @returns Two vectors, one containing each english abbreviation
     * and another containing each english word
     * 
     */

    int loadDictionaries(string abbreviations = "abbreviations_en.txt",
            string dictionary = "dictionary_en.txt") {
        string word;
        string word2;
        ifstream reader(abbreviations);
        ifstream reader2(dictionary);

        if (!reader) {
            cout << "Error opening file" << endl;
            return -1;

        } else {
            while (reader >> word) {
                EnglishAbbreviations.push_back(word);
            }

            reader.close();
        }

        if (!reader2) {
            cout << "Error opening file2" << endl;
            return -1;

        } else {
            while (reader2 >> word2) {
                
                EnglishDictionary.push_back(word2);
                
            }

            reader2.close();

        }

        return 0;

    }

    /*
     * Transforms string to lower case
     * 
     * @pre (input != null)
     * @params string input
     * @modifies string input
     * @returns input such that it is in lower case
     * @post \forall(char in input ; char is in lower case)
     * 
     */

    string toLowerCase(string input) {

        for (char & ch : input)
            ch = tolower(ch);
        return input;

    }

    /*
     * Trims leading and trailing spaces of string
     * 
     * @pre (input != null)
     * @params string input
     * @modifies string input
     * @returns string input such that leading and trailing spaces are excluded
     * 
     */

    string toTrimmed(string input) {

        return regex_replace(input, regex("^ +| +$|( ) +"), "$1");

    }

    /*
     * removes certain character from a string
     * 
     * @pre (input != null) && (c != null)
     * @params string input
     * @params char c
     * @modifies string input
     * @returns string input such that all occurences of char c are removed 
     * from it
     * 
     */

    string removeChar(string input, char c) {
        string result;
        for (size_t i = 0; i < input.size(); i++) {
            char currentChar = input[i];
            if (currentChar != c)
                result += currentChar;
        }
        return result;
    }

    /*
     * Reads all of a textfile into a string
     * 
     * @pre
     * @params string filename (default = "text.txt")
     * @modifies textfile
     * 
     */

    string loadFile(string filename) {

        ifstream t(filename);
        string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
        return str;

    }

    /*
     * Takes a string of text and uses sentence boundary detection to split
     * the text into sentences and load each sentence in a vector.
     * 
     * @pre (textfile != null)
     * @params string text (default = textfile)
     * @modifies vector results
     * @returns vector results 
     * @post \forall(strings in results ; string is a sentence)
     * 
     */


    vector<string> split(string filename = "text.txt") {
        vector<string> sentences;
        int length = 0;
        string text = "";

        //load dictionaries
        loadDictionaries();
        
       
        //load file
        text = loadFile(filename);

        //preprocess text
        text = regex_replace(text, REGEX_RETURN, " ");
        text = removeChar(text, '\031');
        text = regex_replace(text, REGEX_FORGOTTEN_SPACE, "$1$2\031$3");
        text = text + "\n";


        StringBuilder* currentSentence = new StringBuilder();
        int end = 0;

        //iterate over all matches with REGEX_SENTENCE
        for (sregex_iterator i = sregex_iterator(text.begin(), text.end(),
                REGEX_SENTENCE); i != sregex_iterator(); ++i) {
            smatch matches = *i;

            end = matches.suffix().second - text.begin();

            string sentence = toTrimmed(matches[1].str());
            string punctuation = matches[2].str();

            string stuffAfterPeriod = matches.str(3);
            if (stuffAfterPeriod.empty()) {
                stuffAfterPeriod = matches.str(5);
                if (stuffAfterPeriod.empty()) {
                    stuffAfterPeriod = "";
                } else {
                    end = matches[5].second - text.begin();

                }
            } else {
                end = matches[5].second - text.begin();

            }

            sregex_token_iterator iter(sentence.begin(), sentence.end(),
                    REGEX_WHITESPACE, -1);
            sregex_token_iterator endz;
            vector<string> words;
            for (; iter != endz; ++iter) {
                words.push_back(*iter);
            }

            length += words.size();

            string nextWord = matches[4].str();
            if (nextWord.empty()) {
                nextWord = "";
            }

            if (punctuation.compare(".") == 0) {

                // We check if word before period is abbreviation, this is the case if:
                //  1) all consonants and not all capitalised and contain no lower case 
                //  2) a span of single letters followed by periods
                //  3) a single letter (except I).
                //  4) in the known abbreviations list.
                // If any case is true, then the period is NOT a full stop.

                smatch morematches;
                string lastWord = "";
                if (regex_search(sentence, morematches, REGEX_LAST_WORD)) {
                    lastWord = morematches.str(); // works till here              
                }
                

                if ((!regex_match(sentence, morematches, regex(".*[AEIOUaeiou]+.*"))
                        && regex_match(sentence, morematches, regex(".*[a-z]+.*"))
                        && !regex_match(sentence, morematches, regex(".*[y]+.*")))
                        || regex_match(sentence, morematches, regex("([a-zA-Z][\\.])+"))
                        || (regex_match(sentence, morematches, regex("^[A-Za-z]$"))
                        && !regex_match(sentence, morematches, regex("^[I]$")))
                        || find(EnglishAbbreviations.begin(), EnglishAbbreviations.end(),
                        toLowerCase(lastWord)) != EnglishAbbreviations.end()) {

                    // then we have an abbreviation
                    if ((find(EnglishDictionary.begin(), EnglishDictionary.end(),
                            nextWord) != EnglishDictionary.end()) && length > 6) {
                        currentSentence->append(sentence);
                        currentSentence->append(punctuation);
                        currentSentence->append(toTrimmed(stuffAfterPeriod));
                        sentences.push_back(currentSentence->str());
                        currentSentence = new StringBuilder();
                        length = 0;


                    } else {
                        // no sentence break
                        currentSentence->append(sentence);
                        currentSentence->append(punctuation);

                        string::size_type loc = stuffAfterPeriod.find('\031', 0);
                        if (loc != string::npos) {
                            currentSentence->append(" ");
                        }


                    }
                } else {
                    // no sentence break
                    currentSentence->append(sentence);
                    currentSentence->append(punctuation);
                    currentSentence->append(toTrimmed(stuffAfterPeriod));
                    sentences.push_back(currentSentence->str());
                    currentSentence = new StringBuilder();
                    length = 0;

                }

            } else {
                // only consider sentences if ":" comes after at least 6 
                // words from start of sentence
                smatch extramatches;
                if (regex_match(punctuation, extramatches, regex("[!?]"))
                        || (punctuation.compare(":") == 0 && length > 6)) {
                    // sentence break
                    currentSentence->append(sentence);
                    currentSentence->append(punctuation);
                    currentSentence->append(toTrimmed(stuffAfterPeriod));
                    sentences.push_back(currentSentence->str());
                    currentSentence = new StringBuilder();
                    length = 0;

                } else {
                    // no sentence break
                    currentSentence->append(sentence);
                    currentSentence->append(punctuation);
                    string::size_type loc = stuffAfterPeriod.find('\031', 0);
                    if (loc != string::npos) {
                        currentSentence->append(" ");
                    }
                }
            }
        }

        if (end < text.length()) {
            //check after last sentence
            string lastPart = text.substr(end);
            if (!lastPart.empty()) {
                currentSentence->append(lastPart);
            }
        }
        // if currentSentence is not empty we add it to sentence vector
        if (currentSentence->str().length() > 0) {
            sentences.push_back(toTrimmed(currentSentence->str()));
        }

        vector<string> results(0);

        for (int i = 0; i < sentences.size(); i++) {
            string str = sentences.at(i);
            str = removeChar(str, '\031');
            // add to result vector
            results.push_back(str);

        }

        return results;
    }
};

int main(int argc, char** argv) {

    //new pointer to class
    SentenceSplitter *ss;

    //use class pointer's split command like this
    vector<string> results = ss->split("texthard.txt");

    //result vector now usable and contains all sentences
    //i.e. print all sentences:
    for (auto i = results.begin(); i != results.end(); ++i) {
        cout << *i  << endl;
    }

    //delete pointer to class
    delete ss;

    return 0;
}

