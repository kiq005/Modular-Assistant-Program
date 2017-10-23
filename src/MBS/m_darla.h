#ifndef __DARLA_MODULE_H__
#define __DARLA_MODULE_H__

#define CONVERSATION_LOG_SIZE 10
#define DICE_INITIAL_PROXIMITY 0.6

#include <unordered_map>
#include <set>

/* Conversation
 * Hold a circular list as a conversation log.
 */
class conversation {
public:

	conversation(int l){
		size = l;
		idx = 0;
		queue = new std::string[l];
	}
	~conversation(){

	}
	// TODO: Create copy constructor

	inline void push(const std::string& str){
		queue[idx] = str;
		idx=(idx+1)%size;
	}

	std::string last(int back){
		int location = idx - back - 1;
		if(location<0)
			location+=size;
		if(location>size)
			location-=size;
		return queue[location];
	}

private:
	/* DEBUG */
	void printqueue(){
		int i;
		std::cout << "----- LOG -----" << std::endl;
		for(i=size-1; i>=0; --i){
			std::cout << i << " - " << last(i) << std::endl;
		}
		std::cout << "----- END -----" << std::endl;
	}


	std::string *queue;
	int			size;
	int			idx;
};

/* Darla - Dynamic Appendable Rules Learning Assistant
 * Darla is a assistant (now it is a module) that answer given inputs based
 * on a database, that uses guided learning to output answers or do tasks.
 * In general, Darla will receive a input, look for it in the procedure 
 * database, and if it is in there, Darla do the indicated procedure(like 
 * run a file, or display a output). 
 * We use commands to guide Darla's Learning:
 * 
 * :: - Indicate a command
 * ::exit - Exit command
 * ::log - Display conversation log
 *
 * ::@xxx - Indicate to append a xxx answer to the last input line.
 * ::>xxx - Indicate that the answer to the last line is the same as the xxx input.
 */
class DarlaModule : public Module {
public:
	DarlaModule(){
		srand (time(NULL));
		cvs = new conversation(CONVERSATION_LOG_SIZE);
		keyword = "";
		load();
	}

	std::string input(std::string inp){
		cvs->push(process_input(inp));
		save();
		return output();
	}
private:
	static inline std::string process_input(std::string &str){
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	void process_output(std::string &str){
		if(str.length()<3)
			return;
		else if(str[0]=='>'){// Appoint to another
			auto got = (database.find(str.substr(1)));
			if(got == database.end()){// Key does not exist
				str = "";
			}
			else{
				str = (got->second)[ rand()%(got->second.size()) ];
				std::cout << "(Appoint to: " << (got->first) << ")" << std::endl;
				process_output(str);
			}
		}
		else if(str[0]=='#'){// Perform command
			std::cout << "(Command: " << (str.substr(1)) << ")" << std::endl;
		}
	}

	std::string output(){
		std::string str;
		str = answer();
		process_output(str);
		return str;
	}

	std::string answer(){
		std::string str = "";
		std::string lastLine = cvs->last(0);

		if( iscommand(lastLine) ){
			str = command(lastLine);
		}
		else if( indatabase(lastLine) ){
			// Obteve resposta na database
			auto got = (database.find(keyword));
			if(got == database.end()){
				str = "Error: darla.answer - in database but not keyword";
			}
			else{
				str = (got->second)[ rand()%(got->second.size()) ];
			}
		}

		return str;
	}

	static inline bool iscommand(const std::string& word){
		if(word.length() < 3)
			return false;
		if(word[0]==':' and word[1]==':')
			return true;
		return false;
	}

	std::string command(std::string cmd){
		std::string str = "";
		if(cmd=="::exit"){
			str = "$EXIT$";
			destroy();
		}
		else if(cmd=="::log")
			outputLog();
		else if(cmd[2]=='@'){// X has a answer Y
			command_append(cmd);
		}
		else if(cmd[2]=='>'){// X appoint to Y
			command_append('>'+cmd);
		}
		else if(cmd[2]=='#'){// X do Y
			command_append('#'+cmd);
		}

		return str;
	}

	void command_append(std::string cmd){
		std::string key;
		std::string val = cmd.substr(3);
		int i=1;
		while( iscommand(cvs->last(i)) ){
			i++;
		}
		key=cvs->last(i);

		if(indatabase(key)){
			auto got = (database.find(keyword));
			(got->second).push_back(val);
		}
		else{
			std::vector<std::string> v;
			v.push_back(val);
			database.emplace(key, v);
		}
		std::cout << "(Appending '" << val << "' to " << key << ")" << std::endl;
	}

	void outputLog(){
		int i;
		std::cout << "----- LOG -----" << std::endl;
		for(i=CONVERSATION_LOG_SIZE-1; i>=0; --i){
			std::cout << i << " - " << cvs->last(i) << std::endl;
		}
		std::cout << "----- END -----" << std::endl;
	}

	bool indatabase(std::string word){
		float dice_max = DICE_INITIAL_PROXIMITY, coef;
		keyword = "";
		for(const auto& key : database){
			if((coef = dice_coefficient(key.first, word))>dice_max){
				dice_max = coef;
				keyword = key.first;
			}
		}

		return (keyword!="");
	}

	/* FONT: 
	 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Dice%27s_coefficient#C.2B.2B
	 * 
	 * dice coefficient = bigram overlap * 2 / bigrams in a + bigrams in b
	 * (C) 2007 Francis Tyers 
	 * Modifications made by Stefan Koshiw 2010
	 * Now it outputs values [0..1]
	 * Released under the terms of the GNU GPL.
	 */
	float dice_coefficient(std::string string1, std::string string2)
	{
        std::set<std::string> string1_bigrams;
        std::set<std::string> string2_bigrams;

        // Base case: equal strings
        if(string1==string2){
        	return 1;
        }
        // Base case: small strings
        if(string1.length() < 3 || string2.length() <3){
            return 0;
        }
        // Extract character bigrams from string1
        for(unsigned int i = 0; i < (string1.length() - 1); i++) {
                string1_bigrams.insert(string1.substr(i, 2));
        }
        // Extract character bigrams from string2
        for(unsigned int i = 0; i < (string2.length() - 1); i++) {
                string2_bigrams.insert(string2.substr(i, 2));
        }
        // Get set's intersections
        int intersection = 0;
        for(std::set<std::string>::iterator IT = string2_bigrams.begin(); 
            IT != string2_bigrams.end(); 
            IT++)
        {    
                intersection += string1_bigrams.count((*IT));
        }
        // Calculate dice coef.
        int total = string1_bigrams.size() + string2_bigrams.size();
        float dice = (float)(intersection * 2) / (float)total;

        return dice;
	}

	const std::vector<std::string> explode(const std::string& s, const char& c)
	{
		std::string buff{""};
		std::vector<std::string> v;
		
		for(auto n:s)
		{
			if(n != c) buff+=n; else
			if(n == c && buff != "") { v.push_back(buff); buff = ""; }
		}
		if(buff != "") v.push_back(buff);
		
		return v;
	}

	void save(){
		std::fstream fout(datapath("darlabase"), std::ios::out);
		if(fout.fail())
			ERROR("Unable to save Darlabase file.");

		for(const auto& key : database){
			fout << key.first << "|";
			for(const auto& val : key.second){
				fout << "@" << val;
			}
			fout << std::endl;
		}

		fout.flush();
		fout.close();
	}

	void load(){
		// Open darlabase
		std::fstream fin(datapath("darlabase"), std::ios::in);
		if(fin.fail())
			ERROR("Unable to load Darlabase files.");
		// Create buffers
		std::string buffer;
		std::vector<std::string> v;
		std::string key;
		std::vector<std::string> val;
		// Iterate over lines
		while(std::getline(fin, buffer)){
			v = explode(buffer, '|');
			key = v[0];
			val = explode(v[1], '@');
			database.emplace(key, val);
		}
	}

	conversation* cvs;
	std::string keyword;
	std::unordered_map<std::string, std::vector<std::string> > database;
};

#endif