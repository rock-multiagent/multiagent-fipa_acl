#ifndef FIPA_ACL_SERIALIZED_LETTER_H
#define FIPA_ACL_SERIALIZED_LETTER_H

#include <stdint.h>

#include <string>
#include <vector>
#include <base/Time.hpp>
#include <fipa_acl/fipa_acl.h>

namespace fipa {

/**
 * Facilitate transport
 */
struct SerializedLetter 
{
    SerializedLetter();

    /**
     */
    SerializedLetter(const fipa::acl::Letter& letter, fipa::acl::representation::Type representation);

    /**
     * Clear the serialization data
     */
    inline void clear() { representation = fipa::acl::representation::UNKNOWN; data.clear(); }

    /**
     * Retrieve the underlying vector
     */
    inline std::vector<uint8_t> getVector() const { return this->data; }

    /**
     * Set the serialized content
     */
    void setData(const std::string& msg);

    /**
     * Retrieve the size of the serialized content
     */
    inline int size() const { return this->data.size(); }

    /**
     * Get data in string container
     */
    std::string getDataAsString() const;

    /**
     * Retrieve decoded letter
     */
    fipa::acl::Letter deserialize() const;

    fipa::acl::representation::Type representation;
    std::vector<uint8_t> data;
    base::Time timestamp;
};

} // namespace fipa
#endif

