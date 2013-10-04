#ifndef FIPA_ACL_TEST_UTILS_H
#define FIPA_ACL_TEST_UTILS_H

#include <boost/test/auto_unit_test.hpp>
#include <string>

template< template<class> class TIn, typename TOut>
TOut testGrammar(const std::string& storage, bool expectedSuccess = true)
{
    typedef TIn<std::string::const_iterator> grammar_type;
    grammar_type grammar;
    TOut parseTree;

    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    bool r = parse(iter, end, grammar, parseTree);
    if(expectedSuccess)
    {
    	BOOST_REQUIRE_MESSAGE(r, "Test Grammar with string of size: " << storage.size());
    } else {
    	BOOST_REQUIRE_MESSAGE(!r, "Test Grammar with string of size: " << storage.size());
    }
    return parseTree;
}

template< template<class> class TIn, typename TOut>
void testFailGrammar(const std::string& storage)
{
    typedef TIn<std::string::const_iterator> grammar_type;
    grammar_type grammar;
    TOut parseTree;

    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    bool r = parse(iter, end, grammar, parseTree);

    BOOST_REQUIRE(!r);
}
#endif // FIPA_ACL_TEST_UTILS_H
