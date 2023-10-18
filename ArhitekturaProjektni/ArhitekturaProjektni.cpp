#include<iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include<bitset>
#include<vector>

using namespace std;
unsigned long long int r1 = 0, r2 = 0, r3 = 0, r4 = 0;
map<int, unsigned long long int> memoryLocations;
using Map = map<int, unsigned long long int>;

void printRegisters() {
    cout << "r1-> " << bitset<64>(r1) << endl;
    cout << "r2-> " << bitset<64>(r2) << endl;
    cout << "r3-> " << bitset<64>(r3) << endl;
    cout << "r4-> " << bitset<64>(r4) << endl;
    cout << endl;
}
void printMemoryLocations() {
    for (Map::const_iterator it = memoryLocations.begin();it != memoryLocations.end(); ++it)
        cout << "[ " << it->first << " ] = " << it->second << "\n";
}
void addValueToMemory(int memoryID, unsigned long long int value) {
    Map::iterator it = memoryLocations.find(memoryID);
    if (it != memoryLocations.end()) {//znaci da vec postoji vrijednost na ovom ID
        it->second = value;
    }
    else memoryLocations[memoryID] = value;

}
void printValuesFromMemory(int memoryID) {
    Map::iterator it = memoryLocations.find(memoryID);
    if (it != memoryLocations.end()) {
        for (; it != memoryLocations.end(); ++it) {
            cout << (char)it->second;
        }
    }
    else
        cout << "Ne postoji vrijednost na ovoj lokaciji"<<endl;

}
void printEverything(vector<string> words) {
    cout << endl;
    cout << "Linija na kojoj se nalazimo " << endl;
    for (string s : words) {//ispis svih argumenata
        cout << " " << s;
    }
    cout << endl;
    printRegisters();
    printMemoryLocations();
}
unsigned long long int getValueFromMemor(int memoryID) {
    Map::iterator it = memoryLocations.find(memoryID);
    if (it != memoryLocations.end()) {// vec postoji vrijednost na ovom ID
        return it->second;
    }
    else
        return -1;
}
int isArgumentNumber(string s) {
    bool isNum = true;
    for (char c : s)
    {
        if (!isdigit(c)) {
            isNum = false;
            break;
        }
    }
    if (isNum)
        return atoi(s.c_str());
    else
        return -1;

}
unsigned long long* isFirstArgumentRegister(string str) {

    if (str == "r1")
        return &r1;
    else if (str == "r2")
        return &r2;
    else if (str == "r3")
        return &r3;
    else if (str == "r4")
        return &r4;

    return nullptr;
}
unsigned long long isSecondArgumentRegister(string str) {

    if (str == "r1")
        return r1;
    else if (str == "r2")
        return r2;
    else if (str == "r3")
        return r3;
    else if (str == "r4")
        return r4;

    return -1;
}
int isArgumentMemoryLocation(string str) {

    if (str[0] == '[' && str[str.length() - 1] == ']') {
        string tempValue = str.substr(1, str.length() - 2);//"brise" zagrade sa pocetka i kraja
        int number = isSecondArgumentRegister(tempValue);

        if (number != -1) {
            return number;
        }
        else {  //ako nije registar mozda je konstanta upisana za memorijsku lokaciju
          
            return isArgumentNumber(tempValue);
        }
    }
    return -1;
}
vector<string> getArgumentsFromLine(string str) {
    vector<string> words;
    string word;
    stringstream iss(str);// pravljenje string strima
    while (iss >> word) {// citaj rijec po rijec iz strima
        words.push_back(word);
    }
    return words;
}



int main() {
    unsigned long long* firstArgument;
    unsigned long long secondArgument;
    fstream file;
    vector<string>words;
    string line = "";
    file.open("test.txt");
   
    if (file.is_open()) {

        string command = "";//koja komanda kod debagovanja je unesena
        int continueTrg = 0;// upotreba continue
        bool debug = false;//debug mod
        bool jmp = false;// koristenje neke od komandi za grananje
        int eof = 0;//doslo je do kraja fajla jer labela nije ispravna ili ne postoji
        int minArg = 2;// minimalan broj argumenata
        string jumpLabel;//labela na koju se prelazi
        while (getline(file, line))
        {
            command = "";
            minArg = 2;
            if (jmp) {

                file.seekg(0, file.beg);
                line = "";
                eof = 0;
                while (line != jumpLabel && eof == 0)
                {
                    getline(file, line);

                    if (file.eof() != true)
                    {
                        remove_if(line.begin(), line.end(), isspace);
                        if (line == jumpLabel) {// ako je pronadjena labela ucitaj iducu komandu
                            getline(file, line);
                            jmp = false;
                            break;
                        }
                    }
                    else
                    {
                        cout << "Greska! Nije pronadjena zadata labela!!" << endl;
                        eof = 1;
                    }
                }

            }
            if (!eof) {

                int index = 0;
                words = getArgumentsFromLine(line);
                if (words.size() == 0) {
                    continue;
                }
                if (words.size() < 2) { //provjera da li je labela
                    getline(file, line);
                    words = getArgumentsFromLine(line);
                    if (words.size() < 2) {// linija ispod labele mora biti komanda
                        cout << "Neispravan unos, linija ispod tabele mora biti nova komanda!!!" << endl;
                    }
                }
                if (words[index] == "*") {//debug mod
                    debug = true;
                    index++;
                    minArg++;// ukoliko je upaljen debug mod, min broj argumenata je 3 ili 4;
                    continueTrg = 0;
                }
                if (debug && (!continueTrg)) {

                    printEverything(words);
                    cout << "Unesite komandu za nastavak izvrsavanja" << endl;
                    while (command != "continue" && command != "next" && command != "step" && command != "quit") {
                        cout << "Podrzane komande su continue, next, step ili quit(za izlazak iz programa)" << endl;
                        getline(cin, command);
                        remove_if(command.begin(), command.end(), isspace);
                    }
                    if (command == "continue")
                    {
                        continueTrg = 1;
                    }
                    else if (command == "quit") {
                        eof = 1;
                        break;
                    }
                }
                if (words.size() == minArg || words.size() == (minArg + 1)) {

                    //printEverything(words);

                    if (words[index] == "ADD")
                    {
                        firstArgument = isFirstArgumentRegister(words[index + 1]);
                        if (firstArgument != nullptr) {//ako je prvi argument registar
                            secondArgument = isSecondArgumentRegister(words[index + 2]);
                            if (secondArgument != -1)// drugi argument jeste registar
                                *firstArgument = (*firstArgument) + secondArgument;
                            else {//drugi nije registar
                                secondArgument = isArgumentNumber(words[index + 2]);
                                if (secondArgument != -1)//drugi je broj
                                    *firstArgument = (*firstArgument) + secondArgument;
                                else
                                    cout << "Drugi argument mora biti ili registar ili broj" << endl;
                            }

                        }
                        else
                            cout << "Neispravan unos, prvi argument mora biti registar" << endl;
                    }
                    else if (words[index] == "SUB")
                    {
                        firstArgument = isFirstArgumentRegister(words[index + 1]);
                        if (firstArgument != nullptr) {//ako je prvi argument registar
                            secondArgument = isSecondArgumentRegister(words[index + 2]);
                            if (secondArgument != -1)// drugi argument jeste registar
                                *firstArgument = (*firstArgument) - secondArgument;
                            else {//drugi nije registar
                                secondArgument = isArgumentNumber(words[index + 2]);
                                if (secondArgument != -1)//drugi je broj
                                    *firstArgument = (*firstArgument) - secondArgument;
                                else
                                    cout << "Drugi argument mora biti ili registar ili broj" << endl;
                            }

                        }
                        else
                            cout << "Neispravan unos, prvi argument mora biti registar" << endl;
                    }
                    else if (words[index] == "OR")
                    {
                        firstArgument = isFirstArgumentRegister(words[index + 1]); 
                        if (firstArgument != nullptr) {//ako je prvi argument registar
                            secondArgument = isSecondArgumentRegister(words[index + 2]);
                            if (secondArgument != -1)// drugi argument jeste registar
                                *firstArgument = (*firstArgument) | secondArgument;
                            else {//drugi nije registar
                                secondArgument = isArgumentNumber(words[index + 2]);
                                if (secondArgument != -1)//drugi je broj
                                    *firstArgument = (*firstArgument) | secondArgument;
                                else
                                    cout << "Drugi argument mora biti ili registar ili broj" << endl;
                            }

                        }
                        else
                            cout << "Neispravan unos, prvi argument mora biti registar" << endl;
                    }
                    else if (words[index] == "AND")
                    {
                        firstArgument = isFirstArgumentRegister(words[index + 1]);//ako je prvi argument registar
                        if (firstArgument != nullptr) {
                            secondArgument = isSecondArgumentRegister(words[index + 2]);
                            if (secondArgument != -1)// drugi argument jeste registar
                                *firstArgument = (*firstArgument) & secondArgument;
                            else {//drugi nije registar
                                secondArgument = isArgumentNumber(words[index + 2]);
                                if (secondArgument != -1)//drugi je broj
                                    *firstArgument = (*firstArgument) & secondArgument;
                                else
                                    cout << "Drugi argument mora biti ili registar ili konstanta" << endl;
                            }

                        }
                        else
                            cout << "Neispravan unos, prvi argument mora biti registar" << endl;
                    }
                    else if (words[index] == "NOT")
                    {
                        firstArgument = isFirstArgumentRegister(words[index + 1]);
                        if (firstArgument != nullptr)
                            *firstArgument = ~(*firstArgument);
                        else
                            cout << "Neispravan unos, prvi argument mora biti registar" << endl;
                    }
                    else if (words[index] == "MOV") {
                     
                        firstArgument = isFirstArgumentRegister(words[index + 1]);
                        if (firstArgument == nullptr) {//prvi nije registar moze biti samo mem lok
                            int pomFirstValue = isArgumentMemoryLocation(words[index + 1]);
                            if (pomFirstValue != -1) {//garantuje da je prva vrijednost memorijska lokacija
                                int secondValue = isSecondArgumentRegister(words[index + 2]);
                                if (secondValue == -1) {//nije registar, moze biti ili mem lokacija ili konstanta
                                    secondValue = isArgumentMemoryLocation(words[index + 2]);
                                    if (secondValue == -1) {// nije mem lokacija, moze biti samo konstanta
                                        secondValue = isArgumentNumber(words[index + 2]);
                                        if (secondValue == -1)//nije ni jedno od 3 moguce opcije
                                            cout << "Drugi argument mora biti registar,konstanta ili memorijska lokacija";
                                        else // jeste konstanta
                                            addValueToMemory(pomFirstValue, secondValue);
                                    }
                                    else // jeste memorijska lokacija
                                        secondValue = getValueFromMemor(secondValue);
                                    if (secondValue != -1) {
                                        addValueToMemory(pomFirstValue, secondValue);
                                    }
                                    else
                                        cout << "Nema vrijednosti za ovu lokaciju";
                                }
                                else //druga vrijednost jeste registar
                                    addValueToMemory(pomFirstValue, secondValue);

                            }
                            else {//prva vrijednost nije ni registar ni mem lokacija

                                cout << "Prva vrijednost mora biti ili registar ili memorijska lokacija";


                            }
                        }
                        else {// prva vrijednost je registar
                            int secondValue = isSecondArgumentRegister(words[index + 2]);
                            if (secondValue == -1) {//nije registar, moze biti ili mem lokacija ili konstanta
                                secondValue = isArgumentMemoryLocation(words[index + 2]);
                                if (secondValue == -1) {// nije mem lokacija, moze biti samo konstanta
                                    secondValue = isArgumentNumber(words[index + 2]);
                                    if (secondValue == -1)//nije ni jedno od 3 moguce opcije
                                        cout << "Drugi argument mora biti registar,konstanta ili memorijska lokacija";
                                    else // jeste konstanta
                                        *firstArgument = secondValue;

                                }
                                else { // jeste memorijska lokacija
                                    secondValue = getValueFromMemor(secondValue);
                                    if (secondValue != -1) {
                                        *firstArgument = secondValue;
                                    }
                                    else
                                        cout << "Nema vrijednosti za ovu lokaciju";
                                }
                            }
                            else //druga vrijednost jeste registar
                                *firstArgument = secondValue;
                        }

                    }
                    else if (words[index] == "SCAN") {

                        string inputString="";
                        int memoryAddress = isArgumentMemoryLocation(words[index + 1]);
                        if (memoryAddress != -1) {
                            cout << "Unesite string za memorijsku lokaciju "<< words[index+1] << endl;
                            getline(cin, inputString);
                            for (int i = 0; i < inputString.size(); i++) {
                               // cout << "Znak: " << inputString[i] << endl;
                                addValueToMemory(memoryAddress, int(inputString[i]));//cuva kao ascii
                                memoryAddress++;
                            }
                        }
                        else {
                            cout << "Neispravan unos, argument mora biti memorijska adresa"<<endl;
                        }
                    }
                    else if (words[index] == "PRINT") {
                        int memoryAddress = isArgumentMemoryLocation(words[index + 1]);
                        if (memoryAddress != -1) {
                            cout << "Na lokaciji " << words[index+1]<<" se nalazi: ";
                            printValuesFromMemory(memoryAddress);

                        }
                        else {
                            cout << "Neispravan unos, argument mora biti memorijska adresa" << endl;
                        }
                    }
                    if (words[index] == "JMP") {
                        jumpLabel = words[index + 1];
                        jumpLabel += ":";
                        jmp = true;
                    }
                    if (words[index] == "CMP") {
                        unsigned long long firstArgumentCmp = isSecondArgumentRegister(words[index + 1]);
                        if (firstArgumentCmp == -1) {//nije registar
                            firstArgumentCmp = isArgumentNumber(words[index + 1]);
                            if (firstArgumentCmp == -1) {
                                cout << "Prvi argument poslije cmp mora biti registar ili broj" << endl;
                            }
                        }
                        unsigned long long secondArgumentCmp = isSecondArgumentRegister(words[index + 2]);
                        if (secondArgumentCmp == -1) {// nije registar
                            secondArgumentCmp = isArgumentNumber(words[index + 2]);
                            if (secondArgumentCmp == -1) {
                                cout << "Drugi argument poslije cmp mora biti registar ili broj"<<endl;
                            }
                        }
                        getline(file, line);
                        index = 0;
                        words = getArgumentsFromLine(line);
                        if (words[index] == "JE" && firstArgumentCmp == secondArgumentCmp) {
                            jumpLabel = words[index + 1];
                            jumpLabel += ":";
                            jmp = true;
                        }
                        else if (words[index] == "JNE" && firstArgumentCmp != secondArgumentCmp) {

                            jumpLabel = words[index + 1];
                            jumpLabel += ":";
                            jmp = true;
                        }
                        else if (words[index] == "JGE" && firstArgumentCmp >= secondArgumentCmp) {

                            jumpLabel = words[index + 1];
                            jumpLabel += ":";
                            jmp = true;
                        }
                        else if (words[index] == "JL" && firstArgumentCmp < secondArgumentCmp) {

                            jumpLabel = words[index + 1];
                            jumpLabel += ":";
                            jmp = true;
                        }
                        else {
                           // cout << "Nije nista ispunjeno" << endl;
                        }
                    }
                }
                else {
                    cout << "Ako se ne radi o labeli, broj argumenata mora biti 2 ili 3" << endl;
                    break;
                }
            }
            else {
                break;
            }
        }
        cout << endl;
        cout << "FINALNO STANJE" << endl;
        printRegisters();
    }
    else
    cout << "Greska pri otvranju fajla"<<endl;

    file.close();
    

}





























