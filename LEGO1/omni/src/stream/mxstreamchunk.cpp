#include "mxstreamchunk.h"

#include "mxdsbuffer.h"
#include "mxdssubscriber.h"
#include "mxendian.h"
#include "mxutilities.h"

// FUNCTION: LEGO1 0x100c2fe0
MxStreamChunk::~MxStreamChunk()
{
	if (m_buffer) {
		m_buffer->ReleaseRef(this);
	}
}

// FUNCTION: LEGO1 0x100c3050
MxResult MxStreamChunk::ReadChunk(MxDSBuffer* p_buffer, MxU8* p_chunkData)
{
	MxResult result = FAILURE;

	if (p_chunkData != NULL && UnalignedRead<MxU32>(p_chunkData) == FOURCC('M', 'x', 'C', 'h')) {
		if (ReadChunkHeader(p_chunkData + 8)) {
			if (p_buffer) {
				SetBuffer(p_buffer);
				p_buffer->AddRef(this);
			}
			result = SUCCESS;
		}
	}

	return result;
}

// FUNCTION: LEGO1 0x100c30a0
MxU32 MxStreamChunk::ReadChunkHeader(MxU8* p_chunkData)
{
	MxU32 headersize = 0;
	if (p_chunkData) {
		MxU8* chunkData = p_chunkData;

		fprintf(stderr, "DBG ReadChunkHeader: raw bytes:");
		for (MxU32 i = 0; i < 14; i++) fprintf(stderr, " %02x", chunkData[i]);
		fprintf(stderr, "\n");

		m_flags = EndianReadLE16(p_chunkData);
		p_chunkData += sizeof(MxU16);

		m_objectId = EndianReadLE32(p_chunkData);
		p_chunkData += sizeof(MxU32);

		m_time = EndianReadLE32(p_chunkData);
		p_chunkData += sizeof(MxU32);

		m_length = EndianReadLE32(p_chunkData);
		p_chunkData += sizeof(MxU32);

		m_data = p_chunkData;
		headersize = p_chunkData - chunkData;

		fprintf(stderr, "DBG ReadChunkHeader: flags=0x%04x objId=0x%08x time=%d length=%u\n",
			m_flags, m_objectId, (MxS32) m_time, m_length);
	}

	return headersize;
}

// FUNCTION: LEGO1 0x100c30e0
// FUNCTION: BETA10 0x10151517
MxResult MxStreamChunk::SendChunk(MxDSSubscriberList& p_subscriberList, MxBool p_append, MxS16 p_obj24val)
{
	fprintf(stderr, "DBG SendChunk: objId=0x%08x flags=0x%04x time=%d len=%u obj24val=%d nSubscribers=%zu\n",
		m_objectId, m_flags, (MxS32) m_time, m_length, p_obj24val, p_subscriberList.size());
	for (MxDSSubscriberList::iterator it = p_subscriberList.begin(); it != p_subscriberList.end(); it++) {
		fprintf(stderr, "DBG SendChunk  subscriber objId=0x%08x unk48=%d\n",
			(*it)->GetObjectId(), (*it)->GetUnknown48());
		if ((*it)->GetObjectId() == m_objectId && (*it)->GetUnknown48() == p_obj24val) {
			fprintf(stderr, "DBG SendChunk  MATCH, sending\n");
			if (m_flags & DS_CHUNK_END_OF_STREAM && m_buffer) {
				m_buffer->ReleaseRef(this);
				m_buffer = NULL;
			}

			(*it)->AddData(this, p_append);

			return SUCCESS;
		}
	}
	fprintf(stderr, "DBG SendChunk: no subscriber matched, returning FAILURE\n");

	return FAILURE;
}

// FUNCTION: LEGO1 0x100c3170
void MxStreamChunk::SetBuffer(MxDSBuffer* p_buffer)
{
	m_buffer = p_buffer;
}

// FUNCTION: LEGO1 0x100c3180
// FUNCTION: BETA10 0x101515f1
MxU16* MxStreamChunk::IntoFlags(MxU8* p_buffer)
{
	return (MxU16*) (p_buffer + 0x08);
}

// FUNCTION: LEGO1 0x100c3190
MxU32* MxStreamChunk::IntoObjectId(MxU8* p_buffer)
{
	return (MxU32*) (p_buffer + 0x0a);
}

// FUNCTION: LEGO1 0x100c31a0
// FUNCTION: BETA10 0x10151626
MxLong* MxStreamChunk::IntoTime(MxU8* p_buffer)
{
	return (MxLong*) (p_buffer + 0x0e);
}

// FUNCTION: LEGO1 0x100c31b0
MxU32* MxStreamChunk::IntoLength(MxU8* p_buffer)
{
	return (MxU32*) (p_buffer + 0x12);
}
