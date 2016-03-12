#define RTP_HEADER_SIZE 14
#define MAX_PAYLOAD_SIZE 1200
#define MAX_PACKET_SIZE (RTP_HEADER_SIZE + MAX_PAYLOAD_SIZE)

/*
 *
 */
typedef struct RTPHeader {

  // byte 0
  uint8_t csrcCnt : 4;
  uint8_t extension : 1;
  uint8_t padding : 1;
  uint8_t version : 2;

  // byte 1
  uint8_t payloadType : 7;
  uint8_t marker : 1;

  // byte 2-3
  int16_t seqNum;

  // byte 4-7
  int32_t timeStamp;

  // byte 8-11
  int32_t syncSRC;

  // byte 12
  int32_t csrcList[15];

} __attribute__((packed)) RTPHeader_t;

/*
 *
 */
typedef struct FUIndicator {

  // byte 0
  uint8_t type : 5;
  uint8_t nri_idc : 2;
  uint8_t forbidden : 1;

} __attribute__((packed)) FUIndicator_t;

/*
 *
 */
typedef struct FUHeader {

  // byte 0
  uint8_t type : 5;
  uint8_t R : 1;
  uint8_t E : 1;
  uint8_t S : 1;

} __attribute__((packed)) FUHeader_t;
