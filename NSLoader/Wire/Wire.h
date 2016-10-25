#pragma once
#include <memory>
#include "zmq.hpp"
#include "zmq_addon.hpp"
#include "Locator.h"
#define FLATBUFFERS_DEBUG_VERIFICATION_FAILURE
#include "flatbuffers\flatbuffers.h"
#include "HapticEffect_generated.h"
#include "HapticFrame_generated.h"
#include "HapticPacket_generated.h"
#include "HapticSample_generated.h"
#include "ClientStatusUpdate_generated.h"
#include "Experience_generated.h"
#include "EnginePacket_generated.h"
#include "SuitStatusUpdate_generated.h"
#include "HapticClasses.h"
#include "EncodingOperations.h"
class Wire
{
public:
	static void Wire::sendTo(zmq::socket_t& socket, uint8_t* data, int size) {
		zmq::message_t msg(size);
		memcpy((void*)msg.data(), data, size);
		socket.send(msg);
	}
	void Wire::sendToEngine(uint8_t* data, int size) {
		sendTo(*_sendToEngineSocket, data, size);
	}
	/* Sending */
	void Wire::Send(const struct flatbuffers::Offset<NullSpace::HapticFiles::Experience>& input, std::string name) {
		_encoder.Finalize(input, name, boost::bind(&Wire::sendToEngine, this, _1, _2));
	}
	
	void Wire::Send(const struct flatbuffers::Offset<NullSpace::HapticFiles::Pattern>& input, std::string name)
	{
		_encoder.Finalize(input, name, boost::bind(&Wire::sendToEngine, this, _1, _2));
	}

	void Wire::Send(const struct flatbuffers::Offset<NullSpace::HapticFiles::Sequence>& input, std::string name)
	{
	
		_encoder.Finalize(input, name, boost::bind(&Wire::sendToEngine, this, _1, _2));
	}

	void Wire::Send(const struct flatbuffers::Offset<NullSpace::HapticFiles::HapticEffect>& input) {
		_encoder.Finalize(input, boost::bind(&Wire::sendToEngine, this, _1, _2));
	}
	void Wire::Send(const struct flatbuffers::Offset<NullSpace::Communication::ClientStatusUpdate>& input) {
		
	}
	void Wire::Send(const struct flatbuffers::Offset<NullSpace::Communication::SuitStatusUpdate>& input) {
	}
	
	bool Wire::ReceiveStatus(NullSpace::Communication::SuitStatus* status) {
		zmq::message_t msg;
		if (_receiveFromEngineSocket->recv(&msg, ZMQ_DONTWAIT)) {
			flatbuffers::Verifier verifier(reinterpret_cast<uint8_t*>(msg.data()), msg.size());
			if (NullSpace::HapticFiles::VerifyHapticPacketBuffer(verifier)) {
				auto packet =
					std::unique_ptr<const NullSpace::Communication::EnginePacket>(NullSpace::Communication::GetEnginePacket(msg.data()));
				if (packet->packet_type() == NullSpace::Communication::PacketType::PacketType_SuitStatusUpdate) {
					auto decoded = EncodingOperations::Decode(static_cast<const NullSpace::Communication::SuitStatusUpdate*>(packet->packet()));
					*status = decoded;
					return true;
				}
			}
		}
		return false;
	}

	~Wire()
	{
		/* VERY IMPORTANT. If you do not close the sockets, the DLL will cause Unity to freeze!*/
		_sendToEngineSocket->close();
		_receiveFromEngineSocket->close();
	}
	Wire(std::string sendAddress, std::string receiveAddress)
	{
		_context = std::make_unique<zmq::context_t>(1);
		_sendToEngineSocket = std::make_unique<zmq::socket_t>(*_context, ZMQ_PAIR);
		_sendToEngineSocket->connect(sendAddress);

		_receiveFromEngineSocket = std::make_unique<zmq::socket_t>(*_context, ZMQ_SUB);
		_receiveFromEngineSocket->connect(receiveAddress);
		//_receiveFromEngineSocket->setsockopt(ZMQ_SUBSCRIBE, "");

	}

private:
	EncodingOperations _encoder;
	std::unique_ptr<zmq::socket_t> _sendToEngineSocket;
	std::unique_ptr<zmq::socket_t> _receiveFromEngineSocket;
	std::unique_ptr<zmq::context_t> _context;
};




