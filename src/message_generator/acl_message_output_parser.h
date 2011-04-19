    /**
    *
    * \file acl_message_output_parser.h
    * \author Mircea Cretu Stancu
    * \brief Encodes a given ACLMessage according to the fipa Bit-Efficent encoding speciffication(FIPA at http://www.fipa.org).
    * 
    * \version 1.0
    *  - can encode any message accordingly
    *  - as future practical requirements may demand more functionality may be added; currently, some productions of the encoding grammar are not  implemented as I could not identify the practical restraints and scenarios that may demand them;
    *  - as more of the fipa multi-agent systems module speciffications will be implemented, these productions may need to be implemented
    *  - for the actual places where the full functionality has not been implemented see the function comments in the .cpp file
    */
    #ifndef ACLMessageOutputParser_H_
    #define ACLMessageOutputParser_H_
    #include <string>
    #include <fipa_acl/message_generator/agent_id.h>
    #include <fipa_acl/message_generator/userdef_param.h>
    #include <fipa_acl/message_generator/acl_message.h>
    #include <fipa_acl/message_generator/exception.h>

    namespace fipa {

    namespace acl {
        
        //extern bool operator== (UserdefParam &a,UserdefParam &b);
        //extern bool operator== ( AgentID &a,  AgentID &b);
        //extern bool operator== ( ACLMessage &a,  ACLMessage &b);
        
        /**
	    \class ACLMessageOutputParser
	    \brief encodes and ACLMessage class object according to the fipa spec(SC00069) all pointer extractin/passing methods are shallow(only the references passed); this is ok as they are not modiffied in the process of encoding; if this changes as utility increases(objects will be modified) then change to deep-copy retreival should be trivial, with the overloaded operators
        */
    class ACLMessageOutputParser {

        private:
    /**
	  @msg message to be parsed

	  @useCodeTables flag to determine whether we use code tables or not
	  * sice the code tables are not implemented yet, it's value is initialized with 0 and is not to be changed

	  @updateCodeTables flag to determine whether we update the code tables
	  * it does not have any practical relevance unless useCodeTables = 1

	  @version string that keeps the version of the encoder
	  * needed by the speciffication; initialized to "1.0"
	  * important restraint: as the speciffication states the version must be of strictly 2 digits;
	  
	  @res_depth a control variable that speciffies the depth of speciffing resolvers when encoding AgentID's
	  * see speciffication for details 
    */
	      ACLMessage msg;
	      int useCodeTables;
	      int updateCodeTables;
	      std::string version;
	      int res_depth;
	      
        public:
		    ACLMessageOutputParser();
		    /**
		        \brief setter and getter methods for the parameters of the encoder
		    */
		    void setUseCodeTables(int x);
		    int getUseCodeTables();
		    void setUpdateCodeTables(int x);
		    int getUpdateCodeTables();
		    void setResolverDepth(int res);
		    int getResolverDepth();
		    void setVersion(std::string);
		    std::string getVersion();
		    /**
		        \brief not a deep-copy assignment of msg but this should not be a problem as fields are not modified in the encoding process
		    */
		    void setMessage(const ACLMessage &a);
		    
		    /**
		        \brief prints the parsed message to an ofstream given as argument(as a string)
		        \param stream: name of the desired ofstream
		    */
		    int printParsedMessage(std::string stream);		
		    
		    /**
		       \brief  main function in the production tree, returns the encoded message as a sequence of bytes kept in a string object
		       \return string containing the encoded message
		       \throw MessageGeneratorException when the message could not be created correctly
		    */
		    std::string getBitMessage();
		    /**
		        \brief encodes an AgentID instance
		        
		        as some fields are optional the method first checks for their existance
		        it also keeps record of the res_depth
		        
		        \param aid: AgentID object pointer to be encoded
		        \param depth: depth of the resolver tree to be encoded
		        \return the encoded agentAID as string
		    */
		    std::string  getBitAID(AgentID aid, int depth);
		    
        
        private:
	      /**
		\brief returns the very used end-of-collection marker
	      */
	      char getBitEndOfColl();
	      /**
		\brief puts the header toghether
	      */
	      std::string  getBitHeader();
	      /**
		\brief depending on the flags it sets the ID
	      */
	      char  getBitMessageID();
	      /**
		\brief gets the message version in one byte
		
		the message version has a very specific format: digit+dot+digit
	      */
	      char  getBitMessageVersion();
	      /**
		\brief encodes the message performative;
		it compares the string of the message performative to all the predefined ones; if it is one of them it returns accordingly if not it returns the 
		custom performative
	      */
	      std::string  getBitMessageType();
	      /**
		\brief quite frequently used production;
		* currently it does not have complete functionality: the code tables need to be implemented in order to use them
	      */
	      std::string  getBitBinWord(std::string sword);
	      /** \brief puts together all the message parameters */
	      std::string  getBitMessageParameters();
	      /**	
		\brief checks all predefined message parameters whether they are set or not and it encodes them accordingly
	      */
	      std::string  getBitPredefMessageParams();
	      /**
		parses all the user defined parameters of the message if any
	      */
	      std::string  getBitUserdefMessageParams();
	      /**
		\brief parses a set of user defined parametrs;
		different from the getBitUserdefMessageParams() method in that there is more general; the former is to be called only for message parameters;
		difference was imposed by the specification	
	      */
	      std::string  getBitUserdefParams(std::vector<UserdefParam> params);
	      /**
		\brief parses one user defined parameter
		the specification does not differentiate at this level between message parameters and other kinds so this method is called by both
	      */
	      std::string  bitParseParam(UserdefParam p);
	      /**
		\brief currently used to encode the addresses of the AgentID instances(strings)
	      */
	      std::string  getBinURLCol(std::vector<std::string> adrr);
	      /**
		\brief basically a wrapper function of the getBitAIDColl(), to add the required specific flag
	      */
	      std::string  getBitResolvers(std::vector<AgentID> aids,int depth);
	      /**
		\brief parses a set of AgentID instances
		the resolvers depth variable that is being passed around is not modified in this function
	      */
	      std::string  getBitAIDColl(std::vector<AgentID> aids, int depth);
	      /**
		\brief implements the binary expression production of the grammar; not complete(w.r.t. the specification) in functionality
		implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions may/will be used so only a few of the productions were implemented(for the binary expression) and the char parameter was added to choose between them, as no other decission maker/constraint was identified
	      */
	      std::string  getBitBinExpression(std::string sword,char c);
	      /**
		\brief overloaded version of the above function
	      */
	      std::string  getBitBinExpression(double n,char base);
	      /**
		\brief parses a number according to the specification(see comment 9 of the specification)
		for ease it is turned into a string and passed on
	      */
	      std::string  getBitBinNumber(double n,char base);
	      /**
		\brief implements the binary string production; functionality not complete as code tables are not yet implemented
	      */
	      std::string  getBitBinString(std::string sword);

              /**
              * Retrieve the byte length encoded string for the given word 
              * \param sword String containing the data to convert
              */
              std::string getByteLengthEncodedString(std::string sword);

	      /**
		\brief implements the binary string production; functionality not complete as code tables are not yet implemented
		the second argument is an explicit option for codeTables(needed for the getBitBinExpression() )
	      */
	      std::string  getBitBinString(std::string sword,int codeTables);
	      std::string  getBitDigits(std::string dig);
	      /**
		\brief implements the date time token production of the grammar; not complete(w.r.t. the specification) in functionality 
		implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions may/will be used so only a few of the productions were implemented and the char parameter was added to choose between them, as no other decission maker/constraint was identified
	      */
	      std::string  getBitBinDateTimeToken(std::string date1);
	      /**
		\brief takes the string representing the date and passes it's digits 2 by 2(as length 2 sugstrings) to the byte encoding function
		it did not explicitly specify but it was induced from the way it was stated that the date is to be encoded as a coded number(comment 9 of the specification)
	      */
	      std::string  getBitBinDate(std::string date1);
	      /**
		\brief implements a coded number passed as a string
		it goes through it digit by digit(char by char)
	      */
	      std::string  getBitCodedNumber(std::string cn);
	      /**
		\brief different version of the above function needed for parsing the date string
		because the date string is passed as substrings and then concatenated back toghether in the caller function, the above function would not perform as desired(it adds a padding 0x00 byte after each substring)
	      */
	      std::string  getBitCodedNumberByte(std::string cn);
		    
		    

    };


    }//end of acl namespace

    }// end of fipa namespace
    #endif
