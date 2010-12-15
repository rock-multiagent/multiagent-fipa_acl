require 'fipa-message/FipaMessage'

module FipaUtils

    class << self

	# Deserialize content into
        # a FipaMessage	
	def deserialize(content)
		msg = FipaMessage.new
		msg.from_byte_string content

		return msg
	end

	# Serialize a FipaMessage in into
        # a byte vector
	def serialize(msg)
		content = msg.to_byte_array
		content = content.pack("C*")
		return content
	end
    end
end
