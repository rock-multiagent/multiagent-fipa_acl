require 'fipamessage_ruby'

module FIPA

class ACLMessage

#
# :method setPerformative
#
# Set a performative for the message can be one of:
#
# :accept_proposal
# :agree
# :cancel
# :call_for_proposal
# :confirm
# :disconfirm
# :failure
# :inform
# :inform_if
# :inform_ref
# :not_understood
# :propagate
# :propose
# :proxy
# :query_if
# :query_ref
# :refuse
# :reject_proposal
# :request
# :request_when
# :request_whenever
# :subscribe
#

end

#
# FipaUtils
module Utils

    class << self
        # Deserialize content into
        # a FipaMessage
        def deserialize_message(data, pack = true)
            if not (data.is_a?(Array) || data.is_a?(String))
                raise "Cannot serialize class type: #{data.class.name}"
            end

            content = data.to_a
            # Prepare the binary string
            if pack
                content = content.pack("C*")
            else
                content = content.to_s
            end

            msg = ACLMessage.new
            msg.from_byte_string content

            return msg
        end

        # Serialize a FipaMessage in into
        # a byte array
        def serialize_message(msg)
             content = msg.to_byte_array
             return content.to_a
        end

        # Deserialize content into
        # a FipaMessage
        def deserialize_envelope(data, pack = true)
            if not (data.is_a?(Array) || data.is_a?(String))
                raise "Cannot serialize class type: #{data.class.name}"
            end

            content = data.to_a
            # Prepare the binary string
            if pack
                content = content.pack("C*")
            else
                content = content.to_s
            end

            envelope = ACLEnvelope.new
            envelope.from_byte_string content

            return envelope
        end

        # Serialize a FIPA::ACLEnvelope into
        # a byte array
        def serialize_envelope(envelope)
            content = envelope.to_byte_array
         return content.to_a
        end
    end
end # End Utils module

end # End FIPA module

