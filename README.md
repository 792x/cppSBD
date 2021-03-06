# cppSBD
cppSBD is a simple C++/cpp program used for sentence boundary detection (SBD) allowing you to split paragraphs into sentences.

It is able to take a text file and transform it into a vector of sentences with fairly good accuracy. Can be used to parse sentences from paragraphs and as setup for classification purposes. It uses an English word and abbreviation dictionary as well as a predetermined ruleset to determine sentence boundaries. Only the English language is supported at this moment.

What it does:

- Takes a .txt file and uses sentence boundary detection to split the text into sentences and load each sentence in a vector.
- Accounts for carriage returns
- Accounts for forgotten spaces (i.e. "This is a sentence.I forgot to put a space")
- Accounts for numbers
- Skips most abbreviations (Mr., Mrs., Jan., Feb. etc.) and special instances (urls, email adresses, phone numbers etc)
- Handles all end punctuation [!?.:]
- Returns a vector of sentences


Input:

```cpp
This is another example with 022-673000 phone numbers and www.google.com websites such that email@gmail.com adresses are also skipped. On Jan. 20, former Sen. Barack Obama became the 44th President of the U.S. Millions attended the inauguration. The first line of Lorem Ipsum, "Lorem ipsum dolor sit amet..", comes from a line in section 1.10.32.
```
Output:

```cpp
{
[This is another example with 022-673000 phone numbers and www.google.com websites such that email@gmail.com adresses are also skipped.], 
[On Jan. 20, former Sen. Barack Obama became the 44th President of the U.S. Millions attended the inauguration.], 
[The first line of Lorem Ipsum, "Lorem ipsum dolor sit amet..", comes from a line in section 1.10.32.]
}
```


How to use:
```cpp
//new pointer to class
SentenceSplitter *ss;

//use split function
vector<string> results = ss->split("texthard.txt");

//result vector now usable and contains all sentences
//i.e. print all sentences..
for (auto i = results.begin(); i != results.end(); ++i) {
    cout << *i  << endl;
}
```
