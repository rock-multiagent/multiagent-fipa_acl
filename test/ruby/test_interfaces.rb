require 'fipa-message'
require 'test/unit'


class FipaMessageTest < Test::Unit::TestCase

	def test_UserdefinedParameter
		name = "user-def-param"
		value = "user-def-value"
		param = FipaUserDefinedParameter.new(name)
		param.setValue(value)

		assert_equal(name, param.getName)	
		assert_equal(value, param.getValue)
	end

	def test_AgentId
		name = "agent-id"
		addresses = []
		10.times do |count|
			addresses[count] = "address-#{count}"
		end

		resolvers = []
		10.times do |count|
			resolver = FipaAgentId.new("resolver-#{count}")
			# add an inner resolver
			iResolver = FipaAgentId.new("inner-resolver-#{count}")
			resolver.addResolver(iResolver)

			resolvers[count] = resolver
		end

		# This is the single object to start with
		aid = FipaAgentId.new(name)
		addresses.each do |address|
			aid.addAddress(address)
		end

		resolvers.each do |resolver|
			aid.addResolver(resolver)
		end

		# now check the object
		aidAddresses = aid.getAddresses
		aidAddresses = aidAddresses.reverse
		count = 0
		aidAddresses.each do |address|
			#puts "#{address}"
			assert_equal("address-#{count}",address)
			count += 1
		end
		
		count = 0
		aidResolvers = aid.getResolvers
		aidResolvers = aidResolvers.reverse

		aidResolvers.each do |resolver|
			#puts "#{resolver.getName}"
			assert_equal("resolver-#{count}",resolver.getName)
			count += 1
		end
	end

	def test_FipaMessage
		performative = "inform"
		msg = FipaMessage.new
		msg.setPerformative(performative)

		10.times do |count|
			agent = FipaAgentId.new("receiver-#{count}")
			msg.addReceiver(agent)
		end
	
		assert_equal(msg.getPerformative, performative)	

		count = 0
		receivers = msg.getReceivers
		receivers = receivers.reverse
		receivers.each do |rcv|
			assert_equal("receiver-#{count}", rcv.getName)
			count += 1
		end

	end

	def test_InterfaceCalls
 		msg = FipaMessage.new
		msg.setPerformative ""
 		msg.getPerformative
		msg.setContent ""
		msg.getContent
	end

	def test_ByteVector
		msg = FipaMessage.new
		msg.setPerformative "inform"
		msg.setContent ""
		content = msg.to_byte_array
		p content
	end

end

