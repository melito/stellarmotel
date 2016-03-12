#include <stdint.h>

#define MAX_SPS_COUNT 32
#define MAX_PPS_COUNT 256

typedef struct SPS {

  uint16_t size;
  unsigned char *bytes;

  // uint8_t forbiden_zero_bit : 1;
  // uint8_t nal_ref_idc : 2;
  // uint8_t nal_unit_type : 5;
  //
  // uint8_t profile_idc;
  //
  // uint8_t constraint_set0_flag : 1;
  // uint8_t constraint_set1_flag : 1;
  // uint8_t constraint_set2_flag : 1;
  // uint8_t constraint_set3_flag : 1;
  // uint8_t reserved_zero_4bits : 4;
  //
  // uint8_t level_idc;
  // uint8_t seq_parameter_set_id;
  // uint8_t log2_max_frame_num_minus4;
  // uint8_t pic_order_cnt_type;
  // uint8_t log2_max_pic_order_cnt_lsb_minus4;
  // uint8_t num_ref_frames;
  // uint8_t gaps_in_frame_num_value_allowed_flag;
  // uint8_t pic_width_in_mbs_minus_1;
  // uint8_t pic_height_in_map_units_minus_1;
  // uint8_t frame_mbs_only_flag;
  // uint8_t direct_8x8_inference_flag;
  // uint8_t frame_cropping_flag;
  // uint8_t vui_prameters_present_flag;
  // uint8_t rbsp_stop_one_bit;
  //
  // uint8_t chroma_formad_idc;

} SPS_t;

typedef struct PPS {

  uint16_t size;
  unsigned char *bytes;

} PPS_t;

typedef struct AVCCAtom {

  unsigned int position;
  unsigned int length;

  uint16_t size;
  unsigned char *bytes;

  uint8_t version;
  uint8_t profile;
  uint8_t compatibility;
  uint8_t level;

  uint8_t reserved : 6;
  uint8_t nalu_length_minus_one : 2;

  uint8_t reserved_after_length_minus_one : 3;
  uint8_t number_of_sps_nalus : 5;
  SPS_t *sps_array[MAX_SPS_COUNT];

  uint8_t number_of_pps_nalus;
  PPS_t *pps_array[MAX_PPS_COUNT];

} __attribute__((packed)) AVCCAtom_t;
