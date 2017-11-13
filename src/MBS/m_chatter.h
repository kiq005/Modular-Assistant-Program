/*
 * This Chatterbot Module is in part based on the model created 
 * by Gonzales Cenelia, that you can find in:
 * http://ai-programming.blogspot.com.br/2013/05/tutorial-on-making-artificial.html
 */

/* 5 */
#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <ctime>
#include <vector>
#include <string>
#include <locale>
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <map>

class CResponse {
public:
	CResponse() {}
	~CResponse() {}

	void addContext(std::string str) 
	{
		ListOfContext.push_back(str);
	}

	void addResp(std::string str)
	{
		ListOfResp.push_back(str);
	}

	vstring getContextList() const 
	{
		return ListOfContext;
	}

	vstring getRespList() const 
	{
		return ListOfResp;
	}

	void clear()
	{
		ListOfResp.clear();
		ListOfContext.clear();
	}

private:
	vstring ListOfContext;
	vstring ListOfResp;
};

typedef std::map<std::string, std::vector<CResponse> > mapString;

typedef struct {
	std::string first;
	std::string second;
} transpos_t;

class ChatterModule : public Module{
public:
	ChatterModule (std::string str)
		:m_sBotName(str), m_sInput("null")
	{
		srand((unsigned) time(NULL));
		transposListSize = sizeof(transposList) / sizeof(transposList[0]);
		loadDatabase();
	}

	void destroy(){
		save_unknown_input();
		active = false;
	}

	std::string input(std::string inp){
		// Short term memory

		// Get input
		save_prev_input();
		m_sInput = inp;
		preprocess_input();
		// Log
		save_log("USER");
		// Get response
		respond();

		return m_sResponse;
	}
private:
	/* Save the unknown input to latter implementation */
	void save_unknown_input()
	{
		std::fstream fout(datapath("chatter_unknown"), std::ios::out);
		if(fout.fail())
		{
			ERROR("Unlable to save Unknown Input List");
			return;
		}

		vstring::const_iterator iter = ListOfUnknownInput.begin();
		for(; iter != ListOfUnknownInput.end(); ++iter)
			fout << *iter << std::endl;

		fout.flush();
		fout.close();
	}
	/* Save Log */
	void sabe_log()
	{
		time_t ltime;
		time(&ltime);
		logfile.open(logpath("chatter.log"), std::ios::out | std::ios::app);

		if(logfile.fail())
		{
			ERROR("Can't save conversation log.");
			return;
		}

		logfile << "\n\nConversation log - " << ctime(&ltime) << std::endl;
	}

	void save_log(std::string str)
	{
		if(str == "CHATTERBOT")
		{
			logfile << m_sResponse << std::endl;
		}
		else if(str == "USER") 
		{
			logfile << ">" << m_sInput << std::endl;
		}
	}
	/* Load DB */
	void loadDatabase()
	{
		std::fstream fin(datapath("chatter_english.txt"), std::ios::in);
		if(fin.fail())
		{
			ERROR("Unable to load the database: chatter_english.txt");
			return;
		}

		CResponse respObj;
		std::vector<CResponse> ListOfRespObj;

		std::string buffer;
		vstring keyList;

		while(std::getline(fin, buffer))
		{
			char cSymbol = buffer[0];
			buffer.erase(0, 1);
			buffer.erase(buffer.end()-1, buffer.end());// Erases \n

			switch(cSymbol)
			{
			case 'K':
				keyList.push_back(buffer);
				break;
			case 'C':
				respObj.addContext(buffer);
				break;
			case 'R':
				respObj.addResp(buffer);
				break;
			case '%':
				ListOfRespObj.push_back(respObj);
				respObj.clear();
				break;
			case '#':
				if(ListOfRespObj.empty())
					ListOfRespObj.push_back(respObj);

				vstring::const_iterator iter = keyList.begin();
				for( ; iter != keyList.end(); ++iter)
				{
					KnowledgeBase[*iter] = ListOfRespObj;
				}
				keyList.clear();
				ListOfRespObj.clear();
				respObj.clear();
				break;
			}
		}

		fin.close();
	}

	/* Randomic select a response from the response list */
	void select_response() {
		if(bot_understand())
		{
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::shuffle(ListOfResponse.begin(), ListOfResponse.end(), std::default_random_engine(seed));
			m_sResponse = ListOfResponse[0];
			/* Just use a * answer when find the subject */
			while(ListOfResponse.size() > 0 and m_sResponse.find("*") != std::string::npos and not find_subject()){
				// extracting from input
				ListOfResponse.erase(ListOfResponse.begin());
				m_sResponse = ListOfResponse[0];
			}
		}
	}
	/* Save the previous input */
	void save_prev_input() {
		m_sPrevInput = m_sInput;
	}
	/* Save the previous response */
	void save_prev_response() {
		m_sPrevResponse = m_sResponse;
	}
	/* Save the previous event */
	void save_prev_event() {
		m_sPrevEvent = m_sEvent;
	}
	/* Set the current event */
	void set_event(std::string str) {
		m_sEvent = str;
	}
	/* Backup the current input */
	void save_input() {
		m_sInputBackup = m_sInput;
	}
	/* Set the current input */
	void set_input(std::string str) {
		m_sInput = str;
	}
	/* Restore the backuped input */
	void restore_input() {
		m_sInput = m_sInputBackup;
	}
	/* */
	void save_bot_response() {
		if(!m_sResponse.empty()) vResponseLog.push(m_sResponse);
	}
	void update_unkown_input_list() {
		ListOfUnknownInput.push_back(m_sInput);
	}

	size_t findRespPos(const std::string &str) const
	{
		size_t pos = 0;
		std::stack<std::string> s = vResponseLog;
		while(s.empty())
		{
			++pos;
			if(s.top() == str)
			{
				break;
			}
			s.pop();
		}
		return pos;
	}

	/* Check if the bot is repeating himself */
	bool bot_repeat() const {
		size_t pos = findRespPos(m_sResponse);
		if(pos > 0)
			return pos + 1 < ListOfResponse.size();
		return false;
	}
	/* Check if the user is repeathing himself */
	bool user_repeat() const {
		return (m_sPrevInput.length() > 0 &&
			( (m_sInput == m_sPrevInput) || 
			  (m_sInput.find(m_sPrevInput) != std::string::npos) ||
			  (m_sPrevInput.find(m_sInput) != std::string::npos) ) );
	}
	/* Check if the bot has a answer */
	bool bot_understand() const {
		return ListOfResponse.size() > 0;
	}
	/* Check if the input is empty */
	bool null_input() const {
		return (m_sInput.length() == 0 && m_sPrevInput.length() != 0);
	}
	/* Check if the user is sending null inputs */
	bool null_input_repetition() const {
		return (m_sInput.length() == 0 && m_sPrevInput.length() == 0);
	}
	/* Check if the event has changed */
	bool same_event() const {
		return (m_sEvent.length() > 0 && m_sEvent == m_sPrevEvent);
	}
	/* Check if there is no responses */
	bool no_response() const {
		return ListOfResponse.size() == 0;
	}
	/* Check if the input has changed */
	bool same_input() const {
		return (m_sInput.length() > 0  && m_sInput == m_sPrevInput);
	}
	/* Check if there is a similar input */
	bool similar_input() const {
		return (m_sInput.length() > 0 && 
			(m_sInput.find(m_sPrevInput) != std::string::npos ||
			m_sPrevInput.find(m_sInput) != std::string::npos));
	}

	void find_match(){
		ListOfResponse.clear();
		// Keyword hanking
		std::string bestKeyWord;
		vstring ListOfWord;

		if(m_sInput.find("**") == std::string::npos)
		{
			trimRight(m_sInput, ".");
			UpperCase(m_sInput);
			tokenize(m_sInput, ListOfWord);
			bestKeyWord = findBestKey(ListOfWord);
			m_sKeyWord = bestKeyWord;
		}
		else
		{
			m_sKeyWord = m_sInput;
		}

		if(KnowledgeBase.find(m_sKeyWord) != KnowledgeBase.end() )
		{
			std::vector<CResponse> ListOfRespObj = KnowledgeBase[m_sKeyWord];
			extractRespList(ListOfRespObj);
		}
	}

	std::string findBestKey(vstring v)
	{
		std::string buffer;
		int iSize = v.size();
		bool bKeyFound = false;
		//SAY("Looking for the best key...");
		for( int i = 0, j = iSize; i < iSize && j >= 1; ++i, --j)
		{
			for( int k = 0; (k + j) <= iSize; ++k )
			{
				buffer = get_sub_phrase(v, k, k + j);
				//if(KnowledgeBase.find(buffer) != KnowledgeBase.end())
				if(KnowledgeBase.count(buffer)>0)
				{
					bKeyFound = true;
				}
				else
				{
					preprocess_keyword(buffer, k, k + j, iSize);
					//if(KnowledgeBase.find(buffer) != KnowledgeBase.end())
					if(KnowledgeBase.count(buffer)>0)
					{
						bKeyFound = true;
					}
				}
				if(bKeyFound)
					return buffer;
			}
		}
		
		//return buffer;
		return "BOT DON'T UNDERSTAND**";
	}

	void copy(std::string *arr, vstring &v, size_t arr_size){
		for(size_t i = 0; i < arr_size; ++i)
		{
			if( not arr[i].empty())
				v.push_back(arr[i]);
			else
				break;
		}
	}

	void cleanString(std::string &str){
		int len = str.length();
		std::string temp = "";

		char prevChar = 0;

		for(int i=0; i < len; ++i){
			if( (str[i] == ' ' and prevChar != ' ') or not isPunc(str[i]) ) 
			{
				temp += str[i];
				prevChar = str[i];
			}
			else if (prevChar != ' ' and not isPunc(str[i]) )
			{
				temp += ' ';
			}
		}
		str = temp;
	}

	void UpperCase(std::string &str){
		std::locale loc;
		std::string::size_type len = str.length();
		for(std::string::size_type i = 0; i<len; ++i)
			m_sInput[i] = std::toupper(m_sInput[i], loc);
	}

	void preprocess_input()
	{
		// Remove extra spaces and punctuations
		cleanString(m_sInput);
		trimRight(m_sInput, ". ");
		// Upercase
		UpperCase(m_sInput);
		// Add spaces at begin and end
		//insert_space(m_sInput);
	}

	void preprocess_response()
	{
		if(m_sResponse.find("*") != std::string::npos)
		{
			// conjugating subject
			transpose(m_sSubject);
			replace(m_sResponse, "*", " " + m_sSubject);
		}
	}

	void preprocess_keyword(std::string &str, size_t start_pos, size_t end_pos, size_t iSize)
	{
		if(start_pos == 0)
			str.insert(0, "_");
		if(end_pos == iSize - 1 )
			str.insert(str.length(), "_");
	}

	void extractRespList(std::vector<CResponse> objList)
	{
		std::vector<CResponse>::const_iterator iter = objList.begin();
		for( ; iter != objList.end(); ++iter )
		{
			vstring ListOfContext = iter->getContextList();
			if(ListOfContext.empty())
			{
				ListOfResponse = iter->getRespList();
			}
			else if(std::find(ListOfContext.begin(), 
				ListOfContext.end(), m_sPrevResponse) != ListOfContext.end())
			{
				ListOfResponse = iter->getRespList();
				break;
			}
		}
	}

	bool find_subject()
	{
		m_sSubject.erase();// Reset the subject
		trimRight(m_sInput, " ");
		trimLR(m_sKeyWord, "_");
		size_t pos = m_sInput.find(m_sKeyWord);
		if(pos != std::string::npos)
		{
			m_sSubject = m_sInput.substr(pos + m_sKeyWord.length(), m_sInput.length());
		}
		return (m_sSubject.size() > 0);
	}

	/* Sentence Transposition */
	void transpose( std::string &str )
	{
		std::string buffer = " " + str + " ";
		bool bTransposed = false;

		for(size_t i=0; i < transposListSize; ++i)
		{
			std::string first = transposList[i].first;
			std::string second = transposList[i].second;

			while(replace(str, first, second) != std::string::npos)
			{
				bTransposed = true;
			}
		}

		if( not bTransposed)
		{
			for( size_t i =0; i < transposListSize; ++i)
			{
				std::string first = transposList[i].first;
				std::string second = transposList[i].second;
				while(replace(str, second, first) != std::string::npos) ;
			}
		}
		trimLR(buffer, " ");
		str = buffer;
	}

	std::string get_sub_phrase(vstring wordList, size_t start, size_t end_pos)
	{
		std::string buffer;
		for(size_t i = start; i <  end_pos; ++i)
		{
			buffer += wordList[i];
			if( i != end_pos-1)
				buffer += " ";
		}
		return buffer;
	}

	/* Determins if the keyword position is correct */
	bool wrong_location(std::string keyword, char firstChar, char lastChar, size_t pos){
		bool bWrongPos = false;
		pos += keyword.length();
		if( (firstChar == '_' and lastChar == '_' and m_sInput != keyword) or
			(firstChar != '_' and lastChar == '_' and pos != m_sInput.length()) or
			(firstChar == '_' and lastChar != '_' and pos == m_sInput.length()) )
		{
			bWrongPos = true;
		}
		return bWrongPos;
	}

	/* Handle context */
	// TODO: Right context not working!
	bool wrong_context(vstring contextList)
	{
		bool bWrongContext = true;
		if(contextList.size() == 0)
		{
			bWrongContext = false;
		}
		else
		{
			std::string sContext = m_sPrevResponse;
			cleanString(sContext);
			for(size_t i = 0; i < contextList.size(); ++i)
			{
				if(contextList[i] == sContext)
				{
					m_sPrevContext = m_sContext;
					m_sContext = sContext;
					bWrongContext = false;
					break;
				}
			}
		}
		if(m_sPrevContext.length() > m_sContext.length())
		{
			bWrongContext = true;
		}
		return bWrongContext;
	}
	

	void respond(){
		// Memory
		save_prev_response();
		// Define event
		set_event("BOT UNDERSTAND**");

		if(null_input())
		{
			//SAY("NUll input...");
			handle_event("NULL INPUT**");
		}
		else if(null_input_repetition())
		{
			//SAY("Null repetition...");
			handle_event("NULL INPUT REPETITION**");
		}
		else if(user_repeat())
		{
			//SAY("User repeating...");
			handle_user_repetition();
		}
		else
		{
			//SAY("Looking for matches...");
			find_match();
		}

		if( not bot_understand())
		{
			//SAY("I don't know...");
			handle_event("BOT DONT UNDERSTAND**");
			update_unkown_input_list();
		}

		if(ListOfResponse.size() > 0){
			//SAY("Has answer!");
			select_response();
			save_bot_response();
			preprocess_response();
			if( bot_repeat() ){
				handle_repetition();
			}
			save_log("CHATTERBOT");
		}

	}

	void handle_repetition(){
		std::map<int, std::string> ListOfPrevResponse;
		if( ListOfResponse.size() > 1){
			std::stack<std::string> s = vResponseLog;
			vstring::const_iterator iter = ListOfResponse.begin();
			for( ; iter != ListOfResponse.end(); ++iter )
			{
				int pos = findRespPos(*iter);
				ListOfPrevResponse[pos] = *iter;
			}
		}
		std::map<int, std::string>::const_iterator iter2 = ListOfPrevResponse.end();
		--iter2;
		m_sResponse = iter2->second;
	}

	void handle_user_repetition(){
		if(same_input())
			handle_event("REPETITION T1**");
		else if(similar_input())
			handle_event("REPETITION T2**");
	}

	void handle_event(std::string str){
		//SAY("Handling events...");
		// Event
		save_prev_event();
		set_event(str);
		// Input
		save_input();
		set_input(str);
		
		// Treat
		if(not same_event())
			find_match();
		// Restore
		restore_input();
	}

private:

 size_t transposListSize;
	std::string			m_sBotName;
	std::string			m_sUserName;
	std::string			m_sInput;
	std::string			m_sResponse;
	std::string			m_sPrevInput;
	std::string			m_sPrevResponse;
	std::string			m_sEvent;
	std::string			m_sPrevEvent;
	std::string			m_sInputBackup;
	std::string			m_sSubject;
	std::string			m_sKeyWord;
	std::string			m_sContext;
	std::string			m_sPrevContext;

	vstring				ListOfResponse;
	vstring				ListOfUnknownInput;

	std::stack<std::string>		vResponseLog;
	mapString			KnowledgeBase;
	std::fstream		logfile;

transpos_t transposList[17] = {
	{" MYSELF ", " YOURSELF "},
	{" DREAMS ", " DREAM "},
	{" WEREN'T ", " WASN'T "},	
	{" AREN'T ", " AM NOT "},
	{" I'VE ", " YOU'VE "},
	{" MINE ", " YOURS "},
	{" MY ", " YOUR "},
	{" WERE ", " WAS "},
	{" MOM ", " MOTHER "},
	{" I AM ", " YOU ARE "},
	{" I'M ", " YOU'RE "},
	{" DAD ", " FATHER "},
	{" MY ", " YOUR "},
	{" AM ", " ARE "},
	{" I'D ", " YOU'D "},
	{" I ", " YOU "},
	{" ME ", " YOU "}
};


};
