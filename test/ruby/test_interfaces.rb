require 'fipa-message'
require 'minitest/autorun'

include FIPA

class FipaMessageTest < Minitest::Test

    attr_accessor :msg

    def setup
        @msg = FIPA::ACLMessage.new

        msg.setPerformative(:request)
        msg.setProtocol("test-protocol")
        msg.setLanguage("test-language")
        msg.setContent( "test-content" )
        msg.addReceiver(FIPA::AgentId.new("test-to"))
        msg.setSender(FIPA::AgentId.new("test-from"))
        msg.addReplyTo(FIPA::AgentId.new("test-reply-to"))
        msg.setConversationID("test-conversation-id")
    end

    def test_UserdefinedParameter
        name = "user-def-param"
        value = "user-def-value"
        param = UserDefinedParameter.new
                param.setName name
        param.setValue value

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
            resolver = AgentId.new("resolver-#{count}")
            # add an inner resolver
            iResolver = AgentId.new("inner-resolver-#{count}")
            resolver.addResolver(iResolver)

            resolvers[count] = resolver
        end

        # This is the single object to start with
        aid = AgentId.new(name)
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
        addresses.each do |address|
                        assert(aidAddresses.include?(address), "Address: #{address} included")
        end

        count = 0
        aidResolvers = aid.getResolvers
        aidResolvers = aidResolvers.reverse

        resolvers.each do |resolver|
                        assert(aidResolvers.include?(resolver), "aidResolver should contain: #{resolver.getName} was found")
                end
    end

    def test_FipaMessage
        performative = :inform
        msg = ACLMessage.new
        msg.setPerformative(performative)

        10.times do |count|
            agent = AgentId.new("receiver-#{count}")
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

                msg.clearReceivers
                assert(msg.getReceivers.empty?, "Receivers should be empty after calling clearReceivers")
                require 'time'
                e_replyByTime = Time.parse("2017-01-01")
                msg.setReplyBy  e_replyByTime
                assert(msg.getReplyBy == e_replyByTime, "ReplyBy #{e_replyByTime} expected: got #{msg.getReplyBy}")

    end

    def test_ACLEnvelope
        msg
        env = FIPA::ACLEnvelope.new
        representation = FIPARepresentation::BITEFFICIENT
        env.insert(msg, representation)

        assert(env.getBaseEnvelope.getDate, "Default envelope has current date: was #{env.getBaseEnvelope.getDate}")

        time = Time.parse("2017-01-01")
        envelope = ACLBaseEnvelope.new
        envelope.setDate time
        assert(envelope.getDate == time, "Envelope has given date was #{envelope.getDate}")

        env.addExtraEnvelope envelope
        assert(env.flattened.getDate == time, "Flattened envelope has given date, was: #{env.flattened.getDate}")

        mta0 = AgentId.new("mta0")
        mta1 = AgentId.new("mta1")
        env.stamp mta1

        assert(!env.hasStamp?(mta0), "Envelope has no stamp from mta0")
        assert(env.hasStamp?(mta1), "Envelope has stamp from mta1")
    end

    def test_ACLBaseEnvelope
        env = ACLBaseEnvelope.new
        e_receivers = []
        2.times do |idx|
            e_receivers << AgentId.new("receiver-#{idx}")
        end
        env.setTo e_receivers

        e_sender = AgentId.new("sender")
        env.setFrom e_sender

        receivers = env.getTo
        assert(receivers == e_receivers, "All receivers: #{receivers}")
        assert(!receivers.include?(e_sender), "All receivers are not sender")

        sender = env.getFrom
        assert(sender == e_sender, "Senders: #{sender}")
    end

    def test_InterfaceCalls
        msg = ACLMessage.new
        msg.setPerformative :request
        msg.getPerformative
        msg.setContent ""
        msg.getContent
    end

    def test_ByteVector
        msg = ACLMessage.new
        msg.setPerformative :inform
        msg.setContent ""
        content = msg.to_byte_array
        p content
    end
end

