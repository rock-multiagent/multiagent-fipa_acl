require 'profile'
require File.join(File.dirname(__FILE__),'../../bindings/ruby/lib/fipa-message')

include FIPA

class FipaBenchmark

 @content = nil

 def initialize

   msg = ACLMessage.new
   msg.setPerformative :inform
   content = ""
   for i in 1..1000 do
       content += "a"
   end
   msg.setContent "#{content.to_a}"

   # Simple test
   content = Utils.serialize(msg)
   @content = content

   msg = Utils.deserialize(content)

   file =  File.open("test","w")
   file.write(Marshal.dump(content))
   file.close
 end

 def readMessage
  file = File.open("test","r")
  content = file.read
  file.close
  content = Marshal.load(content)

  if content != @content
      raise "Reading data failed"
  end

  (1..100).each do |i|
     msg = Utils.deserialize(content)
  end

 end

end


fb = FipaBenchmark.new
fb.readMessage

