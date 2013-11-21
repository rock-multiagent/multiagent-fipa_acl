#ifndef FIPA_ACL_GRAMMAR_STRING_MESSAGE_H
#define FIPA_ACL_GRAMMAR_STRING_MESSAGE_H

#include <fipa_acl/message_parser/grammar/grammar_common.h>
#include <fipa_acl/message_generator/message_format.h>

namespace fipa {
namespace acl {
namespace grammar {
namespace string {

template<typename Iterator>
struct Resolver : qi::grammar<Iterator, fipa::acl::AgentID()>
{

    Resolver() : Resolver::base_type(agentId, "resolver-string_grammar")
    {
	using phoenix::construct;
	using phoenix::val;

        using namespace fipa::acl;
	namespace label = qi::labels;

        agentId = qi::lit("(agent-identifier")
            >> qi::lit(":name") >> word                           [ phoenix::at_c<0>(label::_val) = label::_1 ]
            //-( << ":addresses" << urlSequence )
            // Leaving out additional resolvers !!! to break recursion
            //*( << userDefinedParam << expression )
            >> ")";
    }

    grammar::Word<Iterator> word;
    qi::rule<Iterator, fipa::acl::AgentID()> agentId;
};

template<typename Iterator>
struct AgentIdentifierSequence : qi::grammar<Iterator,AgentIDList() >
{

    AgentIdentifierSequence() : AgentIdentifierSequence::base_type(agentIdList, "agent_identifier_sequence-string_grammar")
    {
	using phoenix::construct;
	using phoenix::val;

        using namespace fipa::acl;
	namespace label = qi::labels;

        agentIdList = qi::lit("(sequence")
            >> * agentIdentifier [ phoenix::push_back(label::_val, label::_1) ]
            >> ")";
    }

    Resolver<Iterator> agentIdentifier;
    qi::rule<Iterator, fipa::acl::AgentIDList()> agentIdList;
};

template<typename Iterator>
struct AgentIdentifier : qi::grammar<Iterator, fipa::acl::AgentID()>
{

    AgentIdentifier() : AgentIdentifier::base_type(agentId, "agentidentifier-string_grammar")
    {
	using phoenix::construct;
	using phoenix::val;

        using namespace fipa::acl;
	namespace label = qi::labels;

        agentId = qi::lit("(agent-identifier")
            >> qi::lit(":name") >> word                           [ phoenix::at_c<0>(label::_val) = label::_1 ]
            //-( >> ":addresses" >> urlSequence )
            >> -( ":resolvers" >> agentIdSequence                 [ phoenix::at_c<2>(label::_val) = label::_1 ])
            //*( << userDefinedParam << expression )
            >> ")";
    }

    grammar::Word<Iterator> word;
    AgentIdentifierSequence<Iterator> agentIdSequence;
    qi::rule<Iterator, fipa::acl::AgentID()> agentId;

};

template<typename Iterator>
struct AgentIdentifierSet : qi::grammar<Iterator,AgentIDList() >
{

    AgentIdentifierSet() : AgentIdentifierSet::base_type(agentIdList, "agent_identifier_set-string_grammar")
    {
	using phoenix::construct;
	using phoenix::val;

        using namespace fipa::acl;
	namespace label = qi::labels;

        agentIdList = qi::lit("(set")
            >> * agentIdentifier [ phoenix::push_back(label::_val, label::_1) ]
            >> ")";
    }

    AgentIdentifier<Iterator> agentIdentifier;
    qi::rule<Iterator, fipa::acl::AgentIDList()> agentIdList;
};

template<typename Iterator>
struct DateTimeToken : qi::grammar<Iterator, std::string() >
{
    DateTimeToken() : DateTimeToken::base_type(date_time, "date_time_token-string_grammar")
    {
        using encoding::char_;

        date_time = - sign         [ label::_val = label::_1 ]
            >> year                [ label::_val += label::_1 ]
            >> month               [ label::_val += label::_1 ]
            >> day                 [ label::_val += label::_1 ]
            >> char_("T")          [ label::_val += label::_1 ]
            >> hour                [ label::_val += label::_1 ]
            >> minute              [ label::_val += label::_1 ]
            >> second              [ label::_val += label::_1 ]
            >> millisecond         [ label::_val += label::_1 ]
            >> - typeDesignator    [ label::_val += label::_1 ]
        ;

        sign = char_('+')
            | char_('-');

        typeDesignator = char_('a','z')
            | char_('A','Z');

        digit = char_('0','9');

        year = digit
            >> digit
            >> digit
            >> digit;

        month = digit
            >> digit;

        day = month.alias();
        hour = month.alias();
        minute = month.alias();
        second = month.alias();

        millisecond = digit
            >> digit
            >> digit
        ;

    }

    qi::rule<Iterator, std::string()> year;
    qi::rule<Iterator, std::string()> month;
    qi::rule<Iterator, std::string()> day;
    qi::rule<Iterator, std::string()> hour;
    qi::rule<Iterator, std::string()> minute;
    qi::rule<Iterator, std::string()> second;

    qi::rule<Iterator, std::string()> digit;
    qi::rule<Iterator, std::string()> sign;
    qi::rule<Iterator, std::string()> typeDesignator;

    qi::rule<Iterator, std::string()> millisecond;
    qi::rule<Iterator, std::string()> date_time;
};

template<typename Iterator>
struct DateTime : qi::grammar<Iterator, base::Time() >
{
    DateTime() : DateTime::base_type(date_time, "date_time-string_grammar")
    {
        date_time = dateTime [ label::_val = convertToBaseTime(label::_1) ]
        ;
    }

    DateTimeToken<Iterator> dateTime;
    qi::rule<Iterator, base::Time()> date_time;
};

template<typename Iterator>
struct Number : qi::grammar<Iterator, std::string() >
{
    Number() : Number::base_type(number, "number-string_grammar")
    {
        using encoding::char_;

        number = float_number           [ label::_val = label::_1 ]
            | integer_number            [ label::_val = label::_1 ]
        ;

        integer_number = - sign         [ label::_val = label::_1 ]
            >> +digit                   [ label::_val += label::_1 ]
        ;

        sign = char_('+')
            | char_('-');

        digit = char_('0','9');

        float_number = (-sign           [ label::_val = label::_1 ]
                >> float_mantissa       [ label::_val += label::_1 ]
                >> - float_exponent     [ label::_val += label::_1 ]
              )
            | ( -sign                   [ label::_val = label::_1 ]
                >> +digit               [ label::_val += label::_1 ]
                >> float_exponent       [ label::_val += label::_1 ]
              );

        float_mantissa = ( +digit       [ label::_val = label::_1 ]
                >> char_('.')           [ label::_val += label::_1 ]
                >> *digit               [ label::_val += label::_1 ]
              )
            | ( *digit                  [ label::_val = label::_1 ]
                >> char_('.')           [ label::_val += label::_1 ]
                >> +digit               [ label::_val += label::_1 ]
            );

        float_exponent = exponent       [ label::_val = label::_1 ]
            >> - sign                   [ label::_val += label::_1 ]
            >> +digit                   [ label::_val += label::_1 ]
        ;

        exponent = char_('e')
            | char_('E');

    }

    qi::rule<Iterator, std::string()> integer_number;
    qi::rule<Iterator, std::string()> float_number;

    qi::rule<Iterator, std::string()> sign;
    qi::rule<Iterator, std::string()> digit;
    qi::rule<Iterator, std::string()> float_mantissa;
    qi::rule<Iterator, std::string()> float_exponent;
    qi::rule<Iterator, std::string()> exponent;

    qi::rule<Iterator, std::string()> number;
};

template<typename Iterator>
struct Expression : qi::grammar<Iterator,std::string() >
{

    Expression() : Expression::base_type(expression, "expression-string_grammar")
    {
	using phoenix::construct;
	using phoenix::val;

        using namespace fipa::acl;
	namespace label = qi::labels;


        expression = expression_base            [ label::_val = label::_1 ]
            | ( "(" >> *expression              [ label::_val += label::_1 ]
                    >> ")" ) 
        ;

        expression_base = word
            | string
            | number
            | dateTime
        ;
    }

    grammar::Word<Iterator> word;
    grammar::String<Iterator> string;
    Number<Iterator> number;
    DateTimeToken<Iterator> dateTime;

    qi::rule<Iterator, std::string()> expression;
    qi::rule<Iterator, std::string()> expression_base;
};

template<typename Iterator>
struct Message : qi::grammar<Iterator, fipa::acl::ACLMessage()>
{
    Message() : Message::base_type(aclCommunicativeAct, "message-string_grammar")
    {
	using phoenix::construct;
	using phoenix::val;

        using namespace fipa::acl;
        using namespace MessageField;

	// To avoid namespace clashes with boost::bind
	namespace label = qi::labels;

        aclCommunicativeAct = "("
            >> *( qi::lit(":" + MessageFieldTxt[SENDER]) >> agentId           [ phoenix::at_c<0>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[RECEIVER]) >> agentIdList         [ phoenix::at_c<1>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[CONTENT]) >> content              [ phoenix::at_c<2>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[REPLY_WITH]) >> expression        [ phoenix::at_c<3>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[REPLY_BY]) >> dateTime            [ phoenix::at_c<4>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[IN_REPLY_TO]) >> expression       [ phoenix::at_c<5>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[REPLY_TO]) >> agentIdList         [ phoenix::at_c<6>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[LANGUAGE]) >> expression          [ phoenix::at_c<7>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[ENCODING]) >> expression          [ phoenix::at_c<8>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[PROTOCOL]) >> word                [ phoenix::at_c<9>(label::_val) = label::_1 ]
            | qi::lit(":" + MessageFieldTxt[CONVERSATION_ID]) >> expression   [ phoenix::at_c<9>(label::_val) = label::_1 ]
            )
            >> ")";
    }

    AgentIdentifier<Iterator> agentId;
    AgentIdentifierSet<Iterator> agentIdList;
    grammar::String<Iterator> content;
    grammar::Word<Iterator> word;
    DateTime<Iterator> dateTime;
    Expression<Iterator> expression;
    qi::rule<Iterator, fipa::acl::ACLMessage()> aclCommunicativeAct;

};

} // end namespace string
} // end namespace grammar
} // end namespace acl
} // end namespace fipa


#endif //FIPA_ACL_GRAMMAR_STRING_MESSAGE_H
